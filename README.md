# La Hangman

## What is this?
La Hangman is simple hangman game for the LaFortuna Microcontroller.


## How to play
Use the rotary controller and centre button select an option on the menu.
Options:
* VS Computer - Guess words from a set randomly chosen by the computer.
* VS Human - Enter in any word of your choice then handover to someone else for them to guess!

Guess the word before the hangman is complete!

Keyboard controls:
Rotary controller/directional buttons to select letters, and centre button to enter.

Additional notes:
You can quit at any time while playing by entering the "Quit" button on the keyboard. This shows what the word was and returns to the menu.
When entering your own word, type out the letters until you have finished, then select "Enter" to start guessing. You can also backspace the word if you went wrong, and also you can add spaces by entering the "Space" button. It is also possible to enter lowercase characters by switching the keyboard with "abc". VS Computer words are all CAPS.


## Pictures


## Flash instructions
To flash onto the LaFortuna make sure you have a .hex file.

Requirements: dfu-programmer.exe

1. Plug in the LaFortuna to your computer using the USB to Mini-USB cable.
2. Press the S6 button atop the LaFortuna. The screen should turn off and a connection made sound should play.
3. Open the command line in directory containing the hex file.
4. Enter `dfu-programmer.exe at90usb1286 erase`.
5. Enter `dfu-programmer.exe at90usb1286 flash hangman.hex`, where "hangman.hex" is the name of the hex file.
6. Wait until it had finished then either switch and unswitch the USB to BAT switch on the right of the LaFortuna, or unplug and plug in the USB cable.

You should now be able to play La Hangman!


## Compilation instructions
If you do not have a (working) .hex file to flash then this is how to compile (using windows).

Requirements: Programmer's Notepad, WinAVR Tool chain.

1. Open hangman.c in Programmer's Notepad
2. Tools -> [WinAVR] Make Clean
3. Tools -> [WinAVR] Make All

The file "hangman.hex" should now be in the directory.