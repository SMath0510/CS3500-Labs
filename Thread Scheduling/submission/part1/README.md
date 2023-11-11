# Producer-Consumer Multithreaded Simulation

## Overview

This C program demonstrates a multithreaded solution to the classic producer-consumer problem. Multiple producer threads generate random values and add them to a shared buffer, while multiple consumer threads read and print values from the buffer.

In case the buffer is full, then the other producer threads just exit the code.

## Code Structure

### Global Variables

- `num_producers`: Number of producer threads.
- `num_consumers`: Number of consumer threads.
- `buffer_size`: Size of the shared buffer.
- `production_limit`: Maximum items a producer can produce.
- `consumption_limit`: Maximum items a consumer can consume (based on `CONSUMER_CONSTANT`).
- `in_idx`, `out_idx`: Indices for tracking the position in the buffer.
- `counter`: Number of items in the buffer.
- `items_to_consume`: Total items to be consumed by all consumer threads.
- `buffer_overflow`: Flag to indicate buffer overflow.
- `no_more_items`: Flag to indicate that no more items will be produced.

### Functions

- `set_consumption_limit()`: Sets the consumption limit based on a formula.
- `put(int value)`: Adds a value to the buffer.
- `get()`: Retrieves a value from the buffer.

### Producer Thread (`producer`)

1. Generates random values.
2. Checks if the buffer is full:
   - If no more items will be produced or the buffer is full without an overflow, exits the thread.
3. Enters the critical section (protected by a mutex).
4. Stores the generated value in the buffer.
5. Increments indices and counters.
6. Exits the critical section.
7. Sleeps for a random time.

### Consumer Thread (`consumer`)

1. Waits until there are items in the buffer or no more items will be produced.
2. Enters the critical section (protected by a mutex).
3. Checks if the buffer is empty:
   - If no more items will be produced and the consumer has consumed the required number of items, exits the thread.
4. Reads a value from the buffer.
5. Increments indices and counters.
6. Exits the critical section.
7. Sleeps for a random time.

### Main Function

1. Reads command-line arguments.
2. Initializes global variables and the buffer.
3. Creates producer and consumer threads.
4. Waits for all threads to finish.
5. Checks for buffer overflow.
6. Sets the `no_more_items` flag.
7. Destroys mutex and condition variables.
8. Program completion message.
