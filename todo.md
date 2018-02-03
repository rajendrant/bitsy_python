TODO

Many of the following TODO are needed to support a basic and good working version of bitsy_python.


HIGH Priority

Support for basic interrupt usage. At least one interrupt.

Builtin constants support such as None, True, False - DONE

Ability for userlibs(which use more SRAM such as serial, i2c) to be disabled and is_userlib_disabled() used to check for disabled libraries.  - DONE

RF24 library support.  - DONE

Lowpowersleep library support.  - DONE

VCCVoltage userlib to read VCC. - DONE

Read and run bitsy code from EEPROM with example.  - DONE

Write bitsy code to EEPROM using Serial with example. - DONE

Write bitsy code to EEPROM using nrf24 with example.  - DONE




MEDIUM Priority

Advanced interrupt usage with interrupts on multiple pins enabled.

Dynamic memory support for objects. - DONE

range() support needs get_iter instruction to be implemented. - DONE

Add unittest for FunctionStack that uses the generic block allocater. Dynamic memory support will break this.

Classes and objects.

Strings

bitsy userlib to report the runtime memory usage (current, high, low)

A lot of common examples.
