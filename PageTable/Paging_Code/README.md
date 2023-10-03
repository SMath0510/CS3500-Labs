# OS Paging Module

## Usage

Given an input file with a series of instructions, the code parses and executes these instructions. The internal memory management is implemented using Paging.

## Key Points

1. Maintains an LRU SET providing the list of least recently used processes.
2. Loading into the LRU SET occurs under the following conditions:
   a. Loading a process into main memory
   b. Swapping in a process into main memory
   c. Running a process
3. Stops execution if the instruction does not match the given syntax for the main file input.

## Input Format

```bash
gcc paging.cpp -o paging
./paging -M 32 -V 32 -P 512 -i infile -o outfile
```

- If any flags or values are missing, an error is reported.

## Process Functions

A process can have the following functions:

1. `add x y z`
2. `sub x y z`
3. `load a x`
4. `print x`

Skip the instruction if a syntax error occurs.

## Main Input File Functions

1. `load <filenames...>`
2. `run pid`
3. `kill pid`
4. `listpr`
5. `pte pid file`
6. `pteall file`
7. `swapin pid`
8. `swapout pid`
9. `print memloc length`
10. `exit`

These functionalities are implemented using the `PagingHelper` class.

**Additional Note:** In the `pte pid file` and `pteall file` instructions, append to the file rather than overwriting it.