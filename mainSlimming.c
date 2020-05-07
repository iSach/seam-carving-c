/* ------------------------------------------------------------------------- *\
 * NAME
 *      slimming
 * SYNOPSIS
 *      slimming input_file output_file nbPix
 * DESCIRPTION
 *      Apply the slimming algorithm to the given input image
 * ARGUMENTS
 *      input_file      An input image file in PNM format
 *      output_file     An output image file (format will be PNM)
 *      nbPix           The number of pixel (integer) by which
 *                      to decrease the input image (nbPix > 0)
 *
 * USAGE
 *      ./slimming input.pnm output.pnm 50
 *          will ouput an image whose width is 50 pixels less than the input
 \* ------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slimming.h"
#include "PNM.h"


int main(int argc, char* argv[])
{
    /* --- Argument parsing --- */
    if (argc != 4) {
        fprintf(stderr, "Usage: %s input.pnm output.pnm nbPix\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Slimming width
    int nbPix;
    if(sscanf (argv[3], "%d", &nbPix) != 1)
    {
        fprintf(stderr, "%s\n", "Aborting; nbPix should be a positive integer.\n");
        exit(EXIT_FAILURE);
    }
    if(nbPix <= 0)
    {
        fprintf(stderr, "nbPix must be strictly positive. Got '%d'\n", nbPix);
        return EXIT_FAILURE;
    }
    size_t k = (size_t)nbPix;

    // Load image
    PNMImage* original = readPNM(argv[1]);
    if (!original)
    {
        fprintf(stderr, "Aborting; cannot load image '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    if(k >= original->width)
    {
        fprintf(stderr, "Aborting; image of width %zu cannot be reduced by %zu pixels\n", original->width, k);
        freePNM(original);
        return EXIT_FAILURE;
    }

    /* --- Slimming --- */
    PNMImage* output = reduceImageWidth(original, k);

    /* --- Writing output --- */
    if (!output)
    {
        fprintf(stderr, "Aborting; cannot build new image\n");
        freePNM(original);
        return EXIT_FAILURE;
    }

    // Save and free
    writePNM(argv[2], output);
    freePNM(original);
    freePNM(output);

    return EXIT_SUCCESS;
}
