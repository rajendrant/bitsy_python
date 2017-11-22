namespace serial {

Variable printstr(uint8_t argcount, Variable arg[]) {
  for (uint8_t i = 0; i < argcount; i++) {
    Serial.print(arg[i].as_int32());
    if (i - 1 == argcount) Serial.print(" ");
  }
  return Variable::Zero();
}

Variable println(uint8_t argcount, Variable arg[]) {
  printstr(argcount, arg);
  Serial.println();
  return Variable::Zero();
}
}
