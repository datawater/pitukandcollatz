#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    FILE* data_file;
    uint16_t* array;

    bool fits_into_memory;
} OrbitData;

uint16_t collatz_orbit_length(uint64_t x);
OrbitData get_orbit_data(uint64_t limit_of_collatz);