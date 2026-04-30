#include "bme-280.h"
#include "bme280-regs.h"

typedef struct {
    bme280_i2c_read i2c_read;
    bme280_i2c_write i2c_write;
} bme280_ctx_t;

static bme280_ctx_t bme280_ctx = {0};

void bme280_init(bme280_i2c_read i2c_read, bme280_i2c_write i2c_write) {
    bme280_ctx.i2c_read = i2c_read;
    bme280_ctx.i2c_write = i2c_write;
    uint8_t id_reg_buf[1] = {0};
    bme280_read_regs(BME280_REG_id, id_reg_buf, sizeof(id_reg_buf));
    if(id_reg_buf[0] != 0x60) {
        printf("Warning: BME280 ID register does not hold expected value (0x60)");
    }
    uint8_t ctrl_hum_reg_value = 0;
    ctrl_hum_reg_value |= (0b001 << 0); // osrs_h[2:0] = oversampling 1
    bme280_write_reg(BME280_REG_ctrl_hum, ctrl_hum_reg_value);
    uint8_t config_reg_value = 0;
    config_reg_value |= (0b0 << 0); // spi3w_en[0:0] = false
    config_reg_value |= (0b000 << 2); // filter[4:2] = Filter off
    config_reg_value |= (0b001 << 5); // t_sb[7:5] = 62.5 ms
    bme280_write_reg(BME280_REG_config, config_reg_value);
    uint8_t ctrl_meas_reg_value = 0;
    ctrl_meas_reg_value |= (0b11 << 0); // mode[1:0] = Normal mode
    ctrl_meas_reg_value |= (0b001 << 2); //osrs_p[2:0] = oversampling x1
    ctrl_meas_reg_value |= (0b001 << 5); //osrs_t[2:0] = oversampling x1
    bme280_write_reg(BME280_REG_ctrl_meas, ctrl_meas_reg_value);
}

void bme280_read_regs(uint8_t start_reg_address, uint8_t* buffer, uint8_t length) {
    uint8_t data[1] = {start_reg_address};
    bme280_ctx.i2c_write(data, sizeof(data));
    bme280_ctx.i2c_read(buffer, length);
}

void bme280_write_reg(uint8_t reg_address, uint8_t value) {
    uint8_t data[2] = {reg_address, value};
    bme280_ctx.i2c_write(data, sizeof(data));
}

uint32_t bme280_read_temp_raw()
{
	uint8_t read[3] = {0};
	bme280_read_regs(BME280_REG_temp_msb, read, sizeof(read));
	uint32_t value = ((uint32_t)read[0] << 12) | ((uint32_t)read[1] << 4) | ((uint32_t)read[2] >> 4);
	return value;
}

int32_t t_fine = 1;

