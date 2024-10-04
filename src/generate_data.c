#include <assert.h>
#include <omp.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "generate_data.h"

uint64_t get_total_system_ram() {
    int64_t pages = sysconf(_SC_PHYS_PAGES);
    int64_t page_size = sysconf(_SC_PAGE_SIZE);
    return (uint64_t)pages * (uint64_t)page_size;
}

uint16_t collatz_orbit_length(uint64_t x) {
    uint16_t n = 0;
    if (x < 1) return 0;

    while (x != 1) {
        n += 2 - ((x & 1) != 1);
        x = (x & 1) == 0 ? x >> 1 : (3 * x + 1) >> 1;
    }

    return n;
}

OrbitData get_orbit_data(uint64_t limit_of_collatz) {
    OrbitData data = {0};
    
    FILE* data_file = fopen("data/data.bin", "wb");
    assert(data_file != NULL);
    
    uint64_t total_ram = get_total_system_ram();
    uint64_t needed_ram_overall = limit_of_collatz * sizeof(uint16_t);

    data.data_file = data_file;

    uint8_t amount_of_divisions = 1;

    while (needed_ram_overall / amount_of_divisions > total_ram) 
        ++amount_of_divisions;

    size_t needed_memory_to_allocate = sizeof(uint16_t) * (limit_of_collatz / amount_of_divisions);
    uint16_t* collatz_orbit_lengths = calloc(limit_of_collatz / amount_of_divisions, sizeof(uint16_t));
    
    assert(collatz_orbit_lengths != NULL);
    if (amount_of_divisions == 1) {
        data.fits_into_memory = true;
        data.array = collatz_orbit_lengths;
    }

    printf("[INFO] Ended up with these parameters.\n"
            "\tAmount of system ram: %" PRIu64 "\n"
            "\tLimit of orbit length to calculate to: %" PRIu64 "\n"
            "\tNeeded RAM overall: %" PRIu64 "\n"
            "\tAmount of divisions: %" PRIu8 "\n",
            total_ram, limit_of_collatz, needed_ram_overall, amount_of_divisions);

    for (uint8_t division = 0; division < amount_of_divisions; division++) {
        memset(collatz_orbit_lengths, 0, needed_memory_to_allocate);
        
        uint32_t starting_point = limit_of_collatz / amount_of_divisions * division;
        uint32_t end_point = limit_of_collatz / amount_of_divisions * (division + 1);

        #pragma omp parallel for schedule(dynamic)
        for (uint32_t num = starting_point; num < end_point; num++) {
            collatz_orbit_lengths[num - starting_point] = collatz_orbit_length(num);
        }

        fwrite(collatz_orbit_lengths, sizeof(uint16_t), end_point - starting_point, data_file);
    }

    return data;
}
