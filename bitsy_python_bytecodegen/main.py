#!/usr/bin/env python
import bisect
import bitstring
import dis
import importlib
import math
import os
import random
import sys
import types
import userlibsgen

builtins = [
  # This should be in sync with BitsyBuiltin in Builtins.h
  'range',
  'type',
  'iter',
  'getattr',
  'bytearray',
  'len',
]
builtins = {k: idd for idd, k in enumerate(builtins)}

ins_supported = set((
  'LOAD_CONST',
  'LOAD_FAST', 'STORE_FAST', 'DELETE_FAST',
  'LOAD_GLOBAL', 'STORE_GLOBAL', 'DELETE_GLOBAL',
  'POP_TOP', 'DUP_TOP',
  'ROT_TWO', 'ROT_THREE', 'ROT_FOUR',
  'UNARY_POSITIVE', 'UNARY_NEGATIVE', 'UNARY_NOT',
  'UNARY_INVERT',
  'BINARY_POWER', 'BINARY_MULTIPLY', 'BINARY_DIVIDE',
  'BINARY_FLOOR_DIVIDE', 'BINARY_TRUE_DIVIDE',
  'BINARY_MODULO',
  'BINARY_ADD', 'BINARY_SUBTRACT',
  'BINARY_LSHIFT', 'BINARY_RSHIFT',
  'BINARY_AND', 'BINARY_XOR', 'BINARY_OR',
  'BINARY_SUBSCR', 'STORE_SUBSCR',
  'INPLACE_POWER', 'INPLACE_MULTIPLY', 'INPLACE_DIVIDE',
  'INPLACE_FLOOR_DIVIDE', 'INPLACE_TRUE_DIVIDE',
  'INPLACE_MODULO',
  'INPLACE_ADD', 'INPLACE_SUBTRACT',
  'INPLACE_LSHIFT', 'INPLACE_RSHIFT',
  'INPLACE_AND', 'INPLACE_XOR', 'INPLACE_OR',
  'PRINT_ITEM', 'PRINT_NEWLINE',
  'CALL_FUNCTION', 'RETURN_VALUE',
  'COMPARE_OP', 'POP_JUMP_IF_TRUE', 'POP_JUMP_IF_FALSE',
  'JUMP_IF_TRUE_OR_POP', 'JUMP_IF_FALSE_OR_POP',
  'JUMP_FORWARD', 'JUMP_ABSOLUTE',
  'GET_ITER', 'FOR_ITER',
  'BREAK_LOOP', 'CONTINUE_LOOP',
  'SETUP_LOOP', 'POP_BLOCK',
  'LOAD_ATTR',
  'NOP', 'STOP_CODE',
  'UNPACK_SEQUENCE',
))

ins_order = (
  'LOAD_CONST',
  'LOAD_FAST', 'STORE_FAST',
  'LOAD_GLOBAL',

  'PRINT_ITEM', 'PRINT_NEWLINE',

  'CALL_FUNCTION', 'RETURN_VALUE',
  'POP_TOP',

  'STORE_GLOBAL',

  'UNARY_NOT',
  'UNARY_INVERT',
  'BINARY_POWER', 'BINARY_MULTIPLY',
  'BINARY_FLOOR_DIVIDE', 'BINARY_TRUE_DIVIDE',
  'BINARY_MODULO',
  'BINARY_ADD', 'BINARY_SUBTRACT',
  'BINARY_LSHIFT', 'BINARY_RSHIFT',
  'BINARY_AND', 'BINARY_XOR', 'BINARY_OR',

  'COMPARE_OP', 'POP_JUMP_IF_TRUE', 'POP_JUMP_IF_FALSE',
  'JUMP_IF_TRUE_OR_POP', 'JUMP_IF_FALSE_OR_POP',
  'JUMP_ABSOLUTE',

  'GET_ITER', 'FOR_ITER',
  #'BREAK_LOOP', 'CONTINUE_LOOP',
  #'SETUP_LOOP', 'POP_BLOCK',

  'LOAD_ATTR',

  'INPLACE_POWER', 'INPLACE_MULTIPLY',
  'INPLACE_FLOOR_DIVIDE', 'INPLACE_TRUE_DIVIDE',
  'INPLACE_MODULO',
  'INPLACE_ADD', 'INPLACE_SUBTRACT',
  'INPLACE_LSHIFT', 'INPLACE_RSHIFT',
  'INPLACE_AND', 'INPLACE_XOR', 'INPLACE_OR',

  'DELETE_FAST',
  'DELETE_GLOBAL',

  'UNARY_POSITIVE', 'UNARY_NEGATIVE',

  'DUP_TOP',
  'ROT_TWO', 'ROT_THREE', 'ROT_FOUR',

  'NOP', 'STOP_CODE',

  'BINARY_SUBSCR', 'STORE_SUBSCR',
  'UNPACK_SEQUENCE',
)

