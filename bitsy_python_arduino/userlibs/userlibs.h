extern void userlib_init();

extern Variable call_userlib_function(uint8_t module, uint8_t function,
                                      uint8_t argcount, Variable arg[]);

extern void BITSY_PYTHON_PRINT(const char *str);
extern void BITSY_PYTHON_PRINT_VAR(const Variable &v);
