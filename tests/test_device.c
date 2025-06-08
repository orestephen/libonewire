#include "test_device.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test_timer.h"
#include "onewire.h"
#include "onewire_dev.h"

#define TAG "test_device"

#ifdef ONEWIRE_DBG
#define test_device_printf(format, ...) printf(format, ##__VA_ARGS__)
#else
#define test_device_printf(format, ...)
#endif

static struct onewire_dev_struct device;
static volatile uint32_t dev_status;
static uint8_t dev_dir;

/* for simulated bus data */
static struct onewire_buffer_struct dev_bus_buffer;

static timer_t timer;

void test_device_set_data(uint8_t* data, uint32_t size) {
  test_device_printf("%s: %s\r\n", TAG, __func__);

  /* set simulated buffer */
  dev_bus_buffer.data = data;
  dev_bus_buffer.size = size;
  dev_bus_buffer.index = 0;
}

static void transceive_done(union sigval sv) {
  timer_t* timer;

  test_device_printf("%s: %s\r\n", TAG, __func__);

  /* get timer and clear it */
  timer = (timer_t*)sv.sival_ptr;
  timer_delete(timer);

  /* set done status */
  dev_status |= ONEWIRE_DEV_STATUS_DONE;
}

static void reset(void) {
  test_device_printf("%s: %s\r\n", TAG, __func__);

  /* reset device regs */
  dev_status = ONEWIRE_DEV_STATUS_NONE;
  dev_dir = ONEWIRE_DEV_DIR_TX;
}

static void set_dir(uint8_t dir) {
  test_device_printf("%s: %s - dir=%d\r\n", TAG, __func__, dir);

  /* set dir */
  dev_dir = dir;
}

static uint8_t get_dir(void) {
  test_device_printf("%s: %s\r\n", TAG, __func__);

  /* get dir */
  return dev_dir;
}

static uint32_t get_status(void) {
  /* get status */
  return dev_status;
}

static void transceive(uint8_t* buffer, uint32_t size) {
  test_device_printf("%s: %s - data [", TAG, __func__);

  /* sanity check inputs */
  if (!buffer || size < 1) dev_status |= ONEWIRE_DEV_STATUS_ERROR;

  /* check for device error */
  if (dev_status & ONEWIRE_DEV_STATUS_ERROR) return;

  /* clear done flag */
  dev_status &= ~(ONEWIRE_DEV_STATUS_DONE);

  for (uint32_t i = 0; i < size && i < dev_bus_buffer.size; i++) {
    if (dev_dir == ONEWIRE_DEV_DIR_TX) {
      /* set bus to tx data */
      dev_bus_buffer.data[dev_bus_buffer.index++] = buffer[i];
    } else if (dev_dir == ONEWIRE_DEV_DIR_RX) {
      /* set rx data to bus */
      buffer[i] = dev_bus_buffer.data[dev_bus_buffer.index++];
    } else {
      test_device_printf(" ERROR ");
      break;
    }
    test_device_printf(" 0x%02X%s", buffer[i],
                       (i < size - 1 || i < dev_bus_buffer.size - 1 ? " " : ", "));
  }

  test_device_printf("]\r\n");

  /* pretent to send data */
  onewire_timer_create(&timer, transceive_done, (void*)&timer);
  onewire_timer_start(&timer, TEST_DEVICE_BYTE_TIME, 0);
}

struct onewire_dev_struct* test_device_get_dev(void) {
  test_device_printf("%s: %s\r\n", TAG, __func__);

  memset(&device, 0, sizeof(struct onewire_dev_struct));
  memset(&timer, 0, sizeof(timer_t));

  /* set device functions */
  device.reset = reset;
  device.set_dir = set_dir;
  device.get_dir = get_dir;
  device.get_status = get_status;
  device.transceive = transceive;

  /* set device regs */
  dev_status = 0;
  dev_dir = ONEWIRE_DEV_DIR_TX;

  /* set device buffer */
  test_device_set_data(NULL, 0);

  return &device;
}
