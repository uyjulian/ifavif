/* SPDX-License-Identifier: MIT */
/* Copyright (c) ifavif developers */

#include "spi00in.h"
#include "extractor.h"
#include <string.h>
#include <stdio.h>
#include <wchar.h>

EXPORT(int) GetPluginInfo(int info_number, LPSTR buffer, int buffer_length)
{
	if (info_number < 0 || info_number >= (sizeof(plugin_info) / sizeof(const char *)))
	{
		return 0;
	}

	int ret = snprintf(buffer, buffer_length, "%s", plugin_info[info_number]);
	if (ret > buffer_length)
	{
		ret = buffer_length;
	}

	return ret;
}

EXPORT(int) GetPluginInfoW(int info_number, LPWSTR buffer, int buffer_length)
{
	if (info_number < 0 || info_number >= (sizeof(plugin_info) / sizeof(const char *)))
	{
		return 0;
	}

	int ret = _snwprintf(buffer, buffer_length, L"%hs", plugin_info[info_number]);
	if (ret > buffer_length)
	{
		ret = buffer_length;
	}

	return ret;
}

EXPORT(int) IsSupported(LPCSTR filename, DWORD_PTR variant)
{
	int ret;
	char *data;
	char *buffer;
	DWORD read_bytes;

	ret = 0;
	buffer = NULL;
	if ((variant & (~(DWORD_PTR)0xFFFF)) == 0)
	{
		buffer = malloc(header_size);
		if (buffer == NULL)
		{
			return ret;
		}
		if (!ReadFile((HANDLE)variant, buffer, header_size, &read_bytes, NULL))
		{
			free(buffer);
			return ret;
		}
		ret = IsSupported(NULL, (DWORD_PTR)buffer);
		free(buffer);
		return ret;
	}
	else
	{
		data = (char *)variant;
	}

	ret = IsSupportedEx(data);

	return ret;
}

EXPORT(int) IsSupportedW(LPCWSTR filename, DWORD_PTR variant)
{
	return IsSupported(NULL, variant);
}

static int GetBufferFromHandle(HANDLE file_handle, size_t buffer_length, const char** out_data, size_t* out_data_size)
{
	char *data;
	size_t data_size;
	DWORD read_bytes;
	data_size = GetFileSize(file_handle, NULL) - buffer_length;
	if (data_size < header_size)
	{
		return SPI_NOT_SUPPORT;
	}
	if (SetFilePointer(file_handle, buffer_length, NULL, FILE_BEGIN) != (DWORD)buffer_length)
	{
		return SPI_FILE_READ_ERROR;
	}
	data = (char *)LocalAlloc(LMEM_FIXED, data_size);
	if (data == NULL)
	{
		return SPI_NO_MEMORY;
	}
	if (!ReadFile(file_handle, data, data_size, &read_bytes, NULL))
	{
		LocalFree(data);
		return SPI_FILE_READ_ERROR;
	}
	if (read_bytes != (DWORD)data_size)
	{
		LocalFree(data);
		return SPI_FILE_READ_ERROR;
	}
	if (out_data != NULL)
	{
		*out_data = data;
	}
	if (out_data_size != NULL)
	{
		*out_data_size = data_size;
	}
	return SPI_ALL_RIGHT;
}

