//
// Created by ilya on 28.11.2019.
//

#ifndef YANDEXBROWNFINAL_4_BROWN_FINAL_PROJECT_PART_A_MANAGER_H
#define YANDEXBROWNFINAL_4_BROWN_FINAL_PROJECT_PART_A_MANAGER_H
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <list>

#include "json.h"
#include "request.h"
#include "database.h"

void PrintResults(const std::vector<std::string>& results, std::ostream& out = std::cout);

class DatabaseManager {
public:
  DatabaseManager() = delete;
  explicit DatabaseManager(Database& db) : db_(db) {}

  std::vector<std::string> ProcessAllRequests(std::istream& in = std::cin);
  std::string ProcessReadRequest(const std::string& read);
  std::string ProcessModifyRequest(const std::string& modify);

  Json::Node ProcessAllJSONRequests(std::istream& in = std::cin);
  Json::Node ProcessJSONReadRequest(const Json::Node& node);
  Json::Node ProcessJSONModifyRequest(const Json::Node& node);

private:
  template <typename Number>
  Number ReadNumberOnLine(std::istream& stream);
  std::string MakeAnswerFromAnyRequest(RequestHolder request);
  RequestHolder ParseModifyRequest(std::string_view request_str);
  RequestHolder ParseReadRequest(std::string_view request_str);

  Json::Node MakeJSONAnswerFromAnyRequest(RequestHolder request);
  RequestHolder ParseModifyJSONRequest(const Json::Node& node);
  RequestHolder ParseReadJSONRequest(const Json::Node& node);

  template <typename RequestType>
  RequestHolder ParseRequest(RequestType type, std::string_view request_str);
  template <typename RequestType>
  RequestHolder JSONRequest(RequestType type, const Json::Node& node);

  Database& db_;
};

template <typename Number>
Number DatabaseManager::DatabaseManager::ReadNumberOnLine(std::istream& stream) {
  Number number;
  stream >> number;
  std::string dummy;
  getline(stream, dummy);
  return number;
}

#endif //YANDEXBROWNFINAL_4_BROWN_FINAL_PROJECT_PART_A_MANAGER_H
