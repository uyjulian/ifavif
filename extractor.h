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

BOOL IsSupportedEx(char *filename, char *data);
int GetPictureInfoEx(long data_size, char *data,
                     struct PictureInfo *picture_info);
int GetPictureEx(long data_size, HANDLE *bitmap_info, HANDLE *bitmap_data,
                 SPI_PROGRESS progress_callback, long user_data, char *data);
