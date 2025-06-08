#ifndef __ONEWIRE_DEV_H
#define __ONEWIRE_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define ONEWIRE_DEV_DIR_TX 0
#define ONEWIRE_DEV_DIR_RX 1

#define ONEWIRE_DEV_STATUS_NONE 0
#define ONEWIRE_DEV_STATUS_DONE (1 << 1)
#define ONEWIRE_DEV_STATUS_ERROR (1 << 2)

struct onewire_dev_struct {
  void (*reset)(void);
  void (*set_dir)(uint8_t dir);
  uint8_t (*get_dir)(void);
  uint32_t (*get_status)(void);
  void (*transceive)(uint8_t* buffer, uint32_t size);
};

#ifdef __cplusplus
}
#endif

#endif
