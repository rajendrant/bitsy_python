#include "bitsy_python_vm.h"

#include <assert.h>
#include <stdio.h>

#include "bitsy_alloc.h"
#include "Builtins.h"
#include "datatypes/datatype.h"
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

void BitsyPythonVM::binary_arithmetic(uint8_t ins) {
  auto v1 = exec_stack.pop();
  auto v2 = exec_stack.pop();
  Variable ret;
  switch (ins) {
    case BINARY_MULTIPLY:
    case INPLACE_MULTIPLY:
      if (v1.type == Variable::FLOAT || v2.type == Variable::FLOAT)
        ret.set_float(v1.as_float() * v2.as_float());
      else
        ret.set_int32(v1.as_int32() * v2.as_int32());
      break;
    case BINARY_ADD:
    case INPLACE_ADD:
      if (v1.type == Variable::FLOAT || v2.type == Variable::FLOAT)
        ret.set_float(v1.as_float() + v2.as_float());
      else
        ret.set_int32(v1.as_int32() + v2.as_int32());
      break;
    case BINARY_SUBTRACT:
    case INPLACE_SUBTRACT:
      if (v1.type == Variable::FLOAT || v2.type == Variable::FLOAT)
        ret.set_float(v2.as_float() - v1.as_float());
      else
        ret.set_int32(v2.as_int32() - v1.as_int32());
      break;
    case BINARY_MODULO:
    case INPLACE_MODULO:
      // Modulo operation only applicable for ints.
      ret.set_int32(v2.as_int32() % v1.as_int32());
      break;
    case BINARY_TRUE_DIVIDE:
    case INPLACE_TRUE_DIVIDE:
      if (v1.type == Variable::FLOAT || v2.type == Variable::FLOAT)
        ret.set_float(v2.as_float() / v1.as_float());
      else
        ret.set_int32(v2.as_int32() / v1.as_int32());
      break;
    case BINARY_FLOOR_DIVIDE:
    case INPLACE_FLOOR_DIVIDE:
      ret.set_float((int32_t)(v2.as_float() / v1.as_float()));
      break;

    case BINARY_LSHIFT:
    case INPLACE_LSHIFT:
      ret.set_int32(v2.as_int32() << v1.as_int32());
      break;
    case BINARY_RSHIFT:
    case INPLACE_RSHIFT:
      ret.set_int32(v2.as_int32() >> v1.as_int32());
      break;
    case BINARY_AND:
    case INPLACE_AND:
      ret.set_int32(v2.as_int32() & v1.as_int32());
      break;
    case BINARY_XOR:
    case INPLACE_XOR:
      ret.set_int32(v2.as_int32() ^ v1.as_int32());
      break;
    case BINARY_OR:
    case INPLACE_OR:
      ret.set_int32(v2.as_int32() | v1.as_int32());
      break;
    case BINARY_SUBSCR:
      ret = DataType::GetIndex(bitsy_heap, v2, v1.as_uint8());
      break;
    default:
      assert(false);
  }
  exec_stack.push(ret);
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
      break;
    case UNARY_NEGATIVE: {
      auto v = exec_stack.pop();
      if (v.type == Variable::FLOAT)
        v.set_float(-v.as_float());
      else
        v.set_int32(-v.as_int32());
      exec_stack.push(v);
      break;
    }
    case UNARY_NOT: {
      auto v = exec_stack.pop();
      v.set_int32(!v.as_bool());
      exec_stack.push(v);
      break;
    }
    case UNARY_INVERT: {
      auto v = exec_stack.pop();
      v.set_int32(~v.as_int32());
      exec_stack.push(v);
      break;
    }
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
      binary_arithmetic(ins);
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
      assert(!(argcount & 0xF0));  // Keyword arguments not supported.
      Variable argvars[argcount];
      for (uint8_t i = 0; i < argcount; i++) {
        argvars[argcount - i - 1] = exec_stack.pop();
      }
      auto v = exec_stack.pop();
      assert(v.type == Variable::CUSTOM);
      if (v.val.custom_type.type == Variable::CustomType::USER_FUNCTION) {
        auto fn = prog.setup_function_call(v.val.custom_type.val);
        assert(fn.args == argcount);
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
        exec_stack.push(handle_builtin_call(bitsy_heap, (BitsyBuiltin)v.val.custom_type.val,
                                            argcount, argvars));
      } else {
        assert(false);
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
      BITSY_PYTHON_PRINT_VAR(bitsy_heap, v);
      BITSY_PYTHON_PRINT(" ");
      break;
    }
    case PRINT_NEWLINE:
      BITSY_PYTHON_PRINT("\n");
      break;
    case COMPARE_OP: {
      auto v1 = exec_stack.pop();
      auto v2 = exec_stack.pop();
      Variable ret;
      switch (arg.as_int32()) {
        case 0:
          ret.set_int32(v2.as_int32() < v1.as_int32());
          break;
        case 1:
          ret.set_int32(v2.as_int32() <= v1.as_int32());
          break;
        case 2:
          ret.set_int32(v2.as_int32() == v1.as_int32());
          break;
        case 3:
          ret.set_int32(v2.as_int32() != v1.as_int32());
          break;
        case 4:
          ret.set_int32(v2.as_int32() > v1.as_int32());
          break;
        case 5:
          ret.set_int32(v2.as_int32() >= v1.as_int32());
          break;
        default:
          // in, not-in, is, is-not, exception-match, BAD
          BITSY_PYTHON_PRINT("COMPARE_OP not supported operator\n");
          assert(false);
      }
      exec_stack.push(ret);
      break;
    }
    case POP_JUMP_IF_TRUE:
      if (exec_stack.pop().as_bool()) {
        prog.jump_to_target(arg.as_int16());
      }
      break;
    case POP_JUMP_IF_FALSE:
      if (!exec_stack.pop().as_bool()) {
        prog.jump_to_target(arg.as_int16());
      }
      break;
    case JUMP_IF_TRUE_OR_POP:
      if (exec_stack.pop().as_bool()) {
        exec_stack.push(arg);
        prog.jump_to_target(arg.as_int16());
      }
      break;
    case JUMP_IF_FALSE_OR_POP:
      if (!exec_stack.pop().as_bool()) {
        exec_stack.push(arg);
        prog.jump_to_target(arg.as_int16());
      }
      break;
    case JUMP_ABSOLUTE:
      prog.jump_to_target(arg.as_int16());
      break;
    /*case SETUP_LOOP:
    case POP_BLOCK:
        break;
    case BREAK_LOOP:
        prog.jump_to_target(arg.as_int16());
        break;
    case CONTINUE_LOOP:
        prog.jump_to_target(arg.as_int16());
        break;*/
    case LOAD_ATTR: {
      exec_stack.push(Variable::ModuleFunctionVariable(
          exec_stack.pop(), (uint16_t)arg.as_int16()));
      break;
    }
    case STORE_SUBSCR:
      assert(false);
      break;
    default: {
      Variable tmp_ins;
      BITSY_PYTHON_PRINT("UNSUPPORTED INS ");
      tmp_ins.set_int16(ins);
      BITSY_PYTHON_PRINT_VAR(bitsy_heap, tmp_ins);
      // BITSY_PYTHON_PRINT(get_ins_name(ins));
      assert(false);
    }
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
