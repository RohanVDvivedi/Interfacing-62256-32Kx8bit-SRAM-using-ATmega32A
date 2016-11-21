# Interfacing-62256-32Kx8bit-SRAM-using-ATmega32A
This repository is a collection of a program that helps interfacing of a microcontroller with ATmega32A microcontroller.You could control the contents of the RAM using your Computer.

A learning oriented project done to just ensure the correctness of the 62256 32k x 8 SRAM chips that i received recently

It uses ATmega32A microcontroller burned with the "uC-SRAM-interfacing.cpp" using your AVR programmer, after compiling from avr8 gcc compiler. (sorry for the obviousness)

Connect the pins of SRAM and uC as shown.
Connect your uC to PC by UART-USB communication,
i have used PL2303HXA for USB to UART
Compile and run the "console-control-for-RAM.cpp" on your desktop laptop computer.

In the Console :
"Admin :" is where you type the input command.
and "Microcontroller :" is where the uC responds.

here on both the sides ';' is the string termination character.

to read the command is:
              "R-{address in RAM in decimal number};"  (without quotes)
              
to write the command is:
              "W-{data to be written}-{address where you want to write data};"        (without quotes)
              
Example :
  to read from address 11322;
  R-11322;
  
  to write 112 to address 12136;
  W-112-12136;
  
Future Applications: (in any project, not as in future technical invention)
  1. Possibly none
  2. to check the working condition of old days RAM ic chips.
  3. to load machine code in RAM of your self made Processor.
  4. This setup without RAM can be used to read and/or Program microcontrollers using parallel port but in machine code
  
  
  youtube link :: https://www.youtube.com/watch?v=psKOW52DNus&t=3s
