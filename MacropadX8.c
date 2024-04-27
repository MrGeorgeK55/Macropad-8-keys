// ===================================================================================
// Project:   MacroPad Mini for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Firmware example implementation for the MacroPad Mini.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: min. 12 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Press a macro key and see what happens.
// - To enter bootloader hold down key 1 while connecting the MacroPad to USB. All
//   NeoPixels will light up white as long as the device is in bootloader mode
//   (about 10 seconds).

// ===================================================================================

// George mod v0.6
//last changes: Added toggle functionality to the keys 
// see the readme to check the structure of the eeprom


// Libraries
#include <config.h>     // user configurations
#include <system.h>     // system functions
#include <delay.h>      // delay functions
#include <neo.h>        // NeoPixel functions
#include <usb_conkbd.h> // USB HID consumer keyboard functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB)
{
  USB_interrupt();
}

enum KeyType
{
  KEYBOARD = 0,
  CONSUMER = 1,
  MACRO = 2
};

// theorically we can assign up to 14 keys to each macro
// 128 bytes divided by 8 keys = 16 bytes per key 
// 16 minus 2 bytes for the type and ammount of keys = 14 bytes (macros up to 14 keys)


// by now it will only read the first 12 bytes of each key
// you only need to change this value if you want to use more keys
// additionaly change  uint8_t code[X]; with the maximum ammount of keys you want to use
// and change the delay between key presses in the macro changing the value of delayMacro (ms)

int index = 0;
int __xdata incremental = 12; //size of rows of bytes in the eeprom to easily visualize the data
int delayMacro = 5;

// structur with key details
// to make it easier to read, i changed the order of the fields
__xdata struct key
{
  enum KeyType type;     // type of key 00 = keyboard key // 01 = consumer key // 02 = macro key
  uint8_t mod[2];           // modifier of the key (only used in Keyboard type)
  uint8_t ammount;       // ammount of keys in the macro (only used in Macro type)
  uint8_t code[10];      // code of the key (only used in Keyboard and Macro type)
  uint16_t codeConsumer[2]; // code of the key (only used in Consumer type) (was getting an error when using an array)
  uint8_t last;          // last state of the key
  uint8_t toggle;           // toggle for the key
};

// NeoPixel Functions
// Update NeoPixels
void NEO_update(uint8_t *neo)
{
  EA = 0;                       // disable interrupts
  NEO_writeColor(0, neo[0], 0); // Green on neo 1
  NEO_writeColor(0, neo[1], 0); // Green on neo 1
  NEO_writeColor(0, neo[2], 0); // Green on neo 1
  NEO_writeColor(0, neo[3], 0); // Green on neo 1
  NEO_writeColor(0, neo[4], 0); // Green on neo 1
  NEO_writeColor(0, neo[5], 0); // Green on neo 1
  NEO_writeColor(0, neo[6], 0); // Green on neo 1
  NEO_writeColor(0, neo[7], 0); // Green on neo 1
  EA = 1;                       // enable interrupts
}

// Read EEPROM (stolen from https://github.com/DeqingSun/ch55xduino/blob/ch55xduino/ch55xduino/ch55x/cores/ch55xduino/eeprom.c)
// not changed, no idea how it works or what it does
uint8_t eeprom_read_byte(uint8_t addr)
{
  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
  ROM_ADDR_L = addr << 1; // Addr must be even
  ROM_CTRL = ROM_CMD_READ;
  return ROM_DATA_L;
}

