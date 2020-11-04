#include "Common.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

class ValueObj: public Expression {
public:
  ValueObj(int value): value_(value) {}

  int Evaluate() const override {
    return value_;
  }

  string ToString() const override {
    return to_string(value_);
  }

private:
  int value_;
};

class SumObj: public Expression {
public:
  SumObj(ExpressionPtr left, ExpressionPtr right): left_(move(left)), right_(move(right)){}

  int Evaluate() const override {
    return left_->Evaluate() + right_->Evaluate();
  }

  string ToString() const override {
    ostringstream ss;
    ss << '(' << left_->ToString() << ")+(" << right_->ToString() << ')';
    return ss.str();
  }

private:
  ExpressionPtr left_, right_;
};

class ProductObj: public Expression{
public:
  ProductObj(ExpressionPtr left, ExpressionPtr right): left_(move(left)), right_(move(right)){}

  int Evaluate() const override {
    return left_->Evaluate() * right_->Evaluate();
  }

  string ToString() const override {
    ostringstream ss;
    ss << '(' << left_->ToString() << ")*(" << right_->ToString() << ')';
    return ss.str();
  }

private:
  ExpressionPtr left_, right_;
};

ExpressionPtr Value(int value){
  return make_unique<ValueObj>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right){
  return make_unique<SumObj>(move(left), move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right){
  return make_unique<ProductObj>(move(left), move(right));
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}