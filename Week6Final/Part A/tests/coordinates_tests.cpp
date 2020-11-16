//
// Created by ilya on 25.11.2019.
//
#include <gtest/gtest.h>
#include "coordinates.h"

TEST(TestCreateCoords, Default) {
  const Coordinates c;
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 0);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 0);
}


TEST(TestCreateCoords, Test1) {
  const Coordinates c(1, 2);
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 2);
}

TEST(TestCreateCoords, Test2) {
  const Coordinates c(3, 4);
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 3);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 4);
}

TEST(TestCreateCoords, Test3) {
  const Coordinates c(55.611087, 37.20829);
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 55.611087);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 37.20829);
}

TEST(TestCreateCoords, Test4) {
  const Coordinates c(37.595884, 55.209755);
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 37.595884);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 55.209755);
}

TEST(TestCreateCoords, Test5) {
  const Coordinates c(-23.595884, -13.209755);
  ASSERT_DOUBLE_EQ(c.GetLatitude(), -23.595884);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), -13.209755);
}

TEST(TestCalculateDistance, Test1) {
  const Coordinates first(55.611087, 37.20829);
  const Coordinates second(55.595884, 37.209755);
  const Coordinates third(55.632761, 37.333324);
  const double result_1 = Coordinates::Distance(first, second);
  const double result_2 = Coordinates::Distance(first, second);
  const double result = result_1 + result_2;
  const double true_res = 20939.5;
  //ASSERT_DOUBLE_EQ(result, true_res);
}

TEST(TestCoordBuilder, TestAuto) {
  const auto c = CoordinatesBuilder().SetLatitude(1).SetLongitude(2).Build();
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 2);
}

TEST(TestCoordBuilder, TestCpyCtor1) {
  const Coordinates c = CoordinatesBuilder().SetLatitude(1).SetLongitude(2).Build();
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 2);
}

TEST(TestCoordBuilder, TestCpyCtor2) {
  const Coordinates c = CoordinatesBuilder().SetLatitude(33.33).SetLongitude(44.33).Build();
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 33.33);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 44.33);
}

TEST(TestCoordBuilder, TestAssign1) {
  Coordinates c;
  c = CoordinatesBuilder().SetLatitude(1).SetLongitude(2).Build();
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 1);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 2);
}

TEST(TestCoordBuilder, TestAssign2) {
  Coordinates c;
  c = CoordinatesBuilder().SetLatitude(33.33).SetLongitude(44.33).Build();
  ASSERT_DOUBLE_EQ(c.GetLatitude(), 33.33);
  ASSERT_DOUBLE_EQ(c.GetLongitude(), 44.33);
}