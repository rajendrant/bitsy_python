TODO

Many of the following TODO are needed to support a basic and good working version of bitsy_python.


HIGH Priority

Support for basic interrupt usage. At least one interrupt.

Builtin constants support such as None, True, False

Ability for userlibs(which use more SRAM such as serial, i2c) to be disabled and is_userlib_disabled() used to check for disabled libraries.

RF24 library support.

Lowpowersleep library support.

VCCVoltage userlib to read VCC.

Read and run bitsy code from EEPROM with example.

Write bitsy code to EEPROM using Serial with example.

Write bitsy code to EEPROM using nrf24 with example.




MEDIUM Priority

Advanced interrupt usage with interrupts on multiple pins enabled.

Dynamic memory support for objects.

range() support needs get_iter instruction to be implemented.

Add unittest for FunctionStack that uses the generic block allocater. Dynamic memory support will break this.

Classes and objects.

Strings

bitsy userlib to report the runtime memory usage (current, high, low)

A lot of common examples.

