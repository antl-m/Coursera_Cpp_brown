#include "test_runner.h"

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
    std::string id;
    std::string title;
    std::string user;
    int timestamp;
    int karma;
};


// Ðåàëèçóéòå ýòîò êëàññ
class Database {
public:
    bool Put(const Record &record);

    const Record *GetById(const std::string &id) const;

    bool Erase(const std::string &id);

    template<typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const;

    template<typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const;

    template<typename Callback>
    void AllByUser(const std::string &user, Callback callback) const;

private:
    using ListIter = std::list<Record>::iterator;

    struct Iterators {
        std::multimap<int, Database::ListIter>::iterator timestamp_iterator;
        std::multimap<int, Database::ListIter>::iterator karma_iterator;
        std::multimap<std::string, Database::ListIter>::iterator user_iterator;
    };

    std::list<Record> storage_;
    std::unordered_map<std::string, ListIter> id_to_record_;
    std::multimap<int, ListIter> timestamp_map_;
    std::multimap<int, ListIter> karma_map_;
    std::multimap<std::string, ListIter> user_map_;
    std::unordered_map<std::string, Iterators> id_to_iterators_;

    void inserter(const Record &record);

    void deleter(const std::unordered_map<std::string, ListIter>::iterator &iter);
};


template<typename Callback>
void Database::RangeByTimestamp(int low, int high, Callback callback) const {
    auto lower = timestamp_map_.lower_bound(low);
    const auto upper = timestamp_map_.upper_bound(high);

    while (lower != upper && callback(*lower->second)) {
        ++lower;
    }
}


template<typename Callback>
void Database::RangeByKarma(int low, int high, Callback callback) const {
    auto lower = karma_map_.lower_bound(low);
    const auto upper = karma_map_.upper_bound(high);

    while (lower != upper && callback(*lower->second)) {
        ++lower;
    }
}


template<typename Callback>
void Database::AllByUser(const std::string &user, Callback callback) const {
    auto[first, end] = user_map_.equal_range(user);

    while (first != end && callback(*first->second)) {
        ++first;
    }
}

void Database::inserter(const Record &record) {
    storage_.push_front(record);
    id_to_record_[record.id] = storage_.begin();
    const auto timestamp_iter = timestamp_map_.insert({record.timestamp, storage_.begin()});
    const auto karma_iter = karma_map_.insert({record.karma, storage_.begin()});
    const auto user_iter = user_map_.insert({record.user, storage_.begin()});

    id_to_iterators_.insert({record.id,
                             Iterators{timestamp_iter, karma_iter, user_iter}});
}


void Database::deleter(const std::unordered_map<std::string, ListIter>::iterator &iter) {
    const Iterators iterators = id_to_iterators_.at(iter->first);
    timestamp_map_.erase(iterators.timestamp_iterator);
    karma_map_.erase(iterators.karma_iterator);
    user_map_.erase(iterators.user_iterator);

    const ListIter list_iter = iter->second;
    id_to_record_.erase(iter);
    storage_.erase(list_iter);
}


bool Database::Put(const Record &record) {
    const auto finder = id_to_record_.find(record.id);

    if (finder == id_to_record_.end()) {
        inserter(record);

        return true;
    }

    return false;
}


const Record *Database::GetById(const std::string &id) const {
    try {
        return &(*id_to_record_.at(id));
    }
    catch (std::out_of_range &) {
        return nullptr;
    }
}


bool Database::Erase(const std::string &id) {
    const auto finder = id_to_record_.find(id);

    if (finder == id_to_record_.end()) {
        return false;
    }

    deleter(finder);
    return true;
}

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record &) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
