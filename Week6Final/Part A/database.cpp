//
// Created by ilya on 25.11.2019.
//

#include "database.h"

#include <utility>

Stop::Stop() {
  name_ = "unnamed";
}

std::string Stop::GetName() const {
  return name_;
}

Coordinates Stop::GetCoord() const {
  return coord_;
}

std::string Route::GetName() const {
  return name_;
}

std::vector<std::string> Route::GetStopsName() const {
  return stops_name_;
}

size_t LinearRoute::CountOfStops() const {
  return stops_.size() * 2 - 1;
}

size_t LinearRoute::CountOfUniqueStops() const {
  return unique_stops_.size();
}

double LinearRoute::Length() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord()) * 2;
  }
  return result;
}

size_t CycleRoute::CountOfStops() const {
  return stops_.size();
}

size_t CycleRoute::CountOfUniqueStops() const {
  return unique_stops_.size();
}

double CycleRoute::Length() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord());
  }
  return result;
}

void Database::AddStop(const Stop &stop) {
  if (stops_.count(stop.GetName())) {
    (*stops_.at(stop.GetName())) = stop;
  }
  else {
    stops_.insert({stop.GetName(), std::make_shared<Stop>(stop)});
  }
}

std::shared_ptr<Stop> Database::TakeStop(const std::string &stop_name) {
  if (!stops_.count(stop_name)) {
    stops_.insert({stop_name, std::make_shared<Stop>(stop_name, Coordinates())});
  }
  return stops_.at(stop_name);
}

void Database::AddRoute(const std::string& route_name, std::shared_ptr<Route> route) {
  routes_[route_name] = std::move(route);
}

std::shared_ptr<Route> Database::TakeRoute(const std::string &route_name) const {
  if (routes_.count(route_name)) {
    return routes_.at(route_name);
  }
  else {
    return nullptr;
  }
}

std::shared_ptr<Route> RouteBuilder::MakeRoute(RouteInfo&& info) {
  switch (info.type) {
  case Route::RouteTypes::LINEAR:
    return MakeLinear(std::move(info));
  case Route::RouteTypes::CYCLE:
    return MakeCycle(std::move(info));
  default:
    return nullptr;
  }
}

std::shared_ptr<Route> RouteBuilder::MakeCycle(RouteInfo&& info) {
  std::vector<std::shared_ptr<Stop>> stops_ptr;
  stops_ptr.reserve(info.stop_names.size());
  for(auto& str : info.stop_names) {
    stops_ptr.push_back(db_.TakeStop(str));
  }
  return std::make_shared<CycleRoute>(std::move(info.name), std::move(info.stop_names), std::move(stops_ptr));
}

std::shared_ptr<Route> RouteBuilder::MakeLinear(RouteInfo&& info) {
  std::vector<std::shared_ptr<Stop>> stops_ptr;
  stops_ptr.reserve(info.stop_names.size());
  for(auto& str : info.stop_names) {
    stops_ptr.push_back(db_.TakeStop(str));
  }
  return std::make_shared<LinearRoute>(std::move(info.name), std::move(info.stop_names), std::move(stops_ptr));
}

