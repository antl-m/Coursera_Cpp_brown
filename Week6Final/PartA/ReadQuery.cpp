#include "ReadQuery.h"
#include "other.h"
#include "Stop.h"

#include <unordered_set>

ReadQueryPtr ReadQueryPtrByType(ReadType type) {
  switch (type) {
    case ReadType::BUS: return std::make_unique<BusReadQuery>();
    default: return nullptr;
  }
}

ReadQueryPtr ParseReadQuery(std::istream &input) {
  std::string type_str;
  input >> type_str;
  ReadType type = ReadTypes.at(type_str);
  ReadQueryPtr result = ReadQueryPtrByType(type);
  result->ParseFromStream(input);
  return result;
}

ReadQueries ParseReadQueries(std::istream &input) {
  ReadQueries result;
  int count = ReadNumberLine<int>(input);
  result.reserve(count);
  for (int kI = 0; kI < count; ++kI) {
    result.push_back(ParseReadQuery(input));
  }
  return result;
}

void PrintResponses(const Responses &responses, std::ostream &out) {
  for (const ResponsePtr &response : responses) {
    response->PrintToStream(out);
    out << '\n';
  }
}

BusReadQuery::BusReadQuery() :
    ReadQueryBase(Type::BUS),
    bus_num_(0) {}

void BusReadQuery::ParseFromStream(std::istream &input) {
  bus_num_ = ReadNumberLine<int>(input);
}

ResponsePtr BusReadQuery::Process(const DataBase *db) const {
  const Route &route = db->bus_to_route_.at(this->bus_num_);
  int stops_count = route.size();
  std::unordered_set<std::string_view> unique_stops(route.begin(), route.end());
  int unique_stops_count = unique_stops.size();
  //TODO: Use IILE to compute route distance...
  return ResponsePtr();
}