ins_arg2 = {
  'LOAD_CONST',
  'LOAD_FAST', 'STORE_FAST', 'DELETE_FAST',
  'LOAD_GLOBAL', 'STORE_GLOBAL', 'DELETE_GLOBAL',
  'CALL_FUNCTION',
  'COMPARE_OP', 'POP_JUMP_IF_TRUE', 'POP_JUMP_IF_FALSE',
  'JUMP_IF_TRUE_OR_POP', 'JUMP_IF_FALSE_OR_POP',
  'JUMP_FORWARD', 'JUMP_ABSOLUTE',
  'FOR_ITER', 'CONTINUE_LOOP', 'SETUP_LOOP',
  'LOAD_ATTR',
  'UNPACK_SEQUENCE',
}

# instructions that have a target bytecode address or target bytecode delta in
# them.
ins_with_target_bytecode_address = set((
  'POP_JUMP_IF_TRUE', 'POP_JUMP_IF_FALSE',
  'JUMP_IF_TRUE_OR_POP', 'JUMP_IF_FALSE_OR_POP',
  'JUMP_FORWARD', 'JUMP_ABSOLUTE',
  'CONTINUE_LOOP', 'FOR_ITER',
  'BREAK_LOOP',
))

# some instructions could be transformed to a different instruction or can be
# skipped.
ins_transform = {
  'BINARY_DIVIDE' : 'BINARY_TRUE_DIVIDE',
  'INPLACE_DIVIDE' : 'INPLACE_TRUE_DIVIDE',
  'JUMP_FORWARD': 'JUMP_ABSOLUTE',
  'SETUP_LOOP': None,
  'POP_BLOCK': None,
  'BREAK_LOOP': 'JUMP_ABSOLUTE',
  'CONTINUE_LOOP': 'JUMP_ABSOLUTE',
}

assert len(ins_order)+len(ins_transform) == len(ins_supported)

def print_dis(m):
  print dis.dis(m)
  globalls = {}
  functions = {'main':0}
  modules = {}
  assert 'main' in dir(m)
  for k in dir(m):
    f = getattr(m, k)
    if type(f)==types.FunctionType:
      functions[k] = len(functions)
    elif type(f) in (types.BooleanType, types.IntType, types.LongType):
      globalls[k] = len(globalls)
    elif type(f) == types.ModuleType:
      assert('userlibs.'+k in userlibsgen.userlibs)
      modules['userlibs.'+k] = f
  functions['main']=0
  codes = []
  for k,i in sorted(functions.items(), key=lambda x:x[1]):
    f = getattr(m, k)
    #print 'co_consts', f.__code__.co_consts
    #print 'co_varnames', f.__code__.co_varnames
    #print 'co_names', f.__code__.co_names
    #print 'co_argcount', f.__code__.co_argcount
    #print
    code = bitstring.BitArray()
    number_encode(f.__code__.co_argcount, code)
    number_encode(len(f.__code__.co_varnames), code)
    dump_code(f, globalls, functions, modules, code)
    if len(code)%8 != 0:
      code.append(bitstring.BitArray(uint=0, length=8-(len(code)%8)))
    codes.append(code)
  # append header
  allcode = bitstring.BitArray()
  prevlen=int(math.ceil(10.0*(len(codes)+1)/8)) + 2
  allcode.append(bitstring.BitArray(uint=len(codes), length=8))
  sanitychecker = random.randint(0, 255)
  allcode.append(bitstring.BitArray(uint=sanitychecker, length=8))
  for c in codes:
    allcode.append(bitstring.BitArray(uint=prevlen, length=10))
    prevlen += len(c)/8
  allcode.append(bitstring.BitArray(uint=prevlen, length=10))
  if len(allcode)%8 != 0:
    allcode.append(bitstring.BitArray(uint=0, length=8-(len(allcode)%8)))
  for c in codes: allcode.append(c)
  allcode.append(bitstring.BitArray(uint=sanitychecker, length=8))
  return allcode

def ins_encode(insname, newcode):
  l = 4 if ins_order.index(insname) < 16 else 7
  newcode.append(bitstring.BitArray(bin='0' if l==4 else '1'))
  newcode.append(bitstring.BitArray(uint=ins_order.index(insname), length=l))

def number_encode(var, newcode):
  if isinstance(var, float):
    newcode.append(bitstring.BitArray(bin='1110'))
    newcode.append(bitstring.BitArray(float=var, length=32))
    return
  if not isinstance(var, (int,long)): return
  if var<0:
    pre = '1101'
    l = 32
  elif var < 8:
    pre = '0'
    l = 3
  elif var < 64:
    pre = '10'
    l = 6
  elif var < pow(2, 12):
    pre = '1100'
    l = 12
  else:
    pre = '1101'
    l = 32
  newcode.append(bitstring.BitArray(bin=pre))
  if l==32:
    newcode.append(bitstring.BitArray(int=var, length=l))
  else:
    newcode.append(bitstring.BitArray(uint=var, length=l))

