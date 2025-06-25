#include "onewire.h"

#include <stddef.h>
#include <stdio.h>

#include "onewire_dev.h"

#define TAG "onwwire"
#define onewire_printf printf

#define ONEWIRE_DEV_STATUS_ALL (ONEWIRE_DEV_STATUS_DONE | ONEWIRE_DEV_STATUS_ERROR)

static int32_t onewire_setup_buffer(struct onewire_buffer_struct* buffer, uint8_t* data,
                                    uint32_t size) {
  onewire_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!buffer || !data) return ONEWIRE_ERROR_VAL;

  /* setup buffer */
  buffer->data = data;
  buffer->size = size;
  buffer->index = 0;

  return ONEWIRE_ERROR_NONE;
}

static int32_t onewire_wait_timeout(struct onewire_struct* onewire, uint32_t flags, int32_t timeout,
                                    uint32_t start, uint32_t* status) {
  uint32_t tick;

  onewire_printf("%s: %s\r\n", TAG, __func__);

  do {
    /* get device status */
    *status = onewire->dev->get_status();

    /* only checkout for timeout when timeout is greater than zero */
    if (timeout > ONEWIRE_TIMEOUT_INFINITE) {
      /* get system tick */
      tick = onewire->tick();
      /* check for timeout event */
      if (tick - start > (uint32_t)timeout) return ONEWIRE_ERROR_TIMEOUT;
    }

    /* check for one or more status flags */
  } while (!(*status & flags));

  return ONEWIRE_ERROR_NONE;
}

static int32_t onewire_trx_data(struct onewire_struct* onewire, uint32_t size) {
  onewire_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (size < 1) return ONEWIRE_ERROR_VAL;

  /* sanity check onewire buffer */
  if (onewire->buffer.index >= onewire->buffer.size) return ONEWIRE_ERROR_VAL;

  /* transceive data on the bus */
  onewire->dev->transceive(onewire->buffer.data + onewire->buffer.index, size);
  onewire->buffer.index += size;

  return ONEWIRE_ERROR_NONE;
}

static int32_t onewire_trx_wait(struct onewire_struct* onewire, int32_t timeout) {
  uint32_t start;
  uint32_t status;

  onewire_printf("%s: %s\r\n", TAG, __func__);

  if (timeout > ONEWIRE_TIMEOUT_NOWAIT) {
    /* initial tick */
    start = onewire->tick();
  }

  /* iterate onewire buffer data */
  for (uint32_t i = 0; i < onewire->buffer.size; i++) {
    /* send data */
    onewire_trx_data(onewire, 1);

    /* only wait if timeout is infinite or greater than zero, else skip */
    if (timeout != ONEWIRE_TIMEOUT_NOWAIT) {
      /* wait for flag or timeout */
      if (onewire_wait_timeout(onewire, ONEWIRE_DEV_STATUS_ALL, timeout, start, &status))
        return ONEWIRE_ERROR_TIMEOUT;
    }

    /* check for dev error */
    if (status & ONEWIRE_DEV_STATUS_ERROR) return ONEWIRE_ERROR_DEV;
  }

  return ONEWIRE_ERROR_NONE;
}

int32_t onewire_init(struct onewire_struct* onewire, struct onewire_dev_struct* dev,
                     uint32_t (*tick)(void)) {
  onewire_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!onewire) return ONEWIRE_ERROR_VAL;
  if (!dev) return ONEWIRE_ERROR_VAL;

  /* setup onewire */
  onewire->dev = dev;
  onewire->tick = tick;
  onewire_setup_buffer(&onewire->buffer, NULL, 0);
  onewire->state = ONEWIRE_STATE_READY;

  /* setup device */
  onewire->dev->reset();
  onewire->dev->set_dir(ONEWIRE_DEV_DIR_TX);

  return ONEWIRE_ERROR_NONE;
}

int32_t onewire_deinit(struct onewire_struct* onewire) {
  onewire_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!onewire) return -1;

  /* setup onewire */
  onewire->dev = NULL;
  onewire->tick = NULL;
  onewire_setup_buffer(&onewire->buffer, NULL, 0);
  onewire->state = ONEWIRE_STATE_INIT;

  /* setup device */
  onewire->dev->reset();

  return ONEWIRE_ERROR_NONE;
}

int32_t onewire_master_transmit(struct onewire_struct* onewire, uint8_t* buffer, uint32_t size,
                                int32_t timeout) {
  int32_t ret;

  onewire_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!onewire || !buffer || size < 1) return ONEWIRE_ERROR_VAL;

  /* sanity check device */
  if (onewire->state != ONEWIRE_STATE_READY) return ONEWIRE_ERROR_BUSY;

  /* setup onewire */
  onewire_setup_buffer(&onewire->buffer, buffer, size);
  onewire->state = ONEWIRE_STATE_TRANSCEIVE;

  /* setup device */
  onewire->dev->set_dir(ONEWIRE_DEV_DIR_TX);

  /* start transmit */
  ret = onewire_trx_wait(onewire, timeout);

  /* set state and return */
  onewire->state = (ret == ONEWIRE_ERROR_NONE ? ONEWIRE_STATE_READY : ONEWIRE_STATE_ERROR);

  return ret;
}

int32_t onewire_master_receive(struct onewire_struct* onewire, uint8_t* buffer, uint32_t size,
                               int32_t timeout) {
  int32_t ret;

  onewire_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!onewire || !buffer || size < 1) return ONEWIRE_ERROR_VAL;

  /* sanity check device */
  if (onewire->state != ONEWIRE_STATE_READY) return ONEWIRE_ERROR_BUSY;

  /* setup onewire */
  onewire_setup_buffer(&onewire->buffer, buffer, size);
  onewire->state = ONEWIRE_STATE_TRANSCEIVE;

  /* setup device */
  onewire->dev->set_dir(ONEWIRE_DEV_DIR_RX);

  /* start receive */
  ret = onewire_trx_wait(onewire, timeout);

  /* set state and return */
  onewire->state = (ret == ONEWIRE_ERROR_NONE ? ONEWIRE_STATE_READY : ONEWIRE_STATE_ERROR);

  return ret;
}
