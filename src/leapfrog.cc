#include <cassert>
#include <initializer_list>
#include <iostream>
#include <vector>

#include <leapfrog.hpp>

using std::vector;

int main() {
  auto tab1 = vector<int>{0, 1, 3, 4, 5, 6, 7, 8, 9, 11};
  auto tab2 = vector<int>{0, 2, 6, 7, 8, 9, 11};
  auto tab3 = vector<int>{2, 4, 5, 8, 10};

  {
    auto iter = LinearIterator{tab1};
    std::cout << "tab1:\n";
    while (!iter.at_end()) {
      std::cout << '\t' << iter.key() << '\n';
      iter.next();
    }
  }
  {
    auto iter = LinearIterator{tab2};
    std::cout << "tab2:\n";
    while (!iter.at_end()) {
      std::cout << '\t' << iter.key() << '\n';
      iter.next();
    }
  }
  {
    auto iter = LinearIterator{tab3};
    std::cout << "tab3:\n";
    while (!iter.at_end()) {
      std::cout << '\t' << iter.key() << '\n';
      iter.next();
    }
  }
  {
    auto join = LeapFrogJoin<int>{tab1, tab2, tab3};
    std::cout << "join(tab1, tab2, tab3):\n";
    while (!join.at_end()) {
      std::cout << '\t' << join.key() << '\n';
      join.next();
    }
  }
}