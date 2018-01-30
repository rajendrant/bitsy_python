#include "gc.h"

#include <assert.h>
#include <set>

#include "bitsy_alloc.h"
#include "ExecStack.h"
#include "FunctionStack.h"
#include "test_common.h"

namespace bitsy_python {

class GCTest {
  public:

static std::set<BitsyHeap::var_id_t> getAllHeapVars() {
  std::set<BitsyHeap::var_id_t> ret;
  std::set<BitsyHeap::var_id_t> free_ids;
  for(BitsyHeap::var_id_t id=BitsyHeap::freeID(); id != INVALID_VARID;) {
    free_ids.insert(id);
    uint8_t *ptr;
    if (id < BitsyHeap::lastID()) {
      BitsyHeap::GetVar(id, &ptr);
      id = ptr[0];
    }
  }
  for(BitsyHeap::var_id_t v=0; v<BitsyHeap::lastID(); v++) {
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
  bool is_callback_mode;
  var.type = Variable::CUSTOM;
  var.val.custom_type.type = Variable::CustomType::STRING;

  bitsy_alloc_init();
  ExecStack::init();
  BitsyHeap::init();

  assert(getAllHeapVars().empty());
  gc();
  assert(getAllHeapVars().empty());

  auto id1 = BitsyHeap::CreateVar(5, &val);
  assert(compareHeapVars({id1}));
  gc();
  assert(compareHeapVars({}));

  id1 = BitsyHeap::CreateVar(5, &val);
  var.val.custom_type.val = id1;
  ExecStack::push(var);
  gc();
  assert(compareHeapVars({id1}));

  ExecStack::pop();
  assert(compareHeapVars({id1}));
  gc();
  assert(compareHeapVars({}));

  id1 = BitsyHeap::CreateVar(10, &val);
  var.val.custom_type.val = id1;
  FunctionStack::setup_function_call(2, 0x1234);
  FunctionStack::setNthVariable(0, var);

  auto id2 = BitsyHeap::CreateVar(20, &val);
  var.val.custom_type.val = id2;
  ExecStack::push(var);

  auto id3 = BitsyHeap::CreateVar(15, &val);
  var.val.custom_type.val = id3;
  FunctionStack::setNthVariable(1, var);
  assert(compareHeapVars({id1, id2, id3}));

  assert(compareHeapVars({id1, id2, id3}));
  gc();
  assert(compareHeapVars({id1, id2, id3}));

  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  gc();
  assert(compareHeapVars({id2}));

  ExecStack::pop();
  gc();
  assert(compareHeapVars({}));
}

static void test2() {
  Variable var;
  uint8_t *val;
  var.type = Variable::CUSTOM;
  var.val.custom_type.type = Variable::CustomType::STRING;

  bitsy_alloc_init();
  ExecStack::init();
  BitsyHeap::init();

  auto id1 = BitsyHeap::CreateVar(10, &val);
  var.val.custom_type.val = id1;
  ExecStack::push(var);
  auto id2 = BitsyHeap::CreateVar(20, &val);
  var.val.custom_type.val = id2;
  ExecStack::push(var);
  auto id3 = BitsyHeap::CreateVar(15, &val);
  var.val.custom_type.val = id3;
  ExecStack::push(var);
  assert(compareHeapVars({id1, id2, id3}));

  assert(compareHeapVars({id1, id2, id3}));
  gc();
  assert(compareHeapVars({id1, id2, id3}));

  ExecStack::pop();
  gc();
  assert(compareHeapVars({id1, id2}));
  ExecStack::pop();
  gc();
  assert(compareHeapVars({id1}));
  ExecStack::pop();
  gc();
  assert(compareHeapVars({}));
}

static void test_all() {
  test1();
  test2();
}
};

}

int main() {
  bitsy_python::GCTest::test_all();
  printf(__FILE__ " passed\n");
  return 0;
}
