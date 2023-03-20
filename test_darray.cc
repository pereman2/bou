#include <cassert>
#include <cstdio>

#include "darray.h"

struct Foo {
  int a;
  bool b;
};

#define TEST(fn, name)                                                             \
  {                                                                                \
    printf("\033[36mRunning test::%s => %-50s ...\033[36m ", __FILE_NAME__, name); \
    fflush(stdout);                                                                \
    fn();                                                                          \
    printf("\033[32mOK\033\n");                                                    \
  }

void test_simple() {
  darray da;
  init_darray(&da);
  Foo foo;
  foo.a = 1;
  foo.b = 2;
  Foo foo2;
  foo.a = 3;
  foo.b = 4;

  darray_push((&da), sizeof(Foo), &foo);
  darray_push((&da), sizeof(Foo), &foo2);

  darray_get((&da), sizeof(Foo), 0);

  Foo* foo_1 = (Foo*)darray_get((&da), sizeof(Foo), 0);
  Foo* foo2_1 = (Foo*)darray_get((&da), sizeof(Foo), 1);

  assert(foo.a == foo_1->a);
  assert(foo.b == foo_1->b);

  assert(foo2.a == foo2_1->a);
  assert(foo2.b == foo2_1->b);
}

void test_length() {
  darray da;
  init_darray(&da);
  Foo foo;
  foo.a = 1;
  foo.b = 2;
  size_t size = 1 << 8;
  for (int i = 0; i < size; i++) {
    darray_push((&da), sizeof(Foo), &foo);
  }

  assert(darray_length(&da, sizeof(Foo)) == size);
}

void test_reserve() {
  darray da;
  init_darray(&da);
  Foo foo;
  foo.a = 1;
  foo.b = 2;
  size_t size = 1 << 8;
  darray_reserve(&da, sizeof(Foo), size);

  assert(darray_length(&da, sizeof(Foo)) == size);
}

void test_remove() {
  darray da;
  init_darray(&da);
  Foo foo;
  foo.a = 1;
  foo.b = 2;
  size_t size = 1 << 8;
  for (int i = 0; i < size; i++) {
    foo.a = i;
    darray_push((&da), sizeof(Foo), &foo);
  }

  assert(darray_length(&da, sizeof(Foo)) == size);

  int i = 40;
  Foo* res = (Foo*)darray_get(&da, sizeof(Foo), i);
  assert(res->a == i);

  assert(darray_remove(&da, sizeof(Foo), i) == 0);
  assert(res->a == i + 1);
}

int main() {
  TEST(test_simple, "simple");
  TEST(test_length, "length");
  TEST(test_reserve, "reserve");
  TEST(test_remove, "remove");
}
