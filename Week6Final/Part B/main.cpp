#include "manager.h"

int main() {
  Database db;
  DatabaseManager dm(db);
  std::cout.precision(6);
  auto result = dm.ProcessAllRequests();
  PrintResults(result, std::cout);
  return 0;
}
