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

// Firmware Description

// some secuences
// 380 = D S W W A S S
// 120 = D D S A D S
// HELL = S W A S W D S W
// AMMO = S S W D
// REINFORCE = W S D A W
// note for readability the secuences are in capital letters but the actual key presses are in lower case

// or in keyboard hex code
// 380 = 07 16 1A 1A 04 16 16
// 120 = 07 07 16 1E 07 16
// HELL = 16 1A 04 16 1A 07 16 1A
// AMMO = 16 16 1A 04
// REINFORCE = 1A 16 07 1E 1A

// some key codes both for keyboard and consumer

// consumer keys
//  0xB3	Play/Pause Media key // 0xB0	Next Track key // 0xB1	Previous Track key // 0xAE	Volume Down key // 0xAF	Volume Up key // 0xAD	Volume Mute key

// keyboard keys
//  number 1 = 1E // number 2 = 1F 

//  W          1A
// A S D     04 16 07

// note for readability the letters are in capital letters but the actual key presses are in lower case
// if you want to have the secuences in capital letters fortunately they have the same hex code 
// but you need to add the mod 01 for shift key press

// current eeprom data structure and random data just for testing

// Address:   0  1  2  3  4  5  6  7  8  9 10 11 
// Address:  00 01 02 03 04 05 06 07 08 09 0A 0B  (hex) 
//----------------------------------------------
// x0000000: 02 07 07 16 1A 1A 04 16 16 00 00 00  // key 1 //  Type Macro // Keys 7 // 380 //
// x000000c: 02 06 07 07 16 1E 07 16 00 00 00 00  // key 2 //  Type Macro // Keys 6 // 120 //
// x0000018: 02 08 16 1A 04 16 1A 07 16 1A 00 00  // key 3 //  Type Macro // Keys 8 // HELL //
// x0000024: 02 04 16 16 1A 04 00 00 00 00 00 00  // key 4 //  Type Macro // Keys 4 // AMMO //
// x0000030: 02 05 1A 16 07 1E 1A 00 00 00 00 00  // key 5 //  Type Macro // Keys 5 // REINFORCE //
// x000003c: 00 00 1E 00 00 00 00 00 00 00 00 00  // key 6 //  Type Keyboard // Mod 0 // number 1 key //
// x0000048: 01 B3 00 00 00 00 00 00 00 00 00 00  // key 7 //  Type Consumer // Play/Pause Media key //
// x0000054: 00 00 00 00 00 00 00 00 00 00 00 00  // key 8 //  Type Keyboard // Mod 0 // Not used //

// for now bytes from 54 to 127 are not used so they are filled with 0x00
// by now the max ammount of keys a macro can have is 10

// Example of the eeprom data structure

// Address:   0  1  2  3  4  5  6  7  8  9 10 11 
// Address:  00 01 02 03 04 05 06 07 08 09 0A 0B  (hex) 
//----------------------------------------------
// x0000000: T0 M0 C0 00 00 00 00 00 00 00 00 00  // key 1 // used only on keyboard keys
// x000000c: T1 C0 00 00 00 00 00 00 00 00 00 00  // key 2 // used only on consumer keys
// x0000018: T2 A0 C0 C1 C2 C3 C4 C5 C6 C7 C8 C9  // key 3 // used only on macro keys

// T0 represents the type field of the first key
// 00 = keyboard key // 01 = consumer key // 02 = macro key

// For keyboard and consumer keys
// M0 represents the mod field of the first key

// C0 represents the code field of the first key
// (rarelly you need a mod for consumer keys so it is not used)

// C1 to C9 represents the extra code fields only used for macro keys

// For macro keys
// A0 represents the ammount of keys in the macro

// for visibility reasons any 00 after any data its just an FF or empty field
// 00 is different than FF 

// George mod v0.1
/*
mods:

now it has 8 keys
i removed the knob
theorically i still have 1 extra pinout to asign 1 more key
i hate red and blue colors so the Neopixels only turn green




*/



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

