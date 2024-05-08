/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

 
#ifndef __APPDEF_H__
#define __APPDEF_H__

#ifdef __cplusplus
 extern "C" {
#endif
     

/* marco of I2C register */
#define STAT_SLVPEND 		(1 << 8)
#define SLAVE_STATE_MASK	(0x3<<9)
#define STAT_SLVADDR		(0x0 << 9)
#define STAT_SLVRX  	 	(0x1 << 9)
#define STAT_SLVTX  	 	(0x2 << 9)
#define STAT_SLVNOTSTR		(1 << 11)
#define STAT_SLVSEL		 	(1 << 14)
#define STAT_SLVDESEL		(1 << 15)

#define CTL_SLVCONTINUE			(1 << 0)
#define CTL_SLVNACK				(1 << 1)

/* vitural register address, aligned with PCA9502 */
#define REG_IO_DIR          (0x0A)
#define REG_IO_STAT         (0x0B)
#define REG_IO_INTE         (0x0C)
#define REG_IO_CTRL         (0x0E)
     
/* UART definition */
#define UART_BAUD                   (115200)
#define SWM_UART_REG_IDX            (0)
#define SWM_UART_TX_OFS             (0)
#define SWM_UART_RX_OFS             (8)
#define UART_TX_PIN                 (14)
#define UART_RX_PIN                 (0)
     
/* I2C definition */
#define I2C_BAUD                    (400*1000)
#define SWM_I2C_REG_IDX             (5)
#define SWM_SCK_OFS                 (0)
#define SWM_SDA_OFS                 (8)
#define I2C_SCK_PIN                 (10)
#define I2C_SDA_PIN                 (16)
#define I2C_SLV_ADDR                (0x13)  
     
/* */
#define SYSAHBCLKCTRL_PINT_MASK     (1<<28)


#endif
