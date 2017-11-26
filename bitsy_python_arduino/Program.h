#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <stdint.h>

#include "BitString.h"
#include "bitsylimit.h"
#include "variable.h"

namespace bitsy_python {

class Program {
 public:
  typedef struct {
    uint16_t len;
    uint8_t args, vars;
    uint16_t old_ins_ptr;
  } FunctionParams;

#ifdef DESKTOP
  static Program FromFile(const char *fname);
#elif defined(ARDUINO)
  static Program FromEEPROM();
#endif

  // Checks if the full module is available, by checking a byte at the end of
  // the module.
  bool sanity_check();

  bool is_sane();

  uint8_t get_next_instruction(Variable *arg);

  // Gets the number at the current ins pointer, and updates the ins_ptr.
  Variable get_number();

  FunctionParams setup_function_call(uint8_t fn);
  void return_function(uint16_t ins_ptr);

  void jump_to_target(uint16_t target);

 private:
  friend class ProgramTest;
  Program(BitString bits) : bits(bits) {}

#ifdef DESKTOP
  void dump();
#endif

  BitString bits;
  uint16_t ins_ptr = 0;
  uint16_t ins_ptr_function_start = 0;
};
}
#endif /* PROGRAM_H_ */
