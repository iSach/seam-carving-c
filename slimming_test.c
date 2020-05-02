#include "PNM.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static double pixel_energy(PNMImage* image, size_t i, size_t j);

// 0 = red, 1 = green, 2 = blue
static unsigned char
pixel_value(PNMImage* image, size_t i, size_t j, size_t color);

// just a test file -> draw energy
PNMImage* reduceImageWidth(const PNMImage* image, size_t k) {
    PNMImage* reduced_image = createPNM(image->width, image->height);

    for (size_t i = 0; i < image->height; i++) {
        for (size_t j = 0; j < image->width; j++) {
            double energy = pixel_energy(image, i, j);

            // 519 = max energy in 01.pnm
            unsigned char resized = (char) ((energy / 519) * 255);
            PNMPixel* pixel = (reduced_image->data +
                               (i * reduced_image->width + j));
           // printf("E(%zu, %zu) = %lf (-> %d)\n", i, j, energy, resized);
            pixel->red = resized;
            pixel->green = resized;
            pixel->blue = resized;
        }
    }

    return reduced_image;
}

static double pixel_energy(PNMImage* image, size_t i, size_t j) {
    double energy = 0;

    if (i >= image->height || j >= image->width) {
        return 0;
    }

    // iterate over red, green and blue.
    for (size_t c = 0; c < 3; c++) {
        energy += fabs((pixel_value(image, i - 1, j, c) -
                        pixel_value(image, i + 1, j, c))) / 2;
        energy += fabs((pixel_value(image, i, j - 1, c) -
                        pixel_value(image, i, j + 1, c))) / 2;
    }

    return energy;
}

static unsigned char
pixel_value(PNMImage* image, size_t i, size_t j, size_t color) {
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i >= image->height) i = image->height - 1;
    if (j >= image->width) i = image->width - 1;

    PNMPixel* pixel = (image->data + (i * image->width + j));
    switch (color) {
        case 0:
            return pixel->red;
        case 1:
            return pixel->green;
        default:
            return pixel->blue;
    }
}
