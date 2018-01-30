#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <stdint.h>

#include "bitsylimit.h"
#include "variable.h"

namespace bitsy_python {

namespace Program {

typedef struct {
  uint16_t len;
  uint8_t args, vars;
  uint16_t old_ins_ptr;
} FunctionParams;

#ifdef DESKTOP
  void FromFile(const char *fname);
#elif defined(ARDUINO)
  void FromEEPROM();
#endif

  // Checks if the full module is available, by checking a byte at the end of
  // the module.
  bool sanity_check();

  bool is_sane();

  uint8_t get_next_instruction(Variable *arg);

  // Gets the number at the current ins pointer, and updates the ins_ptr.
  Variable get_number();

  FunctionParams setup_function_call(uint8_t fn);
  void return_function(uint16_t insptr);

  void jump_to_target(uint16_t target);

}
}
#endif /* PROGRAM_H_ */
