//
// Created by ilya on 28.11.2019.
//
#include <gtest/gtest.h>
#include "request.h"

TEST(TestModifyRequest, TestAddStopRequest1) {
  auto req = std::make_unique<AddStopRequest>();
  ASSERT_EQ(req->GetType(), Request::Type::ADD_STOP);
  std::string request{"Stop1: 1, 2"};
  req->ParseFrom(request);
  Database db;
  req->Process(db);
  auto stop = db.TakeStop("Stop1");
  ASSERT_EQ(stop->GetName(), "Stop1");
  ASSERT_DOUBLE_EQ(stop->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(stop->GetCoord().GetLongitude(), 2);
}

TEST(TestModifyRequest, TestAddStopRequest2) {
  auto req = std::make_unique<AddStopRequest>();
  ASSERT_EQ(req->GetType(), Request::Type::ADD_STOP);
  std::string request{"Stop With Space: 22.22, 33.33"};
  req->ParseFrom(request);
  Database db;
  req->Process(db);
  auto stop = db.TakeStop("Stop With Space");
  ASSERT_EQ(stop->GetName(), "Stop With Space");
  ASSERT_DOUBLE_EQ(stop->GetCoord().GetLatitude(), 22.22);
  ASSERT_DOUBLE_EQ(stop->GetCoord().GetLongitude(), 33.33);
}

TEST(TestModifyRequest, TestAddRouteRequest1) {
  auto req = std::make_unique<AddRouteRequest>();
  ASSERT_EQ(req->GetType(), Request::Type::ADD_ROUTE);
  std::string request{"Bus1: Stop1 - Stop2 - Stop3 - Stop4"};
  std::vector<std::string> stops_name = {"Stop1", "Stop2", "Stop3", "Stop4"};
  req->ParseFrom(request);
  Database db;
  req->Process(db);
  auto route = db.TakeRoute("Bus1");
  ASSERT_EQ(route->GetName(), "Bus1");
  ASSERT_EQ(route->GetStopsName(), stops_name);
  ASSERT_EQ(route->CountOfStops(), 7);
}

TEST(TestModifyRequest, TestAddRouteRequest2) {
  auto req = std::make_unique<AddRouteRequest>();
  ASSERT_EQ(req->GetType(), Request::Type::ADD_ROUTE);
  std::string request{"Bus With Space: Stop 1 - Stop 2 - Stop 3 - Stop 4"};
  std::vector<std::string> stops_name = {"Stop 1", "Stop 2", "Stop 3", "Stop 4"};
  req->ParseFrom(request);
  Database db;
  req->Process(db);
  auto route = db.TakeRoute("Bus With Space");
  ASSERT_EQ(route->GetName(), "Bus With Space");
  ASSERT_EQ(route->GetStopsName(), stops_name);
  ASSERT_EQ(route->CountOfStops(), 7);
}

TEST(TestModifyRequest, TestaddRouteRequest3) {
  auto req = std::make_unique<AddRouteRequest>();
  ASSERT_EQ(req->GetType(), Request::Type::ADD_ROUTE);
  std::string request{"Bus1: Stop1 > Stop2 > Stop3 > Stop1"};
  std::vector<std::string> stops_name = {"Stop1", "Stop2", "Stop3", "Stop1"};
  req->ParseFrom(request);
  Database db;
  req->Process(db);
  auto route = db.TakeRoute("Bus1");
  ASSERT_EQ(route->GetName(), "Bus1");
  ASSERT_EQ(route->GetStopsName(), stops_name);
  ASSERT_EQ(route->CountOfStops(), 4);
}

TEST(TestModifyRequest, TestaddRouteRequest4) {
  auto req = std::make_unique<AddRouteRequest>();
  ASSERT_EQ(req->GetType(), Request::Type::ADD_ROUTE);
  std::string request{"Bus With Space: Stop 1 > Stop 2 > Stop 3 > Stop 1"};
  std::vector<std::string> stops_name = {"Stop 1", "Stop 2", "Stop 3", "Stop 1"};
  req->ParseFrom(request);
  Database db;
  req->Process(db);
  auto route = db.TakeRoute("Bus With Space");
  ASSERT_EQ(route->GetName(), "Bus With Space");
  ASSERT_EQ(route->GetStopsName(), stops_name);
  ASSERT_EQ(route->CountOfStops(), 4);
}

TEST(TestReadRequest, TestTakeRoute1) {
  Database db;
  {
    auto req = std::make_unique<AddRouteRequest>();
    std::string request{"Bus1: Stop1 > Stop2 > Stop3 > Stop1"};
    std::vector<std::string> stops_name = {"Stop1", "Stop2", "Stop3", "Stop1"};
    req->ParseFrom(request);
    req->Process(db);
  }
  auto req = std::make_unique<TakeRouteRequest>();
  std::string route_name = "Bus1";
  req->ParseFrom(route_name);
  auto answer = req->Process(db);
  ASSERT_EQ(answer.has_value, true);
  std::string answer_str = "Bus Bus1: 4 stops on route, 3 unique stops, 0 route length";
  ASSERT_EQ(req->StringAnswer(answer), answer_str);
}

TEST(TestReadRequest, TestTakeRoute2) {
  Database db;
  {
    auto req = std::make_unique<AddRouteRequest>();
    std::string request{"Bus L: Stop A > Stop B > Stop C > Stop A"};
    std::vector<std::string> stops_name = {"Stop A", "Stop B", "Stop C", "Stop D"};
    req->ParseFrom(request);
    req->Process(db);
  }
  auto req = std::make_unique<TakeRouteRequest>();
  std::string route_name = "NL";
  req->ParseFrom(route_name);
  auto answer = req->Process(db);
  ASSERT_EQ(answer.has_value, false);
  std::string answer_str = "Bus NL: not found";
  ASSERT_EQ(req->StringAnswer(answer), answer_str);
}

class TestReadRequestClass : public ::testing::Test {
protected:
  Database db;
  void SetUp() {
    {
      auto req = std::make_unique<AddRouteRequest>();
      std::string request{"L: Stop A - Stop B - Stop C - Stop D"};
      std::vector<std::string> stops_name = {"Stop A", "Stop B", "Stop C", "Stop D"};
      req->ParseFrom(request);
      req->Process(db);
    }
    {
      auto req = std::make_unique<AddRouteRequest>();
      std::string request{"L1: Stop A - Stop B - Stop C"};
      std::vector<std::string> stops_name = {"Stop A", "Stop B", "Stop C"};
      req->ParseFrom(request);
      req->Process(db);
    }
    {
      auto req = std::make_unique<AddRouteRequest>();
      std::string request{"L2: Stop A - Stop B"};
      std::vector<std::string> stops_name = {"Stop A", "Stop B"};
      req->ParseFrom(request);
      req->Process(db);
    }
  }
};

TEST_F(TestReadRequestClass, TestTakeStop1) {
  {
    auto req = std::make_unique<TakeStopRequest>();
    std::string stop_name = "Stop A";
    req->ParseFrom(stop_name);
    auto answer = req->Process(db);
    ASSERT_EQ(answer.in_base, true);
    std::string answer_str = "Stop Stop A: buses L L1 L2";
    ASSERT_EQ(req->StringAnswer(answer), answer_str);
  }
  {
    auto req = std::make_unique<TakeStopRequest>();
    std::string stop_name = "Stop B";
    req->ParseFrom(stop_name);
    auto answer = req->Process(db);
    ASSERT_EQ(answer.in_base, true);
    std::string answer_str = "Stop Stop B: buses L L1 L2";
    ASSERT_EQ(req->StringAnswer(answer), answer_str);
  }
  {
    auto req = std::make_unique<TakeStopRequest>();
    std::string stop_name = "Stop C";
    req->ParseFrom(stop_name);
    auto answer = req->Process(db);
    ASSERT_EQ(answer.in_base, true);
    std::string answer_str = "Stop Stop C: buses L L1";
    ASSERT_EQ(req->StringAnswer(answer), answer_str);
  }
  {
    auto req = std::make_unique<TakeStopRequest>();
    std::string stop_name = "Stop D";
    req->ParseFrom(stop_name);
    auto answer = req->Process(db);
    ASSERT_EQ(answer.in_base, true);
    std::string answer_str = "Stop Stop D: buses L";
    ASSERT_EQ(req->StringAnswer(answer), answer_str);
  }

}

TEST_F(TestReadRequestClass, TestTakeStop2) {
  auto req = std::make_unique<TakeStopRequest>();
  db.TakeOrAddStop("Stop E");
  std::string stop_name = "Stop E";
  req->ParseFrom(stop_name);
  auto answer = req->Process(db);
  ASSERT_EQ(answer.in_base, true);
  std::string answer_str = "Stop Stop E: no buses";
  ASSERT_EQ(req->StringAnswer(answer), answer_str);
}

TEST_F(TestReadRequestClass, TestTakeStop3) {
  auto req = std::make_unique<TakeStopRequest>();
  std::string stop_name = "Stop F";
  req->ParseFrom(stop_name);
  auto answer = req->Process(db);
  ASSERT_EQ(answer.in_base, false);
  std::string answer_str = "Stop Stop F: not found";
  ASSERT_EQ(req->StringAnswer(answer), answer_str);
}