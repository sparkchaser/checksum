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
 * Simple sum-of-bytes checksum
 *
 * Since the algorithm is the same, all checksum sizes use the same
 * code.  The only thing that differs is the number of digits that
 * get printed.
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <method.h>

static void simple8_help    (void);
static void simple16_help   (void);
static void simple32_help   (void);
static void simple64_help   (void);
static int  simple_init     (struct context* ctx);
static int  simple_process  (struct context* ctx, void* data, size_t len);
static int  simple_finish   (struct context* ctx);

// 8-bit version
struct method_api simple_8 =
{
    .name        = "8-bit sum",
    .args        = "-8",
    .type        = SIMPLE8,
    .output_size = 1,
    .chunk_size  = 0,
    .help        = &simple8_help,
    .sum_init    = &simple_init,
    .sum_process = &simple_process,
    .sum_finish  = &simple_finish
};

// 16-bit version
struct method_api simple_16 =
{
    .name        = "16-bit sum",
    .args        = "-16",
    .type        = SIMPLE16,
    .output_size = 2,
    .chunk_size  = 0,
    .help        = &simple16_help,
    .sum_init    = &simple_init,
    .sum_process = &simple_process,
    .sum_finish  = &simple_finish
};

// 32-bit version
struct method_api simple_32 =
{
    .name        = "32-bit sum",
    .args        = "-32",
    .type        = SIMPLE32,
    .output_size = 4,
    .chunk_size  = 0,
    .help        = &simple32_help,
    .sum_init    = &simple_init,
    .sum_process = &simple_process,
    .sum_finish  = &simple_finish
};

// 64-bit version
struct method_api simple_64 =
{
    .name        = "64-bit sum",
    .args        = "-64",
    .type        = SIMPLE64,
    .output_size = 8,
    .chunk_size  = 0,
    .help        = &simple64_help,
    .sum_init    = &simple_init,
    .sum_process = &simple_process,
    .sum_finish  = &simple_finish
};


// Module-specific context structure
struct simple_context
{
    uint64_t sum;
};

// Help text functions
static void simple8_help(void)
{
    printf("%s - TBD\n", __func__);
}
static void simple16_help(void)
{
    printf("%s - TBD\n", __func__);
}
static void simple32_help(void)
{
    printf("%s - TBD\n", __func__);
}
static void simple64_help(void)
{
    printf("%s - TBD\n", __func__);
}

// Initialize a context structure
static int simple_init(struct context* ctx)
{
    struct simple_context* context;

    // Allocate a new context structure
    context = malloc(sizeof(*context));
    if (context == NULL)
    {
        fprintf(stderr, "No memory\n");
        return 1;
    }

    // Initialize context information
    context->sum = 0;
    ctx->context = context;

    return 0;
}

// Add more bytes to the running total
static int simple_process(struct context* ctx, void* data, size_t len)
{
    struct simple_context* context;
    uint8_t* ptr;

    context = ctx->context;
    for (ptr = data; ptr < &((uint8_t*)data)[len]; ++ptr)
    {
        context->sum += *ptr;
    }

    return 0;
}

// Display result and clean up context data
static int simple_finish(struct context* ctx)
{
    struct simple_context* context;
    int retval = 0;

    // Display result
    context = (ctx->context);
    switch (ctx->which)
    {
        case SIMPLE8:
            printf("0x%02"PRIx8"\n",   (uint8_t)(context->sum & 0xff));
            break;
        case SIMPLE16:
            printf("0x%04"PRIx16"\n",  (uint16_t)(context->sum & 0xffff));
            break;
        case SIMPLE32:
            printf("0x%08"PRIx32"\n",  (uint32_t)(context->sum & 0xffffffff));
            break;
        case SIMPLE64:
            printf("0x%016"PRIx64"\n", context->sum);
            break;
        default:
            fprintf(stderr, "Context information format error\n");
            retval = 1;
    }

    // Clean up
    free(ctx->context);
    ctx->context = NULL;

    return retval;
}
