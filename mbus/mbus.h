//------------------------------------------------------------------------------
// Copyright (C) 2010, Raditex AB
// All rights reserved.
//
// rSCADA
// http://www.rSCADA.se
// info@rscada.se
//
//------------------------------------------------------------------------------

/**
 * @file   mbus.h
 *
 * @brief  Main include file for the Freescada libmbus library.
 *
 * Include this file to access the libmbus API:
\verbatim
#include <mbus/mbus.h>
\endverbatim
 *
 */

/*! \mainpage libmbus
 *
 * These pages contain automatically generated documentation for the libmbus
 * API. For examples on how to use the libmbus library, see the applications
 * in the bin directory in the source code distribution.
 *
 * For more information, see http://www.rscada.se/libmbus
 *
 */

#ifndef _MBUS_H_
#define _MBUS_H_

#include "mbus-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Supported handle types
//
#define MBUS_HANDLE_TYPE_TCP    0
#define MBUS_HANDLE_TYPE_SERIAL 1

//
// Resultcodes for mbus_recv_frame
//
#define MBUS_RECV_RESULT_OK        0
#define MBUS_RECV_RESULT_ERROR     -1
#define MBUS_RECV_RESULT_INVALID   -2
#define MBUS_RECV_RESULT_TIMEOUT   -3
#define MBUS_RECV_RESULT_RESET     -4


typedef struct _mbus_handle {
    int fd;
    char is_serial; /**< _handle type (non zero for serial) */
    int (*open) (struct _mbus_handle *handle);
    int (*close) (struct _mbus_handle *handle);
    int (*send) (struct _mbus_handle *handle, mbus_frame *frame);
    int (*recv) (struct _mbus_handle *handle, mbus_frame *frame);
    void (*data_free) (struct _mbus_handle *handle);
    void *auxdata;
} mbus_handle;

int mbus_init();
const char* mbus_get_current_version();

#ifdef __cplusplus
}
#endif

#endif /* _MBUS_H_ */
