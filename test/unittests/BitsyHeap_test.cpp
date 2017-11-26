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

void test_all() {
  for(int i=1000; i--;)
    test1();
}

}

int main() {
  bitsy_python::test_all();
  printf(__FILE__ " passed\n");
  return 0;
}
