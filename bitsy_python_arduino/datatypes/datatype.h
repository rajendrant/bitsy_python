#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "../BitsyHeap.h"
#include "../variable.h"

#include <stdint.h>

namespace bitsy_python {
namespace DataType {

Variable CreateStr(const char* str, uint8_t len);
Variable CreateByteArray(uint8_t len);
Variable CreateIter(const Variable& container);
Variable CreateInt32(int32_t no);
Variable CreateFloat(float no);
Variable CreateList(uint8_t argcount, Variable args[]);
Variable CreateRange(uint8_t argcount, Variable args[]);

uint16_t Len(const Variable& v);

Variable GetIndex(const Variable& v, uint8_t ind);

void SetIndex(const Variable &v, uint8_t ind, const Variable& val);

void Print(const Variable& v, void (*print)(char));

void updateUsedContainers(uint8_t start_id, Variable::CustomType v, uint32_t *map);

bool InOperator(const Variable& cont, const Variable& e);

// Functions for global variables.
Variable initGlobalVars(uint8_t global_vars);
Variable getGlobalVar(uint8_t id);
void setGlobalVar(uint8_t id, const Variable& v);

}
}

#endif /* DATATYPES_H_ */
