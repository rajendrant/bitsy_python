#include "bitsy_python_vm.h"

#include <stdio.h>

#include "bitsy_alloc.h"
#include "BitString.h"
#include "BitsyHeap.h"
#include "Builtins.h"
#include "datatypes/datatype.h"
#include "datatypes/iter.h"
#include "ExecStack.h"
#include "FunctionStack.h"
#include "gc.h"
#include "Program.h"
#include "instructions.h"
#include "userlibs/userlibs.h"
#include "variable.h"

namespace bitsy_python {

namespace BitsyPythonVM {

void binary_arithmetic(uint8_t ins, uint8_t arg) {
  auto v1 = ExecStack::pop();
  auto v2 = ExecStack::pop();
  Variable ret;
  bool is_float;
  int32_t i1, i2, iret;
  float f1, f2, fret;
  bool done;

  if (ins==BINARY_SUBSCR) {
    ret = DataType::GetIndex(v2, v1.as_uint8());
    goto end;
  } else if (ins==STORE_SUBSCR) {
    DataType::SetIndex(v2, v1.as_uint8(), ExecStack::pop());
    return;
  }

  i1=v1.as_int32();
  i2=v2.as_int32();
  done=true;
  switch (ins) {
    case BINARY_MODULO:
    case INPLACE_MODULO:
      // Modulo operation only applicable for ints.
      iret = i2 % i1;
      break;
    case BINARY_LSHIFT:
    case INPLACE_LSHIFT:
      iret = i2 << i1;
      break;
    case BINARY_RSHIFT:
    case INPLACE_RSHIFT:
      iret = i2 >> i1;
      break;
    case BINARY_AND:
    case INPLACE_AND:
      iret = i2 & i1;
      break;
    case BINARY_XOR:
    case INPLACE_XOR:
      iret = i2 ^ i1;
      break;
    case BINARY_OR:
    case INPLACE_OR:
      iret = i2 | i1;
      break;
    case COMPARE_OP:
      switch (arg) {
        case 0:
          iret = i2 < i1;
          break;
        case 1:
          iret = i2 <= i1;
          break;
        case 2:
          iret = i2 == i1;
          break;
        case 3:
          iret = i2 != i1;
          break;
        case 4:
          iret = i2 > i1;
          break;
        case 5:
          iret = i2 >= i1;
          break;
        case 6:
          iret = DataType::InOperator(v1, v2);
          break;
        case 7:
          iret = !DataType::InOperator(v1, v2);
          break;
        default:
          // is, is-not, exception-match, BAD
          BITSY_PYTHON_PRINT("COMPARE_OP not supported operator\n");
          BITSY_ASSERT(false);
          iret = 0;
      }
      break;
    default:
      done = false;
  }
  if (done) {
    ret.set_int32(iret);
    goto end;
  }
  is_float = (v1.type == Variable::FLOAT || v2.type == Variable::FLOAT);
  f1=v1.as_float();
  f2=v2.as_float();
  if (ins==BINARY_FLOOR_DIVIDE || ins==INPLACE_FLOOR_DIVIDE) {
    ret.set_int32((int32_t)(f2 / f1));
    goto end;
  }
  done = true;
  switch (ins) {
    case BINARY_MULTIPLY:
    case INPLACE_MULTIPLY:
      if (is_float)
        fret = f2 * f1;
      else
        iret = i2 * i1;
      break;
    case BINARY_ADD:
    case INPLACE_ADD:
      if (is_float)
        fret = f2 + f1;
      else
        iret = i2 + i1;
      break;
    case BINARY_SUBTRACT:
    case INPLACE_SUBTRACT:
      if (is_float)
        fret = f2 - f1;
      else
        iret = i2 - i1;
      break;
    case BINARY_TRUE_DIVIDE:
    case INPLACE_TRUE_DIVIDE:
      if (is_float)
        fret = f2 / f1;
      else
        iret = i2 / i1;
      break;
    default:
      done = false;
  }
  if (done) {
    if (is_float) ret.set_float(fret);
    else ret.set_int32(iret);
  }
end:
  ExecStack::push(ret);
}

void unary_arithmetic(uint8_t ins) {
  if (ins==UNARY_POSITIVE) return;
  auto v = ExecStack::pop();
  switch (ins) {
    case UNARY_NEGATIVE:
      if (v.type == Variable::FLOAT)
        v.set_float(-v.as_float());
      else
        v.set_int32(-v.as_int32());
      break;
    case UNARY_NOT:
      v.set_int32(!v.as_bool());
      break;
    case UNARY_INVERT:
      v.set_int32(~v.as_int32());
      break;
  }
  ExecStack::push(v);
  return;
}

void jump_arithmetic(uint8_t ins, uint16_t jump) {
  bool cond = true;
  if (ins==JUMP_ABSOLUTE)
    goto end;
  {
  auto condvar = ExecStack::pop();
  cond = condvar.as_bool();
  switch (ins) {
    case JUMP_IF_TRUE_OR_POP:
      if (cond) {
        ExecStack::push(condvar);
      }
    case POP_JUMP_IF_TRUE:
      break;
    case JUMP_IF_FALSE_OR_POP:
      if (!cond) {
        ExecStack::push(condvar);
      }
    case POP_JUMP_IF_FALSE:
      cond = !cond;
      break;
  }
  }
end:
  if (cond) {
    Program::jump_to_target(jump);
  }
}

#ifdef DESKTOP
void init(const char *fname) {
  Program::FromFile(fname);
#elif defined(ARDUINO)
void init() {
  Program::FromEEPROM();
#endif
  bitsy_alloc_init();
  ExecStack::init();
  FunctionStack::init();
  BitsyHeap::init();
  if (Program::sanity_check())
    FunctionStack::setup_function_call(Program::setup_function_call(0));
}

void execute() {
  while (executeOneStep())
    ;
}

bool executeOneStep() {
  if (!BitString::is_sane())
    return false;

  Variable arg, v;
  auto ins = Program::get_next_instruction(&arg);
  /*
  Serial.print("ins ");
  Serial.print(ins);
  Serial.print(" ");
  Serial.println(arg.as_int32());*/
  // printf("ins %d %s %d\n", ins, get_ins_name(ins), arg.as_int32());
  if(ins==LOAD_CONST) {
    ExecStack::push(arg);
    return true;
  }
  switch (ins) {
    case STOP_CODE:
      return false;
    case NOP:
      break;
    case POP_TOP:
      ExecStack::pop();
      break;

    case UNARY_POSITIVE:
    case UNARY_NEGATIVE:
    case UNARY_NOT:
    case UNARY_INVERT:
      unary_arithmetic(ins);
      break;

    case BINARY_MULTIPLY:
    case BINARY_TRUE_DIVIDE:
    case BINARY_FLOOR_DIVIDE:
    case BINARY_ADD:
    case BINARY_SUBTRACT:
    case BINARY_MODULO:
    case BINARY_LSHIFT:
    case BINARY_RSHIFT:
    case BINARY_AND:
    case BINARY_XOR:
    case BINARY_OR:
    case INPLACE_MULTIPLY:
    case INPLACE_TRUE_DIVIDE:
    case INPLACE_FLOOR_DIVIDE:
    case INPLACE_ADD:
    case INPLACE_SUBTRACT:
    case INPLACE_MODULO:
    case INPLACE_LSHIFT:
    case INPLACE_RSHIFT:
    case INPLACE_AND:
    case INPLACE_XOR:
    case INPLACE_OR:
    case BINARY_SUBSCR:
    case COMPARE_OP:
    case STORE_SUBSCR:
      binary_arithmetic(ins, arg.as_uint8());
      break;

    case POP_JUMP_IF_TRUE:
    case POP_JUMP_IF_FALSE:
    case JUMP_IF_TRUE_OR_POP:
    case JUMP_IF_FALSE_OR_POP:
    case JUMP_ABSOLUTE:
      jump_arithmetic(ins, arg.as_int32());
      break;

    case LOAD_FAST:
      ExecStack::push(FunctionStack::getNthVariable(arg.as_uint8()));
      break;
    case LOAD_CONST:
    case LOAD_GLOBAL:
      ExecStack::push(arg);
      break;
    case STORE_FAST:
      FunctionStack::setNthVariable(arg.as_uint8(), ExecStack::pop());
      break;
    case DELETE_FAST:
      // TODO - support needed ?
      break;
    case STORE_GLOBAL:
      DataType::setGlobalVar(arg.as_uint8(), ExecStack::pop());
      break;

    case CALL_FUNCTION: {
      uint8_t argcount = arg.as_int32();
      BITSY_ASSERT(!(argcount & 0xF0));  // Keyword arguments not supported.
      Variable argvars[argcount];
      for (uint8_t i = 0; i < argcount; i++) {
        argvars[argcount - i - 1] = ExecStack::pop();
      }
      v = ExecStack::pop();
      BITSY_ASSERT(v.type == Variable::CUSTOM);
      if (v.val.custom_type.type == Variable::CustomType::USER_FUNCTION) {
        FunctionStack::setup_function_call(
          Program::setup_function_call(v.val.custom_type.val));
        for (uint8_t i = 0; i < argcount; i++) {
          FunctionStack::setNthVariable(i, argvars[i]);
        }
      } else if (v.val.custom_type.type ==
                 Variable::CustomType::USER_MODULE_FUNCTION) {
        uint8_t module_id = v.val.custom_type.val&0x3F;
        if (is_userlib_module_enabled(module_id)) {
          ExecStack::push(call_userlib_function(
              module_id, v.val.custom_type.val>>6, argcount, argvars));
        }
      } else if (v.val.custom_type.type ==
                 Variable::CustomType::BUILTIN_FUNCTION) {
        gc();
        ExecStack::push(handle_builtin_call((BitsyBuiltin)v.val.custom_type.val,
                                            argcount, argvars));
      } else {
        BITSY_ASSERT(false);
      }
      break;
    }
    case RETURN_VALUE: {
      auto p = FunctionStack::return_function();
      Program::return_function(p);
      if (p.is_callback_mode)
        ExecStack::pop();
      return !FunctionStack::is_empty();
    }

    case PRINT_ITEM:
      BITSY_PYTHON_PRINT_VAR(ExecStack::pop());
      BITSY_PYTHON_PRINT(" ");
      break;
    case PRINT_NEWLINE:
      BITSY_PYTHON_PRINT("\n");
      break;
    case GET_ITER:
      gc();
      arg = ExecStack::pop();
      ExecStack::push(DataType::CreateForType(Variable::CustomType::ITER, 1, &arg));
      break;
    case FOR_ITER: {
      Variable iter, elem;
      iter = ExecStack::pop();
      if(IterForLoopIter(iter, &elem)) {
        ExecStack::push(iter);
        ExecStack::push(elem);
      } else {
        Program::jump_to_target(arg.as_int32());
      }
      break;
    }
    case UNPACK_SEQUENCE: {
      // This implementation does not raise exceptions on non matching length.
      // ValueError: too many values to unpack
      Variable elem, iter = ExecStack::pop();
      BITSY_ASSERT(iter.type == Variable::CUSTOM);
      for(uint8_t i=arg.as_uint8(); i>0; i--) {
        if(IterForLoop(iter.val.custom_type, i-1, &elem))
          ExecStack::push(elem);
      }
      break;
    }
    case BUILD_LIST: {
      gc();
      uint8_t count=arg.as_uint8();
      Variable args[count];
      while(count>0)
        args[--count]=ExecStack::pop();
      ExecStack::push(DataType::CreateForType(Variable::CustomType::LIST, arg.as_uint8(), args));
      break;
    }
    case LOAD_ATTR:
      v = ExecStack::pop();  // Module id
      BITSY_ASSERT(v.type == Variable::CUSTOM &&
             v.val.custom_type.type == Variable::CustomType::USER_MODULE_FUNCTION);
      BITSY_ASSERT(((v.val.custom_type.val>>6)&0x3F) == 0);
      ExecStack::push(Variable::CustomTypeVariable(
          Variable::CustomType::USER_MODULE_FUNCTION,
          v.val.custom_type.val | (arg.as_uint12()<<6)));
      break;
    default:
      BITSY_PYTHON_PRINT("UNSUPPORTED INS ");
      BITSY_PYTHON_PRINT_VAR(ins);
      // BITSY_PYTHON_PRINT(get_ins_name(ins));
      BitString::mark_insane();
      BITSY_ASSERT(false);
  }
  return true;
}

void callUserFunction(uint16_t f, Variable arg) {
  auto p = Program::setup_function_call(f);
  p.is_callback_mode = 1;
  FunctionStack::setup_function_call(p);
  FunctionStack::setNthVariable(0, arg);
}

}
}
