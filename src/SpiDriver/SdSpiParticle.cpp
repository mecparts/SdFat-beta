/**
 * Copyright (c) 2011-2019 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "SdSpiDriver.h"
#if defined(SD_ALT_SPI_DRIVER) && defined(PLATFORM_ID)
static volatile bool SPI_DMA_TransferCompleted = false;
//-----------------------------------------------------------------------------
static void SD_SPI_DMA_TransferComplete_Callback() {
  SPI_DMA_TransferCompleted = true;
}
//------------------------------------------------------------------------------
/** Set SPI options for access to SD/SDHC cards.
 *
 * \param[in] divisor SCK clock divider relative to the APB1 or APB2 clock.
 */
void SdAltSpiDriver::activate() {
  m_spi->beginTransaction(m_spiSettings);
}
//------------------------------------------------------------------------------
/** Initialize the SPI bus.
 *
 * \param[in] chipSelectPin SD card chip select pin.
 */
void SdAltSpiDriver::begin(SdSpiConfig spiConfig) {
  m_csPin = spiConfig.csPin;
  m_spiSettings = LOW_SPEED_SPI_SETTINGS;
  if (spiConfig.spiPort) {
    m_spi = spiConfig.spiPort;
  } else {
    m_spi = &SPI;
  }
  m_spi->begin();
  pinMode(m_csPin, OUTPUT);
  digitalWrite(m_csPin, HIGH);
}
//------------------------------------------------------------------------------
/**
 * End SPI transaction.
 */
void SdAltSpiDriver::deactivate() {
  m_spi->endTransaction();
}
//------------------------------------------------------------------------------
/** Receive a byte.
 *
 * \return The byte.
 */
uint8_t SdAltSpiDriver::receive() {
  return m_spi->transfer(0XFF);
}
//------------------------------------------------------------------------------
/** Receive multiple bytes.
 *
 * \param[out] buf Buffer to receive the data.
 * \param[in] n Number of bytes to receive.
 *
 * \return Zero for no error or nonzero error code.
 */
uint8_t SdAltSpiDriver::receive(uint8_t* buf, size_t n) {
  SPI_DMA_TransferCompleted = false;
  m_spi->transfer(nullptr, buf, n, SD_SPI_DMA_TransferComplete_Callback);
  while (!SPI_DMA_TransferCompleted) {}
  return 0;
}
//------------------------------------------------------------------------------
/** Send a byte.
 *
 * \param[in] b Byte to send
 */
void SdAltSpiDriver::send(uint8_t b) {
  m_spi->transfer(b);
}
//------------------------------------------------------------------------------
/** Send multiple bytes.
 *
 * \param[in] buf Buffer for data to be sent.
 * \param[in] n Number of bytes to send.
 */
void SdAltSpiDriver::send(const uint8_t* buf , size_t n) {
  SPI_DMA_TransferCompleted = false;

  m_spi->transfer(const_cast<uint8_t*>(buf), nullptr, n,
                            SD_SPI_DMA_TransferComplete_Callback);

  while (!SPI_DMA_TransferCompleted) {}
}
#endif  // defined(SD_ALT_SPI_DRIVER) && defined(PLATFORM_ID)

