/*

*/
#pragma once

#include "ioapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
    Fills the zlib_filefunc_def structure with functions that
    will operate on QIODevice*
*/
void qiodevice_fill_fopen_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def, voidpf stream ));


#ifdef __cplusplus
}
#endif


