# Add custom user libraries and functions to the below list.
userlibs_list = [
    {
        'name': 'arduino',
        'functions': ['digitalRead', 'digitalWrite', 'pinMode'],
    },
    {
        'name': 'serial',
        'functions': ['printstr', 'println'],
    },
    {
        'name': 'spi',
        'functions': [],
    },
    {
        'name': 'i2c',
        'functions': [],
    },
    {
        'name': 'servo',
        'functions': [],
    },
    {
        'name': 'nrf24',
        'functions': [],
    },
    {
        'name': 'eeprom',
        'functions': [],
    },
    {
        'name': 'testuserlib',
        'functions': ['printstr', 'println', 'math_add', 'math_power'],
    },
]

userlibs = {}

def init():
    for idx_m, m in enumerate(userlibs_list):
        userlibs['userlibs.'+m['name']] = {'id': idx_m,
        'functions': {f:i for i, f in enumerate(m['functions'])}}

def main():
    import os
    path=os.path.dirname(os.path.abspath(__file__))+'/../bitsy_python_arduino/userlibs/gen/userlibs_defines.h'
    out = open(path, 'w')
    out.write("""\
// DO NOT EDIT THIS FILE
// This file is autogenerated from userlibsgen.py

""")
    for m in userlibs_list:
        if not m['name'].startswith('test'):
            out.write("#ifdef ARDUINO\n")
        out.write("""\
#include "userlibs/%s.h"
""" % (m['name']))
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
        if not m['name'].startswith('test'):
            out.write("#endif\n\n")

    out.write("""\

Variable call_userlib_function(uint8_t module, uint8_t function, uint8_t argcount, Variable arg[]) {
    switch(module) {
""")
    for i, m in enumerate(userlibs_list):
        if not m['name'].startswith('test'):
            out.write("#ifdef ARDUINO\n")
        out.write("""\
    case %d:
        return userlib_module_%s(function, argcount, arg);
""" % (i, m['name']))
        if not m['name'].startswith('test'):
            out.write("#endif\n\n")
    out.write("""\
    default:
        assert(false);
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
    for m in userlibs_list:
        if m['name'].startswith('test'): continue
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