/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any
redistribution

02/18/2013  Charles-Henri Hallard (http://hallard.me)
            Modified for compiling and use on Raspberry ArduiPi Board
            LCD size and connection are now passed as arguments on
            the command line (no more #define on compilation needed)
            ArduiPi project documentation http://hallard.me/arduipi
07/01/2013  Charles-Henri Hallard
            Reduced code size removed the Adafruit Logo (sorry guys)
            Buffer for OLED is now dynamic to LCD size
            Added support of Seeed OLED 64x64 Display

07/26/2013  Charles-Henri Hallard
            modified name for generic library using different OLED type

02/24/2015  Charles-Henri Hallard
            added support for 1.3" I2C OLED with SH1106 driver

*********************************************************************/

#include "./ArduiPi_OLED.h"
#include "./Adafruit_GFX.h"
#include "./ArduiPi_OLED_lib.h"

const char *oled_type_str[] = {"Adafruit I2C 128x32", "Adafruit I2C 128x64",
                               "Seeed I2C 128x64",    "Seeed I2C 96x96",
                               "SH1106 I2C 128x64"};

// 8x8 Font ASCII 32 - 127 Implemented
// Users can modify this to support more characters(glyphs)
// BasicFont is placed in code memory.

// This font can be freely used without any restriction(It is placed in public
// domain)
const unsigned char seedfont[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00},
    {0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, 0x00},
    {0x00, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, 0x00},
    {0x00, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00},
    {0x00, 0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x00},
    {0x00, 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x00, 0x00},
    {0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00},
    {0x00, 0xA0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00},
    {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00},
    {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00},
    {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00},
    {0x00, 0x62, 0x51, 0x49, 0x49, 0x46, 0x00, 0x00},
    {0x00, 0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00},
    {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00},
    {0x00, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00},
    {0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00},
    {0x00, 0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00},
    {0x00, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    {0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00},
    {0x00, 0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xAC, 0x6C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00},
    {0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00},
    {0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00},
    {0x00, 0x32, 0x49, 0x79, 0x41, 0x3E, 0x00, 0x00},
    {0x00, 0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00},
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00},
    {0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00},
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00},
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x41, 0x51, 0x72, 0x00, 0x00},
    {0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00},
    {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00},
    {0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00},
    {0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00},
    {0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00},
    {0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00},
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, 0x00},
    {0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00},
    {0x00, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00},
    {0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00},
    {0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00},
    {0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, 0x00},
    {0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00, 0x00},
    {0x00, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00},
    {0x00, 0x03, 0x04, 0x78, 0x04, 0x03, 0x00, 0x00},
    {0x00, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00},
    {0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00},
    {0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00},
    {0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00},
    {0x00, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00},
    {0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, 0x00},
    {0x00, 0x38, 0x44, 0x44, 0x28, 0x00, 0x00, 0x00},
    {0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, 0x00},
    {0x00, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00},
    {0x00, 0x08, 0x7E, 0x09, 0x02, 0x00, 0x00, 0x00},
    {0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, 0x00, 0x00},
    {0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00},
    {0x00, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x80, 0x84, 0x7D, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00},
    {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, 0x00, 0x00},
    {0x00, 0x7C, 0x08, 0x04, 0x7C, 0x00, 0x00, 0x00},
    {0x00, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00},
    {0x00, 0xFC, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00},
    {0x00, 0x18, 0x24, 0x24, 0xFC, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0x08, 0x04, 0x00, 0x00, 0x00},
    {0x00, 0x48, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00},
    {0x00, 0x04, 0x7F, 0x44, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00},
    {0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, 0x00},
    {0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00, 0x00},
    {0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00},
    {0x00, 0x1C, 0xA0, 0xA0, 0x7C, 0x00, 0x00, 0x00},
    {0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00},
    {0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x01, 0x01, 0x02, 0x01, 0x00, 0x00},
    {0x00, 0x02, 0x05, 0x05, 0x02, 0x00, 0x00, 0x00}};

inline boolean ArduiPi_OLED::isI2C(void) { return (cs == -1 ? true : false); }
// Low level I2C Write function

