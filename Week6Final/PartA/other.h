#pragma once

#include <iostream>

template<typename Number>
Number ReadNumberLine(std::istream &stream) {
  Number number;
  stream >> number;
  std::string dummy;
  getline(stream, dummy);
  return number;
}