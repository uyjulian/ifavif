/////////////////////////////////////////////
//                                         //
//    Copyright (C) 2019-2019 Julian Uy    //
//  https://sites.google.com/site/awertyb  //
//                                         //
//   See details of license at "LICENSE"   //
//                                         //
/////////////////////////////////////////////

#pragma once

#include "spi00in.h"

extern const char *plugin_info[4];
extern const int header_size;

BOOL IsSupportedEx(const char *data);
int GetPictureInfoEx(size_t data_size, const char *data, SusiePictureInfo *picture_info);
int GetPictureEx(size_t data_size, HANDLE *bitmap_info, HANDLE *bitmap_data, SPI_PROGRESS progress_callback, intptr_t user_data, const char *data);
