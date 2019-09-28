/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2019-2019 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#pragma once

#include <windows.h>
#ifdef __cplusplus
#define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall
#else
#define EXPORT(hr) __declspec(dllexport) hr __stdcall
#endif

// Image information structure
#pragma pack(push)
#pragma pack(1) // Make structure member boundary 1 byte
typedef struct PictureInfo {
	long left, top;   // Position to start decompressing the image
	long width;       // Image width (in pixels)
	long height;      // Image height (in pixels)
	WORD x_density;   // Horizontal density of pixels
	WORD y_density;   // Vertical density of pixels
	short colorDepth; // Number of bits per pixel
	HLOCAL hInfo;     // Text information in the image
} PictureInfo;
#pragma pack(pop)

// Error codes
#define SPI_NO_FUNCTION -1     // Unimplemented feature
#define SPI_ALL_RIGHT 0        // Success
#define SPI_ABORT 1            // Aborted due to callback returning non-zero
#define SPI_NOT_SUPPORT 2      // Unknown format
#define SPI_OUT_OF_ORDER 3     // Corrupted data
#define SPI_NO_MEMORY 4        // Unable to allocate memory
#define SPI_MEMORY_ERROR 5     // Memory error
#define SPI_FILE_READ_ERROR 6  // File read error
#define SPI_WINDOW_ERROR 7     // Window can not be opened (private error code)
#define SPI_OTHER_ERROR 8      // Internal error
#define SPI_FILE_WRITE_ERROR 9 // Write error (private error code)
#define SPI_END_OF_FILE 10     // End of file (private error code)

// Function prototype declarations for '00IN' (Image plugin)
typedef int(CALLBACK *SPI_PROGRESS)(int, int, long);
EXPORT(int) GetPluginInfo(int info_number, LPSTR buffer, int buffer_length);
EXPORT(int) IsSupported(LPSTR filename, DWORD variant);
EXPORT(int)
GetPictureInfo(LPSTR buffer, long buffer_length, unsigned int flag,
               struct PictureInfo *picture_info);
EXPORT(int)
GetPicture(LPSTR buffer, long buffer_length, unsigned int flag,
           HANDLE *bitmap_info, HANDLE *bitmap_data,
           SPI_PROGRESS progress_callback, long user_data);
EXPORT(int)
GetPreview(LPSTR buffer, long buffer_length, unsigned int flag,
           HANDLE *bitmap_info, HANDLE *bitmap_data,
           SPI_PROGRESS lpPrgressCallback, long user_data);
