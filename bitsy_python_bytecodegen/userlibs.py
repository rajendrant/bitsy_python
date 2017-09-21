userlibs_list = [
    {
        'name': 'arduino',
        'header': 'arduino.h',
        'functions': ['digitalRead', 'digitalWrite', 'pinMode'],
    },
    {
        'name': 'serial',
        'header': 'serial.h',
        'functions': ['printstr', 'println'],
    },
    {
        'name': 'spi',
        #'header': 'spi.h',
        'functions': [],
    },
    {
        'name': 'i2c',
        #'header': 'i2c.h',
        'functions': [],
    },
    {
        'name': 'servo',
        #'header': 'servo.h',
        'functions': [],
    },
    {
        'name': 'nrf24',
        #'header': 'nrf24.h',
        'functions': [],
    },
    {
        'name': 'eeprom',
        #'header': 'eeprom.h',
        'functions': [],
    },
]

userlibs = {}

def init():
    for idx_m, m in enumerate(userlibs_list):
        userlibs[m['name']] = {'id': idx_m,
        'functions': {f:i for i, f in enumerate(m['functions'])}}


def main():
    import os
    path=os.path.dirname(os.path.abspath(__file__))+'/../bitsy_python_arduino/userlibs/gen/userlibs_defines.h'
    out = open(path, 'w')
    out.write("""\
// DO NOT EDIT THIS FILE
// This file is autogenerated from userlibs.py

""")
    for m in userlibs_list:
        if 'header' in m:
            out.write("""\
#include "userlibs/%s"
""" % (m['header']))

    for m in userlibs_list:
        out.write("""
Variable userlib_module_%s(uint8_t function, uint8_t argcount, Variable arg[]) {
    switch(function) {
""" % (m['name']))
        for i, f in enumerate(m['functions']):
            out.write("""\
    case %d:
        return %s::%s(argcount, arg);
""" % (i, m['name'], f))
        out.write("""\
    default:
        assert(false);
    }
    return Variable::Zero();
}
""")
    out.write("""\

Variable call_userlib_function(uint8_t module, uint8_t function, uint8_t argcount, Variable arg[]) {
    switch(module) {
""")
    for i, m in enumerate(userlibs_list):
        out.write("""\
    case %d:
        return userlib_module_%s(function, argcount, arg);
""" % (i, m['name']))
    out.write("""\
    default:
        assert(false);
    }
    return Variable::Zero();
}
""")

init()
if __name__ == "__main__":
    main()