EXPORT(int) GetPictureInfo(LPCSTR buffer, size_t buffer_length, unsigned int flag, SusiePictureInfo *picture_info)
{
	const char *data;
	size_t data_size;
	HANDLE file_handle;
	int ret;

	if ((flag & 7) == 0)
	{
		file_handle = CreateFileA(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
		{
			return SPI_FILE_READ_ERROR;
		}
		ret = GetBufferFromHandle(file_handle, buffer_length, &data, &data_size);
		CloseHandle(file_handle);
		if (ret != SPI_ALL_RIGHT)
		{
			return ret;
		}
		ret = GetPictureInfo((LPCSTR)data, data_size, flag | SPI_INPUT_MEMORY, picture_info);
		LocalFree((void *)data);
		return ret;
	}
	else
	{
		data = buffer;
		data_size = buffer_length;
	}

	if (!IsSupportedEx(data))
	{
		ret = SPI_NOT_SUPPORT;
	}
	else
	{
		ret = GetPictureInfoEx(data_size, data, picture_info);
	}

	return ret;
}

EXPORT(int) GetPictureInfoW(LPCWSTR buffer, size_t buffer_length, unsigned int flag, SusiePictureInfo *picture_info)
{
	const char *data;
	size_t data_size;
	HANDLE file_handle;
	int ret;

	if ((flag & 7) == 0)
	{
		file_handle = CreateFileW(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
		{
			return SPI_FILE_READ_ERROR;
		}
		ret = GetBufferFromHandle(file_handle, buffer_length, &data, &data_size);
		CloseHandle(file_handle);
		if (ret != SPI_ALL_RIGHT)
		{
			return ret;
		}
		ret = GetPictureInfo((LPCSTR)data, data_size, flag | SPI_INPUT_MEMORY, picture_info);
		LocalFree((void *)data);
		return ret;
	}
	ret = GetPictureInfo((LPCSTR)buffer, buffer_length, flag, picture_info);
	return ret;
}

EXPORT(int) GetPicture(LPCSTR buffer, size_t buffer_length, unsigned int flag, HANDLE *bitmap_info, HANDLE *bitmap_data, SPI_PROGRESS progress_callback, intptr_t user_data)
{
	int ret;
	const char *data;
	size_t data_size;
	HANDLE file_handle;

	ret = SPI_OTHER_ERROR;

	if ((flag & 7) == 0)
	{
		file_handle = CreateFileA(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
		{
			return SPI_FILE_READ_ERROR;
		}
		ret = GetBufferFromHandle(file_handle, buffer_length, &data, &data_size);
		CloseHandle(file_handle);
		if (ret != SPI_ALL_RIGHT)
		{
			return ret;
		}
		ret = GetPicture((LPCSTR)data, data_size, flag | SPI_INPUT_MEMORY, bitmap_info, bitmap_data, progress_callback, user_data);
		LocalFree((void *)data);
		return ret;
	}
	else
	{
		data = buffer;
		data_size = buffer_length;
	}

	if (!IsSupportedEx(data))
	{
		ret = SPI_NOT_SUPPORT;
	}
	else
	{
		ret = GetPictureEx(data_size, bitmap_info, bitmap_data, progress_callback, user_data, data);
	}

	return ret;
}

EXPORT(int) GetPictureW(LPCWSTR buffer, size_t buffer_length, unsigned int flag, HANDLE *bitmap_info, HANDLE *bitmap_data, SPI_PROGRESS progress_callback, intptr_t user_data)
{
	int ret;
	const char *data;
	size_t data_size;
	HANDLE file_handle;

	ret = SPI_OTHER_ERROR;

	if ((flag & 7) == 0)
	{
		file_handle = CreateFileW(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
		{
			return SPI_FILE_READ_ERROR;
		}
		ret = GetBufferFromHandle(file_handle, buffer_length, &data, &data_size);
		CloseHandle(file_handle);
		if (ret != SPI_ALL_RIGHT)
		{
			return ret;
		}
		ret = GetPicture((LPCSTR)data, data_size, flag | SPI_INPUT_MEMORY, bitmap_info, bitmap_data, progress_callback, user_data);
		LocalFree((void *)data);
		return ret;
	}
	ret = GetPicture((LPCSTR)buffer, buffer_length, flag, bitmap_info, bitmap_data, progress_callback, user_data);
	return ret;
}

EXPORT(int) GetPreview(LPCSTR buffer, size_t buffer_length, unsigned int flag, HANDLE *bitmap_info, HANDLE *bitmap_data, SPI_PROGRESS lpPrgressCallback, intptr_t user_data)
{
	return GetPicture(buffer, buffer_length, flag, bitmap_info, bitmap_data, lpPrgressCallback, user_data);
}

EXPORT(int) GetPreviewW(LPCWSTR buffer, size_t buffer_length, unsigned int flag, HANDLE *bitmap_info, HANDLE *bitmap_data, SPI_PROGRESS lpPrgressCallback, intptr_t user_data)
{
	return GetPictureW(buffer, buffer_length, flag, bitmap_info, bitmap_data, lpPrgressCallback, user_data);
}
