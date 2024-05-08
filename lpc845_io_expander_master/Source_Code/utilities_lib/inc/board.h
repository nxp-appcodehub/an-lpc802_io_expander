/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * board.h
 *
 *  Created on:
 *      Author:
 */

#ifndef BOARD_H_
#define BOARD_H_

//#define XPRESSO_812_BOARD   // For the LPC812 Max board
//#define XPRESSO_824_BOARD   // For the LPC824 Max board
#define XPRESSO_845_BOARD   // For the LPC845 Max board

// LPC812 Max board
#ifdef XPRESSO_812_BOARD 
#define RED    (1<<7)
#define BLUE   (1<<16)
#define GREEN  (1<<17)
#define red_led_port   P0_7
#define blue_led_port  P0_16
#define green_led_port P0_17
#define PCF8591_address  0x4F      // The on-board ADC/DAC default I2C address
#define PCA9672_address  0x23      // The on-board GPIO Expander default I2C address
#define TARGET_TX P0_6             // For the VCOM serial port
#define TARGET_RX P0_1             // For the VCOM serial port
#define NUM_IOCON_P0_SLOTS 19      // For looping through the pad controls
#endif

// LPC824 Max board
#ifdef XPRESSO_824_BOARD
#define RED    (1<<12)
#define BLUE   (1<<27)
#define GREEN  (1<<16)
#define red_led_port   P0_12
#define blue_led_port  P0_27
#define green_led_port P0_16
#define TARGET_TX P0_7             // For the VCOM serial port
#define TARGET_RX P0_18            // For the VCOM serial port
#define NUM_IOCON_P0_SLOTS 31      // For looping through the pad controls
#endif

// LPC845 Max board
#ifdef XPRESSO_845_BOARD
#define LED_RED   P0_12
#define LED_BLUE  P1_15
#define LED_GREEN P0_0
#define TARGET_TX P1_17            // For the VCOM serial port
#define TARGET_RX P1_16            // For the VCOM serial port
#define DACOUT0_PIN PIO0_17
#define DACOUT1_PIN PIO0_29
#define AUDIO_AMP_ENABLE_PORT PORT1
#define AUDIO_AMP_ENABLE_PIN Bit11
#define NUM_IOCON_SLOTS 56         // For looping through the pad controls
#endif

#endif /* BOARD_H_ */
