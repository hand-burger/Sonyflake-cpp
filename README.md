# C++ Sonyflake Implementation
This is a C++ implementation of [Sonyflake](https://github.com/sony/sonyflake), a distributed unique ID generation algorithm.

## Features

- **Thread-Safe**: Designed to be safely used across multiple threads.
- **High Performance**: Capable of generating a high volume of unique IDs quickly.
- **Distributed**: Uniqueness is guaranteed across multiple machines by incorporating a machine ID.

## ID Structure

The generated ID is a 64-bit unsigned integer, with the first bit always set to 0. The remaining 63 bits are allocated as follows:

- **39 bits for Timestamp**: Represents the time elapsed since a custom epoch (January 1, 2025, 00:00 UTC) in 10ms units.
- **8 bits for Sequence Number**: A number that increments for each ID generated within the same 10ms interval.
- **16 bits for Machine ID**: A unique identifier for the machine generating the ID.

## Performance

The default configuration allows for `2^8` (256) unique IDs to be generated every 10ms. This translates to a theoretical maximum of **25,600 IDs per second**.

### Customization for Higher Throughput

For applications requiring higher throughput on a single node, the bit allocation can be adjusted. By increasing the bits for the sequence number, more IDs can be generated within a 10ms window. This comes at the trade-off of reducing the bits available for the machine ID, thereby lowering the total number of unique machines possible in the distributed system.

## Usage

Here is a basic example of how to use the `Sonyflake` generator:

```cpp
#include <iostream>
#include "sonyflake.h"

int main() {
    // Initialize with a machine ID
    Sonyflake sf(1);

    // Generate a unique ID
    uint64_t id = sf.GetUniqueId();

    std::cout << "Generated ID: " << id << std::endl;

    return 0;
}
```

## Building the Benchmark

You can compile and run the included benchmark to measure performance on your system.

```sh
g++ -O3 -g benchmark.cpp -pthread -o benchmark
./benchmark
```
