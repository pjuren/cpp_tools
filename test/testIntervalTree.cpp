/**
 * \file  TestIntervalTree.cpp
 * \brief Unit tests for the interval tree class
 *
 * \authors Philip J. Uren
 *
 * \section copyright Copyright Details
 * Copyright (C) 2010-2014 University of Southern California and Philip J. Uren
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 */

// stl includes
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include <unordered_map>

// TinyTest includes
#include "TinyTest.hpp"

// local includes
#include "IntervalTree.hpp"

// bring the following into the local name-space
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::unordered_map;

/**
 * \brief a test class for use in testing the interval tree class
 */
class TestInterval {
 public:
  TestInterval(size_t start, size_t end) {
    this->start = start; this->end = end;
  }
  const size_t getStart() const {return this->start;}
  const size_t getEnd() const {return this->end;}
  bool operator==(const TestInterval other) const {
    return ((this->start == other.start) && (this->end == other.end));
  }
  bool operator!=(const TestInterval other) const {
    return ((this->start != other.start) || (this->end != other.end));
  }
  static bool compare(TestInterval i1, TestInterval i2) {
    if (i1.getStart() == i2.getStart()) return i1.getEnd() < i2.getEnd();
    return i1.getStart() < i2.getStart();
  }
 private:
  size_t start;
  size_t end;
};

std::ostream& operator<<(std::ostream& os, const TestInterval& t) {
  os << "[" << t.getStart() << "," << t.getEnd() << "]";
  return os;
}

// functions for extracting start and end indices from TestInterval objects
static size_t getStartTest(const TestInterval &i) { return i.getStart(); }
static size_t getEndTest(const TestInterval &i) { return i.getEnd(); }

/**
 * \brief a factory class for producing sets of intervals for testing the
 *        interval tree class.
 */
class IntervalFactory {
 public:
  static const vector<TestInterval>& getTestCase(size_t n) {
    static IntervalFactory ifactory;
    return ifactory.cases[n];
  }

 private:
  IntervalFactory() {
    // test case 0 -- empty set
    cases.push_back(vector<TestInterval>());

    // test case 1 -- no overlapping intervals, intervals are in sorted order
    //                final interval has same start and end.
    cases.push_back(vector<TestInterval>());
    cases.back().push_back(TestInterval(10, 20));
    cases.back().push_back(TestInterval(40, 75));
    cases.back().push_back(TestInterval(78, 85));
    cases.back().push_back(TestInterval(89, 94));
    cases.back().push_back(TestInterval(96, 97));
    cases.back().push_back(TestInterval(99, 99));
  }

  vector< vector<TestInterval> > cases;
};

/**
 * \brief Test that attempted construction of an IntervalTree object with an
 *        empty set of intervals throws an IntervalTreeException
 */
TEST(testEmptyThrowsException) {
  // TODO(pjuren) -- can be done more elegantly once TinyTest supports tests
  //                 for exceptions
  bool exceptionHappened = false;
  try {
    IntervalTree<TestInterval, size_t> t(IntervalFactory::getTestCase(0),
                                         &getStartTest, &getEndTest);
  } catch (IntervalTreeError e) {
    exceptionHappened = true;
  }
  EXPECT_EQUAL(exceptionHappened, true);
}

/**
 * \brief Test that we correctly find the set of regions that intersect a point
 *        when the point lays on the start of one of the intervals
 */
TEST(testIntersectingPointStart) {
  IntervalTree<TestInterval, size_t> t(IntervalFactory::getTestCase(1),
                                       &getStartTest, &getEndTest);
  vector<TestInterval> expectedAns;
  expectedAns.push_back(TestInterval(40, 75));
  EXPECT_EQUAL_STL_CONTAINER(t.intersectingPoint(40), expectedAns);
}

/**
 * \brief Test that we correctly find the set of regions that intersect a point
 *        when the point lays on the end of one of the intervals
 */
TEST(testIntersectingPointEnd) {
  IntervalTree<TestInterval, size_t> t(IntervalFactory::getTestCase(1),
                                       &getStartTest, &getEndTest);
  vector<TestInterval> expectedAns;
  expectedAns.push_back(TestInterval(40, 75));
  EXPECT_EQUAL_STL_CONTAINER(t.intersectingPoint(75), expectedAns);
}

/**
 * \brief Test that IntervalTrees can be inserted into STL containers
 */
TEST(testSTLSafeInsert) {
  // have a for loop here so trees are created within it's scope and then
  // the originals are gone by the time we end the loop; this checks that
  // pointers are copied properly -- as the original objects will be destroyed.
  vector< IntervalTree<TestInterval, size_t> > v;
  for (size_t i = 0; i < 1; ++i) {
    IntervalTree<TestInterval, size_t> t(IntervalFactory::getTestCase(1),
                                         &getStartTest, &getEndTest);
    v.push_back(t);
  }

  vector<TestInterval> expectedAns;
  expectedAns.push_back(TestInterval(40, 75));
  EXPECT_EQUAL_STL_CONTAINER(v.back().intersectingPoint(40), expectedAns);
}

/**
 * \brief Test that IntervalTrees can be inserted into STL maps. This is a
 *        good test because it stretches the assignment operator in a way
 *        the vector case above doesn't.
 */
TEST(testSTLMapSafe) {
  typedef IntervalTree<TestInterval, size_t> ChainTree;
  typedef unordered_map<string, ChainTree> ChainTreeMap;

  ChainTreeMap chain_trees;
  const ChainTree c(IntervalFactory::getTestCase(1), &getStartTest,
                    &getEndTest);
  chain_trees["test"] = c;
  EXPECT_EQUAL(chain_trees["test"].size(), 6)
}
