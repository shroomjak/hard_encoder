/*
 * The Clear BSD License
 * Copyright (c) 2018 Adesto Technologies Corporation, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @ingroup SPI_LAYER
 */
/**
 * @file    spi_driver.h
 * @brief   Declarations of spi_driver functions.
 *
 * This is the Adesto SPI driver. It supports standard SPI, dual
 * SPI, and Quad SPI. Functionality is built in for a trigger GPIO
 * as well as a JEDEC reset.
 *
 * Initialization of the driver takes place with the
 * SPI_ConfigureSingleSPIIOs() function which should be called after
 * the pins have been set as GPIOs but before adesto layer commands
 * are run.
 */
#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

//#include "user_config.h"
#include <stdint.h>






/*!
 * @brief Base register used for CSb control.
 * @warning Use port D for Moneta shield, port C for Dataflash/AT25 shield.
 * on K82 board.
 * Port D = 0x400FF0C0U, Port C = 0x400FF080U
 */
#define SPI_CSB_PORT  USER_CONFIG_CSB_PORT
/*!
 * @brief Base register used for TRIGGER control.
 */
#define SPI_TRIGGER_PORT  USER_CONFIG_TRIGGER_PORT
/*!
 * @brief Base register used for SCK control.
 */
#define SPI_SCK_PORT  USER_CONFIG_SCK_PORT
/*!
 * @brief Base register used for MOSI control.
 */
#define SPI_MOSI_PORT USER_CONFIG_MOSI_PORT
/*!
 * @brief Base register used for MISO control.
 */
#define SPI_MISO_PORT USER_CONFIG_MISO_PORT
/*!
 * @brief Base register used for WPb control.
 */
#define SPI_WPB_PORT  USER_CONFIG_IO2_PORT
/*!
 * @brief Base register used for HOLDb control.
 */
#define SPI_HOLDB_PORT USER_CONFIG_IO3_PORT

//! Pin number for CSb
//! @warning Changes based on adapter used!
//! Use pin 4 with Moneta, pin 1 with others on K82 board.
//! @warning Don't forget to update USER_CONFIG_BoardInit()
//! called from main! As is, both CSb pins are configured as outputs
//! so the changes above and below in this file are the only things
//! needed (pin and port numbers for CSb).
#define SPI_CSB_PIN  USER_CONFIG_CSB_PIN
//! Pin number for TRIGGER
#define SPI_TRIGGER_PIN  USER_CONFIG_TRIGGER_PIN
//! Pin number for SCK
#define SPI_SCK_PIN  USER_CONFIG_SCK_PIN
//! Pin number for MOSI
#define SPI_MOSI_PIN USER_CONFIG_MOSI_PIN
//! Pin number for MISO
#define SPI_MISO_PIN USER_CONFIG_MISO_PIN
//! Pin number for WPb - IO2
#define SPI_WPB_PIN USER_CONFIG_IO2_PIN
//! Pin number for HOLDb - IO3
#define SPI_HOLDB_PIN USER_CONFIG_IO3_PIN
//! Half clock period delay interval
#define DELAY 5U

#define SPI 0
#define QPI 1



/*!
 * @brief Sends and receives bytes based on the function parameters.
 * MISO and MOSI fill their standard SPI roles.
 *
 * @param *txBuffer A pointer to the tx byte array to be transmitted.
 * Should have tx_bytes elements.
 * @param txNumBytes The number of bytes to be transmitted.
 * @param *rxBuffer A pointer to the rx byte array where received data will be stored.
 * Should have rx_bytes elements.
 * @param rxNumBytes The number of bytes to be received.
 * @param dummyNumBytes The number of dummy bytes to be sent.
 *
 * @retval void
 */
void SPI_Exchange(uint8_t *txBuffer,
				  uint32_t txNumBytes,
				  uint8_t *rxBuffer,
				  uint32_t rxNumBytes,
				  uint32_t dummyNumBytes);


/*!
 * @brief Receives a byte along MISO and returns the value received.
 *
 * @retval uint8_t The byte received in Little Endian format.
 */
uint8_t SPI_ReceiveByte();


/*!
 * @brief Sends a byte along MOSI.
 *
 * @param transmittedByte Byte to be sent.
 * @retval void
 */
void SPI_SendByte(uint8_t transmittedByte);




/*!
 * @brief Performs a delayTime number of NOPs
 *
 * @param delayTime The number of NOPs to be run.
 * @retval void
 */
void SPI_Delay(uint32_t delayTime);



/*!
 * @brief Triggers a falling edge on the SPI_TRIGGER_PORT/PIN output.
 *
 * @retval void
 */
void SPI_Trigger();


#endif /* SPI_DRIVER_H_ */
