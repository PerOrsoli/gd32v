# GD32V

GigaDevice GD32V RISC-V MCU  
Example for Sipeed Longan Nano development board.  
WIP  

3rd party:
- init
- clocks
- timers
- some of libc bits & pieces

mine:
- build system (GNU make & LD)
- GPIO & GPIO tests (run on MCU)
- EXTI
- UART
- I2C (with external patch for init (baudrate generation))
- PWM (just prototype - uses peripheral lib)
- RTC
- some of libc bits & pieces

drivers for external peripherals
- EEPROM 24C256
- barometer BMP180