double bme280_read_temp_si()
{
    int32_t var1, var2, T;
    uint32_t adc_T = bme280_read_temp_raw();

    // Reading compensation coefficients from BME280 regs
    uint8_t read[2] = {0};
    bme280_read_regs(BME280_REG_dig_T1, read, 2);
    uint16_t dig_T1 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_T2, read, 2);
    int16_t dig_T2 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_T3, read, 2);
    int16_t dig_T3 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);

    var1 = (((((int32_t)adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    var2 = ((((((int32_t)adc_T>>4) - ((int32_t)dig_T1)) * (((int32_t)adc_T>>4) - ((int32_t)dig_T1)))
    >> 12) *
    ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return (double)T / 100.0;
}

uint32_t bme280_read_pres_raw() {
    uint8_t read[3] = {0};
	bme280_read_regs(BME280_REG_press_msb, read, sizeof(read));
    //printf("%x %x %x\n", read[0], read[1], read[2]);
	uint32_t value = ((uint32_t)read[0] << 12) | ((uint32_t)read[1] << 4) | ((uint32_t)read[2] >> 4);
	//printf("%x %u/n", value, value);
    return value;
}

double bme280_read_pres_si()
{
    uint32_t adc_T = bme280_read_temp_raw();
    int32_t adc_P = bme280_read_pres_raw();

    // Reading compensation coefficients from BME280 regs
    uint8_t read[2] = {0};
    bme280_read_regs(BME280_REG_dig_T1, read, 2);
    uint16_t dig_T1 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_T2, read, 2);
    int16_t dig_T2 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_T3, read, 2);
    int16_t dig_T3 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P1, read, 2);
    uint16_t dig_P1 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P2, read, 2);
    int16_t dig_P2 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P3, read, 2);
    int16_t dig_P3 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P4, read, 2);
    int16_t dig_P4 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P5, read, 2);
    int16_t dig_P5 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P6, read, 2);
    int16_t dig_P6 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P7, read, 2);
    int16_t dig_P7 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P8, read, 2);
    int16_t dig_P8 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    bme280_read_regs(BME280_REG_dig_P9, read, 2);
    int16_t dig_P9 = ((uint16_t)read[1] << 8) | ((uint16_t)read[0]);
    
    int32_t t_var1 = (((((int32_t)adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    int32_t t_var2 = ((((((int32_t)adc_T>>4) - ((int32_t)dig_T1)) * (((int32_t)adc_T>>4) - ((int32_t)dig_T1)))
    >> 12) *
    ((int32_t)dig_T3)) >> 14;
    t_fine = t_var1 + t_var2;

    int32_t var1, var2;
    uint32_t p;
    var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
    var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
    var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) *
    var1)>>1))>>18;
    var1 =((((32768+var1))*((int32_t)dig_P1))>>15);
    if (var1 == 0)
    {
    return 0; // avoid exception caused by division by zero
    }
    p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
    if (p < 0x80000000)
    {
    p = (p << 1) / ((uint32_t)var1);
    }
    else
    {
    p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
    return (double)p / 100;
}

uint16_t bme280_read_hum_raw() {
    uint8_t read[2] = {0};
	bme280_read_regs(BME280_REG_hum_msb, read, sizeof(read));
	uint16_t value = ((uint16_t)read[0] << 8) | ((uint16_t)read[1]);
	return value;
}

double bme280_read_hum_si()
{
    uint8_t buffer[1] = {0};
    bme280_read_regs(0xa1, buffer, 1);
    uint8_t dig_H1 = buffer[0];
    uint8_t buffer1[2] = {0};
    bme280_read_regs(0xe1, buffer1, 2);
    int16_t dig_H2 = ((uint16_t)buffer1[0] << 8) | ((uint16_t)buffer1[1]);
    uint8_t buffer2[1] = {0};
    bme280_read_regs(0xe3, buffer2, 1);
    uint8_t dig_H3 = buffer2[0];
    uint8_t buffer3[2] = {0};
    bme280_read_regs(0xe4, buffer3, 2);
    int16_t dig_H4 = ((uint16_t)buffer3[0] << 8) | ((uint16_t)buffer3[1]);
    dig_H4 = dig_H4 >> 4;
    uint8_t buffer4[2] = {0};
    bme280_read_regs(0xe5, buffer4, 2);
    int16_t dig_H5 = ((uint16_t)buffer4[0] << 8) | ((uint16_t)buffer4[1]);//h4, h5 -12 bit
    dig_H5 = dig_H5 & 0b0000111111111111;
    uint8_t buffer5[1] = {0};
    bme280_read_regs(0xe7, buffer5, 1);
    int8_t dig_H6 = buffer5[0];
    uint16_t adc_H = bme280_read_hum_raw();
    double var_H;
    var_H = (((double)t_fine) - 76800.0);
    var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 *
    var_H)) * (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) /
    67108864.0 * var_H *
    (1.0 + ((double)dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
    var_H = 100.0;
    else if (var_H < 0.0)
    var_H = 0.0;
    return var_H;
}