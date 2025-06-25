#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "onewire_dev.h"

#include <stdbool.h>
#include <stdint.h>

#define ONEWIRE_ERROR_NONE 0
#define ONEWIRE_ERROR_DEV -1
#define ONEWIRE_ERROR_VAL -2
#define ONEWIRE_ERROR_STATE -3
#define ONEWIRE_ERROR_BUSY -4
#define ONEWIRE_ERROR_TIMEOUT -5

#define ONEWIRE_STATE_INIT 0
#define ONEWIRE_STATE_READY 1
#define ONEWIRE_STATE_TRANSCEIVE 2
#define ONEWIRE_STATE_ERROR 3

#define ONEWIRE_TIMEOUT_NOWAIT 0
#define ONEWIRE_TIMEOUT_INFINITE -1

struct onewire_buffer_struct {
  uint8_t* data;
  uint32_t size;
  uint32_t index;
};

struct onewire_struct {
  struct onewire_dev_struct* dev;
  struct onewire_buffer_struct buffer;
  uint8_t state;
  uint32_t (*tick)(void);
};

int32_t onewire_init(struct onewire_struct* onewire, struct onewire_dev_struct* dev,
                     uint32_t (*tick)(void));
int32_t onewire_deinit(struct onewire_struct* onewire);
int32_t onewire_master_transmit(struct onewire_struct* onewire, uint8_t* buffer, uint32_t size,
                                int32_t timeout);
int32_t onewire_master_receive(struct onewire_struct* onewire, uint8_t* buffer, uint32_t size,
                               int32_t timeout);

#ifdef __cplusplus
}
#endif

#endif