inline void ArduiPi_OLED::fastI2Cwrite(char *tbuf, uint32_t len)
{
  bcm2835_i2c_alt_write(tbuf, len);
}

// the most basic function, set a single pixel
void ArduiPi_OLED::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  uint8_t *p = poledbuff;

  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  /*
  // check rotation, move pixel around if necessary
switch (getRotation())
{
  case 1:
    swap(x, y);
    x = WIDTH - x - 1;
  break;

  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
  break;

  case 3:
    swap(x, y);
    y = HEIGHT - y - 1;
  break;
}
*/

    // Get where to do the change in the buffer
    p = poledbuff + (x + (y / 8) * oled_width);

    // x is which column
    if (color == WHITE)
      *p |= _BV((y % 8));
    else
      *p &= ~_BV((y % 8));

}

// Display instantiation
ArduiPi_OLED::ArduiPi_OLED()
{
  // Init all var, and clean
  // Command I/O
  rst = 0;
  dc = 0;
  cs = 0;

  // Lcd size
  oled_width = 0;
  oled_height = 0;

  // Empty pointer to OLED buffer
  poledbuff = NULL;
}

// initializer for OLED Type
boolean ArduiPi_OLED::select_oled(uint8_t OLED_TYPE, int8_t i2c_addr)
{
  // Default type
  oled_width = 128;
  oled_height = 64;
  _i2c_addr = 0x00;
  oled_type = OLED_TYPE;

  // default OLED are using internal boost VCC converter
  vcc_type = SSD_Internal_Vcc;

  // Oled supported display
  // Setup size and I2C address
  switch (OLED_TYPE) {

  case OLED_SH1106_I2C_128x64:
    _i2c_addr = SH1106_I2C_ADDRESS;
    break;

  // houston, we have a problem
  default:
    return false;
    break;
  }

  // Override address if necessary
  if (i2c_addr != 0)
    _i2c_addr = i2c_addr;

  // Buffer size differ from OLED type, 1 pixel is one bit
  // execpt for 96x96 seed, 1 pixel is 1 nible
  oled_buff_size = oled_width * oled_height;

  oled_buff_size = oled_buff_size / 8;

  // De-Allocate memory for OLED buffer if any
  if (poledbuff)
    free(poledbuff);

  // Allocate memory for OLED buffer
  poledbuff = (uint8_t *)malloc(oled_buff_size);

  if (!poledbuff)
    return false;

  // Init Raspberry PI GPIO
  if (!bcm2835_init())
    return false;

  return true;
}

// initializer for I2C - we only indicate the reset pin and OLED type !
boolean ArduiPi_OLED::init_i2c(int8_t RST, uint8_t OLED_TYPE, int8_t i2c_addr,
                               int i2c_bus)
{
  dc = cs = -1; // DC and chip Select do not exist in I2C
  rst = RST;

  // Select OLED parameters
  if (!select_oled(OLED_TYPE, i2c_addr))
    return false;

  // Init & Configure Raspberry PI I2C
  if (bcm2835_i2c_alt_begin(i2c_bus) == 0)
    return false;

  bcm2835_i2c_alt_setSlaveAddress(_i2c_addr);

  // Set clock to 400 KHz
  // does not seem to work, will check this later
  // bcm2835_i2c_alt_set_baudrate(400000);

  // Setup reset pin direction as output
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);

  return (true);
}

void ArduiPi_OLED::close(void)
{
  // De-Allocate memory for OLED buffer if any
  if (poledbuff)
    free(poledbuff);

  poledbuff = NULL;

  // Release Raspberry I2C
  if (isI2C())
    bcm2835_i2c_alt_end();

  // Release Raspberry I/O control
  bcm2835_close();
}

void ArduiPi_OLED::reset_offset()
{
  sendCommand(SSD1306_Set_Display_Offset, 0x00);        // no offset
}

