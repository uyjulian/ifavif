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

int getBMPFromAVIF(const uint8_t *input_data, size_t file_size,
				   HANDLE* h_bitmap_info,
				   HANDLE* h_bitmap_data) {
	BITMAPINFOHEADER* bitmap_info_header = NULL;
	uint8_t *bitmap_data = NULL;

	int ret_result = -1;

	int width = 0;
	int height = 0;
	int bit_width, bit_length;
	avifRGBImage rgb;
	memset(&rgb, 0, sizeof(rgb));

	avifDecoder *decoder = avifDecoderCreate();
	avifResult result = avifDecoderSetIOMemory(decoder, input_data, file_size);
	if (result != AVIF_RESULT_OK)
	{
		goto cleanup;
	}

	result = avifDecoderParse(decoder);
	if (result != AVIF_RESULT_OK)
	{
		goto cleanup;
	}
	result = avifDecoderNextImage(decoder);
	if (result == AVIF_RESULT_OK) {
		width = decoder->image->width;
		height = decoder->image->height;
		bit_width = width * 4;
		bit_length = bit_width;
		avifRGBImageSetDefaults(&rgb, decoder->image);
		rgb.depth = 8;
		rgb.format = AVIF_RGB_FORMAT_BGRA;

		*h_bitmap_data = LocalAlloc(LMEM_MOVEABLE, sizeof(uint8_t) * bit_length * height);
		if (!*h_bitmap_data)
		{
			goto cleanup;
		}
		bitmap_data = (uint8_t*)LocalLock(*h_bitmap_data);
		if (!bitmap_data)
		{
			LocalFree(*h_bitmap_data);
			goto cleanup;
		}
		rgb.pixels = bitmap_data;
		rgb.rowBytes = sizeof(uint8_t) * bit_length;
		if (avifImageYUVToRGB(decoder->image, &rgb) != AVIF_RESULT_OK)
		{
			goto cleanup;
		}
		// Flip along the horizontal axis
		for (int j = 0; j < height / 2; j++)
		{
			DWORD *curbit_1 = (DWORD*)(bitmap_data + j * bit_length);
			DWORD *curbit_2 = (DWORD*)(bitmap_data + (height - (1 + j)) * bit_length);
			for (int i = 0; i < width; i++)
			{
				DWORD tmp = curbit_1[i];
				curbit_1[i] = curbit_2[i];
				curbit_2[i] = tmp;
			}
		}
#if 0
		for (int j = 0; j < height; j++) {
			uint8_t *curbit = bitmap_data + (height - (1 + j)) * bit_length;
			for (int i = 0; i < width; i++) {
				curbit[0] =
					rgb.pixels[i + (j * rgb.rowBytes) + 2]; // B
				curbit[1] =
					rgb.pixels[i + (j * rgb.rowBytes) + 1]; // G
				curbit[2] =
					rgb.pixels[i + (j * rgb.rowBytes) + 0]; // R
				curbit[3] =
					rgb.pixels[i + (j * rgb.rowBytes) + 3]; // A
				curbit += 4;
			}
		}
#endif
	} else {
		goto cleanup;
	}

	*h_bitmap_info = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPINFOHEADER));
	if (NULL == *h_bitmap_info)
	{
		goto cleanup;
	}
	bitmap_info_header = (BITMAPINFOHEADER*)LocalLock(*h_bitmap_info);
	if (NULL == bitmap_info_header)
	{
		LocalFree(*h_bitmap_info);
		goto cleanup;
	}

	bitmap_info_header->biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_header->biWidth = width;
	bitmap_info_header->biHeight = height;
	bitmap_info_header->biPlanes = 1;
	bitmap_info_header->biBitCount = 32;
	bitmap_info_header->biCompression = BI_RGB;
	bitmap_info_header->biSizeImage = sizeof(uint8_t) * bit_length * height;

	LocalUnlock(*h_bitmap_data);
	LocalUnlock(*h_bitmap_info);
	ret_result = 0;
cleanup:
	if (ret_result && bitmap_data)
	{
		LocalUnlock(*h_bitmap_data);
		LocalFree(*h_bitmap_data);
		*h_bitmap_data = NULL;
	}
	if (ret_result && bitmap_info_header)
	{
		LocalUnlock(*h_bitmap_info);
		LocalFree(*h_bitmap_info);
		*h_bitmap_info = NULL;
	}
	avifDecoderDestroy(decoder);
	return ret_result;
}

BOOL IsSupportedEx(const char *data) {
	if(strcmp(data + 4, "ftypavif") == 0)
	{
		return TRUE;
	}

	return FALSE;
}

int GetPictureInfoEx(size_t data_size, const char *data, SusiePictureInfo *picture_info)
{
	int width = 0;
	int height = 0;
	int ret_result = SPI_MEMORY_ERROR;

	avifDecoder *decoder = avifDecoderCreate();
	avifResult result = avifDecoderSetIOMemory(decoder, (uint8_t *)data, data_size);
	if (result != AVIF_RESULT_OK)
	{
		goto cleanup;
	}
	result = avifDecoderParse(decoder);
	if (result != AVIF_RESULT_OK)
	{
		goto cleanup;
	}
	width = decoder->image->width;
	height = decoder->image->height;

	picture_info->left = 0;
	picture_info->top = 0;
	picture_info->width = width;
	picture_info->height = height;
	picture_info->x_density = 0;
	picture_info->y_density = 0;
	picture_info->colorDepth = 32;
	picture_info->hInfo = NULL;

	ret_result = SPI_ALL_RIGHT;
cleanup:
	avifDecoderDestroy(decoder);
	return ret_result;
}

int GetPictureEx(size_t data_size, HANDLE *bitmap_info, HANDLE *bitmap_data, SPI_PROGRESS progress_callback, intptr_t user_data, const char *data) {
	if (progress_callback != NULL)
		if (progress_callback(1, 1, user_data))
			return SPI_ABORT;

	if (getBMPFromAVIF((const uint8_t*)data, data_size, bitmap_info, bitmap_data))
		return SPI_MEMORY_ERROR;

	if (progress_callback != NULL)
		if (progress_callback(1, 1, user_data))
			return SPI_ABORT;

	return SPI_ALL_RIGHT;
}
