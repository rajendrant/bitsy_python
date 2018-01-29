#include <math.h>
#include <stdio.h>

extern bitsy_python::BitsyPythonVM *vm_global;

namespace testuserlib {

Variable printstr(uint8_t argcount, Variable arg[]) {
  for (uint8_t i = 0; i < argcount - 1; i++) printf("%d ", arg[i].as_int32());
  printf("%d ", arg[argcount - 1].as_int32());
  return Variable::Zero();
}

Variable println(uint8_t argcount, Variable arg[]) {
  printstr(argcount, arg);
  printf("\n");
  return Variable::Zero();
}

Variable math_add(uint8_t argcount, Variable arg[]) {
  long res = 0;
  for (uint8_t i = 0; i < argcount; i++) res += arg[i].as_float();
  Variable ret;
  ret.set_float(res);
  return ret;
}

Variable math_power(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    Variable ret;
    ret.set_float(pow(arg[0].as_float(), arg[1].as_float()));
    return ret;
  }
  return Variable::Zero();
}

uint16_t callback=0xFF;

Variable init_callback(uint8_t argcount, Variable arg[]) {
  if (argcount==1 && arg[0].type==Variable::CUSTOM &&
      arg[0].val.custom_type.type==Variable::CustomType::USER_FUNCTION) {
    callback = arg[0].val.custom_type.val;
    vm_global->callUserFunction(callback, Variable(123));
  }
  return Variable::Zero();
}

Variable trigger_callback(uint8_t argcount, Variable arg[]) {
  if (callback != 0xFF)
    vm_global->callUserFunction(callback, Variable(123));
  return Variable::Zero();
}

}
