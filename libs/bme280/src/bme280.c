#include "bme280.h"
#include "bme280-regs.h"

typedef struct
{
	bme280_i2c_read i2c_read;
	bme280_i2c_write i2c_write;
} bme280_ctx_t;

static bme280_ctx_t bme280_ctx = {0};

void bme280_init(bme280_i2c_read i2c_read, bme280_i2c_write i2c_write){
    bme280_ctx.i2c_read = i2c_read;
    bme280_ctx.i2c_write = i2c_write;
    uint8_t id_reg_buf[1] = {0};
    bme280_read_regs(BME280_REG_id, id_reg_buf, sizeof(id_reg_buf));
    printf("bme280 id is %x", id_reg_buf[0]);
    uint8_t ctrl_hum_reg_value = 0;
    ctrl_hum_reg_value |= (0b001 << 0); // osrs_h[2:0] = oversampling 1
    bme280_write_reg(BME280_REG_ctrl_hum, ctrl_hum_reg_value);
    uint8_t config_reg_value = 0;
    config_reg_value |= (0b0 << 0); // spi3w_en[0:0] = false
    config_reg_value |= (0b000 << 2); // filter[4:2] = Filter off
    config_reg_value |= (0b001 << 5); // t_sb[7:5] = 62.5 ms
    bme280_write_reg(BME280_REG_config, config_reg_value);
    uint8_t ctrl_mes_reg_value = 0;
    config_reg_value |= (0b11 << 0); 
    config_reg_value |= (0b001 << 2); 
    config_reg_value |= (0b001 << 5); 
    bme280_write_reg(BME280_REG_ctrl_meas, config_reg_value);
}

void bme280_read_regs(uint8_t start_reg_address, uint8_t* buffer, uint8_t length){
    uint8_t data[1] = {start_reg_address};
    bme280_ctx.i2c_write(data, sizeof(data));
    bme280_ctx.i2c_read(buffer, length);
}

void bme280_write_reg(uint8_t reg_address, uint8_t value){
    uint8_t data[2] = {reg_address, value};
    bme280_ctx.i2c_write(data, sizeof(data));
}


uint16_t bme280_read_temp_raw()
{
	uint8_t read[2] = {0};
	bme280_read_regs(BME280_REG_temp_msb, read, sizeof(read));
	uint16_t value = ((uint16_t)read[0] << 8) | ((uint16_t)read[1]);
	return value;
}

uint16_t bme280_read_pres_raw()
{
	uint8_t read[2] = {0};
	bme280_read_regs(BME280_REG_press_msb, read, sizeof(read));
	uint16_t value = ((uint16_t)read[0] << 8) | ((uint16_t)read[1]);
	return value;
}

uint16_t bme280_read_hum_raw()
{
	uint8_t read[2] = {0};
	bme280_read_regs(BME280_REG_hum_msb, read, sizeof(read));
	uint16_t value = ((uint16_t)read[0] << 8) | ((uint16_t)read[1]);
	return value;
}


int32_t t_fine;
int32_t BME280_compensate_T_int32(uint32_t adc_T)
{
uint32_t var1, var2, T;

uint8_t buffer[6] = {0};
bme280_read_regs(0x88, buffer, 6);
uint16_t dig_T1 = ((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]);
int16_t dig_T2 = ((uint16_t)buffer[2] << 8) | ((uint16_t)buffer[3]);
int16_t dig_T3 = ((uint16_t)buffer[4] << 8) | ((uint16_t)buffer[5]);
var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1)))
>> 12) *
((int32_t)dig_T3)) >> 14;
t_fine = var1 + var2;
T = (t_fine * 5 + 128) >> 8;
return T;
}


uint32_t BME280_compensate_P_int64(uint32_t adc_P)
{
int64_t var1, var2, p;
uint8_t buffer[18] = {0};
bme280_read_regs(0x8e, buffer, 18);
uint16_t dig_P1 = ((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]);
int16_t dig_P2 = ((uint16_t)buffer[2] << 8) | ((uint16_t)buffer[3]);
int16_t dig_P3 = ((uint16_t)buffer[4] << 8) | ((uint16_t)buffer[5]);
int16_t dig_P4 = ((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]);
int16_t dig_P5 = ((uint16_t)buffer[2] << 8) | ((uint16_t)buffer[3]);
int16_t dig_P6 = ((uint16_t)buffer[4] << 8) | ((uint16_t)buffer[5]);
int16_t dig_P7 = ((uint16_t)buffer[0] << 8) | ((uint16_t)buffer[1]);
int16_t dig_P8 = ((uint16_t)buffer[2] << 8) | ((uint16_t)buffer[3]);
int16_t dig_P9 = ((uint16_t)buffer[4] << 8) | ((uint16_t)buffer[5]);
var1 = ((int64_t)t_fine) - 128000;
var2 = var1 * var1 * (int64_t)dig_P6;
var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
var2 = var2 + (((int64_t)dig_P4)<<35);
var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
if (var1 == 0)
{
return 0; // avoid exception caused by division by zero
}
p = 1048576-adc_P;
p = (((p<<31)-var2)*3125)/var1;
var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
var2 = (((int64_t)dig_P8) * p) >> 19;
p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
return (uint32_t)p;
}


uint32_t bme280_compensate_H_int32(int32_t adc_H)
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
uint8_t buffer4[2] = {0};
bme280_read_regs(0xe5, buffer4, 2);
int16_t dig_H5 = ((uint16_t)buffer4[0] << 8) | ((uint16_t)buffer4[1]);
uint8_t buffer5[1] = {0};
bme280_read_regs(0xe7, buffer5, 1);
int8_t dig_H6 = buffer5[0];
int32_t v_x1_u32r;
v_x1_u32r = (t_fine - ((int32_t)76800));
v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) *
v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *
((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) +
8192) >> 14));
v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
((int32_t)dig_H1)) >> 4));
v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
return (uint32_t)(v_x1_u32r>>12);
}