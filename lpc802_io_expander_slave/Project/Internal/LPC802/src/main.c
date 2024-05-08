/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"

#include "mq.h"
#include "appdef.h"


/* mq evt define */
enum
{
    kI2C_WriteReg,
    kGPIO_Pulse,
};

int main(void)
{
    int i;
    uint32_t tmp;
    SystemCoreClockUpdate();
    
    DelayInit();
    DelayMs(10);

    /* LIB_DEBUG is global marco define in IDE */
    #if defined(LIB_DEBUG)
    /* cofig SWM */
    SWM_Config(SWM_UART_REG_IDX, UART_TX_PIN, SWM_UART_TX_OFS);
    SWM_Config(SWM_UART_REG_IDX, UART_RX_PIN, SWM_UART_RX_OFS);
   
    UART_Init(HW_UART0, UART_BAUD);
    LIB_TRACE("CoreClock:%dHz\r\n", GetClock(kCoreClock));
    UART_SetIntMode(HW_UART0, kUART_IntRx, true);
    #endif
    
    /* message queue init */
    mq_init();
    
    /* SWM for I2C */
    SWM_Config(SWM_I2C_REG_IDX, I2C_SCK_PIN, SWM_SCK_OFS);
    SWM_Config(SWM_I2C_REG_IDX, I2C_SDA_PIN, SWM_SDA_OFS);
    
    I2C_Init(HW_I2C0, I2C_BAUD);
    
    I2C_SetIntMode(HW_I2C0, kI2C_SlvPending, true);
    I2C_SetIntMode(HW_I2C0, kI2C_SlvDesel, true);
    I2C_SetSlvAddr(HW_I2C0, 0, I2C_SLV_ADDR);
    
    /* enable PININT clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_PINT_MASK;
    
    /* set SWDCLK SWDIO RESET to be GPIO */
    LPC_SWM->PINENABLE0 |= (1<<6) | (1<<5) | (1<<4);
    
    /* int pin */
    GPIO_Init(HW_GPIO0, 8, kGPIO_OPPH);
   
    for(i=0; i<8; i++)
    {
        LPC_SYSCON->PINTSEL[i] = i;
        NVIC_EnableIRQ((IRQn_Type)(PININT0_IRQn + i));
    }
    msg_t *pMsg;
    
    while(1)
    {
        if(mq_exist())
        {
            pMsg = mq_pop();
            if(pMsg->cmd == kI2C_WriteReg)
            {
                LIB_TRACE("cmd: write reg:0x%02X data:0x%02X\r\n", pMsg->reg, pMsg->data);
                switch(pMsg->reg)
                {
                    case REG_IO_DIR:
                        tmp = LPC_GPIO_PORT->DIR0;
                        tmp &= ~0xFF;
                        tmp |= pMsg->data;
                        LPC_GPIO_PORT->DIR0 = tmp;
                        break;
                    case REG_IO_STAT:
                        tmp = LPC_GPIO_PORT->PIN0;
                        tmp &= ~0xFF;
                        tmp |= pMsg->data;
                        LPC_GPIO_PORT->PIN0 = tmp;
                        break;
                    case REG_IO_INTE:
                        LPC_PIN_INT->IST = 0xFF;
                        tmp = LPC_PIN_INT->IENR;
                        tmp &= ~0xFF;
                        tmp |= pMsg->data;
                        LPC_PIN_INT->IENR = tmp;
                        LPC_PIN_INT->IENF = tmp;
                        break;
                    case REG_IO_CTRL:
                        
                        break;
                    default:
                        LIB_TRACE("unknown register:0x%X\r\n", pMsg->reg);
                        break;
                }
            }
            
            if(pMsg->cmd == kGPIO_Pulse)
            {
                /* 9 us interrupt time */
                GPIO_PinWrite(HW_GPIO0, 8, 0);
                DelayUs(2);
                GPIO_PinWrite(HW_GPIO0, 8, 1);
            }
        }
    }
}

