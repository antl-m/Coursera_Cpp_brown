#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template<typename T>
class UniquePtr {
 private:
  T *data_ptr;
 public:
  UniquePtr() : data_ptr(nullptr) {}
  UniquePtr(T *ptr) : data_ptr(ptr) {}
  UniquePtr(const UniquePtr &) = delete;
  UniquePtr(UniquePtr &&other) : data_ptr(other.data_ptr) {
    other.data_ptr = nullptr;
  }
  UniquePtr &operator=(const UniquePtr &) = delete;
  UniquePtr &operator=(nullptr_t) {
    if (data_ptr)
      delete data_ptr;
    data_ptr = nullptr;
    return *this;
  }
  UniquePtr &operator=(UniquePtr &&other) {
    if (other.data_ptr != data_ptr) {
      if (data_ptr)
        delete data_ptr;
      data_ptr = other.data_ptr;
      other.data_ptr = nullptr;
    }
    return *this;
  }
  ~UniquePtr() {
    if (data_ptr)
      delete data_ptr;
  }

  T &operator*() const {
//    if(data_ptr)
    return *data_ptr;
  }

  T *operator->() const {
    return data_ptr;
  }

  T *Release() {
    auto ret = data_ptr;
    data_ptr = nullptr;
    return ret;
  }

  void Reset(T *ptr) {
    delete data_ptr;
    data_ptr = ptr;
  }

  void Swap(UniquePtr &other) {
    std::swap(data_ptr, other.data_ptr);
  }

  T *Get() const {
    return data_ptr;
  }
};

struct Item {
  static int counter;
  int value;
  Item(int v = 0) : value(v) {
    ++counter;
  }
  Item(const Item &other) : value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;

void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
}
