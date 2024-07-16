#ifndef _P_ADC_H_
#define _P_ADC_H_

#include "common.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
void example_adc_calibration_deinit(adc_cali_handle_t handle);
void onehost_adc_unit1_init(void);
void get_adc1_value(void);

#endif