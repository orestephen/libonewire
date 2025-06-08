#ifndef __TEST_DEV_H
#define __TEST_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "onewire_dev.h"

#define TEST_DEVICE_BYTE_TIME 10

struct onewire_dev_struct* test_device_get_dev(void);
void test_device_set_data(uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
