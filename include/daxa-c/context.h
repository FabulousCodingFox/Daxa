#ifndef DAXA_CONTEXT_H
#define DAXA_CONTEXT_H

#include <daxa-c/core.h>
#include <daxa-c/device.h>

typedef struct
{
    daxa_bool enable_validation;
    void (*validation_callback)(daxa_MsgSeverity, daxa_MsgType, char const *);
} daxa_ContextInfo;

typedef void daxa_Context;

DAXA_EXTERN_C void daxa_default_validation_callback(daxa_MsgSeverity severity, daxa_MsgType type, char const * msg);
DAXA_EXTERN_C daxa_Context * daxa_create_context(daxa_ContextInfo const * info);
DAXA_EXTERN_C void daxa_destroy_context(daxa_Context * self);
DAXA_EXTERN_C daxa_Device * daxa_context_create_device(daxa_Context * self, daxa_DeviceInfo const * device_info);

#endif
