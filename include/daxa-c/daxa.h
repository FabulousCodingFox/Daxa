#ifndef DAXA_H
#define DAXA_H

/*

enum struct ([\w]+?)
\{
([\s\S]*?)
\};

->

typedef enum
{
$2
} daxa_$1;

*/

#include <daxa-c/types.h>
#include <daxa-c/core.h>
#include <daxa-c/gpu_resources.h>
// #include <daxa-c/pipeline.h>
// #include <daxa-c/semaphore.h>
// #include <daxa-c/swapchain.h>
// #include <daxa-c/command_list.h>
#include <daxa-c/device.h>
#include <daxa-c/context.h>
// #include <daxa-c/timeline_query.h>

#endif
