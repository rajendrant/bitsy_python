#include "BitsyHeap.h"

#include <assert.h>
#include <map>
#include <string.h>

#include "test_common.h"
#include "gc.h"

namespace bitsy_python {

std::string rand_string() {
  std::string r;
  for(int i=rand()%10+1; i--;)
    r+=(rand()%(0x7a-0x20)) + 0x20;
  return r;
}

void AddToStack(BitsyHeap::var_id_t id) {
  // Add the variable to stack to avoid gc.
  Variable v;
  v.type = Variable::CUSTOM;
  v.val.custom_type.type = Variable::CustomType::STRING;
  v.val.custom_type.val = id;
  ExecStack::push(v);
}

void test1() {
  int tests=200;
  std::map<int, std::string> actual;
  BitsyHeap::init();
  while(tests--) {
    if(actual.size() <= 50 && rand()%10 == 0) {
      auto s = rand_string();
      uint8_t *var;
      auto id = BitsyHeap::CreateVar(s.size(), &var);
      strncpy((char*)var, s.c_str(), s.size());
      assert(actual.find(id)==actual.end());
      actual[id] = s;
      AddToStack(id);
    } else if (actual.size()) {
      uint8_t c = rand()%actual.size();
      auto it=actual.begin();
      for(; c--; it++);
      uint8_t id = it->first;
      std::string &s = it->second;
      uint8_t *var;
      assert(s.size() == BitsyHeap::GetVar(id, &var));
      assert(!strncmp(s.c_str(), (char*)var, s.size()));
      if (s.size() <= 22 && rand()%10 == 0) {
        s += "abc";
        var = BitsyHeap::ExtendVar(id, var, s.size());
        assert(var);
        strncpy((char*)var+s.size()-3, "abc", 3);
        assert(!strncmp(s.c_str(), (char*)var, s.size()));
      }
    }
  }

  // Remove all items from stack, for them to be gc'd automatically later.
  while(ExecStack::getCustomHeapVariableMap(0))
    ExecStack::pop();
}

bool assert_variable(BitsyHeap::var_id_t id, const char *val, uint8_t len) {
  uint8_t *actual;
  return len==BitsyHeap::GetVar(id, &actual) && !memcmp(val, actual, len);
}

bool check_free_id_map(uint8_t id1=INVALID_VARID, uint8_t id2=INVALID_VARID,
                       uint8_t id3=INVALID_VARID) {
  uint32_t expected = 0;
  if (id1!=INVALID_VARID) expected |= (0x1<<id1);
  if (id2!=INVALID_VARID) expected |= (0x1<<id2);
  if (id3!=INVALID_VARID) expected |= (0x1<<id3);
  return BitsyHeap::getFreeIDMap(0)==(expected^0xFFFFFFFF);
}

void test_free() {
  gc();
  assert(BitsyHeap::getFreeIDMap(0)==0xFFFFFFFF);

  uint8_t *val;
  auto id1 = BitsyHeap::CreateVar(5, &val);
  memcpy(val, "1var-", 5);
  assert(assert_variable(id1, "1var-", 5));
  AddToStack(id1);
  auto id2 = BitsyHeap::CreateVar(5, &val);
  memcpy(val, "2var-", 5);
  assert(assert_variable(id2, "2var-", 5));
  AddToStack(id2);
  auto id3 = BitsyHeap::CreateVar(5, &val);
  memcpy(val, "3var-", 5);
  assert(assert_variable(id3, "3var-", 5));
  AddToStack(id3);

  assert(check_free_id_map(id1, id2, id3));

  // Extend
  BitsyHeap::FreeVar(id1);
  assert(check_free_id_map(id2, id3));
  auto id4 = BitsyHeap::CreateVar(10, &val);
  memcpy(val, "4-variable", 10);
  assert(id4 == id1);
  assert(assert_variable(id4, "4-variable", 10));
  assert(assert_variable(id2, "2var-", 5));
  assert(assert_variable(id3, "3var-", 5));
  assert(check_free_id_map(id2, id3, id4));

  // Shrink
  BitsyHeap::FreeVar(id4);
  assert(check_free_id_map(id2, id3));
  auto id5 = BitsyHeap::CreateVar(5, &val);
  memcpy(val, "5var-", 5);
  assert(id5 == id4);
  assert(assert_variable(id5, "5var-", 5));
  assert(assert_variable(id2, "2var-", 5));
  assert(assert_variable(id3, "3var-", 5));
  assert(check_free_id_map(id2, id3, id5));

  // Shrink
  BitsyHeap::FreeVar(id2);
  assert(check_free_id_map(id3, id5));
  auto id6 = BitsyHeap::CreateVar(3, &val);
  memcpy(val, "v6", 3);
  assert(id2 == id6);
  assert(assert_variable(id5, "5var-", 5));
  assert(assert_variable(id6, "v6", 3));
  assert(assert_variable(id3, "3var-", 5));
  assert(check_free_id_map(id3, id5, id6));

  // Shrink
  BitsyHeap::FreeVar(id6);
  assert(check_free_id_map(id3, id5));
  auto id7 = BitsyHeap::CreateVar(6, &val);
  memcpy(val, "7var", 6);
  assert(id6 == id7);
  assert(assert_variable(id5, "5var-", 5));
  assert(assert_variable(id7, "7var", 6));
  assert(assert_variable(id3, "3var-", 5));
  assert(check_free_id_map(id3, id5, id7));

  // Extend
  BitsyHeap::FreeVar(id3);
  assert(check_free_id_map(id5, id7));
  auto id8 = BitsyHeap::CreateVar(10, &val);
  memcpy(val, "8variable", 10);
  assert(id3 == id8);
  assert(assert_variable(id5, "5var-", 5));
  assert(assert_variable(id7, "7var", 6));
  assert(assert_variable(id8, "8variable", 10));
  assert(check_free_id_map(id5, id7, id8));

  // Shrink
  BitsyHeap::FreeVar(id8);
  assert(check_free_id_map(id5, id7));
  auto id9 = BitsyHeap::CreateVar(4, &val);
  memcpy(val, "9var", 4);
  assert(id8 == id9);
  assert(assert_variable(id5, "5var-", 5));
  assert(assert_variable(id7, "7var", 6));
  assert(assert_variable(id9, "9var", 4));
  assert(check_free_id_map(id5, id7, id9));

  // Same size
  BitsyHeap::FreeVar(id9);
  assert(check_free_id_map(id5, id7));
  auto id10 = BitsyHeap::CreateVar(4, &val);
  memcpy(val, "10v", 4);
  assert(id9 == id10);
  assert(assert_variable(id5, "5var-", 5));
  assert(assert_variable(id7, "7var", 6));
  assert(assert_variable(id10, "10v", 4));
  assert(check_free_id_map(id5, id7, id10));

  BitsyHeap::FreeVar(id5);
  assert(check_free_id_map(id7, id10));
  BitsyHeap::FreeVar(id7);
  assert(check_free_id_map(id10));
  BitsyHeap::FreeVar(id10);
  assert(check_free_id_map());
}

void test_all() {
  for(int i=1000; i--;)
    test1();
  test_free();
}

}

int main() {
  bitsy_python::test_all();
  printf(__FILE__ " passed\n");
  return 0;
}
