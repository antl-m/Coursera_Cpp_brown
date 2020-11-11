#include "test_runner.h"

#include <sstream>
#include <string>

#include "DataBase.h"

#include "tests.h"

using namespace std;

void TestPartA() {
  istringstream input("10\n"
                      "Stop Tolstopaltsevo: 55.611087, 37.20829\n"
                      "Stop Marushkino: 55.595884, 37.209755\n"
                      "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > "
                        "Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                      "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                      "Stop Rasskazovka: 55.632761, 37.333324\n"
                      "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n"
                      "Stop Biryusinka: 55.581065, 37.64839\n"
                      "Stop Universam: 55.587655, 37.645687\n"
                      "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
                      "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
                      "3\n"
                      "Bus 256\n"
                      "Bus 750\n"
                      "Bus 751\n");
  const string expected_output("Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n"
                               "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length\n"
                               "Bus 751: not found\n");

  DataBase db(input);
  ostringstream output;
  PrintResponses(db.ProcessReadQueries(), output);
  ASSERT_EQUAL(output.str(), expected_output);
}

void TestParsingReadQuery() {

}