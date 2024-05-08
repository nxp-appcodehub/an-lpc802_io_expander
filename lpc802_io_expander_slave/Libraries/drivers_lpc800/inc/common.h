/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


 
#ifndef __CH_LIB_LPC_COMMON_H__
#define __CH_LIB_LPC_COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>


#ifdef  LPC82X
#include "LPC82x.h"
#elif   LPC8XX
#include "LPC8xx.h"
#else
#error "No CPU defined!"
#endif


#if defined(LIB_DEBUG)
#include <stdio.h>
#define LIB_TRACE	printf
#else
#define LIB_TRACE(...)
#endif

/* ALIGN */
#define ALIGN_DOWN(x, a)    ((x) & -(a))
#define ALIGN_UP(x, a)      (-(-(x) & -(a)))

#ifndef ALIGN
/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#endif /* Compiler Related Definitions */
#endif

#define MAKE_VERSION(major, minor, bugfix) (((major) << 16) | ((minor) << 8) | (bugfix))

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)         (((a) < 0) ? (-(a)) : (a))
#endif

//! @brief Computes the number of elements in an array.
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#ifndef BSWAP_16
#define BSWAP_16(x)     (uint16_t)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
#endif

#ifndef BSWAP_32
#define BSWAP_32(val)	(uint32_t)((BSWAP_16((uint32_t)(val) & (uint32_t)0xFFFF) << 0x10) |  \
                                   (BSWAP_16((uint32_t)((val) >> 0x10))))
#endif


/* CHLib internal use */
#define US_TO_COUNT(us, Hz)    (uint32_t)((uint32_t)us * (Hz / 1000000U))
#define COUNT_TO_US(count, Hz) (uint32_t)((uint32_t)(count * 1000000U) / Hz)
#define MS_TO_COUNT(ms, Hz)    (uint32_t)((uint32_t)ms * (Hz / 1000U))
#define COUNT_TO_MS(count, Hz) (uint32_t)((uint32_t)(count * 1000U) / Hz)

typedef enum
{
    kCoreClock,
    kFROClock,
}Clock_t; 


enum
{
    CH_OK,
    CH_ERR,
    CH_TIMEOUT,
    CH_OVERFLOW,
    CH_IO_ERR,
    CH_BUSY,
};

#if defined(LPC8XX)
#define SYSAHBCLKCTRL  SYSAHBCLKCTRL0
#endif

/* API */
void SystemSoftReset(void);
void SetMainClockSrc(uint8_t src);
void SWM_Config(uint32_t reg, uint32_t pin, uint32_t offset);
void DelayInit(void);
void DelayMs(uint32_t ms);
void DelayUs(uint32_t us);
void SysTick_SetIntMode(bool val);
void SysTick_SetTime(uint32_t us);
void SysTick_Init(void);
uint32_t GetClock(Clock_t clock);
uint32_t GetUID(void);
void SetConsole(int (*putc)(uint8_t ch), int (*getc)(void));
uint32_t GetResetCause(void);
void SetPowerMode(uint32_t Mode, bool enSleepOnExit);
void SetFlexCommClk(uint32_t instance, uint32_t val);

/* software test function */
uint32_t RAMTest(uint32_t address, uint32_t size);
void ShowMemory(uint32_t addr, uint32_t size);

#ifdef __cplusplus
}
#endif


#endif
