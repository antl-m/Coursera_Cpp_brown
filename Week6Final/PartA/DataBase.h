#pragma once

#include "ModifyQuery.h"
#include "ReadQuery.h"
#include "Stop.h"
#include "Bus.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

class DataBase {
 public:
  explicit DataBase(std::istream &input = std::cin);

  Responses ProcessReadQueries() const;

 private:
  friend class BusReadQuery;
 private:

  ModifyQueries modify_queries_;
  ReadQueries read_queries_;
  Stops stop_to_point_;
  Buses bus_to_route_;
};
