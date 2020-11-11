#include "DataBase.h"

DataBase::DataBase(std::istream &input) :
  modify_queries_(ParseModifyQueries(input)),
  read_queries_(ParseReadQueries(input))
  {}

Responses DataBase::ProcessReadQueries() const {
  Responses result;
  result.reserve(read_queries_.size());
  for (const ReadQueryPtr &query : read_queries_) {
    result.push_back(query->Process(this));
  }
  return result;
}
