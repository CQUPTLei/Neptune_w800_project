
/*****************************************************************************
 *
 * File Name : wm_adc.c
 *
 * Description: adc Driver Module
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * Author : dave
 *
 * Date : 2014-8-15
 *****************************************************************************/
#include "wm_adc.h"
#include "wm_dma.h"
#include "wm_gpio_afsel.h"
#include "wm_io.h"
#include "wm_irq.h"
#include "wm_regs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ATTRIBUTE_ISR __attribute__((isr))

// TODO
#define HR_SD_ADC_CONFIG_REG 0
static int adc_offset = 0;

volatile ST_ADC gst_adc;

ATTRIBUTE_ISR void ADC_IRQHandler(void)
{
    u32 adcvalue;
    int reg;

    reg = tls_reg_read32(HR_SD_ADC_INT_STATUS);
    if (reg & ADC_INT_MASK) // ADC�ж�
    {
        tls_adc_clear_irq(ADC_INT_TYPE_ADC);

        if (gst_adc.adc_cb) {
            adcvalue = tls_read_adc_result();
            gst_adc.adc_cb(&adcvalue, 1);
        }
    }
    if (reg & CMP_INT_MASK) {

        tls_adc_clear_irq(ADC_INT_TYPE_ADC_COMP);
        if (gst_adc.adc_bigger_cb)
            gst_adc.adc_bigger_cb(NULL, 0);
    }
}

static void adc_dma_isr_callbk(void)
{
    if (gst_adc.adc_dma_cb)
        gst_adc.adc_dma_cb((u16*)(ADC_DEST_BUFFER_DMA), gst_adc.valuelen);
}

void tls_adc_init(u8 ifusedma, u8 dmachannel)
{
    tls_reg_write32(HR_SD_ADC_CTRL, ANALOG_SWITCH_TIME_VAL(0x50) | ANALOG_INIT_TIME_VAL(0x50) | ADC_IRQ_EN_VAL(0x1));
    tls_irq_enable(ADC_IRQn);

    //ע���жϺ�channel�йأ�������Ҫ������
    if (ifusedma) {
        gst_adc.dmachannel = tls_dma_request(dmachannel, 0); //����dma����Ҫֱ��ָ������Ϊ�����dma���ܻᱻ�������ʹ��
        tls_dma_irq_register(gst_adc.dmachannel, (void (*)(void*))adc_dma_isr_callbk, NULL, TLS_DMA_IRQ_TRANSFER_DONE);
    }

    // printf("\ndma channel = %d\n",gst_adc.dmachannel);
}

void tls_adc_clear_irq(int inttype)
{
    int reg;
    reg = tls_reg_read32(HR_SD_ADC_INT_STATUS);
    if (ADC_INT_TYPE_ADC == inttype) {
        reg |= ADC_INT_MASK;
        tls_reg_write32(HR_SD_ADC_INT_STATUS, reg);
    } else if (ADC_INT_TYPE_ADC_COMP == inttype) {
        reg |= CMP_INT_MASK;
        tls_reg_write32(HR_SD_ADC_INT_STATUS, reg);
    } else if (ADC_INT_TYPE_DMA == inttype) {
        tls_dma_irq_clr(gst_adc.dmachannel, TLS_DMA_IRQ_TRANSFER_DONE);
    }
}

void tls_adc_irq_register(int inttype, void (*callback)(u32* buf, u16 len))
{
    if (ADC_INT_TYPE_ADC == inttype) {
        gst_adc.adc_cb = callback;
    } else if (ADC_INT_TYPE_DMA == inttype) {
        gst_adc.adc_dma_cb = callback;
    } else if (ADC_INT_TYPE_ADC_COMP == inttype) {
        gst_adc.adc_bigger_cb = callback;
    }
}

u32 tls_read_adc_result(void)
{
    u32 value;
    u32 ret;

    value = tls_reg_read32(HR_SD_ADC_RESULT_REG);
    ret = ADC_RESULT_VAL(value);

    return ret;
}

