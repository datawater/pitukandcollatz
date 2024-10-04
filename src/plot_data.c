#include "generate_data.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static const char* plot_output_file = "collatz_output.png"; 

int main(void) {
    uint64_t limit = 1048576;
    OrbitData data = get_orbit_data(limit);

    if (!data.fits_into_memory) {
        assert(0 && "Not enough ram pooks.");
    }

    puts("[INFO] Summoning gnuplot.");
    printf("[INFO] Using '%s' as output file.\n", plot_output_file);

    fflush(stdout);
    FILE* gnuplot = popen("gnuplot", "w");

    assert(gnuplot != NULL);

    fprintf(gnuplot,
           "set terminal png\n"
           "set output '%s'\n", plot_output_file);
    fputs("plot '-'\n", gnuplot);
    for (uint64_t i = 0; i < limit; ++i) {
        fprintf(gnuplot, "%"PRIu64" %"PRIu16"\n", i, data.array[i]);
    }

    fputs("e\n", gnuplot);
    fflush(gnuplot);

    puts("[INFO] Done");

    return 0;
}
