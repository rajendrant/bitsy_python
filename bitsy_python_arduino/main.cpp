#include "bitsylimit.h"
#include "bitsy_python_vm.h"

#ifdef DESKTOP

int main(int argc, char **argv) {
    if (argc != 2)
        return -1;
    
    bitsy_python::BitsyPythonVM vm(argv[1]);
    vm.initExecution();
    vm.execute();
    return 0;
}

#endif
