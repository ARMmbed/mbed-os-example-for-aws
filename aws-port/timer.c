/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer.c
 * @brief CMSIS-RTOS2 implementation of the timer interface.
 */

// Note: Kernel Tick Frequency must be 1000Hz (1ms Tick)

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "timer_platform.h"

bool has_timer_expired(Timer *timer) {
  uint32_t tick;
  uint32_t left;

  tick = osKernelGetTickCount();
  left = timer->end_time - tick;

  return (left >= 0x80000000U);
}

void countdown_ms(Timer *timer, uint32_t timeout) {
  uint32_t tick;

  tick = osKernelGetTickCount();
  timer->end_time = tick + timeout;
}

uint32_t left_ms(Timer *timer) {
  uint32_t tick;
  uint32_t left;

  tick = osKernelGetTickCount();
  left = timer->end_time - tick;
  if (left >= 0x80000000U) {
    left = 0U;
  }

  return left;
}

void countdown_sec(Timer *timer, uint32_t timeout) {
  uint32_t tick;

  tick = osKernelGetTickCount();
  timer->end_time = tick + (timeout * 1000U);
}

void init_timer(Timer *timer) {
  timer->end_time = 0U;
}

#ifdef __cplusplus
}
#endif
