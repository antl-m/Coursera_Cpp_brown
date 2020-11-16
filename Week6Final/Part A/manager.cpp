//
// Created by ilya on 28.11.2019.
//

#include "manager.h"

void PrintResults(const std::vector<std::string>& results, std::ostream& out) {
  for (const auto& result : results) {
    out << result << "\n";
  }
}

std::vector<std::string> DatabaseManager::ProcessAllRequests(std::istream& in) {
  std::vector<std::string> results;
  const size_t COUNT_OF_MODIFY = ReadNumberOnLine<int>(in);
  results.reserve(COUNT_OF_MODIFY);
  for (size_t i = 0; i < COUNT_OF_MODIFY; ++i) {
    std::string raw_request;
    std::getline(in, raw_request);
    auto answer = ProcessModifyRequest(raw_request);
    std::cerr << answer << std::endl;
  }

  const size_t COUNT_OF_READ = ReadNumberOnLine<int>(in);
  results.reserve(COUNT_OF_READ);
  for (size_t i = 0; i < COUNT_OF_READ; ++i) {
    std::string raw_request;
    std::getline(in, raw_request);
    auto answer = ProcessReadRequest(raw_request);
    std::cerr << answer << std::endl;
    results.push_back(std::move(answer));
  }
  return results;
}

std::string DatabaseManager::ProcessReadRequest(const std::string& read) {
  return MakeAnswerFromAnyRequest(ParseReadRequest(read));
}
std::string DatabaseManager::ProcessModifyRequest(const std::string& modify) {
  return MakeAnswerFromAnyRequest(ParseModifyRequest(modify));
}

std::string DatabaseManager::MakeAnswerFromAnyRequest(RequestHolder request) {
  switch (request->GetType()) {
  case Request::Type::TAKE_ROUTE: {
    const auto& cast_request = dynamic_cast<ReadRequest<TakeRouteAnswer>&>(*request);
    const auto result = cast_request.Process(db_);
    return cast_request.StringAnswer(result);
  }
  case Request::Type::ADD_STOP: {
    const auto& cast_request = dynamic_cast<ModifyRequest&>(*request);
    cast_request.Process(db_);
    return "Stop added";
  }
  case Request::Type::ADD_ROUTE: {
    const auto& cast_request = dynamic_cast<ModifyRequest&>(*request);
    cast_request.Process(db_);
    return "Route added";
  }
  default:
    return "error";
  }
}

RequestHolder DatabaseManager::ParseModifyRequest(std::string_view request_str) {
  char split_by = ' ';
  size_t first_space = request_str.find(split_by);
  std::string_view type = request_str.substr(0, first_space);
  if (type == "Stop") {
    return ParseAddStop(request_str.substr(first_space + 1));
  }
  else if (type == "Bus") {
    return ParseAddRoute(request_str.substr(first_space + 1));
  }
  else {
    return nullptr;
  }
}

RequestHolder DatabaseManager::ParseAddStop(std::string_view request_str) {
  auto request_ptr = Request::Create(Request::Type::ADD_STOP);
  if (request_ptr) {
    request_ptr->ParseFrom(request_str);
  }
  return request_ptr;
}

RequestHolder DatabaseManager::ParseAddRoute(std::string_view request_str) {
  auto request_ptr = Request::Create(Request::Type::ADD_ROUTE);
  if (request_ptr) {
    request_ptr->ParseFrom(request_str);
  }
  return request_ptr;
}

RequestHolder DatabaseManager::ParseReadRequest(std::string_view request_str) {
  char split_by = ' ';
  size_t first_space = request_str.find(split_by);
  std::string_view type = request_str.substr(0, first_space);
  if (type == "Bus") {
    return ParseTakeRoute(request_str.substr(first_space + 1));
  }
  else {
    return nullptr;
  }
}

RequestHolder DatabaseManager::ParseTakeRoute(std::string_view request_str) {
  auto request_ptr = Request::Create(Request::Type::TAKE_ROUTE);
  if (request_ptr) {
    request_ptr->ParseFrom(request_str);
  }
  return request_ptr;
}