void ArduiPi_OLED::begin(void)
{
  uint8_t multiplex;
  uint8_t contrast;

  reset(oled_width, oled_height);

  // Setup reset pin direction (used by I2C)
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(rst, HIGH);

  // VDD (3.3V) goes high at start, lets just chill for a ms
  usleep(1000);

  // bring reset low
  bcm2835_gpio_write(rst, LOW);

  // wait 10ms
  usleep(10000);

  // bring out of reset
  bcm2835_gpio_write(rst, HIGH);

  // So 128x64
  multiplex = 0x3F;

  contrast = (vcc_type == SSD_External_Vcc ? 0x9F : 0xCF);

  sendCommand(SSD_Display_Off);
  sendCommand(SSD_Set_Muliplex_Ratio, multiplex);
  sendCommand(0x20); // Set Memory Addressing Mode
  sendCommand(SH1106_Set_Page_Address);         /*set page address*/
  sendCommand(0x30); /*0X30---0X33  set VPP   9V liangdu!!!!*/
  sendCommand(SSD1306_Set_Lower_Column_Start_Address | 0x0);  // low col = 0
  sendCommand(SSD1306_Set_Higher_Column_Start_Address | 0x0); // hi col = 0
  sendCommand(SSD1306_Set_Start_Line | 0x00);          /*set display start line*/
  sendCommand(SSD_Set_Segment_Remap | 0x01);    /*set segment remap*/
  sendCommand(SSD_Set_Muliplex_Ratio, multiplex);
  sendCommand(SSD1306_Set_Com_Output_Scan_Direction_Remap); /*Com scan direction*/
  sendCommand(0xad);
  sendCommand(0x30);

  sendCommand(SSD1306_Set_Display_Offset, 0x00);          /*set display offset*/
  sendCommand(SSD1306_Normal_Display);          /*normal / reverse*/
  sendCommand(0x8d);                            /*set charge pump enable*/
  sendCommand(0x14);

  //sendCommand(SSD1306_Set_Com_Output_Scan_Direction_Remap); /*Com scan direction*/
  //sendCommand(SSD1306_Set_Display_Offset);          /*set display offset*/
  //sendCommand(0x00);                                /*   0x20  */
  //sendCommand(SSD1306_Set_Display_Clock_Div);       /*set osc division*/
  //sendCommand(0x80);
  //sendCommand(SSD1306_Set_Precharge_Period); /*set pre-charge period*/
  //sendCommand(0x1f);                         /*0x22*/
  //sendCommand(SSD1306_Set_Com_Pins);         /*set COM pins*/
  //sendCommand(0x12);
  //sendCommand(SSD1306_Set_Vcomh_Deselect_Level); /*set vcomh*/
  //sendCommand(0x40);

  //sendCommand(SSD_Set_ContrastLevel, contrast);

  stopscroll();

  // Empty uninitialized buffer
  clearDisplay();

  // turn on oled panel
  sendCommand(SSD_Display_On);

  // wait 100ms
  usleep(100000);
}

void ArduiPi_OLED::setBrightness(uint8_t Brightness)
{
  sendCommand(SSD_Set_ContrastLevel);
  sendCommand(Brightness);
}

void ArduiPi_OLED::invertDisplay(uint8_t i)
{
  if (i)
    sendCommand(SSD_Inverse_Display);
  else
    sendCommand(SSD1306_Normal_Display);
}

void ArduiPi_OLED::sendCommand(uint8_t c)
{
    char buff[2];

    // Clear D/C to switch to command mode
    buff[0] = SSD_Command_Mode;
    buff[1] = c;

    // Write Data on I2C
    fastI2Cwrite(buff, sizeof(buff));
}

void ArduiPi_OLED::sendCommand(uint8_t c0, uint8_t c1)
{
  char buff[3];
  buff[1] = c0;
  buff[2] = c1;


    // Clear D/C to switch to command mode
    buff[0] = SSD_Command_Mode;

    // Write Data on I2C
    fastI2Cwrite(buff, 3);
}

