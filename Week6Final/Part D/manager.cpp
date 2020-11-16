//
// Created by ilya on 28.11.2019.
//

#include <sstream>
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
  }

  const size_t COUNT_OF_READ = ReadNumberOnLine<int>(in);
  results.reserve(COUNT_OF_READ);
  for (size_t i = 0; i < COUNT_OF_READ; ++i) {
    std::string raw_request;
    std::getline(in, raw_request);
    auto answer = ProcessReadRequest(raw_request);
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
  case Request::Type::TAKE_STOP: {
    const auto& cast_request = dynamic_cast<ReadRequest<TakeStopAnswer>&>(*request);
    const auto result = cast_request.Process(db_);
    return cast_request.StringAnswer(result);
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
  if (type == "Stop") {
    return ParseTakeStop(request_str.substr(first_space + 1));
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

RequestHolder DatabaseManager::ParseTakeStop(std::string_view request_str) {
  auto request_ptr = Request::Create(Request::Type::TAKE_STOP);
  if (request_ptr) {
    request_ptr->ParseFrom(request_str);
  }
  return request_ptr;
}

Json::Node DatabaseManager::ProcessAllJSONRequests(std::istream &in) {
  auto doc = Json::Load(in);
  auto global_type_map = doc.GetRoot().AsMap();

  const std::string modify_type = "base_requests";
  Json::Node modify_requests = global_type_map.at(modify_type);

  const std::string read_type = "stat_requests";
  Json::Node read_requests = global_type_map.at(read_type);

  for (const auto& node : modify_requests.AsArray()) {
    ProcessModifyRequestJSON(node);
  }

  std::vector<Json::Node> result;
  for (const auto& node : read_requests.AsArray()) {
    result.push_back(ProcessReadRequestJSON(node));
  }
  return Json::Node(result);
}

Json::Node DatabaseManager::ProcessModifyRequestJSON(const Json::Node &node) {
  return MakeJSONAnswerFromAnyRequest(ParseModifyJSONRequest(node));
}

Json::Node DatabaseManager::ProcessReadRequestJSON(const Json::Node &node) {
  return MakeJSONAnswerFromAnyRequest(ParseReadJSONRequest(node));
}

Json::Node DatabaseManager::MakeJSONAnswerFromAnyRequest(RequestHolder request) {
  switch (request->GetType()) {
  case Request::Type::TAKE_ROUTE: {
    const auto& cast_request = dynamic_cast<ReadRequest<TakeRouteAnswer>&>(*request);
    const auto result = cast_request.Process(db_);
    return cast_request.JSONAnswer(result);
  }
  case Request::Type::ADD_STOP: {
    const auto& cast_request = dynamic_cast<ModifyRequest&>(*request);
    cast_request.Process(db_);
    return Json::Node("Stop added");
  }
  case Request::Type::ADD_ROUTE: {
    const auto& cast_request = dynamic_cast<ModifyRequest&>(*request);
    cast_request.Process(db_);
    return Json::Node("Route added");
  }
  case Request::Type::TAKE_STOP: {
    const auto& cast_request = dynamic_cast<ReadRequest<TakeStopAnswer>&>(*request);
    const auto result = cast_request.Process(db_);
    return cast_request.JSONAnswer(result);
  }
  default:
    return Json::Node("error");
  }
}

RequestHolder DatabaseManager::ParseModifyJSONRequest(const Json::Node &node) {
  auto type = node.AsMap().at("type").AsString();
  if (type == "Stop") {
    return JSONAddStop(node);
  }
  else if (type == "Bus") {
    return JSONAddRoute(node);
  }
  else {
    return nullptr;
  }
}

RequestHolder DatabaseManager::JSONAddStop(const Json::Node &node) {
  auto request_ptr = Request::Create(Request::Type::ADD_STOP);
  if (request_ptr) {
    request_ptr->ParseFromJSON(node);
  }
  return request_ptr;
}

RequestHolder DatabaseManager::JSONAddRoute(const Json::Node &node) {
  auto request_ptr = Request::Create(Request::Type::ADD_ROUTE);
  if (request_ptr) {
    request_ptr->ParseFromJSON(node);
  }
  return request_ptr;
}

RequestHolder DatabaseManager::ParseReadJSONRequest(const Json::Node &node) {
  auto type = node.AsMap().at("type").AsString();
  if (type == "Stop") {
    return JSONTakeStop(node);
  }
  else if (type == "Bus") {
    return JSONTakeRoute(node);
  }
  else {
    return nullptr;
  }
}

RequestHolder DatabaseManager::JSONTakeRoute(const Json::Node &node) {
  auto request_ptr = Request::Create(Request::Type::TAKE_ROUTE);
  if (request_ptr) {
    request_ptr->ParseFromJSON(node);
  }
  return request_ptr;
}

RequestHolder DatabaseManager::JSONTakeStop(const Json::Node &node) {
  auto request_ptr = Request::Create(Request::Type::TAKE_STOP);
  if (request_ptr) {
    request_ptr->ParseFromJSON(node);
  }
  return request_ptr;
}