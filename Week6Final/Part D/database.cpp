//
// Created by ilya on 25.11.2019.
//

#include "database.h"

#include <utility>
#include <cassert>

Stop::Stop() {
  name_ = "unnamed";
}
Stop::Stop(std::string  name, const Coordinates& coord) : name_(std::move(name)), coord_(coord) {}

Stop::Stop(std::string  name, const Coordinates& coord, const std::vector<std::pair<std::string, int>>& distances) : name_(std::move(name)), coord_(coord) {
  for (const auto& [key, value] : distances) {
    distance_to_stop[key] = value;
  }
}

Stop::Stop(const Stop &other) {
  name_ = other.name_;
  coord_ = other.coord_;
  distance_to_stop = other.distance_to_stop;
}

Stop::Stop(Stop &&other) noexcept {
  name_ = std::move(other.name_);
  coord_ = other.coord_;
  distance_to_stop = std::move(other.distance_to_stop);
}

Stop& Stop::operator=(const Stop &other) {
  name_ = other.name_;
  coord_ = other.coord_;
  distance_to_stop = other.distance_to_stop;
  return *this;
}

Stop& Stop::operator=(Stop &&other) noexcept {
  name_ = std::move(other.name_);
  coord_ = other.coord_;
  distance_to_stop = std::move(other.distance_to_stop);
  return *this;
}

std::string Stop::GetName() const {
  return name_;
}

Coordinates Stop::GetCoord() const {
  return coord_;
}

void Stop::AddRoute(const std::string &route_name) {
  routes_for_stop.insert(route_name);
}

std::vector<std::string> Stop::TakeRoutes() const {
  return {routes_for_stop.begin(), routes_for_stop.end()};
}

double Route::Curvature() const {
  return RealLength() / Length();
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

/*
Код функции из части С, который прошел тесты
double LinearRoute::RealLength() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    // Вычисление расстояние от А к Б
    bool is_same_length = true;
    if(stops_[i]->distance_to_stop.count(stops_[i + 1]->GetName())) {
      result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName());
      is_same_length = true;
    }
    else {
      result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord());
      is_same_length = false;
    }

    // Вычисление расстояния от Б к А
    if(stops_[i + 1]->distance_to_stop.count(stops_[i]->GetName())) {
      result += stops_[i + 1]->distance_to_stop.at(stops_[i]->GetName());
    }
    else if (is_same_length) {
      result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName());
    }
    else {
      result += Coordinates::Distance(stops_[i + 1]->GetCoord(), stops_[i]->GetCoord());
    }
  }
  return result;
}
*/

double LinearRoute::RealLength() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    // Вычисление расстояние от А к Б
    if(stops_[i]->distance_to_stop.count(stops_[i + 1]->GetName())) {
      result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName());
    }
    else if (stops_[i + 1]->distance_to_stop.count(stops_[i]->GetName())) {
      result += stops_[i + 1]->distance_to_stop.at(stops_[i]->GetName());
    }
    else {
      result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord());
    }

    // Вычисление расстояния от Б к А
    if (stops_[i + 1]->distance_to_stop.count(stops_[i]->GetName())) {
      result += stops_[i + 1]->distance_to_stop.at(stops_[i]->GetName());
    }
    else if(stops_[i]->distance_to_stop.count(stops_[i + 1]->GetName())) {
      result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName());
    }
    else {
      result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord());
    }
  }
  return result;
}

double LinearRoute::Length() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord()) + Coordinates::Distance(stops_[i + 1]->GetCoord(), stops_[i]->GetCoord());
  }
  return result;
}

size_t CycleRoute::CountOfStops() const {
  return stops_.size();
}

size_t CycleRoute::CountOfUniqueStops() const {
  return unique_stops_.size();
}

double CycleRoute::RealLength() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    if(stops_[i]->distance_to_stop.count(stops_[i + 1]->GetName())) {
      result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName());
    }
    else {
      result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord());
    }
  }
  return result;
}

double CycleRoute::Length() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    result += Coordinates::Distance(stops_[i]->GetCoord(), stops_[i + 1]->GetCoord());
  }
  return result;
}

void Database::AddStop(const Stop &stop) {
  std::string stop_name = stop.GetName();
  /*if (stops_.count(stop_name)) {
    (*stops_.at(stop_name)) = stop;
  }
  else {
    stops_.insert({stop.GetName(), std::make_shared<Stop>(stop)});
  }*/
  auto iter = stops_.try_emplace(stop.GetName(), std::make_shared<Stop>(stop));
  if (!iter.second) {
    *(iter.first->second) = stop;
  }
}

std::shared_ptr<Stop> Database::TakeOrAddStop(const std::string &stop_name) {
  if (!stops_.count(stop_name)) {
    stops_.insert({stop_name, std::make_shared<Stop>(stop_name, Coordinates())});
  }
  return stops_.at(stop_name);
}

std::shared_ptr<Stop> Database::TakeStop(const std::string &stop_name) const {
  if (!stops_.count(stop_name)) {
    return nullptr;
  }
  return stops_.at(stop_name);
}

void Database::AddRoute(const std::string& route_name, std::shared_ptr<Route> route) {
  for (const auto& stop_name : route->GetStopsName()) {
    stops_[stop_name]->AddRoute(route_name);
  }
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
    auto stop = db_.TakeOrAddStop(str);
    //stop->AddRoute(info.name);
    stops_ptr.push_back(std::move(stop));
  }
  return std::make_shared<CycleRoute>(std::move(info.name), std::move(info.stop_names), std::move(stops_ptr));
}

std::shared_ptr<Route> RouteBuilder::MakeLinear(RouteInfo&& info) {
  std::vector<std::shared_ptr<Stop>> stops_ptr;
  stops_ptr.reserve(info.stop_names.size());
  for(auto& str : info.stop_names) {
    auto stop = db_.TakeOrAddStop(str);
    //stop->AddRoute(info.name);
    stops_ptr.push_back(std::move(stop));
  }
  return std::make_shared<LinearRoute>(std::move(info.name), std::move(info.stop_names), std::move(stops_ptr));
}
