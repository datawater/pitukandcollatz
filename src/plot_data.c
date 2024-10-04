#include "generate_data.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <plplot.h>

#define NPTS 1048576

void plot();

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

#ifdef PLOT_WITH_PLPLOT
    plparseopts(&argc, argv, PL_PARSE_FULL);
    plinit();
    plsesc('@');

    plot();
    
    plend();
#else
    plot();
#endif

    return 0;
}

double find_max(double* points, size_t number) {
    double max = 0;

    for (size_t i = 0; i < number; i++) {
        if (points[i] > max) {
            max = points[i];
        }
    }

    return max;
}

void plot(void) {
    uint64_t npts = NPTS;
    OrbitData data = get_orbit_data(npts);

    if (!data.fits_into_memory) {
        assert(0 && "Not enough ram pooks.");
    }

    puts("[INFO] Drawing plot");

#ifdef PLOT_WITH_PLPLOT
    PLFLT xmin, xmax, ymin, ymax;

    PLFLT* x = malloc(sizeof(PLFLT) * npts);
    
    #pragma omp parallel
    for (uint64_t i = 0; i < npts; i++)
        x[i] = i; 

    fflush(stdout);

    xmin = 0;
    xmax = npts;
    ymin = 0;
    ymax = find_max(data.array, npts);

    plssym(0.0, 0.15);
    plschr(0, 0.4);
    
    plcol0(15);
    plenv(xmin, xmax, ymin, ymax, 0, 0);

    plcol0(0);
    pllab("Number", "Orbit Length", "Collatz Orbit length");
    
    plcol0(1);
    plpoin(npts, x, data.array, 2);
#else
    FILE* gnuplot = popen("gnuplot", "w");
    const char* plot_output_file = "collatz_output.png";

    printf("[INFO] Using '%s' as an output file\n", plot_output_file);

    assert(gnuplot != NULL);

    fprintf(gnuplot,
           "set terminal png\n"
           "set output '%s'\n", plot_output_file);
    fputs("plot '-'\n", gnuplot);
    for (uint64_t i = 0; i < npts; ++i) {
        fprintf(gnuplot, "%"PRIu64" %"PRIu16"\n", i, (uint16_t) data.array[i]);
    }
    
    fputs("e\n", gnuplot);
    fflush(gnuplot);
#endif

    puts("[INFO] Done");
}