#ifndef __HAL_STM32_I2C_LCD_H__
#define __HAL_STM32_I2C_LCD_H__

#include <inttypes.h>
#include "stm32f1xx_hal.h"

HAL_StatusTypeDef LCD_init(I2C_HandleTypeDef *i2c, uint8_t lcd_addr);
HAL_StatusTypeDef LCD_init_custom(I2C_HandleTypeDef *_i2c, uint8_t lcd_addr, uint8_t column, uint8_t rows);
HAL_StatusTypeDef LCD_clear(void);
HAL_StatusTypeDef LCD_home(void);
HAL_StatusTypeDef LCD_set_pos(uint8_t column, uint8_t row);
HAL_StatusTypeDef LCD_backlight(uint8_t state);
HAL_StatusTypeDef LCD_cursor(uint8_t cursor);
HAL_StatusTypeDef LCD_blink(uint8_t blink);
HAL_StatusTypeDef LCD_off(void);
HAL_StatusTypeDef LCD_on(void);
HAL_StatusTypeDef LCD_write_str(char *str);
HAL_StatusTypeDef LCD_write_data(uint8_t *data, size_t size);

#endif /* __HAL_STM32_I2C_LCD_H__ */
