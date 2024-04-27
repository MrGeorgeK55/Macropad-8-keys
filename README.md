# Macropad 8 Keys


Custom firmware for 8 key macropad (Based on IC CH552g)

![image](https://github.com/MrGeorgeK55/Macropad-8-keys/assets/103085400/7f72080f-97e8-4881-b3bb-01eee13f383a)

![image](https://github.com/MrGeorgeK55/Macropad-8-keys/assets/103085400/f0827927-be25-41ae-b11f-6206588b15ee)



So, I bought a cheap macro pad with 8 keys, 8 RGB LEDs, and a power switch. The hardware is fine, but the original IC is not reprogramable, so I replaced it with the CH552G and did custom wiring.  
If you are asking why I didn't used the original IC, let me ask you first: Do you trust a chinesse .exe file in a rar with password downloaded from a sketchy page just to configure the keys? of-fucking-course the answer is NO!.

(im working on the schematic)  

Original post: 
(https://hackaday.io/project/189914)


## Diferences aganist original


i used the same chip (CH552g) on a different kind of keyboard with 8 keys
so i removed the rotatory encoder and added the support of macros up to 10 keys
with a theorical maximum 14 keys, also i have an unused I/O pin 


# Example data structure for the EEPROM


| Hex Address | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B |
|-----------|----|----|----|----|----|----|----|----|----|----|----|----|
| x0000000: | T0 | M0 | C0 | M1 | C1 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 
| x000000c: | T1 | C0 | C1 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 
| x0000018: | T2 | A0 | C0 | C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8 | C9 |

T0 represents the type field of the first key  
00 = keyboard key // 01 = consumer key // 02 = macro key  

### For keyboard and consumer keys
T0 represents the type field of the first key  
00 = keyboard key // 01 = consumer key // 02 = macro key  
  
For keyboard  
M0 represents the mod field of the first keyboad key  
M1 represents the mod field of the second keyboard key   
  
C0 represents the code field of the first keyboad key  
C1 represents the code field of the second keyboard key   
  
For Consumer   
C0 represents the code field of the first consumer key  
C1 represents the code field of the second consumer key  
(rarely you need a mod for consumer keys so it is not used)  
  
> [!NOTE] 
> The keyboard and consumer keys have 2 states, this because in consumer keys Play/Pause are two different keys and for Keyboard becomes useful to have Ctrl+c and Ctrl+v in the same key  


### For macro keys
A0 represents the ammount of keys in the macro  
**This number must be in Hex value, not decimal**

  
*for visibility reasons any 00 after any data its just an FF or empty field   
00 is different than FF   

## Visual Example data

### Keyboard type

| Raw bytes         | Type     | Mod | Key | Mod2 | Key2 | Result | Result2 |
| ----------- | --- | ----     | ---- | --- | --- | --- | --- |  
| 00 00 04 00 04 .. | Keyboard | None | a | None | a | a | a |
| 00 02 04 02 04 .. | Keyboard | Shift | a | Shift | a | A | A |
| 00 01 06 01 19 .. | Keyboard | Ctrl | c | Ctrl | v | Ctrl+C | Ctrl+V |
| 00 05 4C 03 29 .. | Keyboard | Ctrl+Alt | DEL | Ctrl+Shift | ESC | Ctrl+Alt+DEL | Ctrl+Shift+ESC |


### Consumer Type  

| Raw bytes | Type | Key | Key2 |
| -------- | ------ |  --- |  --- | 
| 01 B6 B6 .. | Consumer | Previous | Previous | 
| 01 B0 B1 .. | Consumer | Play | Pause |
| 01 E9 E9 .. | Consumer | Vol + | Vol + |
| 01 EA EA .. | Consumer | Vol - | Vol - |

### Macro Type

| Raw bytes | Type | # of Keys | Code | Code1 | Code2 | Result |   
| -------- | ------ |  --- |  --- |  --- | --- | --- |
| 02 03 06 04 17 .. | Macro | 3 | c | a | t | cat |
| 02 03 07 12 0A .. | Macro | 3 | d | o | g | dog |

# Compilation

### to compile:
`$ make bin`

### compile & flash to pad:
- if on original firmware: depending on hardware you need to connect P3.6 to
  5V (VCC) using a 1k resistor or P1.5 to GND, while connecting USB
- if on this firmware: press key1 while connecting USB
- `$ make flash`

### configure keys:
1. `$ isp55e0 --data-dump flashdata.bin`
2. edit the bytes of this binary using the example form above, and write it back:
3. `$ isp55e0 --data-flash flashdata.bin`


# Notes:

### Functions not supported yet by only reading the EEPROM  
- Strings of text (you may need a bigger EEPROM) (maybe a huffman algorithm)
- Mouse movements/clicks (not sure, not tested)
- Modifiers on macro secuences
- Modifiers on consumer keys  
  
i still have not found the way to make work the RIGTH_GUI and LEFT_GUI keys  
(aka Windows keys or Apple equivalent)  
  
for now bytes from 60 to 127 are not used so they are filled with 0xFF  
the default code uses 10 keys per macro but you can change it to maximum of 14 keys per macro  
also the macro keys are in sequence, it doesnt support any modifier by now  
you can ajust the timing between presses changing the delay in the delayMacro variable 
  
theorically we can assign up to 14 keys to each macro  
128 bytes of eeprom divided by 8 keys = 16 bytes per key  
16 bytes minus 2 bytes for the type and ammount of keys = 14 bytes (macros up to 14 keys)  


### Modifiers

 | ID | Function |  
 | --- | --- |
 | 00 | None  |  
 | 01 | Ctrl  |  
 | 02 | Shift | 
 | 03 | Ctrl + Shift |   
 | 04 | Alt |   
 | 05 | Ctrl + Alt | 
 | 06 | Shift + Alt | 
 | 07 | Ctrl + Shift + Alt | 

### Consumer Keyboard Keycodes
  

|ID	|Function|
| --- | --- |
|30	|SYS_POWER |
|31|	SYS_RESET|
|32| SYS_SLEEP|
|E2|	VOL_MUTE|
|E9|	VOL_UP|
|EA|	VOL_DOWN|
|B0|	CON_MEDIA_PLAY|
|B1|	CON_MEDIA_PAUSE|
|B2| CON_MEDIA_RECORD|
|B3|	MEDIA_FORWARD|
|B4|	MEDIA_REWIND|
|B5|	MEDIA_NEXT|
|B6|	MEDIA_PREV|
|B7|	MEDIA_STOP|
|B8|	MEDIA_EJECT|
|B9|	MEDIA_RANDOM|
|40|	MENU|
|41|	MENU_PICK|
|42|	MENU_UP|
|43|	MENU_DOWN|
|44|	MENU_LEFT|
|45|	MENU_RIGHT|
|46|	MENU_ESCAPE|
|47|	MENU_INCR|
|48|	MENU_DECR|


# Documentation

  [CH552x Datasheet ](https://www.wch-ic.com/downloads/CH552DS1_PDF.html)  
  [USB HID Codes](https://usb.org/sites/default/files/hut1_21_0.pdf#page=83)  
  [isp55e0](https://github.com/frank-zago/isp55e0)  

### Yet to implement

- After a key is pressed and changes to Second key, add a delay to change it automatically to First Key 
- 
-
(no idea of what else could it need)
