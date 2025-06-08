#include "test_timer.h"

#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define TAG "test_timer"

#ifdef ONEWIRE_DBG
#define test_timer_printf(format, ...) printf(format, ##__VA_ARGS__)
#else
#define test_timer_printf(format, ...)
#endif

int onewire_timer_start(timer_t* timer, uint32_t timeout_ms, uint32_t interval_ms) {
  int ret;
  struct itimerspec its;

  test_timer_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!timer) return -EINVAL;

  ret = 0;
  memset(&its, 0, sizeof(struct itimerspec));

  /* set timeout */
  if (timeout_ms >= SEC_MS) {
    its.it_value.tv_sec = timeout_ms / SEC_MS;
    its.it_value.tv_nsec = (long int)(timeout_ms % SEC_MS) * MSEC_NS;
  } else {
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = (long int)timeout_ms * MSEC_NS;
  }

  /* set interval */
  if (interval_ms >= SEC_MS) {
    its.it_interval.tv_sec = interval_ms / SEC_MS;
    its.it_interval.tv_nsec = (long int)(interval_ms % SEC_MS) * MSEC_NS;
  } else {
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = (long int)interval_ms * MSEC_NS;
  }

  /* set timer */
  ret = timer_settime(*timer, 0, &its, NULL);
  if (ret)
    test_timer_printf("%s: %s - Error=%d, errno=%s\r\n", TAG, __func__, ret, strerror(errno));

  return ret;
}

int onewire_timer_create(timer_t* timer, void (*timer_handler)(union sigval sv), void* ptr) {
  int ret;
  struct sigevent sev;

  test_timer_printf("%s: %s\r\n", TAG, __func__);

  /* sanity check inputs */
  if (!timer || !timer_handler) return -EINVAL;

  ret = 0;
  memset(&sev, 0, sizeof(struct sigevent));

  /* setup signal event */
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_notify_function = timer_handler;
  sev.sigev_value.sival_ptr = ptr;

  /* setup timer */
  ret = timer_create(CLOCK_REALTIME, &sev, timer);
  if (ret) test_timer_printf("%s: %s - Error %d\r\n", TAG, __func__, ret);

  return ret;
}

uint32_t onewire_timer_get_tick(void) {
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (uint32_t)(((ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000));
}
