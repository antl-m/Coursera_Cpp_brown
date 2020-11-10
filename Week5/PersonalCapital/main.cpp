#include "test_runner.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <ctime>

using namespace std;

enum class QueryType {
  COMPUTE_INCOME,
  EARN,
  PAY_TAX,
  SPEND
};

const string COMPUTE_INCOME_STR = "ComputeIncome";
const string EARN_STR = "Earn";
const string PAY_TAX_STR = "PayTax";
const string SPEND_STR = "Spend";

class Date {
 private:
  int year_;
  int month_;
  int day_;
 public:
  time_t AsTimestamp() const {
    tm t{.tm_sec = 0,
        .tm_min = 0,
        .tm_hour = 0,
        .tm_mday = day_,
        .tm_mon = month_ - 1,
        .tm_year = year_ - 1900,
        .tm_isdst = 0};
    return mktime(&t);
  }

  friend bool operator<(const Date &lhs, const Date &rhs) {
    return tie(lhs.year_, lhs.month_, lhs.day_) <
        tie(rhs.year_, rhs.month_, rhs.day_);
  }

  friend istream &operator>>(istream &in, Date &date) {
    in >> date.year_;
    in.ignore();
    in >> date.month_;
    in.ignore();
    in >> date.day_;
    return in;
  }
};

constexpr int SECONDS_IN_DAY = 60 * 60 * 24;

int ComputeDaysDiff(const Date &date_to, const Date &date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

struct Query {
  QueryType type;
  Date from;
  Date to;
  int value = 0;
};

using Queries = vector<Query>;

istream &operator>>(istream &in, QueryType &q_type) {
  string q_type_str;
  in >> q_type_str;
  if (q_type_str == COMPUTE_INCOME_STR) {
    q_type = QueryType::COMPUTE_INCOME;
  } else if (q_type_str == EARN_STR) {
    q_type = QueryType::EARN;
  } else if (q_type_str == PAY_TAX_STR) {
    q_type = QueryType::PAY_TAX;
  } else if (q_type_str == SPEND_STR) {
    q_type = QueryType::SPEND;
  } else {
    throw runtime_error("Cannot parse QueryType from string");
  }
  return in;
}

istream &operator>>(istream &in, Query &q) {
  in >> q.type >> q.from >> q.to;
  if (q.type != QueryType::COMPUTE_INCOME) {
    in >> q.value;
  }
  return in;
}

Queries ReadQueries(istream &in = cin) {
  int q_count;
  in >> q_count;
  Queries queries(q_count);
  for (Query &q: queries) {
    in >> q;
  }
  return queries;
}

class QueryProcessor {
 public:
  explicit QueryProcessor(Queries queries) : queries_(move(queries)) {}

  void Process(ostream &out = cout) {
    out.precision(10);
    for (const Query &q: queries_) {
      if (q.type == QueryType::COMPUTE_INCOME) {
        out << ComputeIncomeProcess(q) << '\n';
      } else if (q.type == QueryType::EARN) {
        EarnProcess(q);
      } else if (q.type == QueryType::PAY_TAX) {
        PayTaxProcess(q);
      } else {
        SpendProcess(q);
      }
    }
  }

 private:

  void EarnProcess(const Query &q) {
    int days_count = ComputeDaysDiff(q.to, q.from) + 1;
    double income_per_day = static_cast<double>(q.value) / static_cast<double>(days_count);
    time_t i = q.from.AsTimestamp();
    for (int it = 0; it < days_count; ++it) {
      day_to_income[i].income += income_per_day;
      i += SECONDS_IN_DAY;
    }
  }

  void SpendProcess(const Query &q) {
    int days_count = ComputeDaysDiff(q.to, q.from) + 1;
    double spending_per_day = static_cast<double>(q.value) / static_cast<double>(days_count);
    time_t i = q.from.AsTimestamp();
    for (int it = 0; it < days_count; ++it) {
      day_to_income[i].spending += spending_per_day;
      i += SECONDS_IN_DAY;
    }
  }

  double ComputeIncomeProcess(const Query &q) {
    auto beg = day_to_income.lower_bound(q.from.AsTimestamp());
    auto end = day_to_income.upper_bound(q.to.AsTimestamp());
    double computed_sum = 0;
    for (auto it = beg; it != end; ++it) {
      computed_sum += it->second.ClearIncome();
    }
    return computed_sum;
  }

  void PayTaxProcess(const Query &q) {
    auto beg = day_to_income.lower_bound(q.from.AsTimestamp());
    auto end = day_to_income.upper_bound(q.to.AsTimestamp());
    double coef = 1. - q.value / 100.;
    for (auto it = beg; it != end; ++it) {
      it->second.income *= coef;
    }
  }

 private:
  struct DayStats {
    double income = 0;
    double spending = 0;

    double ClearIncome() const {
      return income - spending;
    }
  };

  Queries queries_;
  map<time_t, DayStats> day_to_income;
};

void Test1() {
  istringstream in(R"(8
                      Earn 2000-01-02 2000-01-06 20
                      ComputeIncome 2000-01-01 2001-01-01
                      PayTax 2000-01-02 2000-01-03 13
                      ComputeIncome 2000-01-01 2001-01-01
                      Earn 2000-01-03 2000-01-03 10
                      ComputeIncome 2000-01-01 2001-01-01
                      PayTax 2000-01-03 2000-01-03 13
                      ComputeIncome 2000-01-01 2001-01-01)");
  QueryProcessor qp(ReadQueries(in));
  ostringstream out;
  qp.Process(out);
  string expected("20\n"
                  "18.96\n"
                  "28.96\n"
                  "27.2076\n");
  ASSERT_EQUAL(out.str(), expected);
}

void Test2() {
  istringstream in(R"(8
                      Earn 2000-01-02 2000-01-06 20
                      ComputeIncome 2000-01-01 2001-01-01
                      PayTax 2000-01-02 2000-01-03 13
                      ComputeIncome 2000-01-01 2001-01-01
                      Spend 2000-12-30 2001-01-02 14
                      ComputeIncome 2000-01-01 2001-01-01
                      PayTax 2000-12-30 2000-12-30 13
                      ComputeIncome 2000-01-01 2001-01-01)");
  QueryProcessor qp(ReadQueries(in));
  ostringstream out;
  qp.Process(out);
  string expected("20\n"
                  "18.96\n"
                  "8.46\n"
                  "8.46\n");
  ASSERT_EQUAL(out.str(), expected);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test1);
  RUN_TEST(tr, Test2);

  QueryProcessor qp(ReadQueries());
  qp.Process();
  return 0;
}