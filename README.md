### This is a USB mouse project and is based on the STM32F429i-discovery board.

#### Branches:

* **master**

  initial sources
  
* **lcd_ts**

 project where was developed only LCD and touch-screen drivers without USB code

* **usb_gyro**

 at the beginning sources with only USB and gyroscope code but at some point sources from **lcd_ts** branch were added into the project.
 
  > Note: This branch contain all required sources.

#### About project:

All necessary parts to create this USB mouse are already on the board and no external parts are required.

The LCD was used to show left and right buttons and also vertical slider in the middle which represents scroll.

The touch-panel was used to detect user actions such as left and right click. If user moves finger vertically in a straight line between buttons then this action is detected as scrolling.

A gyroscope is using to detect movements: left, right, up or down. Too bad that there is no accelerometer on that board because a gyroscope detects only angular rate and it is hard to control a cursor on the screen.

The source code has been written in C and contain HAL and BSP drivers. The functionality of some BSP ( mostly for touch-panel ) drivers was extended and modified for the specific application. For example the touch-panel's driver can call registered functions when left/right button was pressed or movement on the slider was detected - something like observe pattern.

Everywhere where was possible the hardware is using. For example the LCD is controlled by LVDS peripheral and also DMA2D with external SDRAM memory provides foreground and background frames to display on the LCD. Thanks to all of that the CPU usage is reduced to the minimum.

USB HID is built on HAL drivers and SysTick timer with period of 1ms is using to keep USB timing.
