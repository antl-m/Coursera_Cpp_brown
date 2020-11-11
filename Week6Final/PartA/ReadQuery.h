#pragma once

#include "DataBase.h"

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

class ResponseBase;
using ResponsePtr = std::unique_ptr<ResponseBase>;

class ReadQueryBase {
 public:
  enum class Type {
    BUS
  };

  explicit ReadQueryBase(Type type) : type_(type) {}
  virtual ResponsePtr Process(const DataBase *db) const = 0;
  virtual void ParseFromStream(std::istream &input) = 0;
  virtual ~ReadQueryBase() = default;

 private:
  Type type_;
};

using ReadQueryPtr = std::unique_ptr<ReadQueryBase>;
using ReadQueries = std::vector<ReadQueryPtr>;
using ReadType = ReadQueryBase::Type;

class BusReadQuery : public ReadQueryBase {
 public:
  BusReadQuery();
  ResponsePtr Process(const DataBase *db) const override;
  void ParseFromStream(std::istream &input) override;
 private:
  int bus_num_;
};

const inline std::unordered_map<std::string, ReadQueryBase::Type> ReadTypes = {
    {"Bus", ReadQueryBase::Type::BUS}
};

class ResponseBase {
 public:
  explicit ResponseBase(ReadType type) : type_(type) {}
  virtual void PrintToStream(std::ostream &out) const = 0;
  virtual ~ResponseBase() = default;
 private:
  ReadType type_;
};

using ResponsePtr = std::unique_ptr<ResponseBase>;
using Responses = std::vector<ResponsePtr>;

ReadQueries ParseReadQueries(std::istream &input = std::cin);

void PrintResponses(const Responses &responses, std::ostream &out = std::cout);