void tls_adc_start_with_cpu(int Channel)
{
    u32 value;

    /* Stop adc first */
    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value |= CONFIG_PD_ADC_VAL(1);
    value &= ~(CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    value &= ~(CONFIG_ADC_CHL_SEL_MASK);
    value |= CONFIG_ADC_CHL_SEL(Channel);

    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value);

    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value &= ~(CONFIG_PD_ADC_VAL(1));
    value |= (CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value);
}

void tls_adc_start_with_dma(int Channel, int Length)
{
    u32 value;
    int len;

    if (Channel < 0 || Channel > 11)
        return;

    if (Length > ADC_DEST_BUFFER_SIZE)
        len = ADC_DEST_BUFFER_SIZE;
    else
        len = Length;

    gst_adc.valuelen = len;

    Channel &= 0xF;

    /*disable adc:set adc pd, rstn and ldo disable*/
    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value |= CONFIG_PD_ADC_VAL(1);
    value &= ~(CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value);

    /* Stop dma if necessary */
    while (DMA_CHNLCTRL_REG(gst_adc.dmachannel) & 1) {
        DMA_CHNLCTRL_REG(gst_adc.dmachannel) = 2;
    }

    DMA_SRCADDR_REG(gst_adc.dmachannel) = HR_SD_ADC_RESULT_REG;
    DMA_DESTADDR_REG(gst_adc.dmachannel) = ADC_DEST_BUFFER_DMA;

    /* Dest_add_inc, halfword,  */
    DMA_CTRL_REG(gst_adc.dmachannel) = (1 << 3) | (1 << 5) | ((len * 2) << 8);
    DMA_INTMASK_REG &= ~(0x01 << (gst_adc.dmachannel * 2 + 1));
    DMA_CHNLCTRL_REG(gst_adc.dmachannel) = 1; /* Enable dma */

    /*Enable dma*/
    value = tls_reg_read32(HR_SD_ADC_CTRL);
    value |= (1 << 0);
    tls_reg_write32(HR_SD_ADC_CTRL, value);

    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value |= CONFIG_ADC_CHL_SEL(Channel);
    value &= ~(CONFIG_PD_ADC_VAL(1));
    value |= (CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value); /*start adc*/
}

void tls_adc_stop(int ifusedma)
{
    u32 value;

    tls_reg_write32(HR_SD_ADC_PGA_CTRL, 0);

    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value |= CONFIG_PD_ADC_VAL(1);
    value &= ~(CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value);

    /*Disable dma*/
    value = tls_reg_read32(HR_SD_ADC_CTRL);
    value &= ~(1 << 0);
    tls_reg_write32(HR_SD_ADC_CTRL, value);

    /*Disable compare function and compare irq*/
    value = tls_reg_read32(HR_SD_ADC_CTRL);
    value &= ~(3 << 4);
    tls_reg_write32(HR_SD_ADC_CTRL, value);

    if (ifusedma)
        tls_dma_free(gst_adc.dmachannel);
}

void tls_adc_config_cmp_reg(int cmp_data, int cmp_pol)
{
    u32 value;

    tls_reg_write32(HR_SD_ADC_CMP_VALUE, CONFIG_ADC_INPUT_CMP_VAL(cmp_data));

    value = tls_reg_read32(HR_SD_ADC_CTRL);
    if (cmp_pol) {
        value |= CMP_POLAR_MASK;
    } else {
        value &= ~CMP_POLAR_MASK;
    }
    tls_reg_write32(HR_SD_ADC_CTRL, value);
}

void tls_adc_cmp_start(int Channel, int cmp_data, int cmp_pol)
{
    u32 value;

    /* Stop adc first */
    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value |= CONFIG_PD_ADC_VAL(1);
    value &= ~(CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    value |= CONFIG_ADC_CHL_SEL(Channel);
    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value);

    tls_adc_config_cmp_reg(cmp_data, cmp_pol);

    value = tls_reg_read32(HR_SD_ADC_ANA_CTRL);
    value &= ~(CONFIG_PD_ADC_VAL(1));
    value |= (CONFIG_RSTN_ADC_VAL(1) | CONFIG_EN_LDO_ADC_VAL(1));
    tls_reg_write32(HR_SD_ADC_ANA_CTRL, value); /*start adc*/

    /*Enable compare function and compare irq*/
    value = tls_reg_read32(HR_SD_ADC_CTRL);
    value |= (3 << 4);
    tls_reg_write32(HR_SD_ADC_CTRL, value);
}

