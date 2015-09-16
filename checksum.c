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
 * Flexible checksum utility
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "method.h"

// Structure for making a list of APIs
struct method_list
{
    struct method_api*  api;
    struct method_list* next;
};
static struct method_list* list = NULL;
static struct method_list* list_tail = NULL;

static struct method_api*  current_api = NULL;
static FILE*               input = NULL;

// Local function prototypes
static void usage           (FILE* stream);
static void cleanup         (void);
static int  register_method (struct method_api* api);
static int  register_methods(void);



int main(int argc, char** argv)
{
    int retval;
    struct method_list* ptr;
    struct context ctx;
    void* buf;
    size_t buf_size;
    size_t ret;

    // Register cleanup function
    atexit(&cleanup);

    // Register checksum methods
    retval = register_methods();
    if (retval)
    {
        fprintf(stderr, "Error initializing program [%i]\n", retval);
        return 1;
    }

    // Parse CLI arguments
    if (argc <= 1)
    {
        // TODO: at some point, this should default to something simple
        usage(stderr);
        return 1;
    }
    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
    {
        // TODO: add support for method-specific help, a la "-h -sha256"
        usage(stdout);
        return 0;
    }
    for (ptr = list; ptr != NULL; ptr = ptr->next)
    {
        if (strcmp(argv[1], ptr->api->args) == 0)
        {
            current_api = ptr->api;
            //printf("Using method \"%s\"\n", ptr->api->name);
            break;
        }
    }
    if (current_api == NULL)
    {
        fprintf(stderr, "Unsupported argument: %s\n", argv[1]);
        usage(stderr);
        return 1;
    }

    // Open input file
    if (argc < 3)
    {
        fprintf(stderr, "No input file specified\n");
        return 1;
    }
    if (strcmp(argv[2], "-") == 0)
    {
        // Use stdin instead of a file
        input = stdin;
    }
    else
    {
        // Read data from a file
        input = fopen(argv[2], "rb");
        if (input == NULL)
        {
            fprintf(stderr, "Unable to open file '%s'\n", argv[2]);
            return 1;
        }
    }


    // Initialize context information
    ctx.which = current_api->type;
    ctx.context = NULL;
    if (current_api->sum_init(&ctx))
    {
        fprintf(stderr, "Unable to initialize algorithm\n");
        return 1;
    }

    // Perform checksum
    buf_size = current_api->chunk_size;
    if (buf_size == 0)
        buf_size = 256 * 1024; // default to something relatively sensible
    buf = malloc(buf_size);
    if (buf == NULL)
    {
        fprintf(stderr, "Unable to allocate memory\n");
        return 1;
    }
    while(1)
    {
        ret = fread(buf, 1, buf_size, input);
        if (ret == buf_size)
        {
            // Read successful, process this block
            if (current_api->sum_process(&ctx, buf, buf_size))
            {
                fprintf(stderr, "Error processing data\n");
                free(buf);
                return 1;
            }
            continue;
        }

        // Read less than expected, find out why
        if (!feof(input))
        {
            fprintf(stderr, "Error reading from %s\n", (input == stdin) ? "stdin" : "input file");
            free(buf);
            return 1;
        }

        // Reached the end of the input, so process the final partial block
        if (current_api->sum_process(&ctx, buf, ret))
        {
            fprintf(stderr, "Error processing data\n");
            free(buf);
            return 1;
        }

        // No more input to process
        break;
    };
    free(buf);

    // Output result
    if (current_api->sum_finish(&ctx))
    {
        fprintf(stderr, "Error finalizing checksum\n");
        return 1;
    }

    // Clean up and exit
    return 0;
}

// Display usage information for the program and all known methods
// Argument 'stream' should be either 'stdout' or 'stderr'.
static void usage(FILE* stream)
{
    struct method_list* ptr;

    // Program usage info
    // NOTE: flag begins on column 2, description on column 15
    fprintf(stream, "Usage: checksum [options] [method] file\n");
    fprintf(stream, "Options:\n");
    fprintf(stream, "  -h, --help   Display this information\n");
    fprintf(stream, "\n");

    // Method-specific info
    fprintf(stream, "Methods:\n");
    for (ptr = list; ptr != NULL; ptr = ptr->next)
    {
        fprintf(stream, "  %-10.10s   %s\n", ptr->api->args, ptr->api->name);
    }

    // Other information
    fprintf(stderr, "When file is '-', read standard input.\n");
}

static void cleanup(void)
{
    struct method_list* ptr;

    // Recursively free memory
    while (list != NULL)
    {
        ptr = list;
        list = list->next;
        free(ptr);
    }

    // Clear out pointers
    list = list_tail = NULL;
    current_api = NULL;

    // Close files
    if ((input != NULL) && (input != stdin))
    {
        fclose(input);
        input = NULL;
    }
}


// Register a specific checksum method's API
static int register_method(struct method_api* api)
{
    struct method_list* entry;

    if (api == NULL)
    {
        return -1;
    }

    // Allocate a new list entry
    entry = malloc(sizeof(*entry));
    if (entry == NULL)
    {
        return -1;
    }
    entry->api = api;
    entry->next = NULL;

    // Add entry to list
    if (list == NULL)
    {
        list = entry;
    }
    if (list_tail != NULL)
    {
        list_tail->next = entry;
    }
    list_tail = entry;

    return 0;
}

// Helper macro to avoid duplicate code
#define register_it(x) \
do {\
    int ret;\
    ret = register_method(x);\
    if (ret) return ret;\
} while(0)

// Register all checksum method APIs
static int register_methods(void)
{
    register_it(&simple_8);
    register_it(&simple_16);
    register_it(&simple_32);
    register_it(&simple_64);
    register_it(&sha256);

    return 0;
}

