checksum
========
A simple command-line utility for generating different types of checksums on a file.

This is a combination of a number of one-off tools that I've built over the years,
combined into a single utility with a consistent interface.  The code is released
here in case it is useful to others.

## Status ##
The program is very much a work in progress and is not yet complete.  You are free
to use it, but do so at your own risk.  Source and binary compatibility can and will
break with no warning.

## Checksum Types ##
The following types of checksums are currently supported:
 * Simple sum-of bytes (8-, 16-, 32-, and 64-bit)
 * SHA256 hash

## To-Do List ##
 * Add more checksum types
 * Enhance command-line usage/help text
 * Create automated test harness
 * Come up with a more unique and interesting name

## License ##
See the `LICENSE` file in the root directory for license information.

