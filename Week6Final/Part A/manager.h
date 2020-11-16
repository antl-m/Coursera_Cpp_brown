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

private:
  template <typename Number>
  Number ReadNumberOnLine(std::istream& stream);

  std::string MakeAnswerFromAnyRequest(RequestHolder request);

  RequestHolder ParseModifyRequest(std::string_view request_str);
  RequestHolder ParseAddStop(std::string_view request_str);
  RequestHolder ParseAddRoute(std::string_view request_str);

  RequestHolder ParseReadRequest(std::string_view request_str);
  RequestHolder ParseTakeRoute(std::string_view request_str);

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
