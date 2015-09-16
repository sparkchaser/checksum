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
 * Endian-swapping functions.
 *
 * Many platforms have built-in functions for these, but they are
 * re-implemented from scratch so that this program remains free
 * of platform-specific dependencies or endian-ness macros.
 */

#include <inttypes.h>

// Change a native 16-bit value into big-endian format
uint16_t TO_BE16(uint16_t in)
{
    uint16_t out = 0;
    unsigned char* pout = (unsigned char*)&out;
    int i;

    for (i = sizeof(out) - 1; i >= 0; --i, in >>= 8)
        pout[i] = in & 0xff;

    return out;
}

// Change a big-endian 16-bit value into native format
uint16_t FROM_BE16(uint16_t in)
{
    unsigned char* pin = (unsigned char*)&in;
    uint16_t out;
    int i;

    for (i = 0, out = 0; i < sizeof(in); ++i)
        out = (out << 8) + pin[i];

    return out;
}

// Change a native 32-bit value into big-endian format
uint32_t TO_BE32(uint32_t in)
{
    uint32_t out = 0;
    unsigned char* pout = (unsigned char*)&out;
    int i;

    for (i = sizeof(out) - 1; i >= 0; --i, in >>= 8)
        pout[i] = in & 0xff;

    return out;
}

// Change a big-endian 32-bit value into native format
uint32_t FROM_BE32(uint32_t in)
{
    unsigned char* pin = (unsigned char*)&in;
    uint32_t out;
    int i;

    for (i = 0, out = 0; i < sizeof(in); ++i)
        out = (out << 8) + pin[i];

    return out;
}

// Change a native 64-bit value into big-endian format
uint64_t TO_BE64(uint64_t in)
{
    uint64_t out = 0;
    unsigned char* pout = (unsigned char*)&out;
    int i;

    for (i = sizeof(out) - 1; i >= 0; --i, in >>= 8)
        pout[i] = in & 0xff;

    return out;
}

// Change a big-endian 64-bit value into native format
uint64_t FROM_BE64(uint64_t in)
{
    unsigned char* pin = (unsigned char*)&in;
    uint64_t out;
    int i;

    for (i = 0, out = 0; i < sizeof(in); ++i)
        out = (out << 8) + pin[i];

    return out;
}
