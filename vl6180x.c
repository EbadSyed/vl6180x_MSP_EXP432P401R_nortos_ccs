/*
 * Copyright (c) 2018-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== display.c ========
 */
#include <stdint.h>
#include <unistd.h>

/* TI-Drivers Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogMSP432.h>

/* Driver Configuration */
#include "ti_drivers_config.h"


static Display_Handle display;

I2C_Handle      i2c0;
I2C_Handle      i2c1;

I2C_Params      i2cParams;
I2C_Transaction i2cTransaction;

uint8_t writeBuffer[10];
uint8_t readBuffer[10];

bool retVal = false;

#define TIMEOUT_MS      100

#define TASKSTACKSIZE       640

#define VL6180X_IDENTIFICATION__MODEL_ID                0x000
#define VL6180X_IDENTIFICATION__MODEL_REV_MAJOR         0x001
#define VL6180X_IDENTIFICATION__MODEL_REV_MINOR         0x002
#define VL6180X_IDENTIFICATION__MODULE_REV_MAJOR        0x003
#define VL6180X_IDENTIFICATION__MODULE_REV_MINOR        0x004
#define VL6180X_IDENTIFICATION__DATE_HI                 0x006
#define VL6180X_IDENTIFICATION__DATE_LO                 0x007
#define VL6180X_IDENTIFICATION__TIME_HI                 0x008
#define VL6180X_IDENTIFICATION__TIME_LO                 0x009

#define VL6180X_SYSTEM__MODE_GPIO0                      0x010
#define VL6180X_SYSTEM__MODE_GPIO1                      0x011
#define VL6180X_SYSTEM__HISTORY_CTRL                    0x012
#define VL6180X_SYSTEM__INTERRUPT_CONFIG_GPIO           0x014
#define VL6180X_SYSTEM__INTERRUPT_CLEAR                 0x015
#define VL6180X_SYSTEM__FRESH_OUT_OF_RESET              0x016
#define VL6180X_SYSTEM__GROUPED_PARAMETER_HOLD          0x017

#define VL6180X_SYSRANGE__START                         0x018
#define VL6180X_SYSRANGE__THRESH_HIGH                   0x019
#define VL6180X_SYSRANGE__THRESH_LOW                    0x01A
#define VL6180X_SYSRANGE__INTERMEASUREMENT_PERIOD       0x01B
#define VL6180X_SYSRANGE__MAX_CONVERGENCE_TIME          0x01C
#define VL6180X_SYSRANGE__CROSSTALK_COMPENSATION_RATE   0x01E
#define VL6180X_SYSRANGE__CROSSTALK_VALID_HEIGHT        0x021
#define VL6180X_SYSRANGE__EARLY_CONVERGENCE_ESTIMATE    0x022
#define VL6180X_SYSRANGE__PART_TO_PART_RANGE_OFFSET     0x024
#define VL6180X_SYSRANGE__RANGE_IGNORE_VALID_HEIGHT     0x025
#define VL6180X_SYSRANGE__RANGE_IGNORE_THRESHOLD        0x026
#define VL6180X_SYSRANGE__MAX_AMBIENT_LEVEL_MULT        0x02C
#define VL6180X_SYSRANGE__RANGE_CHECK_ENABLES           0x02D
#define VL6180X_SYSRANGE__VHV_RECALIBRATE               0x02E
#define VL6180X_SYSRANGE__VHV_REPEAT_RATE               0x031
#define VL6180X_SYSALS__START                           0x038
#define VL6180X_SYSALS__THRESH_HIGH                     0x03A
#define VL6180X_SYSALS__THRESH_LOW                      0x03C
#define VL6180X_SYSALS__INTERMEASUREMENT_PERIOD         0x03E
#define VL6180X_SYSALS__ANALOGUE_GAIN                   0x03F
#define VL6180X_SYSALS__INTEGRATION_PERIOD              0x040
#define VL6180X_RESULT__RANGE_STATUS                    0x04D
#define VL6180X_RESULT__ALS_STATUS                      0x04E
#define VL6180X_RESULT__INTERRUPT_STATUS_GPIO           0x04F
#define VL6180X_RESULT__ALS_VAL                         0x050
#define VL6180X_RESULT__HISTORY_BUFFER_0_HI             0x052
#define VL6180X_RESULT__HISTORY_BUFFER_0_LO             0x053
#define VL6180X_RESULT__HISTORY_BUFFER_1_HI             0x054
#define VL6180X_RESULT__HISTORY_BUFFER_1_LO             0x055
#define VL6180X_RESULT__HISTORY_BUFFER_2_HI             0x056
#define VL6180X_RESULT__HISTORY_BUFFER_2_LO             0x057
#define VL6180X_RESULT__HISTORY_BUFFER_3_HI             0x058
#define VL6180X_RESULT__HISTORY_BUFFER_3_LO             0x059
#define VL6180X_RESULT__HISTORY_BUFFER_4_HI             0x05A
#define VL6180X_RESULT__HISTORY_BUFFER_4_LO             0x05B
#define VL6180X_RESULT__HISTORY_BUFFER_5_HI             0x05C
#define VL6180X_RESULT__HISTORY_BUFFER_5_LO             0x05D
#define VL6180X_RESULT__HISTORY_BUFFER_6_HI             0x05E
#define VL6180X_RESULT__HISTORY_BUFFER_6_LO             0x05F
#define VL6180X_RESULT__HISTORY_BUFFER_7_HI             0x060
#define VL6180X_RESULT__HISTORY_BUFFER_7_LO             0x061
#define VL6180X_RESULT__RANGE_VAL                       0x062
#define VL6180X_RESULT__RANGE_RAW                       0x064
#define VL6180X_RESULT__RANGE_RETURN_RATE               0x066
#define VL6180X_RESULT__RANGE_REFERENCE_RATE            0x068
#define VL6180X_RESULT__RANGE_RETURN_SIGNAL_COUNT       0x06C
#define VL6180X_RESULT__RANGE_REFERENCE_SIGNAL_COUNT    0x070
#define VL6180X_RESULT__RANGE_RETURN_AMB_COUNT          0x074
#define VL6180X_RESULT__RANGE_REFERENCE_AMB_COUNT       0x078
#define VL6180X_RESULT__RANGE_RETURN_CONV_TIME          0x07C
#define VL6180X_RESULT__RANGE_REFERENCE_CONV_TIME       0x080

