//
// Created by ilya on 25.11.2019.
//
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <map>
#include <cassert>
#include "json.h"
#include "request.h"

std::vector<std::string> SplitBy(std::string_view s, char split) {
  std::vector<std::string> result;
  auto curr = 0;
  while (curr != s.npos) {
    auto space = s.find(split, curr);
    result.emplace_back(s.substr(curr, space - curr));
    if (space != s.npos) curr = (space + 1);
    else curr = s.npos;
  }
  return result;
}


std::string Strip(std::string_view str) {
  size_t left_border = 0;
  while (str[left_border] == ' ') {
    left_border++;
  }
  size_t right_border = str.size() - 1;
  while (str[right_border] == ' ') {
    right_border--;
  }
  return std::string(str.substr(left_border, right_border - left_border + 1));
}

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
  case Request::Type::ADD_ROUTE:
    return std::make_unique<AddRouteRequest>();
  case Request::Type::ADD_STOP:
    return std::make_unique<AddStopRequest>();
  case Request::Type::TAKE_ROUTE:
    return std::make_unique<TakeRouteRequest>();
  case Request::Type::TAKE_STOP:
    return std::make_unique<TakeStopRequest>();
  case Request::Type::CREATE_ROUTE:
    return std::make_unique<CreateRouteRequest>();
  default:
    return nullptr;
  }
}

void AddStopRequest::ParseFrom(std::string_view input) {
  size_t delim_name = input.find(':');
  stop_name = Strip(input.substr(0, delim_name));
  auto tokens = SplitBy(input.substr(delim_name + 2), ',');
  latitude = std::stod(Strip(tokens[0]));
  longitude = std::stod(Strip(tokens[1]));
  for (size_t i = 2; i < tokens.size(); ++i) {
    size_t m_pos = tokens[i].find('m');
    double distance = std::stod(Strip(tokens[i].substr(0, m_pos)));
    auto tail = tokens[i].substr(m_pos);
    size_t to_pos = tail.find('o');
    std::string name = Strip(tail.substr(to_pos + 1));
    distances.emplace_back(name, distance);
  }
}

void AddStopRequest::ParseFromJSON(const Json::Node &node) {
  const auto& map_elem = node.AsMap();
  stop_name = map_elem.at("name").AsString();
  latitude = map_elem.at("latitude").AsDouble();
  longitude = map_elem.at("longitude").AsDouble();
  if (map_elem.count("road_distances")) {
    const auto& distance_map = map_elem.at("road_distances").AsMap();
    for (const auto& [key, value] : distance_map) {
      std::string name = key;
      int distance = value.AsInt();
      distances.emplace_back(name, distance);
    }
  }
}

void AddStopRequest::Process(Database &db ) const {
  db.AddStop({stop_name,
             CoordinatesBuilder().SetLatitude(latitude).SetLongitude(longitude).Build(), distances});
}

void AddRouteRequest::ParseFrom(std::string_view input) {
  size_t delim_name = input.find(':');
  route_name = Strip(input.substr(0, delim_name));
  std::vector<char> types = {'-', '>'};
  char delim_type = '-';
  for (auto type : types) {
    size_t delim_pos = input.find(type);
    if (delim_pos != input.npos) {
      delim_type = type;
      route_type = sign_to_route.at(delim_type);
      break;
    }
  }
  auto tokens = SplitBy(input.substr(delim_name + 2), delim_type);
  stops_name.reserve(tokens.size());
  for (auto& token : tokens) {
    std::string stop;
    if (token.front() == ' ') {
      if (token.back() == ' ') {
        stop = Strip(token.substr(1, token.size() - 2));
      }
      else {
        stop = Strip(token.substr(1,token.size() - 1));
      }
    }
    else {
      stop = Strip(token.substr(0,token.size() - 1));
    }
    stops_name.push_back(stop);
  }
}

void AddRouteRequest::ParseFromJSON(const Json::Node &node) {
  const auto& map_elem = node.AsMap();
  route_name = map_elem.at("name").AsString();
  route_type = map_elem.at("is_roundtrip").AsBool() ? Route::RouteTypes::CYCLE : Route::RouteTypes::LINEAR;
  if (map_elem.count("stops")) {
    const auto& stops = map_elem.at("stops").AsArray();
    for (const auto& stop : stops) {
      stops_name.push_back(stop.AsString());
    }
  }
}

void AddRouteRequest::Process(Database &db) const {
  db.AddRoute(route_name,
              RouteBuilder(db).MakeRoute({route_type, route_name, stops_name}));
}

void TakeRouteRequest::ParseFrom(std::string_view input) {
  route_name = input;
}

void TakeRouteRequest::ParseFromJSON(const Json::Node &node) {
  const auto& map_elem = node.AsMap();
  route_name = map_elem.at("name").AsString();
  request_id = map_elem.at("id").AsInt();
}

TakeRouteAnswer TakeRouteRequest::Process(const Database &db) const {
  auto route = db.TakeRoute(route_name);
  if (route) {
    return {request_id, true, route_name, route->CountOfStops(), route->CountOfUniqueStops(), route->RealLength(), route->Curvature()};
  }
  else {
    return {request_id, false, route_name, 0, 0, 0, 0};
  }
}

