/*
 * Copyright 2015 Ben Allen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * SHA-256 hash
 *
 * Notes:
 *  - In this context, the term "word" refers to a 32-bit value.
 *  - Variable and function names have been chosen to match the
 *    FIPS 180-4 spec as closely as possible.
 *  - Any undocumented magic numbers were taken directly from the spec.
 */

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <method.h>

// Algorithm parameters
#define BLOCK_SIZE      (512 / 8) // size of input blocks (bytes)
#define HASH_SIZE       (256 / 8) // size of output hash (bytes)
#define HASH_SIZE_WORDS (HASH_SIZE / sizeof(uint32_t))


// Module-specific context structure
struct sha256_context
{
    // current hash value
    uint32_t H[HASH_SIZE_WORDS];

    // current input block
    uint8_t  input[BLOCK_SIZE];

    // amount of data currently in the 'input' buffer (bytes)
    unsigned input_length;

    // total length of the input data seen so far (bytes)
    uint64_t length;
};


static void     sha256_help     (void);
static int      sha256_init     (struct context* ctx);
static int      sha256_process  (struct context* ctx, void* data, size_t len);
static int      sha256_finish   (struct context* ctx);
static uint32_t Ch              (uint32_t x, uint32_t y, uint32_t z);
static uint32_t Maj             (uint32_t x, uint32_t y, uint32_t z);
static uint32_t ROTR            (uint32_t value, unsigned int places);
static int      sha256_update   (struct sha256_context* ctx);


struct method_api sha256 =
{
    .name        = "SHA-256 hash",
    .args        = "-sha256",
    .type        = SHA256,
    .output_size = HASH_SIZE,
    .chunk_size  = 0,
    .help        = &sha256_help,
    .sum_init    = &sha256_init,
    .sum_process = &sha256_process,
    .sum_finish  = &sha256_finish
};

// Constants
static const uint32_t K[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
    0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
    0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
    0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
    0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
    0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
    0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
    0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


// Help text
static void sha256_help(void)
{
    printf("%s - TBD\n", __func__);
}

// Initialize context structure
static int sha256_init(struct context* ctx)
{
    struct sha256_context* context;

    if (ctx == NULL)
    {
        fprintf(stderr, "Invalid context data\n");
        return 1;
    }

    // Ditch any old buffer, if present
    if (ctx->context != NULL)
        free(ctx->context);

    // Allocate a fresh context buffer
    context = malloc(sizeof(*context));
    if (context == NULL)
    {
        fprintf(stderr, "Unable to allocate memory\n");
        return 1;
    }
    memset(context, 0, sizeof(*context));
    ctx->context = context;

    // Initialize hash
    context->H[0] = 0x6a09e667;
    context->H[1] = 0xbb67ae85;
    context->H[2] = 0x3c6ef372;
    context->H[3] = 0xa54ff53a;
    context->H[4] = 0x510e527f;
    context->H[5] = 0x9b05688c;
    context->H[6] = 0x1f83d9ab;
    context->H[7] = 0x5be0cd19;

    return 0;
}

// Process the next sequence of bytes
static int sha256_process(struct context* ctx, void* data, size_t len)
{
    struct sha256_context* context;
    char* ptr;
    unsigned bytes_to_read;

    context = ctx->context;
    ptr = data;

    // Process the incoming data, one message block at a time
    while (len > 0)
    {
        // Calculate how many bytes can be read into the message buffer
        if (len < (BLOCK_SIZE - context->input_length))
            bytes_to_read = len;
        else
            bytes_to_read = (BLOCK_SIZE - context->input_length);

        // Copy over data
        memcpy(&context->input[context->input_length], ptr, bytes_to_read);
        context->input_length += bytes_to_read;

        // If the message buffer is full, update the hash
        if (context->input_length == BLOCK_SIZE)
        {
            if (sha256_update(context))
            {
                fprintf(stderr, "Unable to update hash\n");
                return 1;
            }
        }

        // Update tracking variables
        ptr += bytes_to_read;
        len -= bytes_to_read;
    }

    return 0;
}

// Finish up the hash and calculate the final value
static int sha256_finish(struct context* ctx)
{
    struct sha256_context* context = ctx->context;
    int i;
    uint64_t len_bits;

    assert(context->input_length < BLOCK_SIZE);

    // Append the required '1' bit
    context->input[context->input_length] = 0x80;
    ++context->input_length;

    // Pad message buffer and finalize the hash
    if (context->input_length >= (BLOCK_SIZE - sizeof(len_bits)))
    {
        // Not enough room to finish with current block.
        // Pad the current block ...
        context->input_length = BLOCK_SIZE;
        if (sha256_update(context))
            return 1;

        // ... and add another to finish with.
    }

    // Pad this block and append the message length
    context->length += context->input_length;
    context->length -= 1; // Adjust for the static '0x80' byte, which doesn't count
    len_bits = TO_BE64(context->length * 8);
    memcpy(&context->input[BLOCK_SIZE - sizeof(len_bits)], &len_bits, sizeof(len_bits));
    context->input_length = BLOCK_SIZE;
    if (sha256_update(context))
        return 1;

    // Display hash
    printf("0x");
    for (i = 0; i < HASH_SIZE_WORDS; ++i)
    {
        printf("%08"PRIx32, context->H[i]);
    }
    putchar('\n');

    // Clean up
    free(ctx->context);
    ctx->context = NULL;

    return 0;
}


// === algorithm helper functions ===

static uint32_t Ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ ((~x) & z);
}

