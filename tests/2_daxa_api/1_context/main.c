#include <daxa-c/daxa.h>
#include <stdio.h>

void tests_simplest()
{
    daxa_ContextInfo ctx_info = {
        .enable_validation = 0,
        .validation_callback = NULL,
    };
    daxa_Context * daxa_ctx = daxa_create_context(&ctx_info);
    daxa_destroy_context(daxa_ctx);
}

void custom_validation(daxa_MsgSeverity severity, daxa_MsgType type, char const * msg)
{
    printf("%s\n", msg);
    // maybe call the callback provided by daxa!
    daxa_default_validation_callback(severity, type, msg);
}

void tests_custom_validation_callback()
{
    daxa_ContextInfo daxa_ctx_info = {
        .enable_validation = 1,
        .validation_callback = custom_validation,
    };
    daxa_Context * daxa_ctx = daxa_create_context(&daxa_ctx_info);
    daxa_destroy_context(daxa_ctx);
}

int main()
{
    tests_simplest();
    tests_custom_validation_callback();
}
