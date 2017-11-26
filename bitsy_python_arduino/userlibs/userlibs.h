
extern bool is_userlib_module_enabled(uint8_t module);

extern Variable call_userlib_function(uint8_t module, uint8_t function,
                                      uint8_t argcount, Variable arg[]);

extern void BITSY_PYTHON_PRINT(const char *str);
extern void BITSY_PYTHON_PRINT_VAR(bitsy_python::BitsyHeap &heap, const Variable &v);
