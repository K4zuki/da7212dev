/**
* @author Giles Barton-Owen
*
* @section LICENSE
*
* Copyright (c) 2016 k4zuki
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
*    A Driver set for the I2C half of the DA7212
*
*/


#ifndef DA7212_H
#define DA7212_H
#include "mbed.h"

#define DA7212_CS_HIGH                  true
#define DA7212_CS_LOW                   false

#define DA7212_ON                       true
#define DA7212_OFF                      false

#define DA7212_MUTE                     true
#define DA7212_UNMUTE                   false

#define DA7212_MASTER                   true
#define DA7212_SLAVE                    false

#define DA7212_LINE                     0
#define DA7212_MIC                      1
#define DA7212_NO_IN                    -1

#define DA7212_DE_EMPH_DISABLED     0
#define DA7212_DE_EMPH_32KHZ        1
#define DA7212_DE_EMPH_44KHZ        2
#define DA7212_DE_EMPH_48KHZ        3

/** A class to control the I2C part of the DA7212
 *
 */
class DA7212
{
public:
    /** Create an instance of the DA7212 class
     *
     * @param i2c_sda The SDA pin of the I2C
     * @param i2c_scl The SCL pin of the I2C
     */
    DA7212(PinName i2c_sda, PinName i2c_scl);

    /** Create an instance of the DA7212 class
     *
     * @param i2c_sda The SDA pin of the I2C
     * @param i2c_scl The SCL pin of the I2C
     * @param cs_level The level of the CS pin on the DA7212
     */
    // DA7212(PinName i2c_sda, PinName i2c_scl, bool cs_level);

    /** Control the power of the device
     *
     * @param on_off The power state
     */
    void power(bool on_off);

    /** Control the input source of the device
     *
     * @param input Select the source of the input of the device: DA7212_LINE, DA7212_MIC, DA7212_NO_IN
     */
    void input_select(int input);

    /** Set the headphone volume
     *
     * @param h_volume The desired headphone volume: 0->1
     */
    void headphone_volume(float h_volume);
    /*
     * 1x(-57~+6):64
     * 0b111001 = 57 = 0 dB
    void headphone_volume(uint8_t h_volume);
    */

    /** Set the line in pre-amp volume
     *
     * @param LineIn_volume The desired line in volume: 0->1
     */
    void linein_volume(float LineIn_volume);
    /*
     * 150 x (-27~+36):64
     * 0b110101 = 53 = 0 dB (default)
    void linein_volume(uint8_t LineIn_volume);
    */

    /** Turn on/off the microphone pre-amp boost
     *
     * @param mic_boost Boost on or off
     */
    void microphone_boost(bool mic_boost);
/*
600 x (-1~+6):8
0b001 = 1 = 0 dB (default)
*/

    /** Mute the input
     *
     * @param mute Mute on/off
     */
    void input_mute(bool mute);

    /** Mute the output
     *
     * @param mute Mute on/off
     */
    void output_mute(bool mute);

    /** Turn on/off the input stage
     *
     * @param on_off Input stage on(true)/off(false)
     */
    void input_power(bool on_off);

    /** Turn on/off the output stage
     *
     * @param on_off Output stage on(true)/off(false)
     */
    void output_power(bool on_off);

    /** Select the word size
     *
     * @param words 16/20/24/32 bits
     */
    void wordsize(int words);

    /** Select interface mode: Master or Slave
     *
     * @param master Interface mode: master(true)/slave
     */
    void master(bool master);

    /** Select the sample rate
     *
     * @param freq Frequency: 96/48/32/8 kHz
     */
    void frequency(int freq);

    /** Enable the input highpass filter
     *
     * @param enabled Input highpass filter enabled
     */
    void input_highpass(bool enabled);

    /** Enable the output soft mute
     *
     * @param enabled Output soft mute enabled
     */
    void output_softmute(bool enabled);

    /** Turn on and off the I2S
     *
     * @param on_off Switch the I2S interface on(true)/off(false)
     */
    void interface_switch(bool on_off);

