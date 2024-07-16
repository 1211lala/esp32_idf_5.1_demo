#include "p_i2c_sht20.h"

#define I2C_MASTER_SCL_IO 15
#define I2C_MASTER_SDA_IO 16

#define SHT20_ADDR 0x40
#define SHT20_TEMP_CMD 0XF3
#define SHT20_HUM_CMD 0XF5

const char *TAG = "SHT20";
/******************************************************************************
 * 函数描述: SHT20初始化程序，初始化使用到的IIC
 * 参  数1: IIC总线频率
 *******************************************************************************/
void sht20_init(uint32_t freq)
{
    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = I2C_MASTER_SDA_IO;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = I2C_MASTER_SCL_IO;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = freq;
    i2c_param_config(I2C_NUM_0, &config);

    i2c_driver_install(I2C_NUM_0, config.mode, 0, 0, 0);
}
/******************************************************************************
 * 函数描述: 获取SHT20的温度信息
 * 参  数1: float指针，用于保存温度数据
 * 返  回 : ESP_OK 执行成功 ，其他错误
 *******************************************************************************/
uint8_t sht20_get_temp(float *temp)
{
    uint8_t buf[3];

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHT20_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SHT20_TEMP_CMD, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK);
    i2c_cmd_link_delete(cmd);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C master write failed: %d", err);
        return err;
    }

    vTaskDelay(100 / portTICK);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHT20_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &buf[0], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &buf[1], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &buf[2], I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK);
    i2c_cmd_link_delete(cmd);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C master read failed: %d", err);
        return err;
    }
    if (buf[2] != SHT20_CRC8(buf, 2))
    {
        ESP_LOGI("SHT20", "CRC Error");
        return -1;
    }
    *temp = (175.72 * ((buf[0] << 8) | buf[1]) / 65536 - 46.85); // 温度计算公式
    return ESP_OK;
}

/******************************************************************************
 * 函数描述: 获取SHT20的湿度信息
 * 参  数1: float指针，用于保存湿度数据
 * 返  回 : ESP_OK 执行成功 ，其他错误
 *******************************************************************************/
uint8_t sht20_get_hum(float *hum)
{
    uint8_t buf[3];

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHT20_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SHT20_HUM_CMD, true);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK);
    i2c_cmd_link_delete(cmd);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C master write failed: %d", err);
        return err;
    }

    vTaskDelay(100 / portTICK);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHT20_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &buf[0], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &buf[1], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &buf[2], I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK);
    i2c_cmd_link_delete(cmd);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C master read failed: %d", err);
        return err;
    }
    if (buf[2] != SHT20_CRC8(buf, 2))
    {
        ESP_LOGI("SHT20", "CRC Error");
        return -1;
    }

    *hum = 125.0 * ((buf[0] << 8) | buf[1]) / 65536 - 6.0;
    return ESP_OK;
}

/* SHT20的效验函数 */
unsigned char SHT20_CRC8(unsigned char *data, unsigned int datalen)
{
    unsigned char wCRCin = 0x00; /* CRC初始值 */
    unsigned char wCPoly = 0x31; /* 多项式值 */
    unsigned char wChar = 0;

    while (datalen--)
    {
        wChar = *(data++);
        wCRCin = wCRCin ^ wChar; /* 第一个数据异或上初始值 */
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x80) /* 如果最高位为1，则左移一位再异或上多项式值 */
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else /* 如果最高位为0，则左移一位 */
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}