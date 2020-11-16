//
// Created by ilya on 29.11.2019.
//

#include <gtest/gtest.h>
#include "manager.h"

TEST(TestOneRequest, GoodAddStop1) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Stop S1: 1, 2");
  auto stop = db.TakeStop("S1");
  ASSERT_EQ(stop->GetName(), "S1");
  ASSERT_EQ(stop->GetCoord().GetLatitude(), 1);
  ASSERT_EQ(stop->GetCoord().GetLongitude(), 2);
}

TEST(TestOneRequest, GoodAddStop2) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Stop Name With Space: 1.1, 2.2");
  auto stop = db.TakeStop("Name With Space");
  ASSERT_EQ(stop->GetName(), "Name With Space");
  ASSERT_EQ(stop->GetCoord().GetLatitude(), 1.1);
  ASSERT_EQ(stop->GetCoord().GetLongitude(), 2.2);
}

TEST(TestOneRequest, GoodAddStop3) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Stop Name With Space       : 1.1, 2.2");
  auto stop = db.TakeStop("Name With Space");
  ASSERT_EQ(stop->GetName(), "Name With Space");
  ASSERT_EQ(stop->GetCoord().GetLatitude(), 1.1);
  ASSERT_EQ(stop->GetCoord().GetLongitude(), 2.2);
}

TEST(TestOneRequest, ChangeStop) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Stop Name With Space: 1.1, 2.2");
  dm.ProcessModifyRequest("Stop Name With Space: 2.2, 3.3");
  auto stop = db.TakeStop("Name With Space");
  ASSERT_EQ(stop->GetName(), "Name With Space");
  ASSERT_EQ(stop->GetCoord().GetLatitude(), 2.2);
  ASSERT_EQ(stop->GetCoord().GetLongitude(), 3.3);
}

TEST(TestOneRequest, AddRoute1) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Bus Bus1: Stop1 - Stop2 - Stop3 - Stop4");
  auto route = db.TakeRoute("Bus1");
  ASSERT_EQ(route->GetName(), "Bus1");
  ASSERT_EQ(route->GetStopsName(), std::vector<std::string>({"Stop1", "Stop2", "Stop3", "Stop4"}));
  ASSERT_EQ(route->CountOfStops(), 7);
  ASSERT_EQ(route->CountOfUniqueStops(), 4);
  ASSERT_EQ(route->Length(), 0);
}

TEST(TestOneRequest, AddRoute2) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Bus Bus1: Stop1 > Stop2 > Stop3 > Stop1");
  auto route = db.TakeRoute("Bus1");
  ASSERT_EQ(route->GetName(), "Bus1");
  ASSERT_EQ(route->GetStopsName(), std::vector<std::string>({"Stop1", "Stop2", "Stop3", "Stop1"}));
  ASSERT_EQ(route->CountOfStops(), 4);
  ASSERT_EQ(route->CountOfUniqueStops(), 3);
  ASSERT_EQ(route->Length(), 0);
}

TEST(TestOneRequest, AddRoute3) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Bus Bus Name: Stop A > Stop B > Stop C > Stop A");
  auto route = db.TakeRoute("Bus Name");
  ASSERT_EQ(route->GetName(), "Bus Name");
  ASSERT_EQ(route->GetStopsName(), std::vector<std::string>({"Stop A", "Stop B", "Stop C", "Stop A"}));
  ASSERT_EQ(route->CountOfStops(), 4);
  ASSERT_EQ(route->CountOfUniqueStops(), 3);
  ASSERT_EQ(route->Length(), 0);
}

TEST(TestOneRequest, AddRoute4) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Bus Bus Name:    Stop A   >   Stop B   >    Stop C > Stop A    ");
  auto route = db.TakeRoute("Bus Name");
  ASSERT_EQ(route->GetName(), "Bus Name");
  ASSERT_EQ(route->GetStopsName(), std::vector<std::string>({"Stop A", "Stop B", "Stop C", "Stop A"}));
  ASSERT_EQ(route->CountOfStops(), 4);
  ASSERT_EQ(route->CountOfUniqueStops(), 3);
  ASSERT_EQ(route->Length(), 0);
}

TEST(TestOneRequest, TakeStop1) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Bus Bus 1: Stop A > Stop B > Stop C > Stop A");
  dm.ProcessModifyRequest("Bus Bus 2: Stop A > Stop B > Stop C > Stop A");
  dm.ProcessModifyRequest("Bus Bus 3: Stop A - Stop B - Stop C");
  dm.ProcessModifyRequest("Bus Bus 4: Stop C - Stop A1");
  dm.ProcessModifyRequest("Bus Bus 5: Stop A1 - Stop B1 - Stop C1");

  auto res_A = dm.ProcessReadRequest("Stop Stop A");
  ASSERT_EQ(res_A, "Stop Stop A: buses Bus 1 Bus 2 Bus 3");
  auto res_B = dm.ProcessReadRequest("Stop Stop B");
  ASSERT_EQ(res_B, "Stop Stop B: buses Bus 1 Bus 2 Bus 3");
  auto res_C = dm.ProcessReadRequest("Stop Stop C");
  ASSERT_EQ(res_C, "Stop Stop C: buses Bus 1 Bus 2 Bus 3 Bus 4");
  auto res_A1 = dm.ProcessReadRequest("Stop Stop A1");
  ASSERT_EQ(res_A1, "Stop Stop A1: buses Bus 4 Bus 5");
  auto res_B1 = dm.ProcessReadRequest("Stop Stop B1");
  ASSERT_EQ(res_B1, "Stop Stop B1: buses Bus 5");
  auto res_C1 = dm.ProcessReadRequest("Stop Stop C1");
  ASSERT_EQ(res_C1, "Stop Stop C1: buses Bus 5");
}

TEST(TestOneRequest, TakeStop2) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Stop S1: 1, 2");
  auto res_A = dm.ProcessReadRequest("Stop S1");
  ASSERT_EQ(res_A, "Stop S1: no buses");
}

TEST(TestOneRequest, TakeStop3) {
  Database db;
  DatabaseManager dm(db);
  dm.ProcessModifyRequest("Stop S1: 1, 2");
  auto res_A = dm.ProcessReadRequest("Stop Stop S2");
  ASSERT_EQ(res_A, "Stop Stop S2: not found");
}

TEST(TestFull, Test1) {
  Database db;
  DatabaseManager dm(db);
  std::string input = R"(10
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751)";
  std::stringstream s(input);
  auto res = dm.ProcessAllRequests(s);
  std::vector<std::string> answers = {
  {"Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length"},
  {"Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length"},
  {"Bus 751: not found"}
  };
  ASSERT_EQ(res, answers);
}

TEST(TestFull, Test2) {
  Database db;
  DatabaseManager dm(db);
  std::string input = R"(13
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye)";
  std::stringstream s(input);
  auto res = dm.ProcessAllRequests(s);
  std::vector<std::string> answers = {
      {"Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length"},
      {"Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length"},
      {"Bus 751: not found"},
      {"Stop Samara: not found"},
      {"Stop Prazhskaya: no buses"},
      {"Stop Biryulyovo Zapadnoye: buses 256 828"}
  };
  ASSERT_EQ(res, answers);
}