//
// Created by ilya on 25.11.2019.
//
#include <sstream>
#include <iomanip>
#include <unordered_map>
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
  default:
    return nullptr;
  }
}

void AddStopRequest::ParseFrom(std::string_view input) {
  size_t delim_name = input.find(':');
  stop_name = Strip(input.substr(0, delim_name));
  size_t delim_comma = input.find(',', delim_name + 2);
  latitude = std::stod(std::string(input.substr(delim_name + 2, delim_comma)));
  longitude = std::stod(std::string(input.substr(delim_comma + 1)));
}

void AddStopRequest::Process(Database &db) const {
  db.AddStop({stop_name,
             CoordinatesBuilder().SetLatitude(latitude).SetLongitude(longitude).Build()});
}

// TODO добавить в парсинг избегание пробелов
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

void AddRouteRequest::Process(Database &db) const {
  db.AddRoute(route_name,
              RouteBuilder(db).MakeRoute({route_type, route_name, stops_name}));
}

void TakeRouteRequest::ParseFrom(std::string_view input) {
  route_name = input;
}

TakeRouteAnswer TakeRouteRequest::Process(const Database &db) const {
  auto route = db.TakeRoute(route_name);
  if (route) {
    return {true, route_name, route->CountOfStops(), route->CountOfUniqueStops(), route->Length()};
  }
  else {
    return {false, route_name, 0, 0, 0};
  }
}

std::string TakeRouteRequest::StringAnswer(const TakeRouteAnswer &result) const {
  std::stringstream s;
  if (result.has_value) {
    s << std::setprecision(6) << "Bus " << result.route_name << ": " << result.stops_count << " stops on route, " <<
      result.unique_stops_count << " unique stops, " << result.length << " route length";
  }
  else {
    s << "Bus " << result.route_name << ": not found";
  }
  return s.str();
}

void TakeStopRequest::ParseFrom(std::string_view input) {
  stop_name = input;
}

TakeStopAnswer TakeStopRequest::Process(const Database &db) const {
  auto stop = db.TakeStop(stop_name);
  if (stop) {
    return {true, stop_name, stop->TakeRoutes()};
  }
  else {
    return {false, stop_name, {}};
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