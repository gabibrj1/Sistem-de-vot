#include "i2c-lcd.h"
#include "i2c.h"

#define BACKLIGHT     0x08
#define NOBACKLIGHT   0x00

#define EN 0x04
#define RS 0x01

#define CLEAR_DISPLAY 0x01
#define GO_HOME       0x02
#define CONTROL       0x08

#define DISPLAY_ON    0x04
#define DISPLAY_OFF   0x00
#define CURSOR_ON     0x02
#define CURSOR_OFF    0x00
#define BLINK_ON      0x01
#define BLINK_OFF     0x00

#define DEFAULT_COLUMNS 16
#define DEFAULT_ROWS    2

static uint8_t _addr;
static uint8_t _backlight = BACKLIGHT;
static uint8_t _columns   = DEFAULT_COLUMNS;
static uint8_t _rows      = DEFAULT_ROWS;
static uint8_t _display   = DISPLAY_ON;
static uint8_t _cursor    = CURSOR_OFF;
static uint8_t _blink     = BLINK_OFF;

static I2C_HandleTypeDef *i2c;

static volatile uint32_t wait_time      = 0;
static volatile uint32_t last_send_time = 0;

/* intern – trimite 4 biți high + 4 biți low */
static HAL_StatusTypeDef lcd_send(uint8_t data, uint8_t cmd)
{
    HAL_StatusTypeDef res;

    while (HAL_GetTick() - last_send_time < wait_time)
        ;

    do {
        res = HAL_I2C_IsDeviceReady(i2c, _addr, 1, 100);
    } while (res != HAL_OK);

    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;

    uint8_t data_arr[4];
    data_arr[0] = up | cmd | _backlight | EN;
    data_arr[1] = up | cmd | _backlight;
    data_arr[2] = lo | cmd | _backlight | EN;
    data_arr[3] = lo | cmd | _backlight;

    res = HAL_I2C_Master_Transmit(i2c, _addr, data_arr, 4, 100);
    last_send_time = HAL_GetTick();
    return res;
}

static HAL_StatusTypeDef lcd_send_command(uint8_t cmd)
{
    HAL_StatusTypeDef res = lcd_send(cmd, 0);
    if (res == HAL_OK)
        wait_time = 5;
    return res;
}

static HAL_StatusTypeDef lcd_send_data(uint8_t data)
{
    return lcd_send(data, RS);
}

/* API public */

HAL_StatusTypeDef LCD_init(I2C_HandleTypeDef *_i2c, uint8_t lcd_addr)
{
    i2c   = _i2c;
    _addr = (lcd_addr << 1);        // 0x27 -> 0x4E

    HAL_Delay(50);                  // >40ms după power-on

    HAL_StatusTypeDef res = HAL_OK;

    // secvența standard HD44780 pentru 4-bit mode
    res |= lcd_send_command(0x30);
    HAL_Delay(5);
    res |= lcd_send_command(0x30);
    HAL_Delay(5);
    res |= lcd_send_command(0x30);
    HAL_Delay(5);
    res |= lcd_send_command(0x20);  // 4-bit

    // function set: 2 linii, 5x8 dots
    res |= lcd_send_command(0x28);
    // display on, cursor off, blink off
    res |= lcd_send_command(CONTROL | _display | _cursor | _blink);
    // clear
    res |= lcd_send_command(CLEAR_DISPLAY);
    HAL_Delay(2);

    return res;
}

HAL_StatusTypeDef LCD_init_custom(I2C_HandleTypeDef *_i2c, uint8_t lcd_addr,
                                  uint8_t column, uint8_t rows)
{
    _columns = column;
    _rows    = rows;
    return LCD_init(_i2c, lcd_addr);
}

HAL_StatusTypeDef LCD_clear(void)
{
    return lcd_send_command(CLEAR_DISPLAY);
}

HAL_StatusTypeDef LCD_home(void)
{
    return lcd_send_command(GO_HOME);
}

HAL_StatusTypeDef LCD_set_pos(uint8_t column, uint8_t row)
{
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row >= _rows)
        row = _rows - 1;
    return lcd_send_command(0x80 | (column + row_offsets[row]));
}

HAL_StatusTypeDef LCD_backlight(uint8_t state)
{
    _backlight = state ? BACKLIGHT : NOBACKLIGHT;
    // doar actualizează backlight-ul
    return lcd_send_command(0);
}

HAL_StatusTypeDef LCD_cursor(uint8_t cursor)
{
    _cursor = cursor ? CURSOR_ON : CURSOR_OFF;
    return lcd_send_command(CONTROL | _display | _cursor | _blink);
}

HAL_StatusTypeDef LCD_blink(uint8_t blink)
{
    _blink = blink ? BLINK_ON : BLINK_OFF;
    return lcd_send_command(CONTROL | _display | _cursor | _blink);
}

HAL_StatusTypeDef LCD_off(void)
{
    _display = DISPLAY_OFF;
    return lcd_send_command(CONTROL | _display | _cursor | _blink);
}

HAL_StatusTypeDef LCD_on(void)
{
    _display = DISPLAY_ON;
    return lcd_send_command(CONTROL | _display | _cursor | _blink);
}

HAL_StatusTypeDef LCD_write_str(char *str)
{
    HAL_StatusTypeDef res = HAL_OK;
    while (*str)
        res |= lcd_send_data((uint8_t)*str++);
    return res;
}

HAL_StatusTypeDef LCD_write_data(uint8_t *data, size_t size)
{
    HAL_StatusTypeDef res = HAL_OK;
    for (size_t i = 0; i < size; i++)
        res |= lcd_send_data(data[i]);
    return res;
}
