/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2019-2019 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "extractor.h"
#include <avif/avif.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char *plugin_info[4] = {
    "00IN",
    "AVIF Plugin for Susie Image Viewer",
    "*.avif",
    "AVIF file (*.avif)",
};

const int header_size = 64;

int getBMPFromAVIF(uint8_t *input_data, long file_size,
                   BITMAPFILEHEADER *bitmap_file_header,
                   BITMAPINFOHEADER *bitmap_info_header, uint8_t **data) {

	uint8_t *bitmap_data = NULL;

	int width = 0;
	int height = 0;
	int bit_width, bit_length;
	avifROData raw;
	raw.data = input_data;
	raw.size = file_size;

	avifImage *image = avifImageCreateEmpty();
	avifDecoder *decoder = avifDecoderCreate();
	avifResult decodeResult = avifDecoderRead(decoder, image, &raw);
	if (decodeResult == AVIF_RESULT_OK) {
		avifDecoderDestroy(decoder);
		width = image->width;
		height = image->height;
		bit_width = width * 4;
		bit_length = bit_width;
		bitmap_data = (uint8_t *)malloc(sizeof(uint8_t) * bit_length * height);
		if (!bitmap_data)
			return -1;
		memset(bitmap_data, 0, bit_length * height);
		avifImageYUVToRGB(image);
		for (int j = 0; j < height; j++) {
			uint8_t *curbit = bitmap_data + j * bit_length;
			for (int i = 0; i < width; i++) {
				curbit[0] =
				    image->rgbPlanes[2][i + (j * image->rgbRowBytes[2])]; // B
				curbit[1] =
				    image->rgbPlanes[1][i + (j * image->rgbRowBytes[1])]; // G
				curbit[2] =
				    image->rgbPlanes[0][i + (j * image->rgbRowBytes[0])]; // R
				if (image->alphaPlane)
					curbit[3] =
					    image->alphaPlane[i + (j * image->alphaRowBytes)]; // A
				else
					curbit[3] = 0xFF;
			}
		}
		avifImageDestroy(image);
	} else {
		avifDecoderDestroy(decoder);
		avifImageDestroy(image);
		return -1;
	}

	*data = bitmap_data;

	memset(bitmap_file_header, 0, sizeof(BITMAPFILEHEADER));
	memset(bitmap_info_header, 0, sizeof(BITMAPINFOHEADER));

	bitmap_file_header->bfType = 'M' * 256 + 'B';
	bitmap_file_header->bfSize = sizeof(BITMAPFILEHEADER) +
	                             sizeof(BITMAPINFOHEADER) +
	                             sizeof(uint8_t) * bit_length * height;
	bitmap_file_header->bfOffBits =
	    sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bitmap_file_header->bfReserved1 = 0;
	bitmap_file_header->bfReserved2 = 0;

	bitmap_info_header->biSize = 40;
	bitmap_info_header->biWidth = width;
	bitmap_info_header->biHeight = height;
	bitmap_info_header->biPlanes = 1;
	bitmap_info_header->biBitCount = 32;
	bitmap_info_header->biCompression = 0;
	bitmap_info_header->biSizeImage = bitmap_file_header->bfSize;
	bitmap_info_header->biXPelsPerMeter = bitmap_info_header->biYPelsPerMeter =
	    0;
	bitmap_info_header->biClrUsed = 0;
	bitmap_info_header->biClrImportant = 0;
	return 0;
}

BOOL IsSupportedEx(char *filename, char *data) {
	const char header[] = {0x00, 0x00, 0x00, 0x20, 'f', 't',
	                       'y',  'p',  'a',  'v',  'i', 'f'};
	for (int i = 0; i < sizeof(header); i++) {
		if (data[i] != header[i])
			return FALSE;
	}
	return TRUE;
}