void ArduiPi_OLED::sendCommand(uint8_t c0, uint8_t c1, uint8_t c2)
{
  char buff[4];

  buff[1] = c0;
  buff[2] = c1;
  buff[3] = c2;

  // I2C
    // Clear D/C to switch to command mode
    buff[0] = SSD_Command_Mode;

    // Write Data on I2C
    fastI2Cwrite(buff, sizeof(buff));
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ArduiPi_OLED::startscrollright(uint8_t start, uint8_t stop)
{
  sendCommand(SSD_Right_Horizontal_Scroll);
  sendCommand(0X00);
  sendCommand(start);
  sendCommand(0X00);
  sendCommand(stop);
  sendCommand(0X01);
  sendCommand(0XFF);
  sendCommand(SSD_Activate_Scroll);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ArduiPi_OLED::startscrollleft(uint8_t start, uint8_t stop)
{
  sendCommand(SSD_Left_Horizontal_Scroll);
  sendCommand(0X00);
  sendCommand(start);
  sendCommand(0X00);
  sendCommand(stop);
  sendCommand(0X01);
  sendCommand(0XFF);
  sendCommand(SSD_Activate_Scroll);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ArduiPi_OLED::startscrolldiagright(uint8_t start, uint8_t stop)
{
  sendCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);
  sendCommand(0X00);
  sendCommand(oled_height);
  sendCommand(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  sendCommand(0X00);
  sendCommand(start);
  sendCommand(0X00);
  sendCommand(stop);
  sendCommand(0X01);
  sendCommand(SSD_Activate_Scroll);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ArduiPi_OLED::startscrolldiagleft(uint8_t start, uint8_t stop)
{
  sendCommand(SSD1306_SET_VERTICAL_SCROLL_AREA);
  sendCommand(0X00);
  sendCommand(oled_height);
  sendCommand(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  sendCommand(0X00);
  sendCommand(start);
  sendCommand(0X00);
  sendCommand(stop);
  sendCommand(0X01);
  sendCommand(SSD_Activate_Scroll);
}

void ArduiPi_OLED::setHorizontalScrollProperties(
    bool direction, uint8_t startRow, uint8_t endRow, uint8_t startColumn,
    uint8_t endColumn, uint8_t scrollSpeed)
{
  if (Scroll_Right == direction) {
    // Scroll Right
    sendCommand(SSD_Left_Horizontal_Scroll);
  }
  else {
    // Scroll Left
    sendCommand(SSD_Right_Horizontal_Scroll);
  }
  sendCommand(0x00); // Dummmy byte
  sendCommand(startRow);
  sendCommand(scrollSpeed);
  sendCommand(endRow);
  sendCommand(startColumn + 8);
  sendCommand(endColumn + 8);
  sendCommand(0x00); // Dummmy byte
}

void ArduiPi_OLED::stopscroll(void) { sendCommand(SSD_Deactivate_Scroll); }

void ArduiPi_OLED::sendData(uint8_t c)
{
  // I2C
    char buff[2];

    // Setup D/C to switch to data mode
    buff[0] = SSD_Data_Mode;
    buff[1] = c;

    // Write on i2c
    fastI2Cwrite(buff, sizeof(buff));
}

void ArduiPi_OLED::display(void)
{


    sendCommand(SSD1306_Set_Lower_Column_Start_Address | 0x0);  // low col = 0
    sendCommand(SSD1306_Set_Higher_Column_Start_Address | 0x0); // hi col = 0
    sendCommand(SSD1306_Set_Start_Line | 0x0);                  // line #0


  uint16_t i = 0;

  // pointer to OLED data buffer
  uint8_t *p = poledbuff;

  // I2C
    char buff[17];
    uint8_t x;

    // Setup D/C to switch to data mode
    buff[0] = SSD_Data_Mode;

    if (oled_type == OLED_SH1106_I2C_128x64) {
      for (uint8_t k = 0; k < 8; k++) {
        sendCommand(0xB0 + k); // set page addressSSD_Data_Mode;
        sendCommand(0x00);     // set lower column address Aqui estava o problema da linha suja na esquerda
        sendCommand(0x10);     // set higher column address

        for (i = 0; i < 8; i++) {
          for (x = 1; x <= 16; x++)
            buff[x] = *p++;

          fastI2Cwrite(buff, 17);
        }
      }
    }
    else {
      // loop trough all OLED buffer and
      // send a bunch of 16 data byte in one xmission
      for (i = 0; i < oled_buff_size; i += 16) {
        for (x = 1; x <= 16; x++)
          buff[x] = *p++;

        fastI2Cwrite(buff, 17);
      }
    }
}

// clear everything (in the buffer)
void ArduiPi_OLED::clearDisplay(void) { memset(poledbuff, 0, oled_buff_size); }
