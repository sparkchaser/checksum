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
 * Flexible checksum utility.
 *
 * Common structures and functions.
 */

#ifndef __METHOD_H__
#define __METHOD_H__

#include <inttypes.h>
#include <stddef.h>

// Identify a supported checksum algorithm
enum sum_type
{
    SIMPLE8,
    SIMPLE16,
    SIMPLE32,
    SIMPLE64,
    CRC16,
    CRC32,
    MD5,
    SHA1,
    SHA256
};

// Context information for a checksum operation
struct context
{
    // Identifies the algorithm used in this structure
    enum sum_type which;

    // algorithm-specific context information
    void* context;
};

// Basic API needed for each checksum method
struct method_api
{
    // short name of this method
    char*          name;

    // CLI argument for this method
    char*          args;

    // enumeration value for this type of checksum
    enum sum_type  type;

    // size of the generated checksum, in bytes
    size_t         output_size;

    // if non-zero, checksumming must be done in this size chunks
    size_t         chunk_size;

    // function to print help text
    void (*help)(void);

    // called before starting a checksum
    int (*sum_init)(struct context* ctx);

    // called for each "chunk" of data, in order
    int (*sum_process)(struct context* ctx, void* data, size_t len);

    // called after completing a checksum
    int (*sum_finish)(struct context* ctx);
};


// Core functions used by method implementations
uint16_t TO_BE16    (uint16_t in);
uint16_t TO_LE16    (uint16_t in);
uint16_t FROM_BE16  (uint16_t in);
uint16_t FROM_LE16  (uint16_t in);
uint32_t TO_BE32    (uint32_t in);
uint32_t TO_LE32    (uint32_t in);
uint32_t FROM_BE32  (uint32_t in);
uint32_t FROM_LE32  (uint32_t in);
uint64_t TO_BE64    (uint64_t in);
uint64_t TO_LE64    (uint64_t in);
uint64_t FROM_BE64  (uint64_t in);
uint64_t FROM_LE64  (uint64_t in);


// Method-specific API structures
extern struct method_api simple_8;
extern struct method_api simple_16;
extern struct method_api simple_32;
extern struct method_api simple_64;
extern struct method_api sha256;

#endif