#define RANGE_SCALER                                    0x096

#define VL6180X_READOUT__AVERAGING_SAMPLE_PERIOD        0x10A
#define VL6180X_FIRMWARE__BOOTUP                        0x119
#define VL6180X_FIRMWARE__RESULT_SCALER                 0x120

#define VL6180X_I2C_SLAVE__DEVICE_ADDRESS               0x212
#define VL6180X_INTERLEAVED_MODE__ENABLE                0x2A3

#define VL6180X_CONTINUOUS                              0x03
#define VL6180X_SINGLE                                  0x01

/* Range of possible I2C slave addresses */
#define MIN_ADDRESS         0x08
#define MAX_ADDRESS         0x78

///! Device model identification number
#define VL6180X_REG_IDENTIFICATION_MODEL_ID 0x000
///! Interrupt configuration
#define VL6180X_REG_SYSTEM_INTERRUPT_CONFIG 0x014
///! Interrupt clear bits
#define VL6180X_REG_SYSTEM_INTERRUPT_CLEAR 0x015
///! Fresh out of reset bit
#define VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET 0x016
///! Trigger Ranging
#define VL6180X_REG_SYSRANGE_START 0x018
///! Trigger Lux Reading
#define VL6180X_REG_SYSALS_START 0x038
///! Lux reading gain
#define VL6180X_REG_SYSALS_ANALOGUE_GAIN 0x03F
///! Integration period for ALS mode, high byte
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD_HI 0x040
///! Integration period for ALS mode, low byte
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD_LO 0x041
///! Specific error codes
#define VL6180X_REG_RESULT_RANGE_STATUS 0x04d
///! Interrupt status
#define VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO 0x04f
///! Light reading value
#define VL6180X_REG_RESULT_ALS_VAL 0x050
///! Ranging reading value
#define VL6180X_REG_RESULT_RANGE_VAL 0x062
///! I2C Slave Device Address
#define VL6180X_REG_SLAVE_DEVICE_ADDRESS 0x212

#define VL6180X_ALS_GAIN_1 0x06    ///< 1x gain
#define VL6180X_ALS_GAIN_1_25 0x05 ///< 1.25x gain
#define VL6180X_ALS_GAIN_1_67 0x04 ///< 1.67x gain
#define VL6180X_ALS_GAIN_2_5 0x03  ///< 2.5x gain
#define VL6180X_ALS_GAIN_5 0x02    ///< 5x gain
#define VL6180X_ALS_GAIN_10 0x01   ///< 10x gain
#define VL6180X_ALS_GAIN_20 0x00   ///< 20x gain
#define VL6180X_ALS_GAIN_40 0x07   ///< 40x gain

