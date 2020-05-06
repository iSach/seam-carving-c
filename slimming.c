#include "PNM.h"
#include <stdlib.h>
#include <math.h>

#include "slimming.h"

/**
 * Calculates and returns the minimum energy path in the given energy 2D array.
 *
 * @param energy_tab The 2D array representing the energies of each pixel
 *                   of the image.
 * @param width The width of the image.
 * @param height The height of the image.
 */
static size_t* min_energy_path(const double* energy_tab, size_t width, size_t height);

/**
 * @return The energy of a pixel in a given image, given by its position.
 * @param image The image the pixel is in
 * @param i The row of the pixel
 * @param j The column of the pixel
 */
static double pixel_energy(const PNMImage* image, size_t i, size_t j);

/**
 * @return the intensity of a pixel in a given image, by its color.
 * If the given pixel is outside of the borders, will return the closest border
 * pixel.
 * @param image The image the pixel is in
 * @param i The row of the pixel
 * @param j The column of the pixel
 * @param color The color code to get the intensity of.
 *              0 = red, 1 = green, 2 = blue
 */
static unsigned char pixel_value(const PNMImage* image, size_t i, size_t j,
                                 size_t color);

PNMImage* reduceImageWidth(const PNMImage* image, size_t k) {
    if (image == NULL || k > image->width) {
        return NULL;
    }

    PNMImage* original = (PNMImage*) image;

    size_t height = image->height;
    size_t width = image->width;

    // Build the energy 2D array, that stores the energy values
    // between the k iterations to reduce execution time by not recalculating
    // all values each time.
    double* energy_tab = malloc(height * width * sizeof(double));

    if (energy_tab == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            energy_tab[i * width + j] = pixel_energy(original, i, j);
        }
    }

    for (size_t iteration = 0; iteration < k; iteration++) {
        size_t new_width = width - 1;

        PNMImage* new_image = createPNM(width - 1, height);

        double* new_energy_tab = malloc(height * new_width * sizeof(double));
        if (new_energy_tab == NULL) {
            return NULL;
        }

        size_t* min_path = min_energy_path(energy_tab, width, height);

        // Remove pixels
        for (size_t i = 0; i < height; i++) {
            size_t x = 0;
            x = 0;
            size_t path_col = min_path[i];
            for (size_t j = 0; j < width; j++) {
                if (j == path_col) continue;
                
                new_energy_tab[i * new_width + x] = energy_tab[i * width + j];
                new_image->data[i * new_width + x] = original->data[i * width + j];

                x++;
            }
        }

        // Update pixel energies adjacent to the deleted path
        for (size_t i = 0; i < height; i++) {
            size_t path_col = min_path[i];

            if (path_col < new_width) {
                new_energy_tab[i * new_width + path_col] =
                        pixel_energy(new_image, i, path_col);
            }

            if (path_col > 0) {
                new_energy_tab[i * new_width + path_col - 1] =
                        pixel_energy(new_image, i, path_col - 1);
            }
        }

        if (original != image)
            freePNM(original);
        free(energy_tab);
        free(min_path);

        original = new_image;
        energy_tab = new_energy_tab;
        width--;
    }

    free(energy_tab);

    return original;
}

static size_t*
min_energy_path(const double* energy_tab, size_t width, size_t height) {
    // Stores the energy of the min energy path ending at each pixel
    double* tab = malloc(width * height * sizeof(double));
    if (tab == NULL) {
        return NULL;
    }

    // Stores the optimal move from each pixel.
    // 0 = nothing, 1 = left, 2 = middle, 3 = right
    size_t* moves = calloc(width * height, sizeof(size_t));
    if (moves == NULL) {
        return NULL;
    }

    // Fill the first line (Case 1)
    for (size_t k = 0; k < width; k++)
        tab[k] = energy_tab[k];

    // Build the double array with the cost for each (i, j)
    for (size_t i = 1; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            double top_mid = tab[(i - 1) * width + j];

            if (j == 0) { // Case 2 (left border column)
                double top_right = tab[(i - 1) * width + (j + 1)];

                if (top_mid < top_right) {
                    moves[i * width + j] = 2;
                    tab[i * width + j] = top_mid;
                } else {
                    moves[i * width + j] = 3;
                    tab[i * width + j] = top_right;
                }
            } else if (j == width - 1) { // Case 3 (right border column)
                double top_left = tab[(i - 1) * width + (j - 1)];

                if (top_left < top_mid) {
                    moves[i * width + j] = 1;
                    tab[i * width + j] = top_left;
                } else {
                    moves[i * width + j] = 2;
                    tab[i * width + j] = top_mid;
                }
            } else { // Case 4 (Elsewhere)
                double top_left = tab[(i - 1) * width + (j - 1)];
                double top_right = tab[(i - 1) * width + (j + 1)];

                if (top_left < top_mid && top_left < top_right) {
                    moves[i * width + j] = 1;
                    tab[i * width + j] = top_left;
                } else if (top_mid < top_right && top_mid < top_left) {
                    moves[i * width + j] = 2;
                    tab[i * width + j] = top_mid;
                } else {
                    moves[i * width + j] = 3;
                    tab[i * width + j] = top_right;
                }
            }

            tab[i * width + j] += energy_tab[i * width + j];
        }
    }

    // Select the minimum path within the pixels in the last line.
    double min = tab[(height - 1) * width + 0];
    size_t opt_end_pixel = 0;
    for (size_t k = 1; k < width; k++) {
        double new = tab[(height - 1) * width + k];
        if (new < min) {
            min = new;
            opt_end_pixel = k;
        }
    }
    free(tab);

    // Build the path.
    size_t* path = malloc(height * sizeof(size_t));
    if (path == NULL) {
        return NULL;
    }

    size_t col = opt_end_pixel; // col
    for (size_t k = 0; k < height; k++) {
        size_t row = height - 1 - k; // (n-1) -> (0) = row
        path[row] = col;

        switch (moves[row * width + col]) {
            case 1:
                col--;
                break;
            case 3:
                col++;
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
        unsigned char top_value = 0;
        unsigned char left_value = 0;

        if(i > 0)
            top_value = pixel_value(image, i - 1, j, c);
        else
            top_value = pixel_value(image, i, j, c);

        if(j > 0)
            left_value = pixel_value(image, i, j - 1, c);
        else
            left_value = pixel_value(image, i, j, c);

        energy += fabs(((top_value - pixel_value(image, i + 1, j, c)) / 2.0));
        energy += fabs(((left_value - pixel_value(image, i, j + 1, c)) / 2.0));
    }

    return energy;
}

static unsigned char pixel_value(const PNMImage* image, size_t i, size_t j,
                                 size_t color) {
    if (i >= image->height) i = image->height - 1;
    if (j >= image->width) j = image->width - 1;

    PNMPixel* pixel = image->data + i * image->width + j;
    switch (color) {
        case 0:
            return pixel->red;
        case 1:
            return pixel->green;
        default:
            return pixel->blue;
    }
}