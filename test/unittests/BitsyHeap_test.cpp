#include "BitsyHeap.h"

#include <assert.h>
#include <map>
#include <string.h>

#include "test_common.h"

namespace bitsy_python {

std::string rand_string() {
  std::string r;
  for(int i=rand()%10; i--;)
    r+=(rand()%(0x7a-0x20)) + 0x20;
  return r;
}

void test1() {
  int tests=10000;
  BitsyHeap h;
  std::map<int, std::string> actual;
  while(tests--) {
    if(actual.size() <= 50 && rand()%10 == 0) {
      auto s = rand_string();
      uint8_t *var;
      auto id = h.CreateVar(s.size(), &var);
      strncpy((char*)var, s.c_str(), s.size());
      assert(actual.find(id)==actual.end());
      actual[id] = s;
    } else if (actual.size()) {
      uint8_t c = rand()%actual.size();
      auto it=actual.begin();
      for(; c--; it++);
      uint8_t id = it->first;
      std::string &s = it->second;
      uint8_t *var;
      assert(s.size() == h.GetVar(id, &var));
      assert(!strncmp(s.c_str(), (char*)var, s.size()));
      if (s.size() <= 22 && rand()%10 == 0) {
        s += "abc";
        var = h.ExtendVar(id, var, s.size());
        assert(var);
        strncpy((char*)var+s.size()-3, "abc", 3);
        assert(!strncmp(s.c_str(), (char*)var, s.size()));
      }
    }
  }
}

bool assert_variable(const BitsyHeap& h, BitsyHeap::var_id_t id, const char *val, uint8_t len) {
  uint8_t *actual;
  return len==h.GetVar(id, &actual) && !memcmp(val, actual, len);
}

void test_free() {
  BitsyHeap h;
  assert(h.getFreeIDMap(0)==0xFFFFFFFF);

  uint8_t *val;
  auto id1 = h.CreateVar(5, &val);
  memcpy(val, "1var-", 5);
  auto id2 = h.CreateVar(5, &val);
  memcpy(val, "2var-", 5);
  auto id3 = h.CreateVar(5, &val);
  memcpy(val, "3var-", 5);

  assert(assert_variable(h, id1, "1var-", 5));
  assert(assert_variable(h, id2, "2var-", 5));
  assert(assert_variable(h, id3, "3var-", 5));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Extend
  h.FreeVar(id1);
  assert(h.getFreeIDMap(0)==0xFFFFFFF9);
  auto id4 = h.CreateVar(10, &val);
  memcpy(val, "4-variable", 10);
  assert(id4 == id1);
  assert(assert_variable(h, id4, "4-variable", 10));
  assert(assert_variable(h, id2, "2var-", 5));
  assert(assert_variable(h, id3, "3var-", 5));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Shrink
  h.FreeVar(id4);
  assert(h.getFreeIDMap(0)==0xFFFFFFF9);
  auto id5 = h.CreateVar(5, &val);
  memcpy(val, "5var-", 5);
  assert(id5 == id4);
  assert(assert_variable(h, id5, "5var-", 5));
  assert(assert_variable(h, id2, "2var-", 5));
  assert(assert_variable(h, id3, "3var-", 5));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Shrink
  h.FreeVar(id2);
  assert(h.getFreeIDMap(0)==0xFFFFFFFA);
  auto id6 = h.CreateVar(3, &val);
  memcpy(val, "v6", 3);
  assert(id2 == id6);
  assert(assert_variable(h, id5, "5var-", 5));
  assert(assert_variable(h, id6, "v6", 3));
  assert(assert_variable(h, id3, "3var-", 5));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Shrink
  h.FreeVar(id6);
  assert(h.getFreeIDMap(0)==0xFFFFFFFA);
  auto id7 = h.CreateVar(6, &val);
  memcpy(val, "7var", 6);
  assert(id6 == id7);
  assert(assert_variable(h, id5, "5var-", 5));
  assert(assert_variable(h, id7, "7var", 6));
  assert(assert_variable(h, id3, "3var-", 5));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Extend
  h.FreeVar(id3);
  assert(h.getFreeIDMap(0)==0xFFFFFFFC);
  auto id8 = h.CreateVar(10, &val);
  memcpy(val, "8variable", 10);
  assert(id3 == id8);
  assert(assert_variable(h, id5, "5var-", 5));
  assert(assert_variable(h, id7, "7var", 6));
  assert(assert_variable(h, id8, "8variable", 10));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Shrink
  h.FreeVar(id8);
  assert(h.getFreeIDMap(0)==0xFFFFFFFC);
  auto id9 = h.CreateVar(4, &val);
  memcpy(val, "9var", 4);
  assert(id8 == id9);
  assert(assert_variable(h, id5, "5var-", 5));
  assert(assert_variable(h, id7, "7var", 6));
  assert(assert_variable(h, id9, "9var", 4));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  // Same size
  h.FreeVar(id9);
  assert(h.getFreeIDMap(0)==0xFFFFFFFC);
  auto id10 = h.CreateVar(4, &val);
  memcpy(val, "10v", 4);
  assert(id9 == id10);
  assert(assert_variable(h, id5, "5var-", 5));
  assert(assert_variable(h, id7, "7var", 6));
  assert(assert_variable(h, id10, "10v", 4));
  assert(h.getFreeIDMap(0)==0xFFFFFFF8);

  h.FreeVar(id5);
  assert(h.getFreeIDMap(0)==0xFFFFFFF9);
  h.FreeVar(id7);
  assert(h.getFreeIDMap(0)==0xFFFFFFFB);
  h.FreeVar(id10);
  assert(h.getFreeIDMap(0)==0xFFFFFFFF);
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