static uint32_t Maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

// Circular-rotate a value to the right some number of places
static uint32_t ROTR(uint32_t value, unsigned int places)
{
    assert(places < 32);
    return (value >> places) | (value << (32 - places));
}

#define sigma0(x)   (ROTR((x), 2) ^ ROTR((x),13) ^ ROTR((x), 22))
#define sigma1(x)   (ROTR((x), 6) ^ ROTR((x),11) ^ ROTR((x), 25))
#define gamma0(x)   (ROTR((x), 7) ^ ROTR((x),18) ^ ((x) >> 3))
#define gamma1(x)   (ROTR((x),17) ^ ROTR((x),19) ^ ((x) >> 10))

// Update the hash using a new block of data.
// This function will clear out the message buffer and reset the relevant
//  context data before returning.
static int sha256_update(struct sha256_context* ctx)
{
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t W[64];
    uint32_t T1, T2;
    uint32_t* ptr;
    int t;

    // Ensure that we have enough data to do an iteration
    if (ctx->input_length != BLOCK_SIZE)
    {
        fprintf(stderr, "Can't update hash, available data != block size\n");
        return 1;
    }

    // Prepare message schedule
    ptr = (uint32_t*)&ctx->input;
    for (t = 0; t < 16; ++t)
    {
        W[t] = FROM_BE32(ptr[t]);
    }
    for (t = 16; t < 64; ++t)
    {
        W[t] = gamma1(W[t-2]) + W[t-7] + gamma0(W[t-15]) + W[t-16];
    }

    // Initialize working variables
    a = ctx->H[0];
    b = ctx->H[1];
    c = ctx->H[2];
    d = ctx->H[3];
    e = ctx->H[4];
    f = ctx->H[5];
    g = ctx->H[6];
    h = ctx->H[7];

    // Compute hash update values
    for (t = 0; t < 64; ++t)
    {
        T1 = h + sigma1(e) + Ch(e, f, g) + K[t] + W[t];
        T2 = sigma0(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    // Calculate new intermediate hash value
    ctx->H[0] += a;
    ctx->H[1] += b;
    ctx->H[2] += c;
    ctx->H[3] += d;
    ctx->H[4] += e;
    ctx->H[5] += f;
    ctx->H[6] += g;
    ctx->H[7] += h;

    // Clean up and prepare for next block
    memset(ctx->input, 0, sizeof(ctx->input));
    ctx->input_length = 0;
    ctx->length += ctx->input_length;

    return 0;
}

