#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const{
    if(next)
      next->Process(move(email));
  }

  unique_ptr<Worker> next;

public:
  void SetNext(unique_ptr<Worker> next){
    if(this->next)
        return this->next->SetNext(move(next));
    this->next = move(next);
  }
};


class Reader : public Worker {
public:
  Reader(istream& input): in(input) {}

  void Run() override {
    Email e;
    while(getline(in, e.from) && getline(in, e.to) && getline(in, e.body)){
      PassOn(make_unique<Email>(move(e)));
    }
  }

  void Process(unique_ptr<Email> email) override {
    PassOn(move(email));
  }

    istream& in;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

  explicit Filter(Function f): pred(move(f)) {}

  void Process(unique_ptr<Email> email) override {
    if(pred(*email))
      PassOn(move(email));
  }

public:
  Function pred;
};


class Copier : public Worker {
public:
  string to;

  Copier(string to): to(move(to)) {}

  void Process(unique_ptr<Email> email) override {
    Email e = *email;
    PassOn(move(email));
    if(to != e.to){
      e.to = to;
      PassOn(make_unique<Email>(move(e)));
    }
  }
};


class Sender : public Worker {
public:
  ostream& out;

  Sender(ostream& out): out(out) {}

  void Process(unique_ptr<Email> email) override {
    out << email->from << '\n' << email->to << '\n' << email->body << '\n';
    PassOn(move(email));
  }
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in){
    first = make_unique<Reader>(in);
  }

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter){
    first->SetNext(make_unique<Filter>(filter));
    return *this;
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient){
    first->SetNext(make_unique<Copier>(recipient));
    return *this;
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out){
    first->SetNext(make_unique<Sender>(out));
    return *this;
  }

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build(){
      return move(first);
  }

  unique_ptr<Worker> first;
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
