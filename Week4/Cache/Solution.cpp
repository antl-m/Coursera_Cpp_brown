#include "Common.h"
#include <list>
#include <algorithm>
#include <mutex>

using namespace std;

class LruCache : public ICache {
public:
  LruCache(
      shared_ptr<IBooksUnpacker> books_unpacker,
      const Settings& settings
  ):unpacker_(move(books_unpacker)), config_(settings) {
    // реализуйте метод
  }

  BookPtr GetBook(const string& book_name) override {
    // реализуйте метод
    lock_guard lg(m);
    auto res_it = find_if(books_.begin(), books_.end(), [&](const BookPtr& p){
      return p->GetName() == book_name;
    });
    if(res_it == books_.end()){
      books_.push_front(unpacker_->UnpackBook(book_name));
    }else{
      books_.push_front(*res_it);
      books_.erase(res_it);
    }
    BookPtr result = books_.front();
    shrink();
    return result;
  }

private:
  void shrink(){
    if(summary_size() > config_.max_memory){
      books_.pop_back();
      shrink();
    }
  }

  size_t summary_size(){
    size_t result = 0;
    for_each(books_.begin(), books_.end(), [&](const BookPtr& p){
      result += p->GetContent().size();
    });
    return result;
  }

  shared_ptr<IBooksUnpacker> unpacker_;
  Settings config_;
  list<BookPtr> books_;
  mutex m;
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings
) {
  // реализуйте функцию
  return make_unique<LruCache>(move(books_unpacker), settings);
}
