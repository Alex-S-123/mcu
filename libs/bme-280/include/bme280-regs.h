#define BME280_REG_id 0xD0
#define BME280_REG_reset 0xE0
#define BME280_REG_ctrl_hum 0xF2
#define BME280_REG_status 0xF3
#define BME280_REG_ctrl_meas 0xF4
#define BME280_REG_config 0xF5

#define BME280_REG_press_msb 0xF7
#define BME280_REG_press_lsb 0xF8
#define BME280_REG_press_xlsb 0xF9
#define BME280_REG_temp_msb 0xFA
#define BME280_REG_temp_lsb 0xFB
#define BME280_REG_temp_xlsb 0xFC
#define BME280_REG_hum_msb 0xFD
#define BME280_REG_hum_lsb 0xFE

#define BME280_REG_dig_T1 0x88 // 16 bit (two registers)
#define BME280_REG_dig_T2 0x8A // 16 bit
#define BME280_REG_dig_T3 0x8C // 16 bit
#define BME280_REG_dig_P1 0x8E // 16 bit
#define BME280_REG_dig_P2 0x90 // 16 bit
#define BME280_REG_dig_P3 0x92 // 16 bit
#define BME280_REG_dig_P4 0x94 // 16 bit
#define BME280_REG_dig_P5 0x96 // 16 bit
#define BME280_REG_dig_P6 0x98 // 16 bit
#define BME280_REG_dig_P7 0x9A // 16 bit
#define BME280_REG_dig_P8 0x9C // 16 bit
#define BME280_REG_dig_P9 0x9E // 16 bit
#define BME280_REG_dig_H1 0xA1 // 8 bit (one register)
#define BME280_REG_dig_H2 0xE1 // 16 bit
#define BME280_REG_dig_H3 0xE3 // 8 bit
#define BME280_REG_dig_H4 0xE4 // 12 bit (one and a half register)
#define BME280_REG_dig_H5 0xE5 // 12 bit
#define BME280_REG_dig_H6 0xE7 // 8 bit