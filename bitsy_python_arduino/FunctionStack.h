#ifndef FUNCTIONSTACK_H_
#define FUNCTIONSTACK_H_

#include "bitsylimit.h"
#include "variable.h"
#include <stdint.h>

namespace bitsy_python {

class FunctionStack {
public:
  FunctionStack();

  bool is_empty();
  void setup_function_call(uint8_t args, uint8_t vars, uint16_t old_ins_ptr);
  bool return_function(uint16_t *old_ins_ptr);

  Variable getNthVariable(uint8_t n) const;
  void setNthVariable(uint8_t n, const Variable v);
private:
  uint8_t get_var_hdr(uint8_t n) const;
  void set_var_hdr(uint8_t n, uint8_t val);

  // Start and top location of the current function call.
  uint16_t start = 0, top = 0;
};

}

#endif /* FUNCTIONSTACK_H_ */