std::string TakeRouteRequest::StringAnswer(const TakeRouteAnswer &result) const {
  std::stringstream s;
  if (result.has_value) {
    s << std::setprecision(6) << "Bus " << result.route_name << ": " << result.stops_count << " stops on route, " <<
      result.unique_stops_count << " unique stops, " << result.length << " route length, " << result.curvature << " curvature";
  }
  else {
    s << "Bus " << result.route_name << ": not found";
  }
  return s.str();
}

Json::Node TakeRouteRequest::JSONAnswer(const TakeRouteAnswer &result) const {
  std::map<std::string, Json::Node> answer;
  answer["request_id"] = Json::Node(result.id);
  if (result.has_value) {
    answer["stop_count"] = Json::Node(static_cast<int>(result.stops_count));
    answer["unique_stop_count"] = Json::Node(static_cast<int>(result.unique_stops_count));
    answer["route_length"] = Json::Node(result.length);
    answer["curvature"] = Json::Node(result.curvature);
  }
  else {
    answer["error_message"] = Json::Node(std::string("not found"));
  }
  return Json::Node(answer);
}

void TakeStopRequest::ParseFrom(std::string_view input) {
  stop_name = input;
}

void TakeStopRequest::ParseFromJSON(const Json::Node &node) {
  const auto& map_elem = node.AsMap();
  stop_name = map_elem.at("name").AsString();
  request_id = map_elem.at("id").AsInt();
}

TakeStopAnswer TakeStopRequest::Process(const Database &db) const {
  auto stop = db.TakeStop(stop_name);
  if (stop) {
    return {request_id, true, stop_name, stop->TakeRoutes()};
  }
  else {
    return {request_id, false, stop_name, {}};
  }
}

std::string TakeStopRequest::StringAnswer(const TakeStopAnswer &result) const {
  std::stringstream s;
  if (result.in_base) {
    if (result.names.empty()) {
      s << "Stop " << result.stop_name << ": no buses";
    }
    else {
      s << "Stop " << result.stop_name << ": buses";
      for (const auto& route : result.names) {
        s << " " << route;
      }
    }
  }
  else {
    s << "Stop " << result.stop_name << ": not found";
  }
  return s.str();
}

Json::Node TakeStopRequest::JSONAnswer(const TakeStopAnswer &result) const {
  std::map<std::string, Json::Node> answer;
  answer["request_id"] = Json::Node(result.id);
  if (result.in_base) {
    if (result.names.empty()) {
      answer["buses"] = Json::Node(std::vector<Json::Node>());
    }
    else {
      std::vector<Json::Node> bufer;
      bufer.resize(result.names.size());
      for (size_t i = 0; i < result.names.size(); ++i) {
        bufer[i] = Json::Node(std::string(result.names[i]));
      }
      answer["buses"] = std::move(Json::Node(bufer));
    }
  }
  else {
    answer["error_message"] = Json::Node(std::string("not found"));
  }
  return Json::Node(answer);
}

void CreateRouteRequest::ParseFromJSON(const Json::Node &node) {
  const auto& map_elem = node.AsMap();
  from = map_elem.at("from").AsString();
  to = map_elem.at("to").AsString();
  request_id = map_elem.at("id").AsInt();
}

void CreateRouteRequest::ParseFrom(std::string_view input) {}

CreateRouteAnswer CreateRouteRequest::Process(const Database &db) const {
  auto nodes = db.CreateRoute(from, to);
  auto first = dynamic_cast<InfoNode&>(*nodes.front());
  nodes.pop_front();
  if (!first.total_time) {
    return {request_id, false, {}};
  }
  return {request_id, true, first.total_time.value(), std::move(nodes)};
}

std::string CreateRouteRequest::StringAnswer(const CreateRouteAnswer &result) const {}

Json::Node CreateRouteRequest::JSONAnswer(const CreateRouteAnswer &result) const {
  std::map<std::string, Json::Node> answer;
  answer["request_id"] = Json::Node(result.id);
  if (!result.has_route) {
    answer["error_message"] = Json::Node(std::string("not found"));
    return answer;
  }
  answer["total_time"] = Json::Node(result.total_time);
  std::vector<Json::Node> items;
  for (const auto& node : result.nodes) {
    if (node->type == NodeType::WAIT) {
      auto node_wait = dynamic_cast<WaitNode&>(*node);
      std::map<std::string, Json::Node> wait_ans;
      wait_ans["type"] = Json::Node(std::string("Wait"));
      wait_ans["stop_name"] = Json::Node(node_wait.stop_name);
      wait_ans["time"] = Json::Node(node_wait.time);
      items.emplace_back(wait_ans);
    }
    if (node->type == NodeType::BUS) {
      auto node_bus = dynamic_cast<BusNode&>(*node);
      std::map<std::string, Json::Node> bus_ans;
      bus_ans["type"] = Json::Node(std::string("Bus"));
      bus_ans["bus"] = Json::Node(node_bus.route_name);
      bus_ans["span_count"] = Json::Node(node_bus.span_count);
      bus_ans["time"] = Json::Node(node_bus.time);
      items.emplace_back(bus_ans);
    }
  }
  answer["items"] = Json::Node(items);
  return answer;
}