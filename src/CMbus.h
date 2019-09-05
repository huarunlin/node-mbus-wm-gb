#ifndef _CMBUS_H_
#define _CMBUS_H_

#include <node_api.h>
#include "mbus.h"

#define CMBUS_SERIAL_NAME_MAX_LEN               256

enum {
    CMBUS_FLAG_SERIAL_DEVICE = 0,
    CMBUS_FLAG_SERIAL_BAUDRATE,
    CMBUS_FLAG_SERIAL_DATABITS,
    CMBUS_FLAG_SERIAL_STOPBITS,
    CMBUS_FLAG_SERIAL_PARITY,

    CMBUS_FLAG_ADDR,
    CMBUS_FLAG_METERTYPE,
};

class CMbus 
{
public:
    CMbus();
    virtual ~CMbus();

    static void CMbus::Init(napi_env *env, napi_value *exports);

    static napi_value structure(napi_env env, napi_callback_info info);
    static void destructor(napi_env env, void* nativeObject, void* finalize_hint);
    
    static napi_value getInt32(napi_env env, napi_callback_info info);
    static napi_value setInt32(napi_env env, napi_callback_info info);
    static napi_value getString(napi_env env, napi_callback_info info);
    static napi_value setString(napi_env env, napi_callback_info info);
    static napi_value getBuffer(napi_env env, napi_callback_info info);
    static napi_value setBuffer(napi_env env, napi_callback_info info);

    static napi_value connect(napi_env env, napi_callback_info info);
    static napi_value disconnect(napi_env env, napi_callback_info info);
    static napi_value isconnect(napi_env env, napi_callback_info info);
    static napi_value recv(napi_env env, napi_callback_info info);
    static napi_value send(napi_env env, napi_callback_info info);
private:
    static bool  getParm(napi_env &env, napi_callback_info &info, CMbus **obj, napi_value *args, size_t *argc, int *flag);

    static napi_ref   _constructor;
    
    napi_env    _env;
    napi_ref    _wrapper;
    int32_t     _databits;
    int32_t     _stopbits;
    char        _parity;
    mbus_handle _mbus;

    mbus_frame  _tx;
    mbus_frame  _rx;
};

#endif