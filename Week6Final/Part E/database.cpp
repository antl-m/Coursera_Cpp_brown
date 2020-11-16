//
// Created by ilya on 25.11.2019.
//

#include "database.h"

#include <utility>

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
  for (const auto& [stop_name, distance] : other.distance_to_stop) {
    distance_to_stop[stop_name] = distance;
  }
}

Stop::Stop(Stop &&other) noexcept {
  name_ = std::move(other.name_);
  coord_ = other.coord_;
  for (auto& [stop_name, distance] : other.distance_to_stop) {
    distance_to_stop[std::move(stop_name)] = std::move(distance);
  }
}

Stop& Stop::operator=(const Stop &other) {
  name_ = other.name_;
  coord_ = other.coord_;
  for (const auto& [stop_name, distance] : other.distance_to_stop) {
    distance_to_stop[stop_name] = distance;
  }
  return *this;
}

Stop& Stop::operator=(Stop &&other) noexcept {
  name_ = std::move(other.name_);
  coord_ = other.coord_;
  for (auto& [stop_name, distance] : other.distance_to_stop) {
    distance_to_stop[std::move(stop_name)] = std::move(distance);
  }
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

double LinearRoute::RealLength() const {
  double result = 0;
  for (size_t i = 0; i < stops_.size() - 1; ++i) {
    result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName()) +
              stops_[i + 1]->distance_to_stop.at(stops_[i]->GetName());
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
    result += stops_[i]->distance_to_stop.at(stops_[i + 1]->GetName());
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
  gcache.is_outdated = true;
  auto inserted = stops_.try_emplace(stop.GetName(), std::make_shared<Stop>(stop));
  if (!inserted.second) {
    *(inserted.first->second) = stop;
  }
  for (const auto& [stop_name, distance] : stop.distance_to_stop) {
    auto curr_stop = TakeOrAddStop(stop_name);
    if (!curr_stop->distance_to_stop.count(stop.GetName())) {
      curr_stop->distance_to_stop[stop.GetName()] = distance;
    }
  }
}

Database::Database() {
  params_ = {1, 1};
}

std::shared_ptr<Stop> Database::TakeOrAddStop(const std::string &stop_name) {
  gcache.is_outdated = true;
  if (!stops_.count(stop_name)) {
    auto inserted = stops_.insert({stop_name, std::make_shared<Stop>(stop_name, Coordinates())});
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
  gcache.is_outdated = true;
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
    stops_ptr.push_back(std::move(stop));
  }
  return std::make_shared<CycleRoute>(std::move(info.name), std::move(info.stop_names), std::move(stops_ptr));
}

std::shared_ptr<Route> RouteBuilder::MakeLinear(RouteInfo&& info) {
  std::vector<std::shared_ptr<Stop>> stops_ptr;
  stops_ptr.reserve(info.stop_names.size());
  for(auto& str : info.stop_names) {
    auto stop = db_.TakeOrAddStop(str);
    stops_ptr.push_back(std::move(stop));
  }
  return std::make_shared<LinearRoute>(std::move(info.name), std::move(info.stop_names), std::move(stops_ptr));
}

std::list<std::unique_ptr<BaseNode>> Database::CreateRoute(const std::string& first_stop,
                                                           const std::string& second_stop) const {
  if (gcache.is_outdated) {
    gcache.wgraph = UpdateGraph();
    gcache.router = std::make_unique<Graph::Router<WeightType>>(gcache.wgraph);
    gcache.is_outdated = false;
  }
  auto route = gcache.router->BuildRoute(gcache.name_to_vertex_id.at(first_stop).first,
                                     gcache.name_to_vertex_id.at(second_stop).first);
  std::list<std::unique_ptr<BaseNode>> nodes;
  if (!route) {
    nodes.push_back(std::make_unique<InfoNode>());
    return nodes;
  }
  nodes.push_back(std::make_unique<InfoNode>(route->weight));
  for (size_t i = 0; i < route->edge_count; ++i) {
    Graph::EdgeId edge_id = gcache.router->GetRouteEdge(route->id, i);
    Edge edge = gcache.edges.at(edge_id);
    if (edge.is_wait_edge) {
      nodes.push_back(std::make_unique<WaitNode>(gcache.vertex_id_to_name.at(edge.from).first,
                                                    edge.weight));
    }
    else {
      nodes.push_back(std::make_unique<BusNode>(edge.bus_name, edge.span_count, edge.weight));
    }
  }
  return nodes;
}

Graph::DirectedWeightedGraph<Database::WeightType> Database::UpdateGraph() const {
  AddStops();
  auto new_wgraph = Graph::DirectedWeightedGraph<WeightType>(gcache.name_to_vertex_id.size() * 2);
  for (const auto& [stop_name, stop_ptr] : stops_) {
    auto from = gcache.name_to_vertex_id.at(stop_name).first;
    auto to = gcache.name_to_vertex_id.at(stop_name).second;
    auto edge = new_wgraph.AddEdge({from,
                               to,
                               params_.waiting_time});
    gcache.edges[edge] = {true, params_.waiting_time, 0, "no", from, to};
  }
  for (const auto& [route_name, route_ptr] : routes_) {
    if (route_ptr->route_type == Route::RouteTypes::CYCLE) {
      MakeWieghtFromCycleRoute(new_wgraph, route_ptr);
    }
    if (route_ptr->route_type == Route::RouteTypes::LINEAR) {
      MakeWieghtFromLinearRoute(new_wgraph, route_ptr);
    }
  }
  return new_wgraph;
}

void Database::MakeWieghtFromCycleRoute(Graph::DirectedWeightedGraph<WeightType> &graph,
                                        const std::shared_ptr<Route> &ptr) const {
  auto stop_names = ptr->GetStopsName();
  for (size_t i = 0; i < stop_names.size() - 1; ++i) {
    double accumulate_time = 0;
    int span_count = 0;
    for (size_t j = i + 1; j < stop_names.size(); ++j) {
      accumulate_time += stops_.at(stop_names[j - 1])->distance_to_stop.at(stop_names[j]) * 1.0 / params_.velocity;
      span_count++;
      auto from = gcache.name_to_vertex_id.at(stop_names[i]).second;
      auto to = gcache.name_to_vertex_id.at(stop_names[j]).first;
      auto edge = graph.AddEdge({from,
                     to,
                     accumulate_time});
      gcache.edges[edge] = {false, accumulate_time, span_count, ptr->GetName(), from, to};
    }
  }
}

void Database::MakeWieghtFromLinearRoute(Graph::DirectedWeightedGraph<WeightType> &graph,
                                         const std::shared_ptr<Route> &ptr) const {
  auto stop_names = ptr->GetStopsName();
  for (int64_t i = 0; i < stop_names.size(); ++i) {
    double accumulate_time_str = 0;
    int span_count_str = 0;
    double accumulate_time_rev = 0;
    int span_count_rev = 0;
    for (int64_t j = i - 1; j >= 0; --j) {
      accumulate_time_rev += stops_.at(stop_names[j + 1])->distance_to_stop.at(stop_names[j]) * 1.0 / params_.velocity;
      span_count_rev++;
      auto from = gcache.name_to_vertex_id.at(stop_names[i]).second;
      auto to = gcache.name_to_vertex_id.at(stop_names[j]).first;
      auto edge = graph.AddEdge({from,
                                 to,
                                 accumulate_time_rev});
      gcache.edges[edge] = {false, accumulate_time_rev, span_count_rev, ptr->GetName(), from, to};
    }
    for (int64_t j = i + 1; j < stop_names.size(); ++j) {
      accumulate_time_str += stops_.at(stop_names[j - 1])->distance_to_stop.at(stop_names[j]) * 1.0 / params_.velocity;
      span_count_str++;
      auto from = gcache.name_to_vertex_id.at(stop_names[i]).second;
      auto to = gcache.name_to_vertex_id.at(stop_names[j]).first;
      auto edge = graph.AddEdge({from,
                                 to,
                                 accumulate_time_str});
      gcache.edges[edge] = {false, accumulate_time_str, span_count_str, ptr->GetName(), from, to};
    }
  }
}

void Database::AddStops() const {
  for (const auto& [stop_name, stop_ptr] : stops_) {
    gcache.name_to_vertex_id[stop_name] = {gcache.curr_id, gcache.curr_id + 1};
    gcache.vertex_id_to_name[gcache.curr_id] = {stop_name, "start"};
    gcache.vertex_id_to_name[gcache.curr_id + 1] = {stop_name, "end"};
    gcache.curr_id += 2;
  }
}