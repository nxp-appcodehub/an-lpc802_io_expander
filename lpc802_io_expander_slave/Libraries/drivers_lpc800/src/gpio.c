/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

 
#include "common.h"
#include "gpio.h"

void GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir)
{
    if(dir)
    {
        LPC_GPIO_PORT->DIRSET0 = (1<<pin);
    }
    else
    {
        LPC_GPIO_PORT->DIRCLR0 = (1<<pin);
    }
}

void GPIO_PinWrite(uint32_t instance, uint32_t pin, uint8_t data)
{
    if(data)
    {
        LPC_GPIO_PORT->SET0 = (1<<pin);
    }
    else
    {
        LPC_GPIO_PORT->CLR0 = (1<<pin);
    }
}

uint32_t GPIO_PinRead(uint32_t instance, uint32_t pin)
{
    return ((LPC_GPIO_PORT->PIN0 >> pin) & 0x01);
}

void GPIO_PinToggle(uint32_t instance, uint8_t pin)
{
    LPC_GPIO_PORT->NOT0 = (1<<pin);
}

int GPIO_SetIntMode(uint32_t socket, GPIO_Int_t mode, bool val)
{
    /* all socket use edge sensitive */
    
    LPC_PIN_INT->ISEL &= ~(1<<socket);
    LPC_PIN_INT->IST = 0xFF;
    
    switch(mode)
    {
        case kGPIO_Int_RE:
            (val)?(LPC_PIN_INT->IENR |= (1<<socket)):(LPC_PIN_INT->IENR &= ~(1<<socket));
            break;
        case kGPIO_Int_FE:
            (val)?(LPC_PIN_INT->IENF |= (1<<socket)):(LPC_PIN_INT->IENF &= ~(1<<socket));
            break;
        case kGPIO_Int_EE:
            (val)?(LPC_PIN_INT->IENR |= (1<<socket)):(LPC_PIN_INT->IENR &= ~(1<<socket));
            (val)?(LPC_PIN_INT->IENF |= (1<<socket)):(LPC_PIN_INT->IENF &= ~(1<<socket));
            break;
    }
    
    return CH_OK;
}

uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_t mode)
{
    /* enable GPIO clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
    
    switch(mode)
    {
        case kGPIO_IFT:
            GPIO_SetPinDir(instance, pin, 0);
            break;
        case kGPIO_OPPH:
            GPIO_PinWrite(instance, pin, 1);
            GPIO_SetPinDir(instance, pin, 1);
            break;
        case kGPIO_OPPL:
            GPIO_PinWrite(instance, pin, 0);
            GPIO_SetPinDir(instance, pin, 1);
            break;
        case kGPIO_IPD:
            GPIO_SetPinDir(instance, pin, 0);
            break;
        case kGPIO_IPU:
            GPIO_SetPinDir(instance, pin, 0);
            break;
    }
    
    return CH_OK;
}
    

//! @}
