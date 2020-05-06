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

static void compare_images(PNMImage* a, PNMImage* b) {
    size_t count = 0;
    for (int i = 0; i < a->height; i++) {
        for (int j = 0; j < a->width; j++) {
            PNMPixel* p1 = a->data + i * a->width + j;
            size_t data1 = p1->red + p1->blue + p1->green;
            PNMPixel* p2 = b->data + i * b->width + j;
            size_t data2 = p2->red + p2->blue + p2->green;

            if (data1 != data2) {
                printf("Different at: (%d, %d)\n", i, j);
                count++;
            }
        }
    }
    printf("\nDifferent Count: %zu\n", count);
}

static void const_img() {
    const PNMImage* image = createPNM(500, 500);
    for (size_t i = 0; i < 500; i++) {
        for (size_t j = 0; j < 500; j++) {
            PNMPixel* pixel = image->data + i * 500 + j;
            pixel->red = 200;
            pixel->blue = 200;
            pixel->green = 200;
        }
    }
    writePNM("pnm/const.pnm", image);

    printf("----------| Before |----------\n");
    printf("Img Width: %zu\n", image->width);
    printf("Img Height: %zu\n", image->height);
    for (size_t i = 0; i < 500; i++) {
        for (size_t j = 0; j < 500; j++) {
            PNMPixel* pixel = image->data + i * 500 + j;
            unsigned char r = pixel->red;
            unsigned char g = pixel->blue;
            unsigned char b = pixel->green;

            if(r != 200 || g != 200 || b != 200) {
                printf("Image is not OK!!!\n");
            }
        }
    }

    const PNMImage* modded = reduceImageWidth(image, 2);

    printf("\n\n----------| After |----------\n");
    printf("Img Width: %zu\n", modded->width);
    printf("Img Height: %zu\n", modded->height);
    for (size_t i = 0; i < modded->height; i++) {
        for (size_t j = 0; j < modded->width; j++) {
            PNMPixel* pixel = image->data + i * modded->width + j;
            unsigned char r = pixel->red;
            unsigned char g = pixel->blue;
            unsigned char b = pixel->green;

            if(r != 200 || g != 200 || b != 200) {
                printf("Image is not OK!!!\n");
            }
        }
    }
}

// Compare images...
int main() {
//    PNMImage* original = readPNM("pnm/waves.pnm");
//    PNMImage* naive = reduceImageWidth(original, 1);
//    for (int i = 0; i < 99; i++) {
//        naive = reduceImageWidth(naive, 1);
//    }
//    PNMImage* efficient = reduceImageWidth(original, 100);
//
//    compare_images(naive, efficient);

    const_img();

//    writePNM("pnm/07naive.pnm", naive);
//    writePNM("pnm/07effi.pnm", naive);

    return 0;
}

int main2(int argc, char* argv[]) {
    /* --- Argument parsing --- */
    if (argc != 4) {
        fprintf(stderr, "Usage: %s input.pnm output.pnm nbPix\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Slimming width
    int nbPix;
    if (sscanf(argv[3], "%d", &nbPix) != 1) {
        fprintf(stderr, "%s\n",
                "Aborting; nbPix should be a positive integer.\n");
        exit(EXIT_FAILURE);
    }
    if (nbPix <= 0) {
        fprintf(stderr, "nbPix must be strictly positive. Got '%d'\n", nbPix);
        return EXIT_FAILURE;
    }
    size_t k = (size_t) nbPix;

    // Load image
    PNMImage* original = readPNM(argv[1]);
    if (!original) {
        fprintf(stderr, "Aborting; cannot load image '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (k >= original->width) {
        fprintf(stderr,
                "Aborting; image of width %zu cannot be reduced by %zu pixels\n",
                original->width, k);
        freePNM(original);
        return EXIT_FAILURE;
    }

    /* --- Slimming --- */
    PNMImage* output = reduceImageWidth(original, k);

    /* --- Writing output --- */
    if (!output) {
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
