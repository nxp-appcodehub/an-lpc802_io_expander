/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#include "LPC8xx.h"
#include "i2c.h"
#include "swm.h"
#include "syscon.h"
#include "uart.h"
#include "utilities.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* vitural register address, aligned with PCA9502 */
#define REG_IO_DIR          (0x0A)
#define REG_IO_STAT         (0x0B)
#define REG_IO_INTE         (0x0C)
#define REG_IO_CTRL         (0x0E)
#define SELF_SAVE_ADDRESS   (0x13)

void setup_debug_uart(void);

static void delay(uint32_t ms)
{
    volatile int i,j;
    for(i=0; i<ms; i++)
    {
        for(j=0; j<2000; j++);
    }
}

/*
    dir: bitmap of pin direction
*/
void app_i2c_master_dir(uint8_t dir)
{
    i2c_start((SELF_SAVE_ADDRESS << 1) | 0);
    i2c_send_byte(REG_IO_DIR);
    i2c_send_byte(dir);
    i2c_stop();
}

void app_i2c_master_stat(uint8_t dir)
{
    i2c_start((SELF_SAVE_ADDRESS << 1) | 0);
    i2c_send_byte(REG_IO_STAT);
    i2c_send_byte(dir);
    i2c_stop();
}

void app_i2c_master_read(uint8_t *stat)
{
    
    i2c_start((SELF_SAVE_ADDRESS << 1) | 0);
    i2c_start((SELF_SAVE_ADDRESS << 1) | 1);
    *stat = i2c_read_byte(false);
    i2c_stop();
}


void i2c_master_init(void)
{
    // Provide main_clk as function clock to I2C0
    LPC_SYSCON->I2C0CLKSEL = FCLKSEL_MAIN_CLK;
  

    // Configure the SWM
    // On the LPC824, LPC84x I2C0_SDA and I2C0_SCL are fixed pin functions which are enabled / disabled in the pinenable0 register
    // On the LPC812, I2C0_SDA and I2C0_SCL are movable functions which are assigned using the pin assign registers
    LPC_SWM->PINENABLE0 &= ~(I2C0_SCL|I2C0_SDA); // Use for LPC824 and LPC84x
    //ConfigSWM(I2C0_SCL, P0_10);                // Use for LPC812
    //ConfigSWM(I2C0_SDA, P0_11);                // Use for LPC812

    // Give I2C0 a reset
    LPC_SYSCON->PRESETCTRL0 &= (I2C0_RST_N);
    LPC_SYSCON->PRESETCTRL0 |= ~(I2C0_RST_N);

    // Configure the I2C0 clock divider
    // Desired bit rate = Fscl = 100,000 Hz (1/Fscl = 10 us, 5 us low and 5 us high)
    // Use default clock high and clock low times (= 2 clocks each)
    // So 4 I2C_PCLKs = 100,000/second, or 1 I2C_PCLK = 400,000/second
    // I2C_PCLK = SystemClock = 30,000,000/second, so we divide by 30000000/400000 = 75
    // Remember, value written to DIV divides by value+1
    SystemCoreClockUpdate(); // Get main_clk frequency
    LPC_I2C0->DIV = 3;

    // Configure the I2C0 CFG register:
    // Master enable = true
    // Slave enable = true
    // Monitor enable = false
    // Time-out enable = false
    // Monitor function clock stretching = false
    //
    LPC_I2C0->CFG = CFG_MSTENA;
}



int main(void)
{
    uint8_t pin_stat;
    
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
    setup_debug_uart();

    printf("I2C master test for lpc802 eeporm demo CoreClock:%dHz\r\n", main_clk);
	
    // Enable bus clocks to I2C0, SWM
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (I2C0 | SWM);
    i2c_master_init();

    printf("set dir to 0xFF...\r\n");
    app_i2c_master_dir(0xFF);
    
    while(1)
    {
        delay(500);
        app_i2c_master_stat(0xFF);
        delay(500);
        app_i2c_master_stat(0x00);
        
        app_i2c_master_read(&pin_stat);
        printf("pin status:0x%2X\r\n", pin_stat);
    }
}
