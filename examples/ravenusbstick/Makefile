all:
	${MAKE} TARGET=avr-ravenusb NOAVRSIZE=1 -f Makefile.ravenusbstick ravenusbstick.elf
	avr-objcopy -O ihex -R .signature -R .fuse -R .eeprom ravenusbstick.elf ravenusbstick.hex
	avr-size -C --mcu=at90usb1287 ravenusbstick.elf
	avr-objcopy -O ihex -j .eeprom ravenusbstick.elf ravenusbstick-eeprom.hex

clean:
	${MAKE} -f Makefile.ravenusbstick clean
	rm -rf obj_avr-ravenusb
	rm -rf symbols.c symbols.h ravenusbstick.elf ravenusbstick.hex

burn: all
	dfu-programmer at90usb1287 flash ravenusbstick.hex
	dfu-programmer at90usb1287 start