#define VL6180X_ERROR_NONE 0        ///< Success!
#define VL6180X_ERROR_SYSERR_1 1    ///< System error
#define VL6180X_ERROR_SYSERR_5 5    ///< Sysem error
#define VL6180X_ERROR_ECEFAIL 6     ///< Early convergence estimate fail
#define VL6180X_ERROR_NOCONVERGE 7  ///< No target detected
#define VL6180X_ERROR_RANGEIGNORE 8 ///< Ignore threshold check failed
#define VL6180X_ERROR_SNR 11        ///< Ambient conditions too high
#define VL6180X_ERROR_RAWUFLOW 12   ///< Raw range algo underflow
#define VL6180X_ERROR_RAWOFLOW 13   ///< Raw range algo overflow
#define VL6180X_ERROR_RANGEUFLOW 14 ///< Raw range algo underflow
#define VL6180X_ERROR_RANGEOFLOW 15 ///< Raw range algo overflow


// Read an 8-bit register
uint8_t readReg(uint16_t reg,I2C_Handle i2c)
{

    writeBuffer[0]=(reg >> 8) & 0xFF;
    writeBuffer[1]=reg  & 0xFF;

    i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
    i2cTransaction.writeCount = 2;          /* Number of bytes to be written */
    i2cTransaction.readBuf = readBuffer;    /* Buffer to be read */
    i2cTransaction.readCount = 1;           /* Number of bytes to be read */

    /* Re-try writing to slave till I2C_transfer returns true */
    do {
         retVal = I2C_transfer(i2c, &i2cTransaction);
    } while(!retVal);

    //retVal = I2C_transfer(i2c, &i2cTransaction);
    return readBuffer[0];

}

// Writes an 8-bit register
void writeReg(uint16_t reg, uint8_t value,I2C_Handle i2c)
{
    bool status;

    writeBuffer[0]=(reg >> 8) & 0xFF;
    writeBuffer[1]=reg  & 0xFF;
    writeBuffer[2]=value  & 0xFF;


    i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
    i2cTransaction.writeCount = 3;          /* Number of bytes to be written */
    i2cTransaction.readBuf = NULL;          /* Buffer to be read */
    i2cTransaction.readCount = 0;           /* Number of bytes to be read */

    status = I2C_transfer(i2c, &i2cTransaction); /* Perform I2C transfer */

}

// Writes an 16-bit register
void  writeReg16Bit(uint16_t reg, uint16_t value,I2C_Handle i2c)
{
    bool status;

    writeBuffer[0]=(reg >> 8) & 0xFF;
    writeBuffer[1]=reg  & 0xFF;
    writeBuffer[2]=(value >> 8) & 0xFF;
    writeBuffer[3]=value  & 0xFF;


    i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
    i2cTransaction.writeCount = 4;          /* Number of bytes to be written */
    i2cTransaction.readBuf = NULL;    /* Buffer to be read */
    i2cTransaction.readCount = 0;           /* Number of bytes to be read */

    status = I2C_transfer(i2c, &i2cTransaction); /* Perform I2C transfer */


}

// Read an 16-bit register
uint8_t readReg16Bit(uint16_t reg,I2C_Handle i2c)
{

    writeBuffer[0]=(reg >> 8) & 0xFF;
    writeBuffer[1]=reg  & 0xFF;

    i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
    i2cTransaction.writeCount = 2;          /* Number of bytes to be written */
    i2cTransaction.readBuf = readBuffer;    /* Buffer to be read */
    i2cTransaction.readCount = 2;           /* Number of bytes to be read */

//    /* Re-try writing to slave till I2C_transfer returns true */
//    do {
//         retVal = I2C_transfer(i2c, &i2cTransaction);
//    } while(!retVal);

    retVal = I2C_transfer(i2c, &i2cTransaction);
    uint16_t value;

    value = readBuffer[0] << 8;
    value |= readBuffer[1];
    //Display_printf(display, 0, 0, "Data 16 bit: %d\n", value);

    return value;

}

