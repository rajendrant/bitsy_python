#include "bitsy_python_vm.h"

#include <stdio.h>

#include "bitsy_alloc.h"
#include "Builtins.h"
#include "datatypes/datatype.h"
#include "datatypes/iter.h"
#include "instructions.h"
#include "userlibs/userlibs.h"
#include "variable.h"

namespace bitsy_python {

BitsyHeap bitsy_heap;
ExecStack exec_stack;
FunctionStack function_stack;

#ifdef DESKTOP
BitsyPythonVM::BitsyPythonVM(const char *fname)
    : prog(Program::FromFile(fname)) {
#elif defined(ARDUINO)
BitsyPythonVM::BitsyPythonVM() : prog(Program::FromEEPROM()) {
#endif
  bitsy_alloc_init();
}

void BitsyPythonVM::binary_arithmetic(uint8_t ins, uint8_t arg) {
  auto v1 = exec_stack.pop();
  auto v2 = exec_stack.pop();
  Variable ret;
  bool is_float;
  int32_t i1, i2, iret;
  float f1, f2, fret;
  bool done;

  if (ins==BINARY_SUBSCR) {
    ret = DataType::GetIndex(v2, v1.as_uint8());
    goto end;
  } else if (ins==STORE_SUBSCR) {
    DataType::SetIndex(v2, v1.as_uint8(), exec_stack.pop());
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
      iret = i2 >> i1;
      break;
    case BINARY_RSHIFT:
    case INPLACE_RSHIFT:
      iret = i2 << i1;
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
          iret = v2.as_int32() < v1.as_int32();
          break;
        case 1:
          iret = v2.as_int32() <= v1.as_int32();
          break;
        case 2:
          iret = v2.as_int32() == v1.as_int32();
          break;
        case 3:
          iret = v2.as_int32() != v1.as_int32();
          break;
        case 4:
          iret = v2.as_int32() > v1.as_int32();
          break;
        case 5:
          iret = v2.as_int32() >= v1.as_int32();
          break;
        default:
          // in, not-in, is, is-not, exception-match, BAD
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
  exec_stack.push(ret);
}

void BitsyPythonVM::unary_arithmetic(uint8_t ins) {
  if (ins==UNARY_POSITIVE) return;
  auto v = exec_stack.pop();
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
  exec_stack.push(v);
  return;
}

void BitsyPythonVM::jump_arithmetic(uint8_t ins, uint16_t jump) {
  bool cond = true;
  if (ins==JUMP_ABSOLUTE)
    goto end;
  {
  auto condvar = exec_stack.pop();
  cond = condvar.as_bool();
  switch (ins) {
    case JUMP_IF_TRUE_OR_POP:
      if (cond) {
        exec_stack.push(condvar);
      }
    case POP_JUMP_IF_TRUE:
      break;
    case JUMP_IF_FALSE_OR_POP:
      if (!cond) {
        exec_stack.push(condvar);
      }
    case POP_JUMP_IF_FALSE:
      cond = !cond;
      break;
  }
  }
end:
  if (cond) {
    prog.jump_to_target(jump);
  }
}

void BitsyPythonVM::initExecution() {
  if (prog.sanity_check()) {
    auto fn = prog.setup_function_call(0);
    function_stack.setup_function_call(fn.args, fn.vars, fn.old_ins_ptr);
    // printf("main %d %d %d\n", fn.args, fn.vars, fn.len);
  }
}

void BitsyPythonVM::execute() {
  while (executeOneStep())
    ;
}

bool BitsyPythonVM::executeOneStep() {
  if (!prog.is_sane())
    return false;

  Variable arg;
  auto ins = prog.get_next_instruction(&arg);
  // printf("ins %s %d\n", get_ins_name(ins), arg.as_int32());
  switch (ins) {
    case STOP_CODE:
      return false;
    case NOP:
      break;
    case POP_TOP:
      exec_stack.pop();
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
      jump_arithmetic(ins, arg.as_int16());
      break;

    case LOAD_CONST:
      exec_stack.push(arg);
      break;
    case LOAD_FAST:
      exec_stack.push(function_stack.getNthVariable(arg.as_int32()));
      break;
    case STORE_FAST:
      function_stack.setNthVariable(arg.as_int32(), exec_stack.pop());
      break;
    case DELETE_FAST:
      // TODO - support needed ?
      break;

    case LOAD_GLOBAL:
      exec_stack.push(arg);
      break;

    case CALL_FUNCTION: {
      uint8_t argcount = arg.as_int32();
      BITSY_ASSERT(!(argcount & 0xF0));  // Keyword arguments not supported.
      Variable argvars[argcount];
      for (uint8_t i = 0; i < argcount; i++) {
        argvars[argcount - i - 1] = exec_stack.pop();
      }
      auto v = exec_stack.pop();
      BITSY_ASSERT(v.type == Variable::CUSTOM);
      if (v.val.custom_type.type == Variable::CustomType::USER_FUNCTION) {
        auto fn = prog.setup_function_call(v.val.custom_type.val);
        BITSY_ASSERT(fn.args == argcount);
        function_stack.setup_function_call(fn.args, fn.vars, fn.old_ins_ptr);
        for (uint8_t i = 0; i < argcount; i++) {
          function_stack.setNthVariable(i, argvars[i]);
        }
      } else if (v.val.custom_type.type ==
                 Variable::CustomType::USER_MODULE_FUNCTION) {
        uint8_t module_id = (v.val.custom_type.val >> 6) & 0x3F;
        if (is_userlib_module_enabled((v.val.custom_type.val >> 6) & 0x3F))
          exec_stack.push(call_userlib_function(
              module_id, v.val.custom_type.val & 0x3F, argcount, argvars));
      } else if (v.val.custom_type.type ==
                 Variable::CustomType::BUILTIN_FUNCTION) {
        exec_stack.push(handle_builtin_call((BitsyBuiltin)v.val.custom_type.val,
                                            argcount, argvars));
      } else {
        BITSY_ASSERT(false);
      }
      break;
    }
    case RETURN_VALUE: {
      uint16_t ins_ptr;
      bool ret = !function_stack.return_function(&ins_ptr);
      prog.return_function(ins_ptr);
      return ret;
    }

    case PRINT_ITEM: {
      auto v = exec_stack.pop();
      BITSY_PYTHON_PRINT_VAR(v);
      BITSY_PYTHON_PRINT(" ");
      break;
    }
    case PRINT_NEWLINE:
      BITSY_PYTHON_PRINT("\n");
      break;
    case GET_ITER: {
      Variable argvars[] = {exec_stack.pop()};
      exec_stack.push(argvars[0]);
      auto v=DataType::CreateForType(Variable::CustomType::ITER, 1, argvars);
      exec_stack.pop();
      exec_stack.push(v);
      break;
    }
    case FOR_ITER: {
      Variable iter, elem;
      iter = exec_stack.pop();
      if(IterForLoop(iter, &elem)) {
        exec_stack.push(iter);
        exec_stack.push(elem);
      } else {
        prog.jump_to_target(arg.as_int16());
      }
      break;
    }
    /*case SETUP_LOOP:
    case POP_BLOCK:
        break;
    case BREAK_LOOP:
        prog.jump_to_target(arg.as_int16());
        break;
    case CONTINUE_LOOP:
        prog.jump_to_target(arg.as_int16());
        break;*/
    case LOAD_ATTR:
      exec_stack.push(Variable::ModuleFunctionVariable(
          exec_stack.pop(), (uint16_t)arg.as_int16()));
      break;
    default:
      BITSY_PYTHON_PRINT("UNSUPPORTED INS ");
      arg.set_int16(ins);
      BITSY_PYTHON_PRINT_VAR(arg);
      // BITSY_PYTHON_PRINT(get_ins_name(ins));
      BITSY_ASSERT(false);
  }
  return true;
}
}

void bitsy_print(char ch) {
#if defined(DESKTOP)
  printf("%c", ch);
#elif defined(ENABLE_BITSY_USERLIB_SERIAL)
  Serial.print(ch);
#endif
}
