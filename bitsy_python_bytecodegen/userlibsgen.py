#!/usr/bin/env python
# Add custom user libraries and functions to the below list.
userlibs_list = [
    {
        'name': 'arduino',
        'functions': ['digitalRead', 'digitalWrite', 'pinMode', 'analogRead', 'analogWrite',
                      'delay', 'delayMicroseconds', 'millis', 'micros', 'attachInterrupt',
                      'detachInterrupt'],
    },
    {
        'name': 'serial',
        'functions': ['printstr', 'println'],
    },
    {
        'name': 'spi',
        'functions': ['begin', 'end', 'beginTransaction', 'endTransaction', 'transfer'],
    },
    {
        'name': 'i2c',
        'functions': ['begin', 'beginTransmission', 'endTransmission', 'write', 'available', 'read'],
    },
    {
        'name': 'servo',
        'functions': ['attach', 'detach', 'read', 'write'],
    },
    {
        'name': 'nrf24',
        'functions': ['init', 'set_on_recv_callback', 'send', 'powerdown', 'powerup'],
    },
    {
        'name': 'eeprom',
        'functions': ['read', 'write'],
    },
    {
        'name': 'esp8266wifiudp',
        'functions': ['set_on_recv_callback', 'send_response', 'local_ip'],
    },
    {
        'name': 'readvcc',
        'functions': ['readvcc'],
    },
    {
        'name': 'lowpower',
        'functions': ['powerdown'],
    },
    {
        'name': 'testuserlib',
        'functions': ['printstr', 'println', 'math_add', 'math_power',
            'init_callback', 'trigger_callback'],
    },
]

userlibs = {}

def init():
    for idx_m, m in enumerate(userlibs_list):
        userlibs['userlibs.'+m['name']] = {
            'name': m['name'],
            'id': idx_m,
            'functions': {f:i for i, f in enumerate(m['functions'])},
        }

def gen_userlib_module_handler_api(out, m):
    out.write("""\

#ifdef ENABLE_BITSY_USERLIB_%s
#include "../%s.h"

Variable userlib_module_%s(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
""" % (m['name'].upper(), m['name'], m['name']))
    for i, f in enumerate(m['functions']):
        out.write("""\
    case %d:
      return %s::%s(argcount, arg);
""" % (i, m['name'], f))
    out.write("""\
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif
""")

def main():
    import os
    path=os.path.dirname(os.path.abspath(__file__))+'/../bitsy_python_arduino/userlibs/gen/userlibs_defines.h'
    out = open(path, 'w')
    out.write("""\
// DO NOT EDIT THIS FILE
// This file is autogenerated from userlibsgen.py

#include "../../bitsylimit.h"
""")
    for m in userlibs_list:
        gen_userlib_module_handler_api(out, m)

    out.write("""\

bool is_userlib_module_enabled(uint8_t module) {
  switch (module) {
""")
    for idd, m in enumerate(userlibs_list):
        out.write("""\
#ifdef ENABLE_BITSY_USERLIB_%s
    case %d: // %s
#endif
""" % (m['name'].upper(), idd, m['name']))
    out.write("""\
      return true;
  }
  return false;
}
""")

    out.write("""\

Variable call_userlib_function(uint8_t module, uint8_t function, uint8_t argcount, Variable arg[]) {
    switch(module) {
""")
    for idd, m in enumerate(userlibs_list):
        out.write("""\
#ifdef ENABLE_BITSY_USERLIB_%s
    case %d:
      return userlib_module_%s(function, argcount, arg);
#endif
""" % (m['name'].upper(), idd, m['name']))

    out.write("""\
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
""")
    outpy = open(os.path.dirname(os.path.abspath(__file__))+'/userlibs/__init__.py', 'w')
    outpy.write("""\
# DO NOT EDIT THIS FILE
# This file is autogenerated from userlibsgen.py

""")
    for m in userlibs_list:
        outpy.write("import %s\n"%(m['name']))
    for m in [m for m in userlibs_list if not m['name'].startswith('test')]:
        path=os.path.dirname(os.path.abspath(__file__))+'/userlibs/'+m['name']+'.py'
        outpy = open(path, 'w')
        outpy.write("""\
# DO NOT EDIT THIS FILE
# This file is autogenerated from userlibsgen.py

""")
        for f in m['functions']:
            outpy.write("""\
def %s(*arg):
    pass

""" % (f))


init()
if __name__ == "__main__":
    main()
