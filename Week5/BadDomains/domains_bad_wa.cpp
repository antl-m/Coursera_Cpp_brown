#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <memory>

std::vector<std::string> ReadFullDomains(std::istream& stream) {
  int count;
  stream >> count;
  std::vector<std::string> res(count);
  for (auto& s : res) {
    stream >> s;
  }
  return res;
}

class DomainFilter {
 public:
  explicit DomainFilter(const std::vector<std::string>& raw_domains) {
    ParseFullDomains(raw_domains);
  }

  // true - можно пропустить
  // false - пропустить нельзя
  bool FilterDomain(const std::string& domain) {
    auto subdomains = SplitBy(domain);

    auto dom_first = subdomains.back();
    auto* curr_line = &bad_domains_.data_;
    if (!curr_line->count(dom_first)) {
      return true;
    }
    curr_line = &curr_line->at(dom_first)->data_;
    if (curr_line->count("")) {
      return false;
    }
    subdomains.pop_back();
    for (auto i = subdomains.rbegin(); i != subdomains.rend(); ++i) {
      if (!curr_line->count(*i)) {
        return true;
      }
      auto b = *i;
      curr_line = &curr_line->at(*i)->data_;
      if (curr_line->count("")) {
        return false;
      }
    }
    return true;
  }

 private:
  struct Item {
    std::unordered_map<std::string, std::shared_ptr<Item>> data_;
  };
  Item bad_domains_;

  void ParseFullDomains(const std::vector<std::string>& raw_domains) {
    for (const auto& domain : raw_domains) {
      auto subdomains = SplitBy(domain);
      std::reverse(subdomains.begin(), subdomains.end());
      auto* curr_line = &bad_domains_.data_;
      for (auto& dom : subdomains) {
        curr_line->insert({dom, std::move(std::make_shared<Item>())});
        curr_line = &curr_line->at(dom)->data_;
      }
      curr_line->insert({"", std::make_shared<Item>()});
    }
  }

  static std::vector<std::string> SplitBy(const std::string & s, char split = '.') {
    std::vector<std::string> result;
    auto curr = s.begin();
    while (curr != s.end()) {
      auto space = std::find(curr, s.end(), split);
      result.emplace_back(curr, space);
      if (space != s.end()) curr = (space + 1);
      else curr = s.end();
    }
    return result;
  }
};

int main() {
  auto bad_domains = ReadFullDomains(std::cin);
  auto domains = ReadFullDomains(std::cin);
  DomainFilter filter(bad_domains);
  for (const auto& d : domains) {
    auto res = filter.FilterDomain(d);
    if (res) {
      std::cout << "Good" << std::endl;
    }
    else {
      std::cout << "Bad" << std::endl;
    }
  }
  return 0;
}