    /** Reset the device and settings
     *
     */
    void reset();

    /** Set the microphone sidetone volume
     *
     * @param sidetone_volume The volume of the sidetone: 0->1
     * ;does not exist in 7212?
     */
    void sidetone(float sidetone_vol);

    /** Set the analog bypass
     *
     * @param bypass_en Enable the bypass: enabled(true)
     */
    void bypass(bool bypass_en);

    /** Set the deemphasis frequency
     *
     * @param code The deemphasis code: DA7212_DE_EMPH_DISABLED, DA7212_DE_EMPH_32KHZ, DA7212_DE_EMPH_44KHZ, DA7212_DE_EMPH_48KHZ
     */
    void deemphasis(char code);

    /** Enable the input highpass filter
     *
     * @param enable Enable the input highpass filter enabled(true)
     */

    void adc_highpass(bool enable);

    /** Start the device sending/recieving etc
    */
    void start();

    /** Stop the device sending/recieving etc
    */
    void stop();

private:

    enum reg_address {
        line_in_vol_left        = 0x00,
        line_in_vol_right       = 0x01,
        headphone_vol_left      = 0x02,
        headphone_vol_right     = 0x03,
        path_analog             = 0x04,
        path_digital            = 0x05,
        power_control           = 0x06,
        interface_format        = 0x07,
        sample_rate             = 0x08,
        interface_activation    = 0x09,
        reset_reg               = 0x0A,
        all                     = 0xFF
    };

    enum DA7212_defaults {
        Default_bypass_             = 0,
        Default_ADC_source          = DA7212_LINE,
        Default_mic_mute            = DA7212_UNMUTE,
        Default_LineIn_mute_left    = 0,
        Default_LineIn_mute_right   = 0,
        Default_mic_boost_          = 0,
        Default_out_mute            = DA7212_UNMUTE,

        Default_de_emph_code         = 0x00,
        Default_ADC_highpass_enable  = 0,

        Default_device_all_pwr     = 1,
        Default_device_clk_pwr     = 1,
        Default_device_osc_pwr     = 1,
        Default_device_out_pwr     = 1,
        Default_device_dac_pwr     = 1,
        Default_device_adc_pwr     = 1,
        Default_device_mic_pwr     = 0,
        Default_device_lni_pwr     = 1,

        Default_device_master         = 0,
        Default_device_lrswap         = 0,
        Default_device_lrws           = 0,
        Default_device_bitlength      = 32,

        Default_ADC_rate            = 32000,
        Default_DAC_rate            = 32000,

        Default_device_interface_active = 0
    };


    I2C i2c;
    uint8_t address;
    void command(reg_address add, uint16_t byte);
    void form_cmd(reg_address add);
    void defaulter();

    char gen_samplerate();

    //I2S i2s_tx(I2S_TRANSMIT, p5, p6 , p7);
    //I2S i2s_rx(I2S_RECIEVE , p8, p29, p30);

    float hp_vol_left, hp_vol_right;
    float LineIn_vol_left, LineIn_vol_right;
    float sdt_vol;
    bool LineIn_mute_left, LineIn_mute_right;
    bool bypass_;
    bool ADC_source;
    bool ADC_source_old;
    bool mic_mute;
    bool mic_boost_;
    bool out_mute;
    char de_emph_code;
    bool ADC_highpass_enable;

    bool device_all_pwr;
    bool device_clk_pwr;
    bool device_osc_pwr;
    bool device_out_pwr;
    bool device_dac_pwr;
    bool device_adc_pwr;
    bool device_mic_pwr;
    bool device_lni_pwr;

    bool device_master;
    bool device_lrswap;
    bool device_lrws;
    char device_bitlength;
    static const char device_data_form = 0x02;

    int ADC_rate;
    int DAC_rate;
    static const bool device_usb_mode = false;
    static const bool device_clk_in_div = false;
    static const bool device_clk_out_div = false;
    bool device_interface_active;

};


#endif