// handle key press
void handle_key(uint8_t current, struct key *key, uint8_t *neo)
{
  if (current != key->last)
  {                      // state changed?
    key->last = current; // update last state flag
    if (current)
    { // key was pressed?
      if (key->type == KEYBOARD)
      {
        if (key->toggle == 0)
        {
          KBD_code_press(key->mod[0], key->code[0]); // press keyboard/keypad key
        }
        else
        {
          KBD_code_press(key->mod[1], key->code[1]); // release
        }
      }

      else if (key->type == CONSUMER)
      {
        if (key->toggle == 0)
        {
          CON_press(key->codeConsumer[0]); // press consumer key
        }
        else
        {
          CON_press(key->codeConsumer[1]); // release
        }
      }

      else if (key->type == MACRO)
      {
        for (int i = 0; i < key->ammount; i++)
        {
          KBD_code_press(0, key->code[i]);
          DLY_ms(5);
          KBD_code_release(0, key->code[i]);
          DLY_ms(5);
        }
      }

      if (neo)
        *neo = NEO_MAX; // light up corresponding NeoPixel
    }
    else
    { // key was released?
      if (key->type == KEYBOARD)
      {
        if (key->toggle == 0)
        {
          KBD_code_release(key->mod[0], key->code[0]); // release
          key->toggle = 1;
        }
        else
        {
          KBD_code_release(key->mod[1], key->code[1]); // press keyboard/keypad key
          key->toggle = 0;
        }
      }
      else if (key->type == CONSUMER)
      {
        if (key->toggle == 0)
        {
          CON_release(key->codeConsumer[0]); // release
          key->toggle = 1;
        }
        else
        {
          CON_release(key->codeConsumer[1]); // release
          key->toggle = 0;
        }
      }
      // no need to release macro keys but enter cooldown in case it is a macro key
      if (key->type == MACRO)
      {
        DLY_ms(50);
      }
    }
  }
  else if (key->last)
  { // key still being pressed?
    if (neo)
      *neo = NEO_MAX; // keep NeoPixel on
  }
}

// Main Function
void main(void)
{
  // Variables
  __xdata struct key keys[8]; // array of struct for keys
  __idata uint8_t i;
  __idata uint8_t j;  // temp variable
  uint8_t neo[8] =
      {0, NEO_MAX, 0}; // brightness of NeoPixels

  // Enter bootloader if key 1 is pressed
  NEO_init(); // init NeoPixels
  if (!PIN_read(PIN_KEY1))
  {              // key 1 pressed?
    NEO_latch(); // make sure pixels are ready
    for (i = 9; i; i--)
      NEO_sendByte(NEO_MAX); // light up all pixels
    BOOT_now();              // enter bootloader
  }

  // Setup
  CLK_config(); // configure system clock
  DLY_ms(5);    // wait for clock to settle
  KBD_init();   // init USB HID keyboard
  WDT_start();  // start watchdog timer


  // TODO: Read eeprom for key characters
  // it sohuld read each row of 12 bytes and assign them to the key struct
  for (i = 0; i < 8; i++)
  {
    if (eeprom_read_byte(index) == 0)
    {
      keys[i].type = KEYBOARD;
      keys[i].mod[0] = eeprom_read_byte(index + 1);
      keys[i].code[0] = eeprom_read_byte(index + 2);
      keys[i].mod[1] = eeprom_read_byte(index + 3);
      keys[i].code[1] = eeprom_read_byte(index + 4);
      keys[i].last = 0;
      keys[i].toggle = 0;
    }
    else if (eeprom_read_byte(index) == 1)
    {
      keys[i].type = CONSUMER;
      keys[i].codeConsumer[0] = eeprom_read_byte(index + 1);
      keys[i].codeConsumer[1] = eeprom_read_byte(index + 2);
      keys[i].last = 0;
      keys[i].toggle = 0;
    }
    else if (eeprom_read_byte(index) == 2)
    {
      keys[i].type = MACRO;
      keys[i].ammount = eeprom_read_byte(index + 1);
      for (j = 0; j < keys[i].ammount; j++)
      {
        keys[i].code[j] = eeprom_read_byte(index + 2 + j);
      }
      keys[i].last = 0;
    }
    index += incremental;
  }


  // Loop
  while (1)
  {
    handle_key(!PIN_read(PIN_KEY1), &keys[0], &neo[0]);
    handle_key(!PIN_read(PIN_KEY2), &keys[1], &neo[1]);
    handle_key(!PIN_read(PIN_KEY3), &keys[2], &neo[2]);
    handle_key(!PIN_read(PIN_KEY4), &keys[3], &neo[3]);
    handle_key(!PIN_read(PIN_KEY5), &keys[4], &neo[4]);
    handle_key(!PIN_read(PIN_KEY6), &keys[5], &neo[5]);
    handle_key(!PIN_read(PIN_KEY7), &keys[6], &neo[6]);
    handle_key(!PIN_read(PIN_KEY8), &keys[7], &neo[7]);

    // Update NeoPixels
    NEO_update(neo);
    for (i = 0; i < 8; i++)
    {
      if (neo[i] > NEO_GLOW)
        neo[i]--; // fade down NeoPixel
    }
    DLY_ms(5);   // latch and debounce
    WDT_reset(); // reset watchdog
  }
}