void UART0_IRQHandler(void)
{
    uint8_t ch;
    
    if(UART_GetChar(HW_UART0, &ch) == CH_OK)
    {
        LIB_TRACE("%c", ch);
    }
}


void I2C_IRQHandler(void)
{
    static uint8_t slv_rx_ctr;
    static msg_t msg;
    static uint8_t chip_addr;
    static uint8_t i2c_slv_rx_buf[2];
    
    /* slave deselct */
    if(LPC_I2C0->STAT & STAT_SLVDESEL)
    {
        LPC_I2C0->STAT |= STAT_SLVDESEL;
        slv_rx_ctr = 0;
        return;
    }
    
    if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVADDR)
    {
        chip_addr = LPC_I2C0->SLVDAT;
        //LIB_TRACE("STAT_SLVADDR:0x%X\r\n", chip_addr);
        slv_rx_ctr = 0;
        LPC_I2C0->SLVCTL = CTL_SLVCONTINUE; 
    }

    /* slave receive data */
    if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVRX)
    {
        //LIB_TRACE("STAT_SLVRX\r\n");
        
        /* a write transfer */
        if((chip_addr & 0x01) == 0x00)
        {
            i2c_slv_rx_buf[slv_rx_ctr++] = LPC_I2C0->SLVDAT;
            
            /* receive reg address */
            if(slv_rx_ctr == 1)
            {
                msg.reg = i2c_slv_rx_buf[0];
                LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;
                return ;
            }
            
            /* received data */
            if(slv_rx_ctr >= 2)
            {
                msg.cmd = kI2C_WriteReg;
                msg.data = i2c_slv_rx_buf[1];
                mq_push(msg);
                slv_rx_ctr = 0;
                LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;
                return;
            }
           
            return;
        }
    }
    
    /* slave need send data to master */
    if ((LPC_I2C0->STAT & SLAVE_STATE_MASK) == STAT_SLVTX)
    {
        /* a read transfer */
        if((chip_addr & 0x01) == 0x01)
        {
            switch(msg.reg)
            {
                case REG_IO_DIR:
                    LPC_I2C0->SLVDAT = LPC_GPIO_PORT->DIR0;
                    break;
                case REG_IO_STAT:
                    LPC_I2C0->SLVDAT = LPC_GPIO_PORT->PIN0;
                    GPIO_PinWrite(HW_GPIO0, 8, kGPIO_OPPH);
                    break;
                case REG_IO_INTE:
                    LPC_I2C0->SLVDAT = LPC_PIN_INT->IENR;
                    break;
                case REG_IO_CTRL:
                    LPC_I2C0->SLVDAT = 0xFF;
                    break;
                default:
                    
                    break;
            }
            
            LPC_I2C0->SLVCTL = CTL_SLVCONTINUE;
        }
        
    }
}

void PININT_IRQHandler(uint8_t stat)
{
    LPC_PIN_INT->IST = stat;
    msg_t msg;
    msg.cmd = kGPIO_Pulse;
    mq_push(msg);
    //LIB_TRACE("PININT_IRQHandler 0x%X\r\n", stat);
}

void PININT0_IRQHandler(void)
{
    PININT_IRQHandler(1<<0);
}

void PININT1_IRQHandler(void)
{
    PININT_IRQHandler(1<<1);
}

void PININT2_IRQHandler(void)
{
    PININT_IRQHandler(1<<2);
}

void PININT3_IRQHandler(void)
{
    PININT_IRQHandler(1<<3);
}

void PININT4_IRQHandler(void)
{
    PININT_IRQHandler(1<<4);
}

void PININT5_IRQHandler(void)
{
    PININT_IRQHandler(1<<5);
}

void PININT6_IRQHandler(void)
{
    PININT_IRQHandler(1<<6);
}

void PININT7_IRQHandler(void)
{
    PININT_IRQHandler(1<<7);
}

void HardFault_Handler(void)
{
    LIB_TRACE("HardFault_Handler\r\n");
    while(1);
}
