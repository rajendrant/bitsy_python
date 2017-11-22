#include "datatype.h"

#include <cassert>
#include <string.h>

#include "bitsy_python_vm.h"

namespace bitsy_python {

//static
Variable DataType::CreateStr(const char* str, uint8_t len) {
    Variable v;
    uint8_t *var;
    uint8_t id = bitsy_heap.CreateVar(len, &var);
    strncpy((char*)var, str, len);
	v.set_CustomType(Variable::CustomType::STRING, id);
	return v;
}

//static
Variable DataType::CreateForType(uint8_t t, uint8_t argcount, Variable args[]) {
    Variable v;
    switch(t) {
    case Variable::CustomType::BYTEARRAY: {
        assert(argcount == 1);
        uint8_t *var;
        uint8_t id = bitsy_heap.CreateVar(args[0].as_int16(), &var);
		v.set_CustomType(t, id);
        break;
    }
    default:
        assert(false);
    }
    return v;
}

//static
Variable DataType::GetIndex(const Variable& v, uint8_t ind) {
    Variable ret;
    assert(v.type == Variable::CUSTOM);
    uint8_t *var;
    uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
    switch(v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
        assert(false);
        break;
    case Variable::CustomType::STRING:
        assert(len > ind);
        ret.set_CustomType(Variable::CustomType::CHARACTER, var[ind]);
        break;
    default:
        assert(false);
    }
    return ret;
}

//static
uint16_t DataType::Len(const Variable& v) {
    assert(v.type == Variable::CUSTOM);
    uint8_t *var;
    uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
    switch(v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
        return 1;
    case Variable::CustomType::BYTEARRAY:
    case Variable::CustomType::STRING:
        return len-1;
    default:
        assert(false);
    }
}

//static
void DataType::Print(const Variable& v, void (*print)(char)) {
    assert(v.type == Variable::CUSTOM);
    uint8_t *var;
    uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
    switch(v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
        print((char)v.val.custom_type.val);
        break;
    case Variable::CustomType::BYTEARRAY:
        assert(false);
        break;
    case Variable::CustomType::STRING:
        for(uint8_t i=0; i<len-1; i++)
            print(var[i]);
        break;
    default:
        assert(false);
    }
}

}
