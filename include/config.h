// ===================================================================================
// User Configurations for CH552E USB MacroPad Mini
// ===================================================================================

#pragma once

//other
#define KEY_AMMOUNT 8 //also the number of Neopixels only if all of each keys has a neopixel

// Pin definitions
#define PIN_NEO             P34         // pin connected to NeoPixel
#define PIN_KEY1            P11         // pin connected to key 1
#define PIN_KEY2            P17         // pin connected to key 2
#define PIN_KEY3            P16         // pin connected to key 3
#define PIN_KEY4          P33         // pin connected to knob switch
#define PIN_KEY5           P31         // pin connected to knob outA
#define PIN_KEY6           P30         // pin connected to knob outB
#define PIN_KEY7            P32
#define PIN_KEY8            P15

// NeoPixel configuration
#define NEO_GRB                         // type of pixel: NEO_GRB or NEO_RGB
#define NEO_MIN             0
#define NEO_GLOW            8
#define NEO_MAX             127

// USB device descriptor
#define USB_VENDOR_ID       0x1189      // VID
#define USB_PRODUCT_ID      0x8890      // PID
#define USB_DEVICE_VERSION  0x0100      // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50          // max power in mA

// USB descriptor strings
#define MANUFACTURER_STR    'M','r','-','G','T','H'
#define PRODUCT_STR         'M','a','c','r','o','P','a','d','x','8'
#define SERIAL_STR          'C','H','5','5','2','x','H','I','D'
#define INTERFACE_STR       'H','I','D','-','K','e','y','b','o','a','r','d'
