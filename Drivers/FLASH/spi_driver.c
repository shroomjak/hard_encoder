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
 * @file    spi_driver.c
 * @brief   Definitions of spi_driver functions.
 */
#include "spi_driver.h"
#include "main.h"





void SPI_Delay(uint32_t delayTime)
{
    volatile uint32_t i = 0;
    for (i = 0; i < delayTime; ++i)
    {
        __asm("NOP"); /* delay */
    }
}


uint8_t SPI1_RW(uint8_t txByte)
{
    uint16_t spiTimeout = 1000;
    LL_SPI_TransmitData8(SPI1, txByte);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    spiTimeout = 1000;
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1))
        if ((spiTimeout--) == 0)
            return 0;
    return (uint8_t)LL_SPI_ReceiveData8(SPI1);
}







void SPI_Exchange(uint8_t *txBuffer,
				  uint32_t txNumBytes,
				  uint8_t *rxBuffer,
				  uint32_t rxNumBytes,
				  uint32_t dummyNumBytes)
{
	uint32_t i = 0;
	// Begin data exchange

	// Select chip
        LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_4);


	// Send each byte
	for(i = 0; i < txNumBytes; i = i+1)
		SPI1_RW(txBuffer[i]);
	// Receive each byte
	for(i = 0; i < dummyNumBytes; i = i+1)
		SPI1_RW(0xff);
	// Receive each byte
	for(i = 0; i < rxNumBytes; i = i+1)
		rxBuffer[i] = SPI1_RW(0xff);

	// End data exchange


	// Deselect chip
        LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_4);
}


