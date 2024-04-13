# Macropad 8 Keys
Custom firmware for 8 key macropad 


Original post: 
(https://hackaday.io/project/189914)


### Diferences aganist original

i used the same chip (CH552g) on a different kind of keyboard with 8 keys
so i removed the rotatory encoder and added the support of macros up to 10 keys 
with a theorical maximum 14 keys


### Example data structure for the EEPROM


Address:   0  1  2  3  4  5  6  7  8  9 10 11 
Address:  00 01 02 03 04 05 06 07 08 09 0A 0B  (hex) 
----------------------------------------------
x0000000: T0 M0 C0 00 00 00 00 00 00 00 00 00  // key 1 // used only on keyboard keys
x000000c: T1 C0 00 00 00 00 00 00 00 00 00 00  // key 2 // used only on consumer keys
x0000018: T2 A0 C0 C1 C2 C3 C4 C5 C6 C7 C8 C9  // key 3 // used only on macro keys

T0 represents the type field of the first key
00 = keyboard key // 01 = consumer key // 02 = macro key

# For keyboard and consumer keys
M0 represents the mod field of the first key

C0 represents the code field of the first key
(rarelly you need a mod for consumer keys so it is not used)

C1 to C9 represents the extra code fields only used for macro keys

# For macro keys
A0 represents the ammount of keys in the macro

for visibility reasons any 00 after any data its just an FF or empty field
00 is different than FF 


### Current Example of structure for the EEPROM

some example secuences
380 = D S W W A S S
120 = D D S A D S
HELL = S W A S W D S W
AMMO = S S W D
REINFORCE = W S D A W

or in keyboard hex code
380 = 07 16 1A 1A 04 16 16
120 = 07 07 16 1E 07 16
HELL = 16 1A 04 16 1A 07 16 1A
AMMO = 16 16 1A 04
REINFORCE = 1A 16 07 1E 1A

some key codes both for keyboard and consumer

consumer keys
//  0xB3	Play/Pause Media key // 0xB0	Next Track key // 0xB1	Previous Track key // 0xAE	Volume Down key // 0xAF	Volume Up key // 0xAD	Volume Mute key

keyboard keys
//  number 1 = 1E // number 2 = 1F 

  W          1A
 A S D     04 16 07

note for readability the letters are in capital letters but the actual key presses are in lower case
if you want to have the secuences in capital letters fortunately they have the same hex code 
but you need to add the mod 01 for shift key press

# visual example

Address:   0  1  2  3  4  5  6  7  8  9 10 11 
Address:  00 01 02 03 04 05 06 07 08 09 0A 0B  (hex) 
----------------------------------------------
x0000000: 02 07 07 16 1A 1A 04 16 16 00 00 00  // key 1 //  Type Macro // Keys 7 // 380 //
x000000c: 02 06 07 07 16 1E 07 16 00 00 00 00  // key 2 //  Type Macro // Keys 6 // 120 //
x0000018: 02 08 16 1A 04 16 1A 07 16 1A 00 00  // key 3 //  Type Macro // Keys 8 // HELL //
x0000024: 02 04 16 16 1A 04 00 00 00 00 00 00  // key 4 //  Type Macro // Keys 4 // AMMO //
x0000030: 02 05 1A 16 07 1E 1A 00 00 00 00 00  // key 5 //  Type Macro // Keys 5 // REINFORCE //
x000003c: 00 00 1E 00 00 00 00 00 00 00 00 00  // key 6 //  Type Keyboard // Mod 0 // number 1 key //
x0000048: 01 B3 00 00 00 00 00 00 00 00 00 00  // key 7 //  Type Consumer // Play/Pause Media key //
x0000054: 00 00 00 00 00 00 00 00 00 00 00 00  // key 8 //  Type Keyboard // Mod 0 // Not used //

for now bytes from 54 to 127 are not used so they are filled with 0x00
by now the max ammount of keys a macro can have is 10 (theorical maximum 14 keys)
also the macro keys are in secuence, it doesnt support any modifier by now



### to compile:
`$ make bin`

### compile & flash to pad:
- if on original firmware: depending on hardware you need to connect P3.6 to
  5V (VCC) using a 1k resistor or P1.5 to GND, while connecting USB
- if on this firmware: press key1 while connecting USB
- `$ make flash`

### configure keys:
1. `$ isp55e0 --data-dump flashdata.bin`
2. edit first 6 bytes of this binary (3 keys, plus 3 for the knob), and write it back:
3. `$ isp55e0 --data-flash flashdata.bin`
