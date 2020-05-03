#include "PNM.h"
#include <stdio.h>
#include <stdlib.h>

static size_t* min_energy_path_asc(const PNMImage* image);
static double pixel_energy(const PNMImage* image, size_t i, size_t j);
// 0 = red, 1 = green, 2 = blue
static unsigned char
pixel_value(const PNMImage* image, size_t i, size_t j, size_t color);
static double min(double a, double b);

PNMImage* reduceImageWidth(const PNMImage* image, size_t k) {
    PNMImage* reduced_image = image;

    int height = image->height;
    int width = image->width;

    for (size_t iteration = 0; iteration < k; iteration++) {
        PNMImage* new_image = createPNM(width - 1, height);

        double* tab = malloc(height * width * sizeof(double));
        for (size_t i = 0; i < width * height; i++)
            tab[i] = -1;

        double min_energy = -1;
        size_t* min_path = min_energy_path_asc(image);
        /*for (size_t l = 0; l < width; l++) {
            size_t* path = malloc(height * sizeof(size_t));
            double new_energy = min_energy_path((PNMImage*) reduced_image,
                                                height - 1, l, tab);
            if (min_energy == -1 || new_energy < min_energy) {
                min_energy = new_energy;
                min_path = path;
            } else {
                //free(path);
            }
        }*/
        //min_energy = min_energy_path_cost_asc(image);

        // Remove pixels
        size_t x = 0;
        for (size_t a = 0; a < height; a++) {
            x = 0;
            for (size_t b = 0; b < width; b++) {
                if (b == min_path[a]) continue;

                new_image->data[a * new_image->width + x] =
                        reduced_image->data[a * reduced_image->width + b];
                x++;
            }
        }

        width--;

        reduced_image = new_image;
    }

    return reduced_image;
}

static size_t* min_energy_path_asc(const PNMImage* image) {
    size_t m = image->width, n = image->height;

    double* tab = malloc(m * n * sizeof(double));
    if (tab == NULL) {
        perror("Error while allocating memory. Exiting.");
        return 0;
    }

    // Stores optimal move from each pixel.
    // 0 = nothing, 1 = left, 2 = middle, 3 = right
    size_t* moves = calloc(m * n, sizeof(size_t));
    if (moves == NULL) {
        perror("Error while allocating memory. Exiting.");
        return 0;
    }

    // Fill the first line
    for (size_t k = 0; k < m; k++)
        tab[k] = pixel_energy(image, 0, k);

    // Build the double array with the cost for each (i, j)
    for (size_t a = 1; a < n; a++) {
        for (size_t b = 0; b < m; b++) {
            double top_mid = tab[(a - 1) * m + b];

            if (b == 0) { // Cas 2
                double top_right = tab[(a - 1) * m + (b + 1)];

                if (top_mid < top_right) {
                    moves[a * m + b] = 2;
                    tab[a * m + b] = top_mid;
                } else {
                    moves[a * m + b] = 3;
                    tab[a * m + b] = top_right;
                }
            } else if (b == m - 1) { // Cas 3
                double top_left = tab[(a - 1) * m + (b - 1)];

                tab[a * m + b] = min(top_left, top_mid);
                if (top_left < top_mid) {
                    moves[a * m + b] = 1;
                    tab[a * m + b] = top_left;
                } else {
                    moves[a * m + b] = 2;
                    tab[a * m + b] = top_mid;
                }
            } else { // Cas 4
                double top_left = tab[(a - 1) * m + (b - 1)];
                double top_right = tab[(a - 1) * m + (b + 1)];

                if (top_left <= top_mid && top_left <= top_right) {
                    moves[a * m + b] = 1;
                    tab[a * m + b] = top_left;
                }
                if (top_mid <= top_right && top_mid <= top_left) {
                    moves[a * m + b] = 2;
                    tab[a * m + b] = top_mid;
                } else {
                    moves[a * m + b] = 3;
                    tab[a * m + b] = top_right;
                }
            }

            tab[a * m + b] += pixel_energy(image, a, b);
        }
    }

    // Select minimum.
    double min = tab[(n - 1) * m + 0];
    double opt_end_pixel = 0;
    for (size_t k = 1; k < m; k++) {
        double new = tab[(n - 1) * m + k];
        if (new < min) {
            min = new;
            opt_end_pixel = k;
        }
    }
    free(tab);

    // Build the path.
    size_t* path = malloc(n * sizeof(size_t));
    if (path == NULL) {
        perror("Error while allocating memory. Exiting.");
        return 0;
    }

    size_t j = opt_end_pixel; // col
    for (size_t k = n - 1; k-- > 0;) { // row
        path[n - 1 - k] = j;
        switch (moves[k * m + j]) {
            case 1:
                j--;
                break;
            case 3:
                j++;
                break;
        }
    }

    free(moves);

    return path;
}

static double pixel_energy(const PNMImage* image, size_t i, size_t j) {
    double energy = 0;

    if (i >= image->height || j >= image->width) {
        return 0;
    }

    // iterate over red, green and blue.
    for (size_t c = 0; c < 3; c++) {
        energy += abs((pixel_value(image, i - 1, j, c) -
                       pixel_value(image, i + 1, j, c))) / 2;
        energy += abs((pixel_value(image, i, j - 1, c) -
                       pixel_value(image, i, j + 1, c))) / 2;
    }

    return energy;
}

static unsigned char
pixel_value(const PNMImage* image, size_t i, size_t j, size_t color) {
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

static double min(double a, double b) {
    return a > b ? b : a;
}
