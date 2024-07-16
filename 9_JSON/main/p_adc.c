/*
ESP32的ADC资源

GPIO36      ADC1_CH0            GPIO4       ADC2_CH0
GPIO37      ADC1_CH1            GPIO0       ADC2_CH1
GPIO38      ADC1_CH2            GPIO2       ADC2_CH2
GPIO39      ADC1_CH3            GPIO15      ADC2_CH3
GPIO32      ADC1_CH4            GPIO13      ADC2_CH4
GPIO33      ADC1_CH5            GPIO12      ADC2_CH5
GPIO34      ADC1_CH6            GPIO14      ADC2_CH6
GPIO35      ADC1_CH7            GPIO27      ADC2_CH7
                                GPIO25      ADC2_CH8
                                GPIO26      ADC2_CH9
注
    在使用WIFI时不要使用ADC2
*/
#include "p_adc.h"

/* 宏定义ADC的引脚，参考上面列表 */
#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_4
#define EXAMPLE_ADC1_CHAN1 ADC_CHANNEL_5

/* ADC衰减，调节量程 */
#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_11

const static char *TAG = "TEST";

static int adc_raw[2];
static int voltage[2];

/* ADC单次转换句柄 */
adc_oneshot_unit_handle_t adc1_handle;
 
/* 校准句柄 */
adc_cali_handle_t adc1_cali_chan0_handle = NULL;
adc_cali_handle_t adc1_cali_chan1_handle = NULL;
 
/* 保存ADC是否校准 */
bool do_calibration1_chan1;
bool do_calibration1_chan0;


/******************************************************************************
 * 函数描述: 对ADC1进行初始化，并校准使用的引脚(32, 33)
 * 过   程: 初始化一次性转换ADC -->  配置ADC通道 --> 校准ADC
 *******************************************************************************/
void onehost_adc_unit1_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config1;
    init_config1.unit_id = ADC_UNIT_1;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config;
    config.atten = EXAMPLE_ADC_ATTEN;
    config.bitwidth = ADC_BITWIDTH_DEFAULT;
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));

    do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, EXAMPLE_ADC1_CHAN0, EXAMPLE_ADC_ATTEN, &adc1_cali_chan0_handle);
    do_calibration1_chan1 = example_adc_calibration_init(ADC_UNIT_1, EXAMPLE_ADC1_CHAN1, EXAMPLE_ADC_ATTEN, &adc1_cali_chan1_handle);
}

void get_adc1_value(void)
{
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0]);
    if (do_calibration1_chan0)
    {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw[0], &voltage[0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, voltage[0]);
    }

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[1]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[1]);
    if (do_calibration1_chan1)
    {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw[1], &voltage[1]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, voltage[1]);
    }
}

/******************************************************************************
 * 函数描述: 校准ADC
 *******************************************************************************/
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    if (!calibrated)
    {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}
/******************************************************************************
 * 函数描述: 服务ADC
 *******************************************************************************/
void example_adc_calibration_deinit(adc_cali_handle_t handle)
{

    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
}