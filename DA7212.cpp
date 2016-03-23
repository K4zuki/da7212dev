/**
* @author Giles Barton-Owen; fork by Kazuki Yamamoto
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

#include "DA7212.h"

#define DA7212_HP_VOL_DF_MASK 0x80


#define DA7212_Default_HP_Volume_Left         (57)
#define DA7212_Default_HP_Volume_Right        (57)
#define DA7212_Default_LineIn_Volume_Left     (53)
#define DA7212_Default_LineIn_Volume_Right    (53)
#define DA7212_DF_sdt_vol                     0

const uint8_t base_address = 0x1A; //0x34 in 8bit address


DA7212::DA7212(PinName i2c_sda, PinName i2c_scl): i2c(i2c_sda,i2c_scl)  {
    address = base_address;
    defaulter();
    form_cmd(all);
}

// DA7212::DA7212(PinName i2c_sda, PinName i2c_scl, bool cs_level): i2c(i2c_sda,i2c_scl)  {
//     address = base_address + (1*cs_level);
//     defaulter();
//     form_cmd(all);
// }

void DA7212::power(bool on_off) {
    device_all_pwr = on_off;
    form_cmd(power_control);
}

void DA7212::input_select(int input) {

    switch(input)
    {
        // case DA7212_NO_IN:
        //     device_adc_pwr = false;
        //     device_mic_pwr = false;
        //     device_lni_pwr = false;
        //     form_cmd(power_control);
        //     break;
        case DA7212_LINE:
            device_adc_pwr  = true;
            device_lni_pwr  = true;
            device_mic_pwr  = false;
            ADC_source      = DA7212_LINE;
            form_cmd(power_control);
            form_cmd(path_analog);
            break;
        // case DA7212_MIC:
        //     device_adc_pwr = true;
        //     device_lni_pwr = false;
        //     device_mic_pwr = true;
        //     ADC_source = DA7212_MIC;
        //     form_cmd(power_control);
        //     form_cmd(path_analog);
        //     break;
        default:
            device_adc_pwr     = Default_device_adc_pwr;
            device_mic_pwr     = Default_device_mic_pwr;
            device_lni_pwr     = Default_device_lni_pwr;
            ADC_source         = Default_ADC_source;
            form_cmd(power_control);
            form_cmd(path_analog);
            break;
    }
    ADC_source_old = ADC_source;
}

void DA7212::headphone_volume(float h_volume) {
    hp_vol_left   = h_volume;
    hp_vol_right  = h_volume;
    form_cmd(headphone_vol_left);
    form_cmd(headphone_vol_right);
}
// void DA7212::headphone_volume(uint8_t h_volume) {
//     hp_vol_left  = h_volume & 0x3F;
//     hp_vol_right = h_volume & 0x3F;
//     form_cmd(headphone_vol_left);
//     form_cmd(headphone_vol_right);
// }

void DA7212::linein_volume(float LineIn_volume) {
    LineIn_vol_left   = LineIn_volume;
    LineIn_vol_right  = LineIn_volume;
    form_cmd(line_in_vol_left);
    form_cmd(line_in_vol_right);
}
// void DA7212::linein_volume(uint8_t LineIn_volume){
//     LineIn_vol_left  = LineIn_volume & 0x3F;
//     LineIn_vol_right = LineIn_volume & 0x3F;
//     form_cmd(line_in_vol_left);
//     form_cmd(line_in_vol_right);
// }

void DA7212::microphone_boost(bool mic_boost) {
    mic_boost_ = mic_boost;
}
// void DA7212::microphone_boost(uint8_t mic_boost) {
//     mic_boost_ = mic_boost & 0x07;
// }

void DA7212::input_mute(bool mute) {
    if(ADC_source == DA7212_MIC)
    {
        mic_mute = mute;
        form_cmd(path_analog);
    }
    else // DA7212_LINE
    {
        LineIn_mute_left    = mute;
        LineIn_mute_right   = mute;
        form_cmd(line_in_vol_left);
        form_cmd(line_in_vol_right);
    }
}

void DA7212::output_mute(bool mute) {
    out_mute = mute;
    form_cmd(path_digital);
}

void DA7212::input_power(bool on_off) {

    device_adc_pwr = on_off;

    if(ADC_source == DA7212_MIC)
    {
        device_mic_pwr = on_off;
        device_lni_pwr = false;
    }
    else
    {
        device_mic_pwr = false;
        device_lni_pwr = on_off;
    }

    form_cmd(power_control);
}

void DA7212::output_power(bool on_off) {
    device_dac_pwr = on_off;
    device_out_pwr = on_off;

    form_cmd(power_control);
}

void DA7212::wordsize(int words) {
    device_bitlength = words;
    form_cmd(interface_format);
}

void DA7212::master(bool master) {
    device_master = master;
    form_cmd(interface_format);
}

void DA7212::frequency(int freq) {
    ADC_rate = freq;
    DAC_rate = freq;
    form_cmd(sample_rate);
}

void DA7212::input_highpass(bool enabled) {
    ADC_highpass_enable = enabled;
    form_cmd(path_digital);
}

void DA7212::output_softmute(bool enabled) {
    out_mute = enabled;
    form_cmd(path_digital);
}

void DA7212::interface_switch(bool on_off) {
    device_interface_active = on_off;
    form_cmd(interface_activation);
}

void DA7212::sidetone(float sidetone_vol) {
    sdt_vol = sidetone_vol;
    form_cmd(path_analog);
}

void DA7212::deemphasis(char code) {
    de_emph_code = code & 0x03;
    form_cmd(path_digital);
}

void DA7212::reset() {
    form_cmd(reset_reg);
}

void DA7212::start() {
    interface_switch(true);
}

void DA7212::bypass(bool enable) {
    bypass_ = enable;
    form_cmd(path_analog);
}

void DA7212::stop() {
    interface_switch(false);
}

void DA7212::command(reg_address add, uint16_t cmd) {
    char temp[2];
    temp[0] = (char(add)<<1) | ((cmd >> 6) & 0x01);
    temp[1] = (cmd & 0xFF);
    i2c.write((address<<1), temp, 2);
}

void DA7212::form_cmd(reg_address add) {
    uint16_t cmd = 0;
    int temp = 0;
    bool mute;
    switch(add)
    {
        case line_in_vol_left:
            temp = int(LineIn_vol_left * 32) - 1;
            mute = LineIn_mute_left;

            if(temp < 0)
            {
                temp = 0;
                mute = true;
            }
            cmd = temp & 0x1F;
            cmd |= mute << 7;
            break;
        case line_in_vol_right:
            temp = int(LineIn_vol_right * 32) - 1;
            mute = LineIn_mute_right;
            if(temp < 0)
            {
                temp = 0;
                mute = true;
            }
            cmd = temp & 0x1F;
            cmd |= mute << 7;
            break;

        case headphone_vol_left:
            temp = int(hp_vol_left * 80) + 47;
            cmd = DA7212_HP_VOL_DF_MASK;
            cmd |= temp & 0x7F;
            break;
        case headphone_vol_right:
            temp = int(hp_vol_right * 80) + 47;
            cmd = DA7212_HP_VOL_DF_MASK;
            cmd |= temp & 0x7F;
            break;

        case path_analog:
            temp = int(sdt_vol * 5);
            char vol_code = 0;
            switch(temp)
            {
                case 5:
                    vol_code = 0x0C;
                    break;
                case 0:
                    vol_code = 0x00;
                    break;
                default:
                    vol_code = ((0x04 - temp)&0x07) | 0x08;
                    break;
            }
            cmd = vol_code << 5;
            cmd |= 1 << 4;
            cmd |= bypass_ << 3;
            cmd |= ADC_source << 2;
            cmd |= mic_mute << 1;
            cmd |= mic_boost_;
            break;

        case path_digital:
            cmd |= out_mute << 3;
            cmd |= ((de_emph_code & 0x3) << 1);
            cmd |= ADC_highpass_enable;
            break;

        case power_control:
            cmd |= !device_all_pwr << 7;
            cmd |= !device_clk_pwr << 6;
            cmd |= !device_osc_pwr << 5;
            cmd |= !device_out_pwr << 4;
            cmd |= !device_dac_pwr << 3;
            cmd |= !device_adc_pwr << 2;
            cmd |= !device_mic_pwr << 1;
            cmd |= !device_lni_pwr << 0;
            break;

        case interface_format:
            cmd |= device_master << 6;
            cmd |= device_lrswap << 5;
            cmd |= device_lrws     << 4;
            temp = 0;
            switch(device_bitlength)
            {
                case 16:
                    temp = 0;
                    break;
                case 20:
                    temp =  1;
                    break;
                case 24:
                    temp = 2;
                    break;
                case 32:
                    temp = 3;
                    break;
            }
            cmd |= (temp & 0x03) << 2;
            cmd |= (device_data_form & 0x03);
            break;

        case sample_rate:
            temp = gen_samplerate();
            cmd = device_usb_mode;
            cmd |= (temp & 0x03) << 1;
            cmd |= device_clk_in_div << 6;
            cmd |= device_clk_out_div << 7;
            break;

        case interface_activation:
            cmd = device_interface_active;
            break;

        case reset_reg:
            cmd = 0;
            break;

        case all:
            for( int i = line_in_vol_left; i <= reset_reg; i++)
            {
                form_cmd((reg_address)i);
            }
            break;
    }
    if(add != all) command(add , cmd);
}

void DA7212::defaulter() {
    hp_vol_left       = DA7212_Default_HP_Volume_Left;
    hp_vol_right      = DA7212_Default_HP_Volume_Right;
    LineIn_vol_left   = DA7212_Default_LineIn_Volume_Left;
    LineIn_vol_right  = DA7212_Default_LineIn_Volume_Right;
    sdt_vol           = DA7212_DF_sdt_vol;
    bypass_           = Default_bypass_;

    ADC_source      = Default_ADC_source;
    ADC_source_old  = Default_ADC_source;

    mic_mute          = Default_mic_mute;
    LineIn_mute_left  = Default_LineIn_mute_left;
    LineIn_mute_right = Default_LineIn_mute_right;


    mic_boost_            = Default_mic_boost_;
    out_mute              = Default_out_mute;
    de_emph_code          = Default_de_emph_code;
    ADC_highpass_enable   = Default_ADC_highpass_enable;

    device_all_pwr  = Default_device_all_pwr;
    device_clk_pwr  = Default_device_clk_pwr;
    device_osc_pwr  = Default_device_osc_pwr;
    device_out_pwr  = Default_device_out_pwr;
    device_dac_pwr  = Default_device_dac_pwr;
    device_adc_pwr  = Default_device_dac_pwr;
    device_mic_pwr  = Default_device_mic_pwr;
    device_lni_pwr  = Default_device_lni_pwr;

    device_master         = Default_device_master;
    device_lrswap         = Default_device_lrswap;
    device_lrws           = Default_device_lrws;
    device_bitlength      = Default_device_bitlength;


    ADC_rate  = Default_ADC_rate;
    DAC_rate  = Default_DAC_rate;

    device_interface_active  = Default_device_interface_active;
}

char DA7212::gen_samplerate() {
    char temp = 0;
    switch(ADC_rate)
    {
/*
        case 96000:
            temp = 0x0E;
            break;
        case 48000:
            temp = 0x00;
            if(DAC_rate == 8000) temp = 0x02;
            break;
*/
        case 32000:
            temp = 0x0C;
            break;
/*
        case 8000:
            temp = 0x03;
            if(DAC_rate == 48000) temp = 0x04;
            break;
            */
        default:
            temp = 0x0C;
            break;
    }
    return temp;
}
