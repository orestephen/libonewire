#include "test_device.h"
#include "test_timer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "onewire.h"

struct onewire_struct onewire;

int main(int argc, char** argv) {
  uint8_t test_buffer[] = {0x00, 0x00};
  uint8_t known_buffer[] = {0x23, 0x34};

  assert(onewire_init(&onewire, test_device_get_dev(), onewire_timer_get_tick) ==
         ONEWIRE_ERROR_NONE);
  test_device_set_data(known_buffer, sizeof(known_buffer));
  assert(onewire_master_receive(&onewire, test_buffer, sizeof(test_buffer),
                                TEST_DEVICE_BYTE_TIME * (sizeof(known_buffer) + 1)) ==
         ONEWIRE_ERROR_NONE);

  /* test buffers */
  assert(!memcmp(test_buffer, known_buffer, sizeof(known_buffer)));

  /* sanity check device */
  assert(onewire.dev->get_status() == ONEWIRE_DEV_STATUS_DONE);
  assert(onewire.state == ONEWIRE_STATE_READY);
}
