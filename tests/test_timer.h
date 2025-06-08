#ifndef __TEST_TIMER_H
#define __TEST_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>
#include <signal.h>

#define SEC_MS 1000
#define MSEC_NS 1000000

int onewire_timer_start(timer_t* timer, uint32_t timeout_ms, uint32_t interval_ms);
int onewire_timer_create(timer_t* timer, void (*timer_handler)(union sigval sv), void* ptr);
uint32_t onewire_timer_get_tick(void);

#ifdef __cplusplus
}
#endif

#endif
