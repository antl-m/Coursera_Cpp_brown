//
// Created by ilya on 25.11.2019.
//
#ifndef YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_DATABASE_H
#define YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_DATABASE_H
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <memory>
#include <list>

#include "graph.h"
#include "router.h"
#include "json.h"
#include "coordinates.h"

class Stop {
public:

  Stop();
  Stop(std::string  name, const Coordinates& coord);
  Stop(std::string  name, const Coordinates& coord, const std::vector<std::pair<std::string, int>>& distances);
  Stop(const Stop& other);
  Stop(Stop&& other) noexcept;
  Stop& operator=(const Stop& other);
  Stop& operator=(Stop&& other) noexcept;

  std::string GetName() const;
  Coordinates GetCoord() const;

  void AddRoute(const std::string& route_name);
  std::vector<std::string> TakeRoutes() const;

  std::unordered_map<std::string, int> distance_to_stop;
private:
  std::string name_;
  Coordinates coord_;
  std::set<std::string> routes_for_stop;
};

class Route {
public:
  enum class RouteTypes {LINEAR, CYCLE};
  explicit Route(const std::string& name, const std::vector<std::string>& stops_name, RouteTypes type) : name_(name), stops_name_(stops_name), route_type(type) {}
  virtual ~Route() = default;
  virtual size_t CountOfStops() const = 0;
  virtual size_t CountOfUniqueStops() const = 0;
  virtual double RealLength() const = 0;
  virtual double Length() const = 0;
  double Curvature() const;

  std::string GetName() const;
  std::vector<std::string> GetStopsName() const;

  RouteTypes route_type;
protected:
  std::string name_;
  std::vector<std::string> stops_name_;
};

static const std::unordered_map<char, Route::RouteTypes> sign_to_route = {{'-', Route::RouteTypes::LINEAR},
                                                                          {'>', Route::RouteTypes::CYCLE}};

class LinearRoute : public Route {
public:
  LinearRoute() = default;
  explicit LinearRoute(const std::string& name, const std::vector<std::string>& stops_name,
                       const std::vector<std::shared_ptr<Stop>>& stops) : Route(name, stops_name, RouteTypes::LINEAR), stops_(stops) {
    for (const auto& elem : stops_name_) {
      unique_stops_.insert(elem);
    }
  }
  size_t CountOfStops() const override;
  size_t CountOfUniqueStops() const override;
  double RealLength() const override;
  double Length() const override;
private:
  // Порядок маршрутов с данными о остановках
  std::vector<std::shared_ptr<Stop>> stops_;
  // Хранение уникальных остановок
  std::unordered_set<std::string_view> unique_stops_;
};

class CycleRoute : public Route {
public:
  CycleRoute() = default;
  explicit CycleRoute(const std::string& name, const std::vector<std::string>& stops_name,
                      const std::vector<std::shared_ptr<Stop>>& stops) : Route(name, stops_name, RouteTypes::CYCLE), stops_(stops) {
    for (const auto& elem : stops_name_) {
      unique_stops_.insert(elem);
    }
  }
  size_t CountOfStops() const override;
  size_t CountOfUniqueStops() const override;
  double RealLength() const override;
  double Length() const override;
private:
  // Порядок маршрутов с данными о остановках
  std::vector<std::shared_ptr<Stop>> stops_;
  // Хранение уникальных остановок
  std::unordered_set<std::string> unique_stops_;
};

struct RouteInfo {
  Route::RouteTypes type;
  std::string name;
  std::vector<std::string> stop_names;
};

enum class NodeType {INFO, WAIT, BUS};

struct BaseNode {
  BaseNode() = default;
  BaseNode(NodeType t) : type(t) {}
  virtual ~BaseNode() = default;
  NodeType type;
};

struct InfoNode : public BaseNode {
  using BaseNode::BaseNode;
  InfoNode() = default;
  InfoNode(double val) : BaseNode(NodeType::INFO), total_time(val) {}
  std::optional<double> total_time;
};

struct WaitNode : public BaseNode {
  using BaseNode::BaseNode;
  WaitNode() = default;
  WaitNode(const std::string& stop_name_, double time_)
  : BaseNode(NodeType::WAIT), stop_name(stop_name_), time(time_) {}
  std::string stop_name;
  double time;
};

struct BusNode : public BaseNode {
  using BaseNode::BaseNode;
  BusNode() = default;
  BusNode(const std::string& route_name_, int span_count_, double time_)
  : BaseNode(NodeType::BUS), route_name(route_name_), span_count(span_count_), time(time_) {}
  std::string route_name;
  int span_count;
  double time;
};

class Database {
  using StopData = std::unordered_map<std::string, std::shared_ptr<Stop>>;
  using RouteData = std::unordered_map<std::string, std::shared_ptr<Route>>;
  using WeightType = double;
  struct RoutingParam {
    double velocity;
    double waiting_time;
  };
public:
  Database();
  void AddStop(const Stop &stop);
  std::shared_ptr<Stop> TakeOrAddStop(const std::string &stop_name);
  std::shared_ptr<Stop> TakeStop(const std::string &stop_name) const;

  void AddRoute(const std::string& route_name, std::shared_ptr<Route> route);
  std::shared_ptr<Route> TakeRoute(const std::string &route_name) const;

  std::list<std::unique_ptr<BaseNode>> CreateRoute(const std::string &first_stop, const std::string &second_stop) const;

  RoutingParam params_;
private:
  Graph::DirectedWeightedGraph<WeightType> UpdateGraph() const;
  void MakeWieghtFromCycleRoute(Graph::DirectedWeightedGraph<WeightType>& graph, const std::shared_ptr<Route>& ptr) const;
  void MakeWieghtFromLinearRoute(Graph::DirectedWeightedGraph<WeightType>& graph, const std::shared_ptr<Route>& ptr) const;
  void AddStops() const;

  StopData stops_;
  RouteData routes_;
  template <typename Weight>
  struct Edge {
    bool is_wait_edge;
    double weight;
    int span_count;
    std::string bus_name;
    Graph::VertexId from;
    Graph::VertexId to;
  };
  template <typename Weight>
  struct GraphCache {
    bool is_outdated = true;
    Graph::DirectedWeightedGraph<Weight> wgraph;
    std::unique_ptr<Graph::Router<Weight>> router = nullptr;
    Graph::VertexId curr_id = 0;
    std::unordered_map<std::string, std::pair<Graph::VertexId, Graph::VertexId>> name_to_vertex_id;
    std::unordered_map<Graph::VertexId, std::pair<std::string, std::string>> vertex_id_to_name;
    std::unordered_map<Graph::EdgeId, Edge<Weight>> edges;
  };
  mutable GraphCache<WeightType> gcache;
};

class RouteBuilder {
public:
  RouteBuilder() = delete;
  explicit RouteBuilder(Database& db) : db_(db) {}
  std::shared_ptr<Route> MakeRoute(RouteInfo&& info);
private:
  Database& db_;
  // Для каждого типа маршрута своя логика создания, даже если она повторяется
  // Все что их объединяет - наличие вектора имен для остановок и имя маршрута
  std::shared_ptr<Route> MakeCycle(RouteInfo&& info);
  std::shared_ptr<Route> MakeLinear(RouteInfo&& info);
};

#endif //YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_DATABASE_H
