/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include <string.h>
#include "common.h"


typedef struct
{
    int (*putc)(uint8_t ch);
    int (*getc)(void);
}Console_t;

static uint32_t fac_us = 0;             /* usDelay Mut */
static uint32_t fac_ms = 0;             /* msDelay Mut */
volatile static Console_t Console;      /*   */
bool _is_fitst_init = true;

#ifdef LIB_DEBUG
#if (defined(__CC_ARM)) || (defined(__ICCARM__))
void __aeabi_assert(const char *failedExpr, const char *file, int line)
{
    printf("ASSERT ERROR \" %s \": file \"%s\" Line \"%d\" \n", failedExpr, file, line);
    for (;;)
    {
    }
}
#elif(defined(__GNUC__))
void __assert_func(const char *file, int line, const char *func, const char *failedExpr)
{
    printf("ASSERT ERROR \" %s \": file \"%s\" Line \"%d\" function name \"%s\" \n", failedExpr, file, line, func);
    for (;;)
    {
    }
}
#endif /* (defined(__CC_ARM)) ||  (defined (__ICCARM__)) */
#endif /* LIB_DEBUG */


uint32_t GetClock(Clock_t clock)
{
    uint32_t val;
    switch(clock)
    {
        case kCoreClock:
            val = SystemCoreClock;
            break;
        case kFROClock:
            #if defined(LPC80X)
            switch(LPC_SYSCON->FROOSCCTRL & 0x03)
            {
                case 0:
                    val = 9*1000*1000;
                    break;
                case 1:
                    val = 12*1000*1000;
                    break;
                case 2:
                    val = 15*1000*1000;
                    break;
            }
            #endif
            break;
        default:
            val = 0;
    }

    return val;
}

void SWM_Config(uint32_t reg, uint32_t pin, uint32_t offset)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
    switch(reg)
    {
        case 0:
            LPC_SWM->PINASSIGN0 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN0 |= (pin << offset);
            break;
        case 1:
            LPC_SWM->PINASSIGN1 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN1 |= (pin << offset);
            break;
        case 2:
            LPC_SWM->PINASSIGN2 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN2 |= (pin << offset);
            break;
        case 3:
            LPC_SWM->PINASSIGN3 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN3 |= (pin << offset);
            break;
        case 4:
            LPC_SWM->PINASSIGN4 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN4 |= (pin << offset);
            break;
        case 5:
            LPC_SWM->PINASSIGN5 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN5 |= (pin << offset);
            break;
        case 6:
            LPC_SWM->PINASSIGN6 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN6 |= (pin << offset);
            break;
        case 7:
            LPC_SWM->PINASSIGN7 &= ~(0xFF << offset);
            LPC_SWM->PINASSIGN7 |= (pin << offset);
            break;
    }
}

/* 0:IRC, 1:PLL IN 2:WDOG_IN  3:PLL_OUT */
void SetMainClockSrc(uint8_t src)
{
    LPC_SYSCON->MAINCLKSEL = src;                      /* Select SYSPLL input        */
    LPC_SYSCON->MAINCLKUEN = 0x00;                   /* Toggle Update Register     */
    LPC_SYSCON->MAINCLKUEN = 0x01;
    SystemCoreClockUpdate();
}

void SysTick_Init(void)
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    fac_us = GetClock(kCoreClock);
    fac_us /= 1000000;
    fac_ms = fac_us * 1000;
}

#pragma weak DelayInit
void DelayInit(void)
{
    SysTick_Init();
}

#pragma weak DelayMs
void DelayMs(uint32_t ms)
{
    volatile uint32_t temp;
    uint32_t i;
    SysTick->LOAD = fac_ms;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->VAL = 0;
    for(i=0; i<ms; i++)
	{
		do
		{
			temp = SysTick->CTRL;
		}
        while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
	}
}

void SysTick_SetTime(uint32_t us)
{
    ((us*fac_us) > 0xFFFFFF)?(SysTick->LOAD = 0xFFFFFF):(SysTick->LOAD = us*fac_us);
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
}

#pragma weak DelayUs
void inline DelayUs(uint32_t us)
{
    volatile uint32_t temp;
    SysTick_SetTime(us);
    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
}

void SysTick_SetIntMode(bool val)
{
    (true == val)?(SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk):(SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk);
}


void SetConsole(int (*putc)(uint8_t ch), int (*getc)(void))
{
    Console.getc = getc;
    Console.putc = putc;
}

#include <stdio.h>
#ifdef __CC_ARM // MDK Support
struct __FILE 
{ 
	int handle;
}; 
/* FILE is typedef d in stdio.h. */ 
FILE __stdout;
FILE __stdin;
int fputc(int ch,FILE *f)
{
    if(Console.putc)
    {
        Console.putc((uint8_t)ch);
    }
	return ch;
}

int fgetc(FILE *f)
{
    if(Console.getc)
    {
        return (Console.getc() & 0xFF);
    }
    return 0;
}

#elif __ICCARM__ /* IAR support */
#include <yfuns.h>

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        return 0;
    }
    if ((handle != _LLIO_STDOUT) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* Send data.*/
    while (size--)
    {
        if(Console.putc)
        {
            Console.putc(*buffer++);
        }
        ++nChars;   
    }
    return nChars;
}

size_t __read(int handle, unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        return 0;
    }
    if ((handle != _LLIO_STDIN) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* read data.*/
    while (size--)
    {
        if(Console.getc)
        {
            *buffer++ = Console.getc() & 0xFF;
        }
        ++nChars;
    }
    return nChars;
}
#endif /* comiler support */

