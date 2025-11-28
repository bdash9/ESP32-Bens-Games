//====================================================================================
// User_Setup.h for TTGO WROVER-E with ST7796 TFT and JoyWing (I2C)
//====================================================================================

#define ST7796_DRIVER      // TFT Controller

// SPI pins for TTGO WROVER-E
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   27
#define TFT_RST  33

// Backlight pin
#define TFT_BACKLIGHT_ON HIGH

// Display resolution
#define TFT_WIDTH  320
#define TFT_HEIGHT 480

// Fonts and features
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// We are not using touch
//#define TOUCH_CS -1