void tls_adc_reference_sel(int ref)
{
    u32 value;

    value = tls_reg_read32(HR_SD_ADC_PGA_CTRL);
    if (ADC_REFERENCE_EXTERNAL == ref) {
        value |= BYPASS_INNER_REF_SEL;
    } else if (ADC_REFERENCE_INTERNAL == ref) {
        value &= ~BYPASS_INNER_REF_SEL;
    }
    tls_reg_write32(HR_SD_ADC_PGA_CTRL, value);
}

void tls_adc_set_clk(int div)
{
    u32 value;

    value = tls_reg_read32(HR_CLK_SEL_CTL);
    value &= ~(0xFF << 8);
    value |= (div & 0xFF) << 8;
    tls_reg_write32(HR_CLK_SEL_CTL, value);
}

void tls_adc_set_pga(int gain1, int gain2)
{
    u32 val = 0;
    u8 gain1times = 0;
    u8 gain2times = 0;
    switch (gain1) {
        case 1:
            gain1times = 0;
            break;
        case 16:
            gain1times = 1;
            break;
        case 32:
            gain1times = 2;
            break;
        case 64:
            gain1times = 3;
            break;
        case 128:
            gain1times = 4;
            break;
        case 256:
            gain1times = 5;
            break;
        default:
            gain1times = 0;
            break;
    }

    switch (gain2) {
        case 1:
            gain2times = 0;
            break;
        case 2:
            gain2times = 1;
            break;
        case 3:
            gain2times = 2;
            break;
        case 4:
            gain2times = 3;
            break;
        default:
            gain2times = 0;
            break;
    }

    val = tls_reg_read32(HR_SD_ADC_PGA_CTRL);
    val = GAIN_CTRL_PGA_VAL(gain2times) | CLK_CHOP_SEL_PGA_VAL(gain1times) | PGA_BYPASS_VAL(0) | PGA_CHOP_ENP_VAL(1) |
          PGA_EN_VAL(1);
    tls_reg_write32(HR_SD_ADC_PGA_CTRL, val);
}

void signedToUnsignedData(u32* adcValue)
{
    if (*adcValue & 0x20000) {
        *adcValue = *adcValue & 0x1FFFF;
    } else {
        *adcValue = *adcValue | 0x20000;
    }
}

static void waitForAdcDone(void)
{
    int cnt = 2;
    while (cnt--) {
#if 0    
        int reg = tls_reg_read32(HR_SD_ADC_INT_STATUS);
		//printf("reg:%x\r\n", reg);
        if(reg & ADC_INT_MASK)      //ADC�ж�
        {
            tls_adc_clear_irq(ADC_INT_TYPE_ADC);
            break;
        }

        if(reg & CMP_INT_MASK)      //ADC�ж�
        {
            tls_adc_clear_irq(ADC_INT_TYPE_ADC_COMP);
            break;
        }
#else
        tls_os_time_delay(1);
#endif
    }
}

u32 adc_get_offset(void)
{
    adc_offset = 0;
    tls_adc_init(0, 0);
    tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
    tls_adc_start_with_cpu(CONFIG_ADC_CHL_OFFSET);
    tls_adc_set_pga(1, 1);
    tls_adc_set_clk(0x28);

    waitForAdcDone();
    adc_offset = tls_read_adc_result(); //��ȡadcת�����
    signedToUnsignedData(&adc_offset);
    tls_adc_stop(0);

    return adc_offset;
}

int adc_get_interTemp(void)
{
    return adc_temp();
}

