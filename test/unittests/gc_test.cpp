#include "gc.h"

#include <assert.h>
#include <set>
#include <set>

#include "test_common.h"

namespace bitsy_python {

class GCTest {
  public:

static std::set<BitsyHeap::var_id_t> getAllHeapVars() {
  std::set<BitsyHeap::var_id_t> ret;
  std::set<BitsyHeap::var_id_t> free_ids;
  for(BitsyHeap::var_id_t id=bitsy_heap.hdr.free_id; id != INVALID_VARID;) {
    free_ids.insert(id);
    uint8_t *ptr;
    if (id < bitsy_heap.hdr.last) {
      bitsy_heap.GetVar(id, &ptr);
      id = ptr[0];
    }
  }
  for(BitsyHeap::var_id_t v=0; v<bitsy_heap.hdr.last; v++) {
    if(free_ids.find(v)==free_ids.end())
      ret.insert(v);
  }
  return ret;
}

static bool compareHeapVars(const std::set<BitsyHeap::var_id_t> expected) {
  return expected == getAllHeapVars();
}

static void test1() {
  Variable var;
  uint8_t *val;
  uint16_t ins_ptr;
  var.type = Variable::CUSTOM;
  var.val.custom_type.type = Variable::CustomType::STRING;

  assert(getAllHeapVars().empty());
  gc();
  assert(getAllHeapVars().empty());

  auto id1 = bitsy_heap.CreateVar(5, &val);
  assert(compareHeapVars({id1}));
  gc();
  assert(compareHeapVars({}));

  id1 = bitsy_heap.CreateVar(5, &val);
  var.val.custom_type.val = id1;
  exec_stack.push(var);
  gc();
  assert(compareHeapVars({id1}));

  exec_stack.pop();
  assert(compareHeapVars({id1}));
  gc();
  assert(compareHeapVars({}));

  id1 = bitsy_heap.CreateVar(10, &val);
  var.val.custom_type.val = id1;
  function_stack.setup_function_call(0, 2, 0x1234);
  function_stack.setNthVariable(0, var);

  auto id2 = bitsy_heap.CreateVar(20, &val);
  var.val.custom_type.val = id2;
  exec_stack.push(var);

  auto id3 = bitsy_heap.CreateVar(15, &val);
  var.val.custom_type.val = id3;
  function_stack.setNthVariable(1, var);
  assert(compareHeapVars({id1, id2, id3}));

  assert(compareHeapVars({id1, id2, id3}));
  gc();
  assert(compareHeapVars({id1, id2, id3}));

  function_stack.return_function(&ins_ptr);
  gc();
  assert(compareHeapVars({id2}));

  exec_stack.pop();
  gc();
  assert(compareHeapVars({}));
}

static void test2() {
  Variable var;
  uint8_t *val;
  var.type = Variable::CUSTOM;
  var.val.custom_type.type = Variable::CustomType::STRING;

  auto id1 = bitsy_heap.CreateVar(10, &val);
  var.val.custom_type.val = id1;
  exec_stack.push(var);
  auto id2 = bitsy_heap.CreateVar(20, &val);
  var.val.custom_type.val = id2;
  exec_stack.push(var);
  auto id3 = bitsy_heap.CreateVar(15, &val);
  var.val.custom_type.val = id3;
  exec_stack.push(var);
  assert(compareHeapVars({id1, id2, id3}));

  assert(compareHeapVars({id1, id2, id3}));
  gc();
  assert(compareHeapVars({id1, id2, id3}));

  exec_stack.pop();
  gc();
  assert(compareHeapVars({id1, id2}));
  exec_stack.pop();
  gc();
  assert(compareHeapVars({id1}));
  exec_stack.pop();
  gc();
  assert(compareHeapVars({}));
}

static void test_all() {
  test1();
  //test2();
}
};

}

int main() {
  bitsy_python::GCTest::test_all();
  printf(__FILE__ " passed\n");
  return 0;
}