void VL6180X_init(I2C_Handle i2c)
{
    if (readReg(VL6180X_SYSTEM__FRESH_OUT_OF_RESET,i2c) == 1)
     {

       writeReg(0x207, 0x01,i2c);
       writeReg(0x208, 0x01,i2c);
       writeReg(0x096, 0x00,i2c);
       writeReg(0x097, 0xFD,i2c); // RANGE_SCALER = 253
       writeReg(0x0E3, 0x00,i2c);
       writeReg(0x0E4, 0x04,i2c);
       writeReg(0x0E5, 0x02,i2c);
       writeReg(0x0E6, 0x01,i2c);
       writeReg(0x0E7, 0x03,i2c);
       writeReg(0x0F5, 0x02,i2c);
       writeReg(0x0D9, 0x05,i2c);
       writeReg(0x0DB, 0xCE,i2c);
       writeReg(0x0DC, 0x03,i2c);
       writeReg(0x0DD, 0xF8,i2c);
       writeReg(0x09F, 0x00,i2c);
       writeReg(0x0A3, 0x3C,i2c);
       writeReg(0x0B7, 0x00,i2c);
       writeReg(0x0BB, 0x3C,i2c);
       writeReg(0x0B2, 0x09,i2c);
       writeReg(0x0CA, 0x09,i2c);
       writeReg(0x198, 0x01,i2c);
       writeReg(0x1B0, 0x17,i2c);
       writeReg(0x1AD, 0x00,i2c);
       writeReg(0x0FF, 0x05,i2c);
       writeReg(0x100, 0x05,i2c);
       writeReg(0x199, 0x05,i2c);
       writeReg(0x1A6, 0x1B,i2c);
       writeReg(0x1AC, 0x3E,i2c);
       writeReg(0x1A7, 0x1F,i2c);
       writeReg(0x030, 0x00,i2c);

       writeReg(VL6180X_SYSTEM__FRESH_OUT_OF_RESET, 0,i2c);
     }
    else
     {
       // Sensor has already been initialized, so try to get scaling settings by
       // reading registers.

       uint16_t s = readReg16Bit(RANGE_SCALER,i2c);

     }

}

// Note that this function does not set up GPIO1 as an interrupt output as
// suggested, though you can do so by calling:
// writeReg(SYSTEM__MODE_GPIO1, 0x10);
void VL6180X_configureDefault(I2C_Handle i2c)
{
  // "Recommended : Public registers"

  // readout__averaging_sample_period =
  writeReg(VL6180X_READOUT__AVERAGING_SAMPLE_PERIOD, 0x3C,i2c);

  // sysals__analogue_gain_light = 6 (ALS gain = 1 nominal, actually 1.01 according to Table 14 in datasheet)
  writeReg(VL6180X_SYSALS__ANALOGUE_GAIN, 0x46,i2c);

  // sysrange__vhv_repeat_rate = 255 (auto Very High Voltage temperature recalibration after every 255 range measurements)
  writeReg(VL6180X_SYSRANGE__VHV_REPEAT_RATE, 0xFF,i2c);

  // sysals__integration_period = 99 (100 ms)
  // AN4545 incorrectly recommends writing to register 0x040; 0x63 should go in the lower byte, which is register 0x041.
  writeReg16Bit(VL6180X_SYSALS__INTEGRATION_PERIOD, 0x0063,i2c);

  // sysrange__vhv_recalibrate = 1 (manually trigger a VHV recalibration)
  writeReg(VL6180X_SYSRANGE__VHV_RECALIBRATE, 0x01,i2c);


  // "Optional: Public registers"

  // sysrange__intermeasurement_period = 9 (100 ms)
  writeReg(VL6180X_SYSRANGE__INTERMEASUREMENT_PERIOD, 0x01,i2c);

  // sysals__intermeasurement_period = 49 (500 ms)
  writeReg(VL6180X_SYSALS__INTERMEASUREMENT_PERIOD, 0x31,i2c);

  // als_int_mode = 4 (ALS new sample ready interrupt); range_int_mode = 4 (range new sample ready interrupt)
  writeReg(VL6180X_SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24,i2c);


  // Reset other settings to power-on defaults

  // sysrange__max_convergence_time = 63 (63 ms)
  writeReg(VL6180X_SYSRANGE__MAX_CONVERGENCE_TIME, 0x3F,i2c);

  // disable interleaved mode
  writeReg(VL6180X_INTERLEAVED_MODE__ENABLE, 0,i2c);

}

/**************************************************************************/
/*!
    @brief  start Single shot ranging. The caller of this should have code
    that waits until the read completes, by either calling
    {@link waitRangeComplete} or calling {@link isRangeComplete} until it
    returns true.  And then the code should call {@link readRangeResult}
    to retrieve the range value and clear out the internal status.
    @return true if range completed.
*/
/**************************************************************************/

