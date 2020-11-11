#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

class ModifyQueryBase {
 public:
  enum class Type {
    STOP,
    BUS
  };

 private:

};

const inline std::unordered_map<std::string, ModifyQueryBase::Type> ModifyTypes = {
    {"Stop", ModifyQueryBase::Type::STOP},
    {"Bus", ModifyQueryBase::Type::BUS}
};

using ModifyQueryPtr = std::unique_ptr<ModifyQueryBase>;
using ModifyQueries = std::vector<ModifyQueryPtr>;

ModifyQueryPtr ParseModifyQuery(std::istream &input = std::cin);

ModifyQueries ParseModifyQueries(std::istream &input = std::cin);
