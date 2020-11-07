//#include "help.h"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void PrintStats(vector<Person> persons) {
  auto females_beg = begin(persons);
  auto females_end = partition(begin(persons), end(persons),
                               [](const Person &p) { return p.gender == Gender::FEMALE; });
  auto males_beg = females_end;
  auto males_end = end(persons);

  auto employed_females_beg = females_beg;
  auto employed_females_end = partition(females_beg, females_end, [](const Person &p) { return p.is_employed; });
  auto unemployed_females_beg = employed_females_end;
  auto unemployed_females_end = females_end;

  auto employed_males_beg = males_beg;
  auto employed_males_end = partition(males_beg, males_end, [](const Person &p) { return p.is_employed; });
  auto unemployed_males_beg = employed_males_end;
  auto unemployed_males_end = males_end;

  cout << "Median age = " << ComputeMedianAge(begin(persons), end(persons)) << '\n';
  cout << "Median age for females = " << ComputeMedianAge(females_beg, females_end) << '\n';
  cout << "Median age for males = " << ComputeMedianAge(males_beg, males_end) << '\n';
  cout << "Median age for employed females = " << ComputeMedianAge(employed_females_beg, employed_females_end) << '\n';
  cout << "Median age for unemployed females = " << ComputeMedianAge(unemployed_females_beg, unemployed_females_end)
       << '\n';
  cout << "Median age for employed males = " << ComputeMedianAge(employed_males_beg, employed_males_end) << '\n';
  cout << "Median age for unemployed males = " << ComputeMedianAge(unemployed_males_beg, unemployed_males_end) << endl;
}
/***********************************************************************************************************************
int main() {
  vector<Person> persons = {
      {31, Gender::MALE, false},
      {40, Gender::FEMALE, true},
      {24, Gender::MALE, true},
      {20, Gender::FEMALE, true},
      {80, Gender::FEMALE, false},
      {78, Gender::MALE, false},
      {10, Gender::FEMALE, false},
      {55, Gender::MALE, true},
  };
  PrintStats(persons);
  return 0;
}
***********************************************************************************************************************/
//Median age = 40
//Median age for females = 40
//Median age for males = 55
//Median age for employed females = 40
//Median age for unemployed females = 80
//Median age for employed males = 55
//Median age for unemployed males = 78
