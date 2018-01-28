#include "FunctionStack.h"

#include <string.h>

#include "bitsy_alloc.h"
#include "gc.h"

namespace bitsy_python {

typedef struct {
  uint8_t var_count;
  uint16_t ins_ptr;
  uint16_t start;
}__attribute__((packed)) FunctionStackHeader;

#define HDR_START sizeof(FunctionStackHeader)
#define HDR_SIZE_FOR_VARS(v) (((v)-1) / 4 + 1)
#define MAX_VAR_SIZE 4

#define stack ((uint8_t *)blocks)

FunctionStack::FunctionStack() { }

bool FunctionStack::is_empty() { return top == 0; }

void FunctionStack::setup_function_call(uint8_t args, uint8_t vars,
                                        uint16_t old_ins_ptr) {
  while (top_block_size() < top + HDR_START + HDR_SIZE_FOR_VARS(vars) + vars * MAX_VAR_SIZE) {
    top_block_alloc();
  }
  FunctionStackHeader *hdr = (FunctionStackHeader*)(stack+top);
  hdr->var_count = vars;
  hdr->ins_ptr = old_ins_ptr;
  hdr->start = start;
  start = top;
  top += HDR_START + HDR_SIZE_FOR_VARS(vars) + vars;
  memset(stack + start + HDR_START, 0, top - start);
}

bool FunctionStack::return_function(uint16_t *old_ins_ptr) {
  top = start;
  FunctionStackHeader *hdr = (FunctionStackHeader*)(stack+start);
  *old_ins_ptr = hdr->ins_ptr;
  start = hdr->start;
  while (top_block_size() > top + 100) {
    top_block_free();
  }
  return is_empty();
}

uint8_t FunctionStack::get_var_hdr(uint8_t n) const {
  return ((stack[start + HDR_START + n / 4] >> (2 * (n % 4))) & 0x3);
}

void FunctionStack::set_var_hdr(uint8_t n, uint8_t val) {
  stack[start + HDR_START + n / 4] =
      (stack[start + HDR_START + n / 4] & ~(0x3 << ((2 * (n % 4))))) |
      (val & 0x3) << ((2 * (n % 4)));
}

Variable FunctionStack::getNthVariable(uint8_t n) const {
  FunctionStackHeader *hdr = (FunctionStackHeader*)(stack+start);
  BITSY_ASSERT(n < hdr->var_count);
  Variable v;
  uint16_t pre = start + HDR_START + HDR_SIZE_FOR_VARS(hdr->var_count);
  for (uint8_t i = 0; i < n; i++) {
    pre += Variable::get_size_from_type(get_var_hdr(i));
  }
  v.type = get_var_hdr(n);
  memcpy(&v.val, stack + pre, v.size());
  return v;
}

void FunctionStack::setNthVariable(uint8_t n, const Variable& v) {
  FunctionStackHeader *hdr = (FunctionStackHeader*)(stack+start);
  BITSY_ASSERT(n < hdr->var_count);
  uint16_t pre = start + HDR_START + HDR_SIZE_FOR_VARS(stack[start]);
  for (uint8_t i = 0; i < n; i++) {
    pre += Variable::get_size_from_type(get_var_hdr(i));
  }
  auto old_size = Variable::get_size_from_type(get_var_hdr(n));
  auto size = v.size();
  if (size != old_size) {
    set_var_hdr(n, v.type);
    memmove(stack + pre + size, stack + pre + old_size, top - pre - old_size);
    if (size > old_size)
      top += size - old_size;
    else
      top -= old_size - size;
  }
  memcpy(stack + pre, &v.val, size);
}

uint32_t FunctionStack::getCustomHeapVariableMap(uint8_t start_id) const {
  uint32_t map = 0;
  for(auto f=start; f<top;) {
    FunctionStackHeader *hdr = (FunctionStackHeader*)(stack+f);
    uint16_t pre = f + HDR_START + HDR_SIZE_FOR_VARS(hdr->var_count);
    for(uint8_t v=0; v<hdr->var_count; v++) {
      uint8_t size, type;
      type = ((stack[f + HDR_START + v / 4] >> (2 * (v % 4))) & 0x3);
      size = Variable::get_size_from_type(type);
      if (type == Variable::CUSTOM) {
        Variable var;
        var.type = Variable::CUSTOM;
        memcpy(&var.val, stack + pre, size);
        updateCustomHeapVariableMap(start_id, var, &map);
      }
      pre += size;
    }
    if(f==0 && hdr->start==0) break;
    f=hdr->start;
  }
  return map;
}

}
