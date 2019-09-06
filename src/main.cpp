#include <node_api.h>
#include <stdio.h>
#include "CMbus.h"

napi_value Init(napi_env env, napi_value exports) {
  CMbus::Init(&env, &exports);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)