def const_encode(var, newcode):
  vartype = type(var)
  if vartype==types.NoneType:
    var = 0
  elif vartype==types.BooleanType:
    var = 1 if var else 0
  elif vartype in (types.IntType, types.LongType, types.FloatType):
    pass
  elif vartype==types.StringType:
    newcode.append(bitstring.BitArray(bin='1111'))
    newcode.append(bitstring.BitArray(uint=len(var)+1, length=8))
    for c in var:
      newcode.append(bitstring.BitArray(uint=ord(c), length=8))
    newcode.append(bitstring.BitArray(uint=0, length=8))
  else:
    print var, vartype
    assert False
  if isinstance(var, (int,long,float)):
    number_encode(var, newcode)

def dump_code(f, globalls, functions, modules, newcode):
  bytecode_counter = 0    # counter for the bytecode
  bytecode_map = []       # map of bytecode address to its bitsycode address.
  target_address_loc = []
  nested_loops = []
  modules_stack = []
  bytecode = bytearray(f.__code__.co_code)
  while bytecode_counter < len(bytecode):
    var = None
    ins = bytecode[bytecode_counter]
    bytecode_map.append([len(newcode), bytecode_counter])
    bytecode_counter += 1

    insname = dis.opname[ins]
    assert insname in ins_supported
    is_load = insname.startswith('LOAD_')
    is_store = insname.startswith('STORE_')
    is_delete = insname.startswith('DELETE_')

    if insname not in ins_transform:
      ins_encode(insname, newcode)
    elif ins_transform[insname]:
      ins_encode(ins_transform[insname], newcode)

    if insname in ins_arg2:
      var = bytecode[bytecode_counter] | (bytecode[bytecode_counter+1]<<8)
      bytecode_counter += 2

    if insname.endswith('_CONST'):
      var = f.__code__.co_consts[var]
      const_encode(var, newcode)
    elif insname.endswith('_GLOBAL'):
      var = f.__code__.co_names[var]
      if var in globalls:
        newcode.append(bitstring.BitArray(bin='000'))
        const_encode(globalls[var], newcode)
      elif var in functions:
        newcode.append(bitstring.BitArray(bin='001'))
        const_encode(functions[var], newcode)
      elif var in builtins:
        newcode.append(bitstring.BitArray(bin='010'))
        const_encode(builtins[var], newcode)
      elif 'userlibs.'+var in modules:
        assert('userlibs.'+var in userlibsgen.userlibs)
        modules_stack.insert(0, modules['userlibs.'+var])
        newcode.append(bitstring.BitArray(bin='011'))
        const_encode(userlibsgen.userlibs['userlibs.'+var]['id'], newcode)
      elif var in ('True', 'False'):
        newcode.append(bitstring.BitArray(bin='100'))
        const_encode(var=='True', newcode)
      else:
        print var
        assert False
    elif insname.endswith('_FAST'):
      const_encode(var, newcode)
      var = f.__code__.co_varnames[var]
    elif insname=='CALL_FUNCTION':
      const_encode(var, newcode)
    elif insname=='COMPARE_OP':
      const_encode(var, newcode)
    elif insname in ins_with_target_bytecode_address:
      # mark the location where the bytecode address needs to be inserted.
      if insname in ('JUMP_FORWARD', 'FOR_ITER'):
        var += bytecode_counter
      if insname=='BREAK_LOOP': var = nested_loops[-1]
      target_address_loc.append((len(newcode), var))
    elif insname=='LOAD_ATTR':
      var = f.__code__.co_names[var]
      m = next((userlibsgen.userlibs[i.__name__] for i in modules_stack if getattr(i, var)), None)
      assert m
      const_encode(m['functions'][var], newcode)
    elif insname=='UNPACK_SEQUENCE':
      const_encode(var, newcode)

    if insname=='SETUP_LOOP':
      nested_loops.append(var+bytecode_counter)
    elif insname=='POP_BLOCK':
      nested_loops.pop()
    #print insname, var if var else ''

  target_address_loc.reverse()
  for insert_pos, _ in target_address_loc:
    for i in range(bisect.bisect(bytecode_map, [insert_pos,0]), len(bytecode_map)):
      bytecode_map[i][0] += 10
  bytecode_map = {v[1] : v[0] for v in bytecode_map}
  for insert_pos, target in target_address_loc:
    newcode.insert(bitstring.BitArray(uint=bytecode_map[target], length=10), insert_pos)


"""
sanity-check:
1. Main function exists.
2. Main function takes no argument.
3. Other functions do not take keyword parameters.
4. global variables are not yet supported.
"""

if len(sys.argv)==3:
  pyfile = sys.argv[1]
  sys.path.append(os.path.abspath(os.path.dirname(pyfile)))
  if pyfile.endswith('.py'): # Remove the .py extension
    pyfile=pyfile[:-3]
  print_dis(importlib.import_module(os.path.basename(pyfile))).tofile(open(sys.argv[2], 'wb'))
