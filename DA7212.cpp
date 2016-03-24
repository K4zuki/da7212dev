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

const uint8_t base_address = 0x34; //0x1A in 7bit address


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

//     case DA721X_HP:
//         /*
//         1x(-57~+6):64
//         0b111001 = 57 = 0 dB
//         */
//         i2c_reg_update_bits(REG_HP_L_GAIN, 0x3F, (vol*100-HP_PGA_MIN)/OUT_PGA_STEP);
//         i2c_reg_update_bits(REG_HP_R_GAIN, 0x3F, (vol*100-HP_PGA_MIN)/OUT_PGA_STEP);
//         break;

// void DA7212::headphone_volume(float h_volume) {
//     hp_vol_left   = h_volume;
//     hp_vol_right  = h_volume;
//     form_cmd(headphone_vol_left);
//     form_cmd(headphone_vol_right);
// }
void DA7212::headphone_volume(uint8_t h_volume) {
    hp_vol_left  = h_volume & 0x3F;
    hp_vol_right = h_volume & 0x3F;
    i2c_register_write(REG_HP_L_GAIN, hp_vol_left  );
    i2c_register_write(REG_HP_R_GAIN, hp_vol_right );
}

// void DA7212::linein_volume(float LineIn_volume) {
//     LineIn_vol_left   = LineIn_volume;
//     LineIn_vol_right  = LineIn_volume;
//     form_cmd(line_in_vol_left);
//     form_cmd(line_in_vol_right);
// }
// /**
// 150 x (-27~+36):64
// 0b110101 = 53 = 0 dB (default)
// */
void DA7212::linein_volume(uint8_t LineIn_volume){
    LineIn_vol_left  = LineIn_volume & 0x3F;
    LineIn_vol_right = LineIn_volume & 0x3F;
    i2c_register_write(REG_AUX_L_GAIN, LineIn_vol_left );
    i2c_register_write(REG_AUX_R_GAIN, LineIn_vol_right);
}

// void DA7212::microphone_boost(bool mic_boost) {
//     mic_boost_ = mic_boost;
// }
void DA7212::microphone_boost(uint8_t mic_boost) {
    mic_boost = mic_boost & 0x07;
    i2c_register_write(REG_MIC_1_GAIN, mic_boost);
}
//     case DA721X_MIC1: // headset mic
//         /*
//         600 x (-1~+6):8
//         0b001 = 1 = 0 dB (default)
//         */
//         i2c_reg_update_bits(REG_MIC_1_GAIN, 0x07, (vol*100-MIC_PGA_MIN)/MIC_PGA_STEP);
//         break;
//     case DA721X_MIC2: // onboard unpop mic
//         /*
//         600 x (-1~+6):8
//         0b001 = 1 = 0 dB (default)
//         */
//         i2c_reg_update_bits(REG_MIC_2_GAIN, 0x07, (vol*100-MIC_PGA_MIN)/MIC_PGA_STEP);
//         break;

// int da7212_mute_control(enum da7212_endpoint endpoint,int mute)
// {
//     printf("da7212_dac_mute_control(%s)\n",mute==1?"mute":mute==0?"Unmute":"unknown");
//     switch(endpoint) {
//         case DA721X_MIC1:
//             i2c_reg_update_bits(REG_MIC1_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_MIC2_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
//         case DA721X_ADC:
//             i2c_reg_update_bits(REG_ADC_L_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_ADC_R_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
//         case DA721X_DAC:
// #ifdef DA7212_SOFTMUTE_EN
//             i2c_register_write(REG_DAC_FILTERS5, (mute ? 0x80 : 0x00));    //SOFT MUTE ON! for DAC
// #else
//             i2c_reg_update_bits(REG_DAC_L_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_DAC_R_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
// #endif
//             break;
//         case DA721X_HP:
//             i2c_reg_update_bits(REG_HP_L_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_HP_R_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
//         case DA721X_SPEAKER:
//             i2c_reg_update_bits(REG_LINE_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
//         default:
//         break;
//     }
//     //delay HERE if need to remove pop noise.
//     return 0;
// }

