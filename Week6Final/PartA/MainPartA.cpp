/**
 * Database class:
 *   Stops: Stop -> Coordinates{latitude, longitude}
 *   Buses: Bus -> Route
 *
 * ModifyQuery:
 *   Stop X: latitude, longitude
 *   Bus X: route
 *
 * ReadQuery:
 *   Bus X
 **/

#include "test_runner.h"
#include "tests.h"

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestPartA);

  return 0;
}