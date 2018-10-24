#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "mandelbrot_set.h"

#define CHUNK_LEN 8

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct data {
	int* y_index;
	int x_resolution;
	int y_resolution;
	int max_iter;
	double view_x0;
	double view_y1;
	double x_stepsize;
	double y_stepsize;
	int palette_shift;
	unsigned char ***image;
};

void* work(void* args) {
	struct data d = *(struct data*) args;
	int* y_index = d.y_index;
	int y_resolution = d.y_resolution;
	int x_resolution = d.x_resolution;
	int max_iter = d.max_iter;
	double view_x0 = d.view_x0;
	double view_y1 = d.view_y1;
	double x_stepsize = d.x_stepsize;
	double y_stepsize = d.y_stepsize;
	int palette_shift = d.palette_shift;
	unsigned char (*image)[x_resolution][3] = (unsigned char(*)[x_resolution][3])d.image;

	double y;
	double x;

	complex double Z;
	complex double C;

	int k;

    int y_start, y_end;
	
    for (;;) {
        pthread_mutex_lock(&mutex);
        if (y_resolution - *y_index < 1) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        y_start = *y_index;
        *y_index += CHUNK_LEN;
        pthread_mutex_unlock(&mutex);
        
        y_end = y_resolution > y_start + CHUNK_LEN ? y_start + CHUNK_LEN : y_resolution;
        for (int i = y_start; i < y_end; i++)
        {
            for (int j = 0; j < x_resolution; j++)
            {
                y = view_y1 - i * y_stepsize;
                x = view_x0 + j * x_stepsize;

                Z = 0 + 0 * I;
                C = x + y * I;

                k = 0;

                do
                {
                    Z = Z * Z + C;
                    k++;
                } while (cabs(Z) < 2 && k < max_iter);

                if (k == max_iter)
                {
                    memcpy(image[i][j], "\0\0\0", 3);
                }
                else
                {
                    int index = (k + palette_shift)
                                % (sizeof(colors) / sizeof(colors[0]));
                    memcpy(image[i][j], colors[index], 3);
                }
            }
        }
    }

	return NULL;
}

void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
			                double view_x0, double view_x1, double view_y0, double view_y1,
						                double x_stepsize, double y_stepsize,
									                int palette_shift, unsigned char (*image)[x_resolution][3],
																	 int num_threads) {
	pthread_t* threads = (pthread_t*) malloc(num_threads * sizeof(*threads));
	struct data* d = (struct data*) malloc(num_threads * sizeof(*d));
    int y_index = 0;
	for (int t = 0; t < num_threads; t++) {
		d[t].y_index = &y_index;
		d[t].x_resolution = x_resolution;
		d[t].y_resolution = y_resolution;
		d[t].max_iter = max_iter;
		d[t].view_x0 = view_x0;
		d[t].view_y1 = view_y1;
		d[t].x_stepsize = x_stepsize;
		d[t].y_stepsize = y_stepsize;
		d[t].palette_shift = palette_shift;
		d[t].image = (unsigned char***)image;
		pthread_create(&threads[t], NULL, work, d+t);
	}

	for (int t = 0; t < num_threads; t++) {
		pthread_join(threads[t], NULL);
	}

	free(threads);
	free(d);
}