void DA7212::input_mute(bool mute) {
    if(ADC_source == DA7212_MIC)
    {
        mic_mute = mute;
        form_cmd(path_analog);
//         case DA721X_MIC1:
//             i2c_reg_update_bits(REG_MIC1_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_MIC2_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
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
//         case DA721X_DAC:
// #ifdef DA7212_SOFTMUTE_EN
//             i2c_register_write(REG_DAC_FILTERS5, (mute ? 0x80 : 0x00));    //SOFT MUTE ON! for DAC
// #else
//             i2c_reg_update_bits(REG_DAC_L_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_DAC_R_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
// #endif
//             break;
//         case DA721X_HP:
//             i2c_reg_update_bits(REG_HP_L_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             i2c_reg_update_bits(REG_HP_R_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
//         case DA721X_SPEAKER:
//             i2c_reg_update_bits(REG_LINE_CTRL, DA721X_MUTE_EN, (mute ? DA721X_MUTE_EN : 0));
//             break;
//         default:
//         break;
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
    Sample_rate = freq;
    // ADC_rate = freq;
    // DAC_rate = freq;
    form_cmd(sample_rate);
}

// void DA7212::input_highpass(bool enabled) {
//     ADC_highpass_enable = enabled;
//     form_cmd(path_digital);
// }

void DA7212::output_softmute(bool enabled) {
    out_mute = enabled;
    // form_cmd(path_digital);
    i2c_register_write(REG_DAC_FILTERS5, (enabled ? 0x80 : 0x00));    //SOFT MUTE ON! for DAC
}

void DA7212::interface_switch(bool on_off) {
    device_interface_active = on_off;
    form_cmd(interface_activation);
}

// void DA7212::sidetone(float sidetone_vol) {
//     sdt_vol = sidetone_vol;
//     form_cmd(path_analog);
// }

// void DA7212::deemphasis(char code) {
//     de_emph_code = code & 0x03;
//     form_cmd(path_digital);
// }

void DA7212::reset() {
    form_cmd(reset_reg);
}

void DA7212::start() {
    interface_switch(true);
}

// void DA7212::bypass(bool enable) {
//     bypass_ = enable;
//     form_cmd(path_analog);
// }

void DA7212::stop() {
    interface_switch(false);
}

void DA7212::command(reg_address add, uint16_t cmd) {
    char temp[2];
    temp[0] = (char(add)<<1) | ((cmd >> 6) & 0x01);
    temp[1] = (cmd & 0xFF);
    i2c.write((address<<1), temp, 2);
}

void DA7212::i2c_register_write(DA7212Registers register, uint8_t command){
    char temp[2];
    temp[0] = (char)register;
    temp[1] = (char)command;
    i2c.write((address | 0), (const char*)temp, 2);
}

uint8_t DA7212::i2c_register_read(DA7212Registers register){
    char temp = (char)register;
    i2c.write((address | 0), (const char*)temp, 1, true); //will do repeated start
    i2c.read((address | 1), &temp, 1);
    return temp;
}

// int da7212_set_vol_dB(enum da7212_endpoint endpoint, int vol)
// {
//     printf("Endpoint(%d):%ddB\n",endpoint, vol);
//     if(volume_check(endpoint, vol) < 0) {
//         return -1;
//     }
//
//     switch(endpoint) {
//     case DA721X_MIC1:
//         /*
//         600 x (-1~+6):8
//         0b001 = 1 = 0 dB (default)
//         */
//         i2c_reg_update_bits(REG_MIC_1_GAIN, 0x07, (vol*100-MIC_PGA_MIN)/MIC_PGA_STEP);
//         break;
//     case DA721X_MIC2:
//         /*
//         600 x (-1~+6):8
//         0b001 = 1 = 0 dB (default)
//         */
//         i2c_reg_update_bits(REG_MIC_2_GAIN, 0x07, (vol*100-MIC_PGA_MIN)/MIC_PGA_STEP);
//         break;
//     case DA721X_AUX:
//         /*
//         150 x (-3~12):16
//         0b0011 = 3 = 0 dB
//         */
//         i2c_reg_update_bits(REG_AUX_L_GAIN, 0x3F, 17+(vol*100-AUX_PGA_MIN)/AUX_PGA_STEP);
//         i2c_reg_update_bits(REG_AUX_R_GAIN, 0x3F, 17+(vol*100-AUX_PGA_MIN)/AUX_PGA_STEP);
//         break;
//     case DA721X_MIXIN:
//         /*
//         150 x (-3~12):16
//         0b0011 = 3 = 0 dB
//         */
//         i2c_reg_update_bits(REG_MIXIN_L_GAIN, 0x0F, (vol*100-MIXIN_PGA_MIN)/MIXIN_PGA_STEP);
//         i2c_reg_update_bits(REG_MIXIN_R_GAIN, 0x0F, (vol*100-MIXIN_PGA_MIN)/MIXIN_PGA_STEP);
//         break;
//     case DA721X_ADC:
//         /*
//         75 x (-111~16):128
//         0b1101111 = 111(decimal!) = 0 dB (default)
//         */
//         i2c_reg_update_bits(REG_ADC_L_GAIN, 0x7F, 8+(vol*100-ADC_PGA_MIN)/DIGITAL_PGA_STEP);
//         i2c_reg_update_bits(REG_ADC_R_GAIN, 0x7F, 8+(vol*100-ADC_PGA_MIN)/DIGITAL_PGA_STEP);
//         break;
//     case DA721X_DAC:
//         /*
//         75 x (-111~16):128
//         0b1101111 = 111(decimal!) = 0 dB (default)
//         */
//         i2c_reg_update_bits(REG_DAC_L_GAIN, 0x7F, 8+(vol*100-DAC_PGA_MIN)/DIGITAL_PGA_STEP);
//         i2c_reg_update_bits(REG_DAC_R_GAIN, 0x7F, 8+(vol*100-DAC_PGA_MIN)/DIGITAL_PGA_STEP);
//         break;
//     case DA721X_HP:
//         /*
//         1x(-57~+6):64
//         0b111001 = 57 = 0 dB
//         */
//         i2c_reg_update_bits(REG_HP_L_GAIN, 0x3F, (vol*100-HP_PGA_MIN)/OUT_PGA_STEP);
//         i2c_reg_update_bits(REG_HP_R_GAIN, 0x3F, (vol*100-HP_PGA_MIN)/OUT_PGA_STEP);
//         break;
//     case DA721X_SPEAKER:
//         /*
//         1x(-48~+15):64
//         0b110000 = 48 = 0 dB
//         */
//         i2c_reg_update_bits(REG_LINE_GAIN, 0x3F, (vol*100-SPK_PGA_MIN)/OUT_PGA_STEP);
//         break;
//     default:
//         break;
//     }
//     return 0;
// }

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
// 0x29 DAI_CTRL| DAI_EN[7]| DAI_OE[6] |DAI_TDM_MODE_EN[5]| DAI_MONO_MODE_EN[4]| DAI_WORD_LENGTH[3..2]| DAI_FORMAT[1..0]|
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

    Sample_rate = Default_Sample_rate;
    // ADC_rate  = Default_ADC_rate;
    // DAC_rate  = Default_DAC_rate;

    device_interface_active  = Default_device_interface_active;
}

