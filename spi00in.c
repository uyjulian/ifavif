/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2019-2019 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#include "spi00in.h"
#include "extractor.h"
#include <string.h>

EXPORT(int)
GetPluginInfo(int info_number, LPSTR buffer, int buffer_length) {
	if (info_number < 0 ||
	    info_number >= (sizeof(plugin_info) / sizeof(const char *)))
		return 0;

	strncpy(buffer, plugin_info[info_number], buffer_length);

	return strlen(buffer);
}

EXPORT(int)
IsSupported(LPSTR filename, DWORD variant) {
	int ret = 0;
	char *data;
	char *buffer = NULL;
	DWORD read_bytes;

	if ((variant & 0xFFFF0000) == 0) {
		buffer = malloc(header_size);
		if (!buffer)
			return 0;
		if (!ReadFile((HANDLE)variant, buffer, header_size, &read_bytes,
		              NULL)) {
			free(buffer);
			return 0;
		}
		data = buffer;
	} else {
		data = (char *)variant;
	}

	ret = IsSupportedEx(filename, data);

	if (buffer)
		free(buffer);

	return ret;
}

EXPORT(int)
GetPictureInfo(LPSTR buffer, long buffer_length, unsigned int flag,
               struct PictureInfo *picture_info) {
	char *data;
	char *filename;
	long data_size;
	HANDLE file_handle;
	DWORD read_bytes;

	if ((flag & 7) == 0) {
		file_handle = CreateFileA(buffer, GENERIC_READ, FILE_SHARE_READ, NULL,
		                          OPEN_EXISTING, 0, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
			return SPI_FILE_READ_ERROR;
		data_size = GetFileSize(file_handle, NULL) - buffer_length;
		if (data_size < header_size) {
			CloseHandle(file_handle);
			return SPI_NOT_SUPPORT;
		}
		if (SetFilePointer(file_handle, buffer_length, NULL, FILE_BEGIN) !=
		    (DWORD)buffer_length) {
			CloseHandle(file_handle);
			return SPI_FILE_READ_ERROR;
		}
		data = (char *)LocalAlloc(LMEM_FIXED, data_size);
		if (data == NULL) {
			CloseHandle(file_handle);
			return SPI_NO_MEMORY;
		}
		if (!ReadFile(file_handle, data, data_size, &read_bytes, NULL)) {
			CloseHandle(file_handle);
			LocalFree(data);
			return SPI_FILE_READ_ERROR;
		}
		CloseHandle(file_handle);
		if (read_bytes != (DWORD)data_size) {
			LocalFree(data);
			return SPI_FILE_READ_ERROR;
		}
		filename = buffer;
	} else {
		data = buffer;
		data_size = buffer_length;
		filename = NULL;
	}

	if (!IsSupportedEx(filename, data))
		return SPI_NOT_SUPPORT;
	else
		return GetPictureInfoEx(data_size, data, picture_info);
}

EXPORT(int)
GetPicture(LPSTR buffer, long buffer_length, unsigned int flag,
           HANDLE *bitmap_info, HANDLE *bitmap_data,
           SPI_PROGRESS progress_callback, long user_data) {
	int ret = SPI_OTHER_ERROR;
	char *data;
	char *filename;
	long data_size;
	HANDLE file_handle;
	DWORD read_bytes;

	if ((flag & 7) == 0) {
		file_handle = CreateFileA(buffer, GENERIC_READ, FILE_SHARE_READ, NULL,
		                          OPEN_EXISTING, 0, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
			return SPI_FILE_READ_ERROR;
		data_size = GetFileSize(file_handle, NULL) - buffer_length;
		if (data_size < header_size) {
			CloseHandle(file_handle);
			return SPI_NOT_SUPPORT;
		}
		if (SetFilePointer(file_handle, buffer_length, NULL, FILE_BEGIN) !=
		    (DWORD)buffer_length) {
			CloseHandle(file_handle);
			return SPI_FILE_READ_ERROR;
		}
		data = (char *)LocalAlloc(LMEM_FIXED, data_size);
		if (data == NULL) {
			CloseHandle(file_handle);
			return SPI_NO_MEMORY;
		}
		if (!ReadFile(file_handle, data, data_size, &read_bytes, NULL)) {
			CloseHandle(file_handle);
			LocalFree(data);
			return SPI_FILE_READ_ERROR;
		}
		CloseHandle(file_handle);
		if (read_bytes != (DWORD)data_size) {
			LocalFree(data);
			return SPI_FILE_READ_ERROR;
		}
		filename = buffer;
	} else {
		data = buffer;
		data_size = buffer_length;
		filename = NULL;
	}

	if (!IsSupportedEx(filename, data))
		ret = SPI_NOT_SUPPORT;
	else
		ret = GetPictureEx(data_size, bitmap_info, bitmap_data,
		                   progress_callback, user_data, data);

	if ((flag & 7) == 0)
		LocalFree(data);

	return ret;
}

EXPORT(int)
GetPreview(LPSTR buffer, long buffer_length, unsigned int flag,
           HANDLE *bitmap_info, HANDLE *bitmap_data,
           SPI_PROGRESS lpPrgressCallback, long user_data) {
	return SPI_NO_FUNCTION;
}
