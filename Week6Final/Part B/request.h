//
// Created by ilya on 25.11.2019.
//

#ifndef YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_REQUEST_H
#define YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_REQUEST_H
#include <string>
#include <optional>
#include "database.h"

std::vector<std::string> SplitBy(std::string_view s, char split = '.');
std::string Strip(std::string_view str);

class Request;
using RequestHolder = std::unique_ptr<Request>;
class Request {
public:
  enum class Type {
    ADD_ROUTE,
    ADD_STOP,
    TAKE_ROUTE,
    TAKE_STOP
  };
  explicit Request(Type type) : type_(type) {}
  virtual void ParseFrom(std::string_view input) = 0;
  virtual ~Request() = default;

  static RequestHolder Create(Request::Type type);

  Type GetType() const {
    return type_;
  }
private:
  Type type_;
};

template <typename Result>
class ReadRequest : public Request {
public:
  using Request::Request;
  virtual Result Process(const Database& db) const = 0;
  void ParseFrom(std::string_view input) override = 0;
  virtual std::string StringAnswer(const Result& result) const = 0;
};

class ModifyRequest : public Request {
public:
  using Request::Request;
  virtual void Process(Database& db) const = 0;
  void ParseFrom(std::string_view input) override = 0;
};

class AddStopRequest : public ModifyRequest {
public:
  AddStopRequest() : ModifyRequest(Request::Type::ADD_STOP) {}
  void ParseFrom(std::string_view input);
  void Process(Database& db) const override;
private:
  std::string stop_name;
  double latitude, longitude;
};

class AddRouteRequest : public ModifyRequest {
public:
  AddRouteRequest() : ModifyRequest(Request::Type::ADD_ROUTE) {}
  void ParseFrom(std::string_view input);
  void Process(Database& db) const override;
private:
  std::string route_name;
  Route::RouteTypes route_type;
  std::vector<std::string> stops_name;
};

struct TakeRouteAnswer {
  bool has_value;
  std::string route_name;
  size_t stops_count;
  size_t unique_stops_count;
  double length;
};

class TakeRouteRequest : public ReadRequest<TakeRouteAnswer> {
public:
  TakeRouteRequest() : ReadRequest(Request::Type::TAKE_ROUTE) {}
  void ParseFrom(std::string_view input);
  TakeRouteAnswer Process(const Database& db) const override;
  std::string StringAnswer(const TakeRouteAnswer& result) const override;
private:
  std::string route_name;
};

struct TakeStopAnswer {
  bool in_base;
  std::string stop_name;
  std::vector<std::string> names;
};

class TakeStopRequest : public ReadRequest<TakeStopAnswer> {
public:
  TakeStopRequest() : ReadRequest(Request::Type::TAKE_STOP) {}
  void ParseFrom(std::string_view input);
  TakeStopAnswer Process(const Database& db) const override;
  std::string StringAnswer(const TakeStopAnswer& result) const override;
private:
  std::string stop_name;
};
#endif //YANDEXYELLOWFINAL_4_BROWN_FINAL_PROJECT_PART_A_REQUEST_H