char DA7212::gen_samplerate() {
    char temp = 0;
    switch(Sample_rate)
    {
/*

        case 96000:
        // DA721X_SR_96000     =(0xF << 0),
            temp = 0x0E;
            break;
        case 88200:
        // DA721X_SR_88200     =(0xE << 0),
            temp = 0x0E;
            break;
        case 48000:
        // DA721X_SR_48000     =(0xB << 0), //REG_MIXED_SAMPLE_MODE = 1; other=0
            temp = 0x00;
            if(DAC_rate == 8000) temp = 0x02;
            break;
        case 44100:
        // DA721X_SR_44100     =(0xA << 0),
            temp = 0x0E;
            break;
*/
        case 32000:
            // DA721X_SR_32000     =(0x9 << 0),
            temp = 0x0C;
            break;
/*
        case 24000:
        // DA721X_SR_24000     =(0x7 << 0),
            temp = 0x0E;
            break;
        case 22050:
        // DA721X_SR_22050     =(0x6 << 0),
            temp = 0x0E;
            break;
        case 16000:
        // DA721X_SR_16000     =(0x5 << 0),
            temp = 0x0E;
            break;
        case 12000:
        // DA721X_SR_12000     =(0x3 << 0),
            temp = 0x0E;
            break;
        case 11025:
        // DA721X_SR_11025     =(0x2 << 0),
            temp = 0x0E;
            break;
        case 8000:
            // DA721X_SR_8000      =(0x1 << 0),
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

// int da7212_I2Splayback(void)    //Init path from Digital Audio Interface to HP or SPK
// {
//     printf("Start da7212_I2Splayback!\n");
//     i2c_register_write(REG_GAIN_RAMP_CTRL   , 0x00); //Set Ramp rate to default 0x92 GAIN_RAMP_CTRL
//     i2c_register_write(REG_REFERENCES       , 0x08); //Enable master bias
//     wait_ms(40); //40ms delay
//     i2c_register_write( REG_LDO_CTRL         , 0x80); //Enable Digital LDO
//
//     i2c_register_write( REG_DAI_CTRL         , 0xC0); //Enable AIF 16bit I2S mode
//     i2c_register_write( REG_PC_COUNT         , 0x02); //Set PC sync to resync
//     i2c_register_write( REG_DAC_FILTERS5     , 0x80); //SOFT MUTE ON!
//     i2c_register_write( REG_DIG_ROUTING_DAC  , 0x32); //DACR and DACL source.
//     //    i2c_register_write(REG_MIXOUT_L_SELECT, 0x08); //MIXOUT_L input from DACL
//     i2c_register_write( REG_MIXOUT_R_SELECT  , 0x08); //MIXOUT_R input from DACR
// #ifdef DA7212_HP
//     i2c_register_write( REG_CP_CTRL          , 0xF1); // CP_CTRL - Signal size+Boost
//     i2c_register_write( REG_CP_VOL_THRESHOLD1, 0x36); // CP_VOL_THRESHOLD
//     i2c_register_write( REG_CP_DELAY         , 0xA5); // CP_DELAY
//     i2c_register_write( REG_HP_L_GAIN        , 0x39); // Set HP_L gain to 0dB
//     i2c_register_write( REG_HP_R_GAIN        , 0x39); // Set HP_R gain to 0dB
//
//     i2c_register_write( REG_DAC_L_CTRL   , 0x80); // DAC_L
//     i2c_register_write( REG_DAC_R_CTRL   , 0x80); // DAC_R
//     i2c_register_write( REG_HP_L_CTRL    , 0xA8); // HP_L
//     i2c_register_write( REG_HP_R_CTRL    , 0xA8); // HP_R
//     i2c_register_write( REG_MIXOUT_L_CTRL, 0x88); // MIXOUT_L
//     i2c_register_write( REG_MIXOUT_R_CTRL, 0x88); // MIXOUT_R
//
//     //if use DA7212_SYSTEM_MODE
//     i2c_register_write( REG_SYSTEM_MODES_OUTPUT  , 0xF1); // Enable DAC, HP
// #else // DA7212_SPK
//     i2c_register_write( REG_LINE_GAIN            , 0x30); // SPEAKER GAIN 0dB
//
//     //    i2c_register_write(REG_DAC_L_CTRL     , 0x80); // DAC_L
//     i2c_register_write( REG_DAC_R_CTRL       , 0x80); // DAC_R
//     i2c_register_write( REG_LINE_CTRL        , 0xA8); // SPEAKER
//     //    i2c_register_write(REG_MIXOUT_L_CTRL  , 0x88); // MIXOUT_L
//     i2c_register_write(REG_MIXOUT_R_CTRL    , 0x88); // MIXOUT_R
//
//     //if use DA7212_SYSTEM_MODE
//     //i2c_register_write(REG_SYSTEM_MODES_OUTPUT, 0xC9); // SET Enable DAC, SPEAKER(LINE)
// #endif
//     i2c_register_write( REG_TONE_GEN_CFG2, 0x60); // Tone generater.
//     da7212_set_clock( CONFIG_SAMPLE_RATE );
//     da7212_set_dai_fmt( CPU_I2S_MASTER );
//     wait_ms(40); //40ms delay
//     i2c_register_write(REG_DAC_FILTERS5, 0x00);    //SOFT MUTE OFF!
//     return 0;
// }
//
// int da7212_duplex(void)    //Init path from MIC1 to Digital Audio Interface
// {
//     //DEBUG_PRINTF("---ticks : %d\n",ticks);
//     i2c_register_write(REG_REFERENCES       , 0x08); //Enable master bias
//     i2c_register_write(REG_GAIN_RAMP_CTRL   , 0x02); //Set Ramp rate to 1S
//     wait_ms(40); //40ms delay
//     i2c_register_write(REG_LDO_CTRL         , 0x80); //Enable Digital LDO
//     da7212_set_clock(CONFIG_SAMPLE_RATE);
//     da7212_set_dai_fmt(CPU_I2S_MASTER);
//     i2c_register_write( REG_DAI_CTRL        , 0xC0); //Enable AIF 16bit I2S mode
//     i2c_register_write( REG_PC_COUNT        , 0x02); //Set PC sync to resync
//     i2c_register_write( REG_DIG_ROUTING_DAI , 0x10); //DIG_ROUTING_DAI, from DAI L/R
//     i2c_register_write( REG_MICBIAS_CTRL    , 0x0A); //Enable MICBIAS1
//     i2c_register_write( REG_MIC_1_CTRL      , 0x84); //Set Mic1 to be single ended from MIC1_SE connector
//     i2c_register_write( REG_MIXIN_L_SELECT  , 0x02); //MIXIN_L input from MIC1
//     i2c_register_write( REG_MIXIN_R_SELECT  , 0x04); //MIXIN_R input from MIC1
//     i2c_register_write( REG_GAIN_RAMP_CTRL  , 0x00); //Set Ramp rate to default
//
//     i2c_register_write( REG_MIXIN_L_CTRL    , 0x88); //Enable MIXIN Left
//     i2c_register_write( REG_MIXIN_R_CTRL    , 0x88); //Enable MIXIN Left
//     i2c_register_write( REG_ADC_L_CTRL      , 0xA0); //Enable ADC Left and unmute.
//     i2c_register_write( REG_ADC_R_CTRL      , 0xA0); //Enable ADC Right
//
//     da7212_I2Splayback();
//     //DEBUG_PRINTF("---ticks : %d\n",ticks);
//
//     printf("Start da7212_duplex!\n");
//     return 0;
// }
//
// int da7212_record(void)    //Init path from MIC1 to Digital Audio Interface
// {
//     i2c_register_write(REG_REFERENCES       , 0x08); //Enable master bias
//     i2c_register_write(REG_GAIN_RAMP_CTRL   , 0x02); //Set Ramp rate to 1S
//     wait_ms(40); //40ms delay
//     i2c_register_write(REG_LDO_CTRL         , 0x80); //Enable Digital LDO
//     da7212_set_clock(CONFIG_SAMPLE_RATE);
//     da7212_set_dai_fmt(CPU_I2S_MASTER);
//     i2c_register_write( REG_DAI_CTRL        , 0xC0); //Enable AIF 16bit I2S mode
//     i2c_register_write( REG_PC_COUNT        , 0x02); //Set PC sync to resync
//     i2c_register_write( REG_DIG_ROUTING_DAI , 0x10); //DIG_ROUTING_DAI, from DAI L/R
//     i2c_register_write( REG_MICBIAS_CTRL    , 0x0A); //Enable MICBIAS1
//     i2c_register_write( REG_MIC_1_CTRL      , 0x84); //Set Mic1 to be single ended from MIC1_SE connector
//     i2c_register_write( REG_MIXIN_L_SELECT  , 0x02); //MIXIN_L input from MIC1
//     i2c_register_write( REG_MIXIN_R_SELECT  , 0x04); //MIXIN_R input from MIC1
//     i2c_register_write( REG_GAIN_RAMP_CTRL  , 0x00); //Set Ramp rate to default
// #ifndef DA7212_SYSTEM_MODE
//     i2c_register_write( REG_MIXIN_L_CTRL  , 0x88); //Enable MIXIN Left
//     i2c_register_write( REG_MIXIN_R_CTRL  , 0x88); //Enable MIXIN Left
//     i2c_register_write( REG_ADC_L_CTRL    , 0xA0); //Enable ADC Left and unmute.
//     i2c_register_write( REG_ADC_R_CTRL    , 0xA0); //Enable ADC Right
// #else
//     i2c_register_write( REG_SYSTEM_MODES_INPUT  , 0xF4); //Enable MIXIN,ADC.
// #endif
//     return 0;
// }

// /* Supported PLL input frequencies are 5MHz - 54MHz. */
//
// int da7212_set_dai_pll(uint8_t sampling_rate) {
//     uint8_t pll_ctrl, indiv_bits, indiv;
//     uint8_t pll_frac_top, pll_frac_bot, pll_integer;
//     uint8_t use_pll = 0x80; //ENABLE PLL
//     /* Reset PLL configuration */
//     i2c_register_write(REG_DAC_FILTERS5, 0x80);    //SOFT MUTE ON!
//
//     i2c_register_write(REG_PLL_CTRL, 0); //  system clock is MCLK; SRM disabled; 32 kHz mode disabled; squarer at the MCLK disabled; input clock range for the PLL= 2 - 10 MHz

//
//     if (i2c_register_write(REG_SR, sampling_rate) < 0) {
//         printf("codec_set_sample_rate: error in write reg .\n");
//         return-1;
//     }
//
//     pll_ctrl = 0;
//     /* Workout input divider based on MCLK rate */
//     if (DA721X_MCLK < 5000000) {
//         goto pll_err;
//     } else if (DA721X_MCLK <= 10000000) {
//         indiv_bits = DA721X_PLL_INDIV_5_10_MHZ;
//         indiv = DA721X_PLL_INDIV_5_10_MHZ_VAL;
//     } else if (DA721X_MCLK <= 20000000) {
//         indiv_bits = DA721X_PLL_INDIV_10_20_MHZ;
//         indiv = DA721X_PLL_INDIV_10_20_MHZ_VAL;
//     } else if (DA721X_MCLK <= 40000000) {
//         indiv_bits = DA721X_PLL_INDIV_20_40_MHZ;
//         indiv = DA721X_PLL_INDIV_20_40_MHZ_VAL;
//     } else if (DA721X_MCLK <= 54000000) {
//         indiv_bits = DA721X_PLL_INDIV_40_54_MHZ;
//         indiv = DA721X_PLL_INDIV_40_54_MHZ_VAL;
//     } else if(DA721X_MCLK == 12288000){
//         use_pll = 0;
//     } else {
//         goto pll_err;
//     }
//     pll_ctrl |= indiv_bits;
//
//     /*
//      * If Codec is slave and SRM enabled,
//      */
//     if (CPU_I2S_MASTER && DA721X_SRM_EN) {
//         pll_ctrl |= DA721X_PLL_SRM_EN;
//         pll_frac_top = 0x0D;
//         pll_frac_bot = 0xFA;
//         pll_integer = 0x1F;
//     }else{
//         switch (sampling_rate) {
//         case DA721X_SR_8000:
//         case DA721X_SR_12000:
//         case DA721X_SR_16000:
//         case DA721X_SR_24000:
//         case DA721X_SR_32000:
//         case DA721X_SR_48000:
//         case DA721X_SR_96000:
//             pll_frac_top = 0x18;
//             pll_frac_bot = 0x93;
//             pll_integer = 0x20;
//             break;
//         case DA721X_SR_11025:
//         case DA721X_SR_22050:
//         case DA721X_SR_44100:
//         case DA721X_SR_88200:
//             pll_frac_top = 0x03;
//             pll_frac_bot = 0x61;
//             pll_integer = 0x1E;
//             break;
//         default:
//             printf("codec_set_sample_rate: invalid parameter. \n");
//             return -1;
//         }
//     }
//
//     /* Write PLL dividers */
//     i2c_register_write(REG_PLL_FRAC_TOP, pll_frac_top);
//     i2c_register_write(REG_PLL_FRAC_BOT, pll_frac_bot);
//     i2c_register_write(REG_PLL_INTEGER, pll_integer);
//
//     /* Enable MCLK squarer if required */
// #if (DA721X_MCLK_SQR_EN)
//     pll_ctrl |= DA721X_PLL_MCLK_SQR_EN;
// #endif
//     /* Enable PLL */
//     pll_ctrl |= use_pll;
//     i2c_register_write(REG_PLL_CTRL, pll_ctrl);
//     wait_us(10*1000); //10ms delay
//     i2c_register_write(REG_DAC_FILTERS5, 0x00);    //SOFT MUTE OFF!
//     return 0;
//
// pll_err:
//     printf("Unsupported PLL input frequency %d\n",DA721X_MCLK);
//     return -1;
// }
