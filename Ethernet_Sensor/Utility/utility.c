/*
 * utility.c
 *
 *  Created on: Jun 19, 2020
 *      Author: johnm
 */

#include "utility.h"
/**
 * Copies data from src to dst
 * Should be able to work on any data type if given the proper access function to extra data from src and dst
 */

size_t copy_buffer(void *dst, void *src, size_t n, size_t dst_data_size,
		size_t src_data_size, void (*access_function)(void *, void *)) {
	char *dst_ptr = (char *) dst;
	char *src_ptr = (char *) src;
	size_t bytes = 0;
	if (access_function == NULL) {
		for (size_t i = 0; i < n; i++) {
			dst_ptr[i] = src_ptr[i];
			bytes++;
		}
	} else {
		size_t src_index = 0;
		size_t dst_index = 0;
		for (size_t i = 0; i < n; i++) {
			access_function((void *) &dst[dst_index], (void *) &src[src_index]);
			dst_index += dst_data_size;
			src_index += src_data_size;
			bytes++;

		}
	}
	return bytes;
}
void display_init(void) {
	Crystalfontz128x128_Init();
	Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
}
void draw(tContext *sContext,void *data, size_t n, size_t data_size,
		void (*draw_function)(tContext *, size_t, size_t, void *, void *)) {
	GrContextInit(sContext, &g_sCrystalfontz128x128);
	GrContextFontSet(sContext, &g_sFontFixed6x8);

	tRectangle rectFullScreen = { 0, 0, GrContextDpyWidthGet(sContext) - 1,
	GrContextDpyHeightGet(sContext) - 1 };

	GrContextForegroundSet(sContext, ClrBlack);
	GrRectFill(sContext, &rectFullScreen);


	draw_data(sContext, data,n,data_size, draw_function);

}
void draw_grid(tContext *sContext) {
	GrContextForegroundSet(sContext, ClrMidnightBlue);

	for (uint16_t i = 0; i < 7; i++) {
		GrLineDrawH(sContext, 0, 128, 20 * i + 4);
		GrLineDrawV(sContext, 20 * i + 4, 0, 128);
	}

	GrContextForegroundSet(sContext, ClrRed);
	GrLineDrawH(sContext, 0, 128, 64);
	GrLineDrawV(sContext, 64, 0, 128);
}
void draw_text(tContext *sContext, void (*text_function)(tContext *)) {
	text_function(sContext);
}
void draw_data(tContext *sContext, void *data, size_t n, size_t data_size,
		void (*draw_function)(tContext *, size_t, size_t, void *, void *)) {
	GrContextForegroundSet(sContext, ClrYellow);

	if (draw_function == NULL) {

	} else {
		size_t index = 0;
		for (size_t i = 0; i < n - 1; i++) {
			draw_function(sContext, i, i + 1, (void *) &data[index],
					(void *) &data[index + data_size]);
			index += data_size;
		}
	}
}
