/**
* @author Ioannis Kedros, Daniel Worrall
*
* @section LICENSE
*
* Copyright (c) 2011 mbed
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @section DESCRIPTION
*    Library for Dialog Semiconductor DA7212 library NXP LPC1768
*
*/ 

#include "mbed.h"
#include "DA7212.h"

DA7212::DA7212(PinName sda, PinName scl, int addr, PinName tx_sda, PinName tx_ws, PinName clk, PinName rx_sda, PinName rx_ws)
                         : mAddr(addr), mI2c_(sda, scl), mI2s_(tx_sda, tx_ws, clk, rx_sda, rx_ws){
    mI2c_.frequency(150000);
    reset();                                //TLV resets
    power(0x07);                            //Power Up the DA7212, but not the MIC, ADC and LINE
    format(16, STEREO);                     //16Bit I2S protocol format, STEREO
    frequency(44100);                       //Default sample frequency is 44.1kHz
    bypass(false);                          //Do not bypass device
    mute(false);                            //Not muted
    activateDigitalInterface_();            //The digital part of the chip is active
    outputVolume(0.7, 0.7);                 //Headphone volume to the default state
    rxBuffer = &mI2s_.rxBuffer[0];
} 
//Public Functions
/******************************************************
 * Function name:   inputVolume()
 *
 * Description:     set line in volume for left and right channels
 *
 * Parameters:      float leftVolumeIn, float rightVolumeIn
 * Returns:         int 0 (success), -1 (value out of range)
******************************************************/
int DA7212::inputVolume(float leftVolumeIn, float rightVolumeIn){
    //check values are in range
    if((leftVolumeIn < 0.0)||leftVolumeIn > 1.0) return -1;
    if((rightVolumeIn < 0.0)||rightVolumeIn > 1.0) return -1;
    //convert float to encoded char
    char left = (char)31*leftVolumeIn;          
    char right = (char)31*rightVolumeIn;
    //Left Channel
    cmd[1] = left | (0 << 7);                           //set volume
    cmd[0] = LEFT_LINE_INPUT_CHANNEL_VOLUME_CONTROL;    //set address
    mI2c_.write(mAddr, cmd, 2);                         //send
    //Right Channel
    cmd[1] = right | (0 << 7);                          //set volume
    cmd[0] = RIGHT_LINE_INPUT_CHANNEL_VOLUME_CONTROL;   //set address
    mI2c_.write(mAddr, cmd, 2);                         //send
    return 0;
}
/******************************************************
 * Function name:   outputVolume()
 *
 * Description:     Set headphone (line out) volume for left an right channels
 *
 * Parameters:      float leftVolumeOut, float rightVolumeOut
 * Returns:         int 0 (success), -1 (value out of range)
******************************************************/
int DA7212::outputVolume(float leftVolumeOut, float rightVolumeOut){
    //check values are in range
    if((leftVolumeOut < 0.0)||leftVolumeOut > 1.0) return -1;
    if((rightVolumeOut < 0.0)||rightVolumeOut > 1.0) return -1;
    //convert float to encoded char
    char left = (char)(79*leftVolumeOut)+0x30;
    char right = (char)(79*rightVolumeOut)+0x30;
    //Left Channel
    cmd[1] = left | (1 << 7);                           //set volume
    cmd[0] = LEFT_CHANNEL_HEADPHONE_VOLUME_CONTROL;     //set address
    mI2c_.write(mAddr, cmd, 2);                         //send
    //Right Channel
    cmd[1] = right | (1 << 7);                          //set volume
    cmd[0] = RIGHT_CHANNEL_HEADPHONE_VOLUME_CONTROL;    //set address
    mI2c_.write(mAddr, cmd, 2);                         //send
    return 0;
}
/******************************************************
 * Function name:   bypass()
 *
 * Description:     Send DA7212 into bypass mode, i.e. connect input to output 
 *
 * Parameters:      bool bypassVar
 * Returns:         none
******************************************************/
void DA7212::bypass(bool bypassVar){
    if(bypassVar == true)
        cmd[1] = (1 << 3) | (0 << 4) | (0 << 5);//bypass enabled, DAC disabled, sidetone insertion disabled
    else
        cmd[1] = (0 << 3) | (1 << 4);           //bypass disabled, DAC enabled
    cmd[1] |= (0 << 2);
    cmd[0] = ANALOG_AUDIO_PATH_CONTROL;         //set address
    mI2c_.write(mAddr, cmd, 2);                 //send
}
/******************************************************
 * Function name:   mute()
 *
 * Description:     Send DA7212 into mute mode
 *
 * Parameters:      bool softMute
 * Returns:         none
******************************************************/
void DA7212::mute(bool softMute){   
    if(softMute == true) cmd[1] = 0x08;         //set instruction to mute
    else cmd[1] = 0x00;                         //set instruction to NOT mute
     
    cmd[0] = DIGITAL_AUDIO_PATH_CONTROL;        //set address  
    mI2c_.write(mAddr, cmd, 2);                 //send
}    
/******************************************************
 * Function name:   power()
 *
 * Description:     Switch DA7212 on/off
 *
 * Parameters:      bool powerUp
 * Returns:         none
******************************************************/      
void DA7212::power(bool powerUp){
    if(powerUp == true) cmd[1] = 0x00;          //everything on
    else cmd[1] = 0xFF;                         //everything off
    
    cmd[0] = POWER_DOWN_CONTROL;                //set address
    mI2c_.write(mAddr, cmd, 2);                  //send
}
/******************************************************
 * Function name:   power()
 *
 * Description:     Switch on individual devices on DA7212
 *
 * Parameters:      int device
 * Returns:         none
******************************************************/
void DA7212::power(int device){
    cmd[1] = (char)device;                      //set user defined commands
    cmd[0] = POWER_DOWN_CONTROL;                //set address
    mI2c_.write(mAddr, cmd, 2);                 //send
}
/******************************************************
 * Function name:   format()
 *
 * Description:     Set interface format
 *
 * Parameters:      char length, bool mode
 * Returns:         none
******************************************************/      
void DA7212::format(char length, bool mode){  
    char modeSet = (1 << 6);   
    modeSet |= (1 << 5);                        //swap left and right channels
    
    switch (length)                             //input data into instruction byte
    {
        case 16:
            cmd[1] = modeSet | 0x02; 
            break;
        case 20:
            cmd[1] = modeSet | 0x06;
            break;
        case 24:
            cmd[1] = modeSet | 0x0A;
            break;
        case 32:
            cmd[1] = modeSet | 0x0E;
            break;
        default:
            break;
    }
    mI2s_.format(length, mode);
    cmd[0] = DIGITAL_AUDIO_INTERFACE_FORMAT;        //set address
    mI2c_.write(mAddr, cmd, 2);                     //send
}
/******************************************************
 * Function name:   frequency()
 *
 * Description:     Set sample frequency
 *
 * Parameters:      int hz
 * Returns:         int 0 (success), -1 (value not recognised)
******************************************************/
int DA7212::frequency(int hz){
    char rate;
    switch(hz){
        case 8000:
            rate = 0x03; 
            break;
        case 8021:
            rate = 0x0B;
            break;
        case 32000:
            rate = 0x06;
            break;
        case 44100:
            rate = 0x08; 
            break;
        case 48000:
            rate = 0x00; 
            break;
        case 88200:
            rate = 0x0F;  
            break;
        case 96000:
            rate = 0x07;
            break;
        default:
            return -1;
    }
    char clockInChar = (0 << 6);
    char clockModeChar = (1 << 0);

    cmd[1] = (rate << 2) | clockInChar | clockModeChar;      //input data into instruciton byte
    cmd[0] = SAMPLE_RATE_CONTROL;           //set address  
    mI2c_.write(mAddr, cmd, 2);              //send
    return 0;
}   
/******************************************************
 * Function name:   reset()
 *
 * Description:     Reset DA7212
 *
 * Parameters:      none
 * Returns:         none
******************************************************/        
void DA7212::reset(void){
    cmd[0] = RESET_REGISTER;                //set address
    cmd[1] = 0x00;                          //this resets the entire device
    mI2c_.write(mAddr, cmd, 2);               
}
/******************************************************
 * Function name:   start()
 *
 * Description:     Enable interrupts on the I2S port
 *
 * Parameters:      int mode
 * Returns:         none
******************************************************/
void DA7212::start(int mode){
    mI2s_.start(mode);
}
/******************************************************
 * Function name:   stop()
 *
 * Description:     Disable interrupts on the I2S port
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void DA7212::stop(void){
    mI2s_.stop();
}
/******************************************************
 * Function name:   write()
 *
 * Description:     Write (part of) a buffer to the I2S port
 *
 * Parameters:      int *buffer, int from, int length
 * Returns:         none
******************************************************/
void DA7212::write(int *buffer, int from, int length){
    mI2s_.write(buffer, from, length);
}
/******************************************************
 * Function name:   read()
 *
 * Description:     Place I2SRXFIFO in rxBuffer
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void DA7212::read(void){
    mI2s_.read();
}
/******************************************************
 * Function name:   attach()
 *
 * Description:     Attach a void/void function or void/void static member function to IRQHandler
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void DA7212::attach(void(*fptr)(void)){
    mI2s_.attach(fptr);
}
//Private Functions
/******************************************************
 * Function name:   setSampleRate_()
 *
 * Description:     Clocking control
 *
 * Parameters:      char rate, bool clockIn, bool clockMode, bool bOSR
 * Returns:         none
******************************************************/
void DA7212::setSampleRate_(char rate, bool clockIn, bool clockMode, bool bOSR){
    char clockInChar;
    char clockModeChar;
    char baseOverSamplingRate;
    if(bOSR){
        baseOverSamplingRate = (1 << 0);
    } else {
        baseOverSamplingRate = (0 << 0);
    }
    if(clockIn){
        clockInChar = (1 << 6);
    } else {
        clockInChar = (0 << 6);
    }
    if(clockMode){
        clockModeChar = 0x01;
    } else {
        clockModeChar = 0x00;
    }
    cmd[1] = (rate << 2) | clockInChar | clockModeChar | baseOverSamplingRate;      //input data into instruciton byte
    cmd[0] = SAMPLE_RATE_CONTROL;               //set address  
    mI2c_.write(mAddr, cmd, 2);                 //send
}
/******************************************************
 * Function name:   activateDigitalInterface_()
 *
 * Description:     Activate digital part of chip
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
void DA7212::activateDigitalInterface_(void){
    cmd[1] = 0x01;                          //Activate  
    cmd[0] = DIGITAL_INTERFACE_ACTIVATION;  //set address
    mI2c_.write(mAddr, cmd, 2);             //send
}
/******************************************************
 * Function name:   deactivateDigitalInterface_
 *
 * Description:     Deactivate digital part of chip
 *
 * Parameters:      none
 * Returns:         none
******************************************************/
//Digital interface deactivation 
void DA7212::deactivateDigitalInterface_(void){
    cmd[1] = 0x00;                          //Deactivate
    cmd[0] = DIGITAL_INTERFACE_ACTIVATION;  //set address
    mI2c_.write(mAddr, cmd, 2);             //send
}