int GetPictureInfoEx(long data_size, char *data,
                     struct PictureInfo *picture_info) {
	int width = 0;
	int height = 0;
	avifROData raw;
	raw.data = (uint8_t *)data;
	raw.size = data_size;

	avifImage *image = avifImageCreateEmpty();
	avifDecoder *decoder = avifDecoderCreate();
	avifResult decodeResult = avifDecoderRead(decoder, image, &raw);
	if (decodeResult == AVIF_RESULT_OK) {
		avifDecoderDestroy(decoder);
		width = image->width;
		height = image->height;
		avifImageDestroy(image);
	} else {
		avifDecoderDestroy(decoder);
		avifImageDestroy(image);
		return SPI_MEMORY_ERROR;
	}

	picture_info->left = 0;
	picture_info->top = 0;
	picture_info->width = width;
	picture_info->height = height;
	picture_info->x_density = 0;
	picture_info->y_density = 0;
	picture_info->colorDepth = 32;
	picture_info->hInfo = NULL;

	return SPI_ALL_RIGHT;
}

int GetPictureEx(long data_size, HANDLE *bitmap_info, HANDLE *bitmap_data,
                 SPI_PROGRESS progress_callback, long user_data, char *data) {
	uint8_t *data_u8;
	BITMAPINFOHEADER bitmap_info_header;
	BITMAPFILEHEADER bitmap_file_header;
	BITMAPINFO *bitmap_info_locked;
	unsigned char *bitmap_data_locked;

	if (progress_callback != NULL)
		if (progress_callback(1, 1, user_data))
			return SPI_ABORT;

	if (!getBMPFromAVIF((uint8_t *)data, data_size, &bitmap_file_header,
	                    &bitmap_info_header, &data_u8))
		return SPI_MEMORY_ERROR;
	*bitmap_info = LocalAlloc(LMEM_MOVEABLE, sizeof(BITMAPINFOHEADER));
	*bitmap_data = LocalAlloc(LMEM_MOVEABLE, bitmap_file_header.bfSize -
	                                             bitmap_file_header.bfOffBits);
	if (*bitmap_info == NULL || *bitmap_data == NULL) {
		if (*bitmap_info != NULL)
			LocalFree(*bitmap_info);
		if (*bitmap_data != NULL)
			LocalFree(*bitmap_data);
		return SPI_NO_MEMORY;
	}
	bitmap_info_locked = (BITMAPINFO *)LocalLock(*bitmap_info);
	bitmap_data_locked = (unsigned char *)LocalLock(*bitmap_data);
	if (bitmap_info_locked == NULL || bitmap_data_locked == NULL) {
		LocalFree(*bitmap_info);
		LocalFree(*bitmap_data);
		return SPI_MEMORY_ERROR;
	}
	bitmap_info_locked->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_locked->bmiHeader.biWidth = bitmap_info_header.biWidth;
	bitmap_info_locked->bmiHeader.biHeight = bitmap_info_header.biHeight;
	bitmap_info_locked->bmiHeader.biPlanes = 1;
	bitmap_info_locked->bmiHeader.biBitCount = 32;
	bitmap_info_locked->bmiHeader.biCompression = BI_RGB;
	bitmap_info_locked->bmiHeader.biSizeImage = 0;
	bitmap_info_locked->bmiHeader.biXPelsPerMeter = 0;
	bitmap_info_locked->bmiHeader.biYPelsPerMeter = 0;
	bitmap_info_locked->bmiHeader.biClrUsed = 0;
	bitmap_info_locked->bmiHeader.biClrImportant = 0;
	memcpy(bitmap_data_locked, data_u8,
	       bitmap_file_header.bfSize - bitmap_file_header.bfOffBits);

	LocalUnlock(*bitmap_info);
	LocalUnlock(*bitmap_data);
	free(data_u8);

	if (progress_callback != NULL)
		if (progress_callback(1, 1, user_data))
			return SPI_ABORT;

	return SPI_ALL_RIGHT;
}