uint8_t index = 0;
uint8_t incremental = 12;
uint8_t ammountOfMacroKeys = 0;

// #define KEY_EEPROM_FIELDS 12

// structur with key details
struct key
{
  enum KeyType type;
  uint8_t mod;
  uint8_t ammount;
  char code[10];
  uint8_t last;
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
        KBD_code_press(key->mod, key->code[0]); // press keyboard/keypad key
      }

      else if (key->type == CONSUMER)
      {
        CON_press(key->code[0]); // press consumer key
      }

      else if (key->type == MACRO)
      {
        execute_macro(current, key, neo);
      }
      if (neo)
        *neo = NEO_MAX; // light up corresponding NeoPixel
    }
    else
    { // key was released?
      if (key->type == KEYBOARD)
      {
        KBD_code_release(key->mod, key->code); // release
      }
      else if (key->type == CONSUMER)
      {
        CON_release(key->code); // release
      }
      // no need to release macro keys but enter cooldown in case it is a macro key
      if (key->type == MACRO)
      {
        DLY_ms(250);
      }
    }
  }
  else if (key->last)
  { // key still being pressed?
    if (neo)
      *neo = NEO_MAX; // keep NeoPixel on
  }
  
}

// ===================================================================================
// Experimental area
// not yet fully implemented

// execute macro witch is a sequence of 11 key presses on a single press of a key in the keyboard, the key secuence are read from the eeprom only if key type is MACRO
void execute_macro(uint8_t current, struct macro *macro, uint8_t *neo)
{

  for (i = 0; i < 10; i++)
  {
    KBD_code_press(macro->code[i]););
    DLY_ms(10);
    KBD_code_release(macro->code[i]););
    DLY_ms(10);
  }
  DLY_ms(250);
}

// ===================================================================================

// Main Function
void main(void)
{
  // Variables
  struct key keys[8]; // array of struct for keys
  __idata uint8_t i;  // temp variable
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
      keys[i].mod = eeprom_read_byte(index + 1);
      keys[i].code[0] = eeprom_read_byte(index + 2);
      keys[i].last = 0;
    }
    else if (eeprom_read_byte(index) == 1)
    {
      keys[i].type = CONSUMER;
      keys[i].code[0] = eeprom_read_byte(index + 1);
      keys[i].last = 0;
    }
    else if (eeprom_read_byte(index) == 2)
    {
      keys[i].type = MACRO;
      keys[i].ammount = eeprom_read_byte(index + 1);
      for (j = 0; j < macros[i].ammount; j++)
      {
        keys[i].code[j] = eeprom_read_byte(index + 2 + j);
      }
      keys[i].last = 0;
    }
    index += incremental;
  }

  /*
   // Backup in case i fuck up the code
    for (i = 0; i < 8; i++)
    {
      keys[i].mod = (char)eeprom_read_byte(i * KEY_EEPROM_FIELDS);
      keys[i].type = eeprom_read_byte(i * KEY_EEPROM_FIELDS + 1);
      keys[i].code = (char)eeprom_read_byte(i * KEY_EEPROM_FIELDS + 2);
      keys[i].last = 0;
  }

  */

  //================================================================================================

  // Loop
  while (1)
  {
    handle_key(!PIN_read(PIN_KEY1), &keys[0], &neo[0]);
    handle_key(!PIN_read(PIN_KEY2), &keys[1], &neo[1]);
    handle_key(!PIN_read(PIN_KEY3), &keys[2], &neo[3]);
    handle_key(!PIN_read(PIN_KEY4), &keys[3], &neo[4]);
    handle_key(!PIN_read(PIN_KEY5), &keys[4], &neo[5]);
    handle_key(!PIN_read(PIN_KEY6), &keys[5], &neo[6]);
    handle_key(!PIN_read(PIN_KEY7), &keys[6], &neo[7]);
    handle_key(!PIN_read(PIN_KEY8), &keys[7], &neo[8]);

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
