#pragma once

#include <Types/All.h>
#include <offsets/offsets.h>

#ifdef __cplusplus
extern "C" {
#endif

int kloader_load(void* payload, size_t payload_size);

#ifdef __cplusplus
}
#endif