bool VL6180X_startRange(I2C_Handle i2c) {
  // wait for device to be ready for range measurement

  while (!(readReg(VL6180X_REG_RESULT_RANGE_STATUS,i2c) & 0x01)){

  }
    ;

  // Start a range measurement
  writeReg(VL6180X_REG_SYSRANGE_START, 0x01,i2c);

  return true;
}

/**************************************************************************/
/*!
    @brief  Single shot ranging. Be sure to check the return of {@link
   readRangeStatus} to before using the return value!
    @return Distance in millimeters if valid
*/
/**************************************************************************/

uint8_t VL6180X_readRange(I2C_Handle i2c) {
  // wait for device to be ready for range measurement
  while (!(readReg(VL6180X_REG_RESULT_RANGE_STATUS,i2c) & 0x01))
    ;

  // Start a range measurement
  writeReg(VL6180X_REG_SYSRANGE_START, 0x01,i2c);

  // Poll until bit 2 is set
  while (!(readReg(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO,i2c) & 0x04))
    ;

  // read range in mm
  uint8_t range = readReg(VL6180X_REG_RESULT_RANGE_VAL,i2c);

  // clear interrupt
  writeReg(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07,i2c);

  return range;
}

/**************************************************************************/
/*!
    @brief  Request ranging success/error message (retreive after ranging)
    @returns One of possible VL6180X_ERROR_* values
*/
/**************************************************************************/

uint8_t VL6180X_readRangeStatus(I2C_Handle i2c) {
  return (readReg(VL6180X_REG_RESULT_RANGE_STATUS,i2c) >> 4);
}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{

    i2cTransaction.slaveAddress = 0x29;

    Watchdog_Handle watchdogHandle;
    Watchdog_Params params;

    Display_init();
    Watchdog_init();
    GPIO_init();
    I2C_init();

    /* Open a Watchdog driver instance */
    Watchdog_Params_init(&params);
    params.resetMode = Watchdog_RESET_ON;

    watchdogHandle = Watchdog_open(CONFIG_WATCHDOG_0, &params);
    if (watchdogHandle == NULL) {
        /* Error opening Watchdog */
        while (1) {}
    }


    /* Open the HOST display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
           while (1);
    }

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c1 = I2C_open(CONFIG_I2C_1, &i2cParams);
    if (i2c1 == NULL) {
//        Display_printf(display, 0, 0, "Error Initializing I2C\n");
        while (1);
    }
    else {
//        Display_printf(display, 0, 0, "I2C Initialized Bus 1!\n");
    }

    i2c0 = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c0 == NULL) {
//            Display_printf(display, 0, 0, "Error Initializing I2C\n");
            while (1);
     }
     else {
//            Display_printf(display, 0, 0, "I2C Initialized Bus 0!\n");
     }


    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );
    GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );
    GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );
    GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );

    while (1) {
        GPIO_toggle(CONFIG_GPIO_LED_0);

        Watchdog_clear(watchdogHandle);

        GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL );
        GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL );

        VL6180X_init(i2c1);
        VL6180X_init(i2c0);
        VL6180X_configureDefault(i2c1);
        VL6180X_configureDefault(i2c0);

        VL6180X_startRange(i2c1);
        VL6180X_startRange(i2c0);

        uint8_t range1 = VL6180X_readRange(i2c1);
        uint8_t status1 = VL6180X_readRangeStatus(i2c1);
        uint8_t range3 = VL6180X_readRange(i2c0);
        uint8_t status3 = VL6180X_readRangeStatus(i2c0);

        GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );
        GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );

        GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL );
        GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL );

        VL6180X_init(i2c1);
        VL6180X_init(i2c0);
        VL6180X_configureDefault(i2c1);
        VL6180X_configureDefault(i2c0);
        VL6180X_startRange(i2c1);
        VL6180X_startRange(i2c0);

        uint8_t range2 = VL6180X_readRange(i2c1);
        uint8_t status2 = VL6180X_readRangeStatus(i2c1);
        uint8_t range4 = VL6180X_readRange(i2c0);
        uint8_t status4 = VL6180X_readRangeStatus(i2c0);

        GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );
        GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW );


        Display_printf(display, 0, 0, "%d,%d,%d,%d,%d,%d,%d,%d",status1,range1,status2,range2,status3,range3,status4,range4);

    }
}