int adc_get_inputVolt(u8 channel)
{
    int average = 0;
    double voltage = 0.0;

    adc_get_offset();

    tls_adc_init(0, 0);
    tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
    tls_adc_set_pga(1, 1);
    tls_adc_set_clk(0x28);
    tls_adc_start_with_cpu(channel);

    waitForAdcDone();
    average = tls_read_adc_result();
    signedToUnsignedData(&average);
    tls_adc_stop(0);

    if ((channel == 8) || (channel == 9)) {
        voltage = ((double)average - (double)adc_offset) / 4.0;
        voltage = voltage * (126363 / 1000) / 1000000;
    } else {
        voltage = ((double)average - (double)adc_offset) / 4.0;
        voltage = 1.196 + voltage * (126363 / 1000.0) / 1000000;
    }

    average = (int)(voltage * 1000);
    return average;
}

u16 adc_get_interVolt(void)
{
    /*dummy for test range only 2.5V*/
#if 0
	u32 voltValue;
	u32 code = 0;	
	int i = 0;
	adc_get_offset();

    tls_adc_init(0, 0); 
	tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
	tls_adc_start_with_cpu(CONFIG_ADC_CHL_VOLT);
	tls_adc_set_pga(1,1);
	tls_adc_set_clk(0x28);	
	for (i = 0;i < 10; i++)
	{
		waitForAdcDone();
		voltValue += tls_read_adc_result();
	}
	voltValue = voltValue/10;
	signedToUnsignedData(&voltValue);
	code = voltValue;
	voltValue = voltValue>>2;
	tls_adc_stop(0);
	voltValue = (voltValue - adc_offset)*685/20+1200000;
	printf("Power voltage code:0x%x, interVolt:%d.%d\r\n", code, voltValue/1000000, (voltValue%1000000)/1000);
    return voltValue;
#else
    return 3300;
#endif
}

/**
 * @brief          This function is used to get chip's internal work temperature
 *
 * @return         chip temperature, unit: 1/1000 degree
 *
 * @note           Only use to get chip's internal work temperature.
 */
int adc_temp(void)
{
    u32 code1 = 0, code2 = 0;
    u32 val = 0;
    int temperature = 0;

    tls_adc_init(0, 0);
    tls_adc_reference_sel(ADC_REFERENCE_INTERNAL);
    tls_adc_set_pga(1, 4);
    tls_adc_start_with_cpu(CONFIG_ADC_CHL_TEMP);
    tls_adc_set_clk(0x28);
    val = tls_reg_read32(HR_SD_ADC_TEMP_CTRL);
    val &= ~TEMP_GAIN_MASK;
    val |= TEMP_GAIN_VAL(0);
    val |= TEMP_EN_VAL(1);

    val &= (~(TEMP_CAL_OFFSET_MASK));
    tls_reg_write32(HR_SD_ADC_TEMP_CTRL, val);
    waitForAdcDone();
    code1 = tls_read_adc_result();
    signedToUnsignedData(&code1);

    val |= TEMP_CAL_OFFSET_MASK;
    tls_reg_write32(HR_SD_ADC_TEMP_CTRL, val);
    waitForAdcDone();
    code2 = tls_read_adc_result();
    signedToUnsignedData(&code2);

    tls_adc_stop(0);

    temperature = ((int)code1 - (int)code2);

    temperature = ((temperature * 1000 / (int)(2 * 2 * 4) - 4120702) * 1000 / 15548);

    return temperature;
}

int tls_get_adc_value(u8 channel, u16* data)
{
    int average = 0;
    if ((channel > 1 && channel < 8) ||
        (channel > 8 && channel != CONFIG_ADC_CHL_TEMP && channel != CONFIG_ADC_CHL_VOLT)) {
        return WM_FAILED;
    }
    if (channel <= 1) {
        wm_adc_config(channel);
    } else if (channel == 8) {
        wm_adc_config(0);
        wm_adc_config(1);
    }
    average = adc_get_inputVolt(channel);

    *data = average;
    return WM_SUCCESS;
}