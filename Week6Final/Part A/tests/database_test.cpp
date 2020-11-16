//
// Created by ilya on 27.11.2019.
//
#include <gtest/gtest.h>
#include "database.h"

// TODO Тесты на stops
// TODO Тесты на routes

TEST(TestStop, Default) {
  std::string stop_name = "unnamed";
  Stop s;
  ASSERT_EQ(s.GetCoord().GetLatitude(), 0);
  ASSERT_EQ(s.GetCoord().GetLongitude(), 0);
  ASSERT_EQ(s.GetName(), stop_name);
}

TEST(TestStop, Test1) {
  std::string stop_name = "Stop1";
  Stop s(stop_name, Coordinates(1, 2));
  ASSERT_EQ(s.GetCoord().GetLatitude(), 1);
  ASSERT_EQ(s.GetCoord().GetLongitude(), 2);
  ASSERT_EQ(s.GetName(),stop_name);
}

TEST(TestStop, Test2) {
  std::string stop_name = "Stop2";
  Stop s(stop_name, Coordinates(3, 4));
  ASSERT_EQ(s.GetCoord().GetLatitude(), 3);
  ASSERT_EQ(s.GetCoord().GetLongitude(), 4);
  ASSERT_EQ(s.GetName(),stop_name);
}

TEST(TestStop, Test3) {
  std::string stop_name = "Stop3";
  Stop s(stop_name, Coordinates(23.3, 33.3));
  ASSERT_EQ(s.GetCoord().GetLatitude(), 23.3);
  ASSERT_EQ(s.GetCoord().GetLongitude(), 33.3);
  ASSERT_EQ(s.GetName(),stop_name);
}

TEST(TestRouteInfo, Test1) {
  Stop a;
  a.AddRoute("Bus 1");
  a.AddRoute("Bus 2");
  ASSERT_EQ(a.TakeRoutes(), std::vector<std::string>({"Bus 1", "Bus 2"}));
}

TEST(TestRouteInfo, Test2) {
  Stop a;
  a.AddRoute("Bus C");
  a.AddRoute("Bus B");
  a.AddRoute("Bus A");
  ASSERT_EQ(a.TakeRoutes(), std::vector<std::string>({"Bus A", "Bus B", "Bus C"}));
}

TEST(TestRouteInfo, Test3) {
  Stop a;
  ASSERT_EQ(a.TakeRoutes(), std::vector<std::string>());
}

TEST(TestCtor, TestCopy) {
  std::string stop_name = "Stop1";
  Stop s1(stop_name, Coordinates(23.3, 33.3));
  Stop s2(s1);
  ASSERT_EQ(s2.GetCoord().GetLatitude(), 23.3);
  ASSERT_EQ(s2.GetCoord().GetLongitude(), 33.3);
  ASSERT_EQ(s2.GetName(),stop_name);
}

TEST(TestStopCpyCtor, TestMove) {
  std::string stop_name = "Stop1";
  Stop s1(stop_name, Coordinates(23.3, 33.3));
  Stop s2(std::move(s1));
  ASSERT_EQ(s2.GetCoord().GetLatitude(), 23.3);
  ASSERT_EQ(s2.GetCoord().GetLongitude(), 33.3);
  ASSERT_EQ(s2.GetName(),stop_name);
}

TEST(TestStopAssign, TestCopy) {
  std::string stop_name = "Stop1";
  Stop s1(stop_name, Coordinates(23.3, 33.3));
  Stop s2;
  s2 = s1;
  ASSERT_EQ(s2.GetCoord().GetLatitude(), 23.3);
  ASSERT_EQ(s2.GetCoord().GetLongitude(), 33.3);
  ASSERT_EQ(s2.GetName(),stop_name);
}

TEST(TestStopAssign, TestMove) {
  std::string stop_name = "Stop1";
  Stop s1(stop_name, Coordinates(23.3, 33.3));
  Stop s2;
  s2 = std::move(s1);
  ASSERT_EQ(s2.GetCoord().GetLatitude(), 23.3);
  ASSERT_EQ(s2.GetCoord().GetLongitude(), 33.3);
  ASSERT_EQ(s2.GetName(),stop_name);
}

TEST(TestRouteCycle, Default) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop1"};
  std::vector<std::shared_ptr<Stop>> stops_;
  for (const auto& name : names) {
    stops_.push_back(db.TakeOrAddStop(name));
  }
  std::shared_ptr<Route> route = std::make_shared<CycleRoute>(route_name, names, stops_);
  ASSERT_EQ(route->CountOfStops(), 4);
  ASSERT_EQ(route->CountOfUniqueStops(), 3);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestRouteCycle, RouteBuilder1) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop1"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names});
  ASSERT_EQ(route->CountOfStops(), 4);
  ASSERT_EQ(route->CountOfUniqueStops(), 3);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestRouteCycle, RouteBuilder2) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4", "Stop1"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names});
  ASSERT_EQ(route->CountOfStops(), 5);
  ASSERT_EQ(route->CountOfUniqueStops(), 4);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestRouteCycle, RouteBuilder3) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Value1", "Value2", "Value3", "Value4", "Value5", "Value1"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names});
  for (size_t i = 0; i < names.size(); ++i) {
    db.AddStop({names[i], Coordinates()});
  }
  ASSERT_EQ(route->CountOfStops(), 6);
  ASSERT_EQ(route->CountOfUniqueStops(), 5);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestRouteCycle, RouteBuilder4) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Value1", "Value2", "Value3", "Value4", "Value2", "Value1"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names});
  for (size_t i = 0; i < names.size(); ++i) {
  db.AddStop({names[i], Coordinates()});
  }
  ASSERT_EQ(route->CountOfStops(), 6);
  ASSERT_EQ(route->CountOfUniqueStops(), 4);
  ASSERT_EQ(route->Length(), 0);
}

