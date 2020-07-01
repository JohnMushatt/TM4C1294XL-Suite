/*
 * utility.h
 *
 *  Created on: Jun 19, 2020
 *      Author: johnm
 */

#ifndef UTILITY_UTILITY_H_
#define UTILITY_UTILITY_H_
#include "kernel_include.h"
size_t copy_buffer(void *dst, void *src, size_t n, size_t dst_data_size,
		size_t src_data_size, void (*access_function)(void *, void *));
void display_init(void);
void draw_data(tContext *sContext,void *data, size_t n,size_t data_size, void (*draw_function)(tContext *, size_t, size_t, void *, void *));
void draw_grid(tContext *sContext);
void draw_text(tContext *sContext, void (*text_function)(tContext *));
void draw(tContext *sContext,void *data, size_t n,size_t data_size, void (*draw_function)(tContext *, size_t, size_t, void *, void *));
#endif /* UTILITY_UTILITY_H_ */
