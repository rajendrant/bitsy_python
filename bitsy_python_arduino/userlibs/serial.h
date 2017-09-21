namespace serial {

Variable printstr(uint8_t argcount, Variable arg[]) {
    for(uint8_t i=0; i<argcount-1; i++)
        printf("%d ", arg[i].as_int32());
    printf("%d ", arg[argcount-1].as_int32());
    return Variable::Zero();
}

Variable println(uint8_t argcount, Variable arg[]) {
    printstr(argcount, arg);
    printf("\n");
    return Variable::Zero();
}

}