TEST(TestRouteLinear, RouteBuilder1) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names});
  ASSERT_EQ(route->CountOfStops(), 7);
  ASSERT_EQ(route->CountOfUniqueStops(), 4);
  ASSERT_EQ(route->Length(), 0);
}

TEST(TestRouteLinear, RouteBuilder2) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4", "Stop5"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names});
  ASSERT_EQ(route->CountOfStops(), 9);
  ASSERT_EQ(route->CountOfUniqueStops(), 5);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestRouteLinear, RouteBuilder3) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Value1", "Value2", "Value3", "Value4", "Value5", "Value6"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names});
  for (size_t i = 0; i < names.size(); ++i) {
  db.AddStop({names[i], Coordinates()});
  }
  ASSERT_EQ(route->CountOfStops(), 11);
  ASSERT_EQ(route->CountOfUniqueStops(), 6);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestRouteLinear, RouteBuilder4) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Value1", "Value2", "Value3", "Value2", "Value1", "Value6"};
  std::shared_ptr<Route> route = RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names});
  for (size_t i = 0; i < names.size(); ++i) {
  db.AddStop({names[i], Coordinates()});
  }
  ASSERT_EQ(route->CountOfStops(), 11);
  ASSERT_EQ(route->CountOfUniqueStops(), 4);
  ASSERT_EQ(route->Length(),0);
}

TEST(TestNotAddedStop, Test1) {
  Database db;
  ASSERT_EQ(db.TakeOrAddStop("Stop1")->GetName(), "Stop1");
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 0);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 0);
}

TEST(TestNotAddedStop, Test2) {
  Database db;
  ASSERT_EQ(db.TakeOrAddStop("Stop1")->GetName(), "Stop1");
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 0);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 0);
  ASSERT_EQ(db.TakeOrAddStop("Stop2")->GetName(), "Stop2");
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLatitude(), 0);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLongitude(), 0);
}

TEST(TestAddStop, Base1) {
  Database db;
  db.AddStop({"Stop1", Coordinates(1, 2)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 2);
}

TEST(TestAddStop, Base2) {
  Database db;
  db.AddStop({"Stop1", Coordinates(1, 2)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 2);
  db.AddStop({"Stop2", Coordinates(3, 4)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLatitude(), 3);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLongitude(), 4);
}

TEST(TestAddStop, ManyAdd) {
  Database db;
  db.AddStop({"Stop1", Coordinates(1, 2)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 2);
  db.AddStop({"Stop2", Coordinates(3, 4)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLatitude(), 3);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLongitude(), 4);
  db.AddStop({"Stop3", Coordinates(5, 6)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop3")->GetCoord().GetLatitude(), 5);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop3")->GetCoord().GetLongitude(), 6);
  db.AddStop({"Stop4", Coordinates(7.7, 8.8)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop4")->GetCoord().GetLatitude(), 7.7);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop4")->GetCoord().GetLongitude(), 8.8);
}

TEST(TestAddStop, AddAndChange) {
  Database db;
  db.TakeOrAddStop("Stop1");
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 0);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 0);
  db.TakeOrAddStop("Stop2");
  db.TakeOrAddStop("Stop2");
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLatitude(), 0);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLongitude(), 0);

  db.AddStop({"Stop1", Coordinates(1, 2)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 2);
  db.AddStop({"Stop2", Coordinates(3, 4)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLatitude(), 3);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLongitude(), 4);
}

TEST(TestAddStop, TakeWithoutAdd) {
  Database db;
  ASSERT_EQ(db.TakeStop("Stop 1"), nullptr);
}

TEST(TestAddStop, AddAfterTake) {
  Database db;
  db.AddStop({"Stop1", Coordinates(1, 2)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop1")->GetCoord().GetLongitude(), 2);
  db.AddStop({"Stop2", Coordinates(3, 4)});
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLatitude(), 3);
  ASSERT_DOUBLE_EQ(db.TakeOrAddStop("Stop2")->GetCoord().GetLongitude(), 4);

  db.AddStop({"Stop1", Coordinates(1, 2)});
  ASSERT_DOUBLE_EQ(db.TakeStop("Stop1")->GetCoord().GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(db.TakeStop("Stop1")->GetCoord().GetLongitude(), 2);
  db.AddStop({"Stop2", Coordinates(3, 4)});
  ASSERT_DOUBLE_EQ(db.TakeStop("Stop2")->GetCoord().GetLatitude(), 3);
  ASSERT_DOUBLE_EQ(db.TakeStop("Stop2")->GetCoord().GetLongitude(), 4);
}


TEST(TestAddRoute, BaseAddCycle) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4"};
  std::vector<std::shared_ptr<Stop>> stops_;
  for (const auto& name : names) {
    stops_.push_back(db.TakeOrAddStop(name));
  }
  db.AddRoute(route_name, std::make_shared<CycleRoute>(route_name, names, stops_));
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(take_names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BuildedCycle1) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4"};
  db.AddRoute(route_name, RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names}));
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(take_names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BuildedCycle2) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Value1", "Stop3", "Value2", "Stop3", "Value3"};
  for (size_t i = 1; i < names.size(); i+=2) {
    db.AddStop({names[i], {}});
  }
  db.AddRoute(route_name, RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names}));
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size() - 2; ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BuildedCycle3) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Value1", "Value2", "Value3", "Value4", "Value5", "Value6"};
  db.AddRoute(route_name, RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name, names}));
  for (size_t i = 0; i < names.size(); ++i) {
    db.AddStop({names[i], {}});
  }
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(take_names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BaseAddLinear) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4"};
  std::vector<std::shared_ptr<Stop>> stops_;
  for (const auto& name : names) {
    stops_.push_back(db.TakeOrAddStop(name));
  }
  db.AddRoute(route_name, std::make_shared<CycleRoute>(route_name, names, stops_));
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(take_names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BuildedLinear1) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Stop2", "Stop3", "Stop4"};
  db.AddRoute(route_name, RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names}));
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(take_names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BuildedLinear2) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Stop1", "Value1", "Stop3", "Value2", "Stop3", "Value3"};
  for (size_t i = 1; i < names.size(); i+=2) {
    db.AddStop({names[i], {}});
  }
  db.AddRoute(route_name, RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names}));
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size() - 2; ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(names[i])->GetName(), names[i]);
  }
}

TEST(TestAddRoute, BuildedLinear3) {
  Database db;
  std::string route_name = "Route1";
  std::vector<std::string> names{"Value1", "Value2", "Value3", "Value4", "Value5", "Value6"};
  db.AddRoute(route_name, RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name, names}));
  for (size_t i = 0; i < names.size(); ++i) {
    db.AddStop({names[i], {}});
  }
  auto route = db.TakeRoute(route_name);
  ASSERT_EQ(route->GetName(), route_name);
  auto take_names = route->GetStopsName();
  ASSERT_EQ(names.size(), take_names.size());
  for (size_t i = 0; i < names.size(); ++i) {
    ASSERT_EQ(names[i], take_names[i]);
    ASSERT_EQ(db.TakeOrAddStop(take_names[i])->GetName(), names[i]);
  }
}

TEST(TestTakeRoute, TakeNoRoute) {
  Database db;
  auto route = db.TakeRoute("NoRoute");
  ASSERT_EQ(route, nullptr);
}

TEST(TestTakeStop, FullAnswer) {
  Database db;
  std::string route_name_1 = "Bus 1";
  std::string route_name_2 = "Bus 2";
  std::string route_name_3 = "Bus 3";
  std::string stop_name_1 = "Stop A";
  std::string stop_name_2 = "Stop B";
  std::string stop_name_3 = "Stop C";

  db.AddRoute(route_name_1, RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name_1, {stop_name_1, stop_name_2, stop_name_3}}));
  db.AddRoute(route_name_2, RouteBuilder(db).MakeRoute({Route::RouteTypes::CYCLE, route_name_2, {stop_name_1, stop_name_2}}));
  db.AddRoute(route_name_3, RouteBuilder(db).MakeRoute({Route::RouteTypes::LINEAR, route_name_3, {stop_name_1}}));

  auto stop_1 = db.TakeStop(stop_name_1);
  ASSERT_EQ(stop_1->GetName(), stop_name_1);
  ASSERT_EQ(stop_1->TakeRoutes(), std::vector<std::string>({route_name_1, route_name_2, route_name_3}));

  auto stop_2 = db.TakeStop(stop_name_2);
  ASSERT_EQ(stop_2->GetName(), stop_name_2);
  ASSERT_EQ(stop_2->TakeRoutes(), std::vector<std::string>({route_name_1, route_name_2}));

  auto stop_3 = db.TakeStop(stop_name_3);
  ASSERT_EQ(stop_3->GetName(), stop_name_3);
  ASSERT_EQ(stop_3->TakeRoutes(), std::vector<std::string>({route_name_1}));
}

TEST(TestTakeStop, NoRoutes) {
  Database db;
  auto res = db.TakeOrAddStop("Stop A");
  ASSERT_EQ(res->TakeRoutes(), std::vector<std::string>());
}

TEST(TestTakeStop, NoStop) {
  Database db;
  ASSERT_EQ(db.TakeStop("Stop 1"), nullptr);
}