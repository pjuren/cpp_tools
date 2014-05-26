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

// TinyTest includes
#include "TinyTest.hpp"

// local includes
#include "IntervalTree.hpp"

// stl includes
#include <string>
#include <sstream> 
#include <iostream>
#include <assert.h>

// bring the following into the local name-space
using std::cerr;
using std::endl;
using std::string;
using std::vector;

/**
 * \brief a test class for use in testing the interval tree class
 */
class TestInterval {
public:
    TestInterval(size_t start, size_t end) {this->start=start; this->end=end;};
    const size_t getStart() const {return this->start;}
    const size_t getEnd() const {return this->end;}
    bool operator==(const TestInterval other) const {
      return ((this->start == other.start) && (this->end == other.end));
    }
    bool operator!=(const TestInterval other) const {
      return ((this->start != other.start) || (this->end != other.end));
    }
    static bool compare(TestInterval i1, TestInterval i2){
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
    cases.back().push_back(TestInterval(10,20));
    cases.back().push_back(TestInterval(40,75));
    cases.back().push_back(TestInterval(78,85));
    cases.back().push_back(TestInterval(89,94));
    cases.back().push_back(TestInterval(96,97));
    cases.back().push_back(TestInterval(99,99));
  }

  vector< vector<TestInterval> > cases;
};

/**
 * \brief Test that attempted construction of an IntervalTree object with an
 *        empty set of intervals throws an IntervalTreeException
 */
TEST(testEmptyThrowsException) {
  // TODO -- can be done more elegantly once TinyTest supports tests for
  //         exceptions
  bool exceptionHappened = false;
  try {
    IntervalTree<TestInterval,size_t> t (IntervalFactory::getTestCase(0),
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
  IntervalTree<TestInterval,size_t> t (IntervalFactory::getTestCase(1),
                                       &getStartTest, &getEndTest);
  vector<TestInterval> expectedAns;
  expectedAns.push_back(TestInterval(40,75));
  EXPECT_EQUAL_STL_CONTAINER(t.intersectingPoint(40), expectedAns);
}

/**
 * \brief Test that we correctly find the set of regions that intersect a point
 *        when the point lays on the end of one of the intervals
 */
TEST(testIntersectingPointEnd) {
  IntervalTree<TestInterval,size_t> t (IntervalFactory::getTestCase(1),
                                       &getStartTest, &getEndTest);
  vector<TestInterval> expectedAns;
  expectedAns.push_back(TestInterval(40,75));
  EXPECT_EQUAL_STL_CONTAINER(t.intersectingPoint(75), expectedAns);
}



/*

void 
TestIntervalTree::testIntersectingInterval() {
	cerr << "\ttesting intersecting interval...";
	double r = ((double) rand()) / RAND_MAX;
	double s = (r * (MAX_INTERVAL - MIN_INTERVAL)) + MIN_INTERVAL;
	r = ((double) rand()) / RAND_MAX;
	double e = (r * (MAX_INTERVAL - s)) + s;
			
	// do it the slow way...
	vector<TestInterval> correctAnswer, actualAnswer;
	for (vector<TestInterval>::iterator it = this->intervals.begin(); it != this->intervals.end(); it++) {
		if (((s >= it->getStart()) && (s <= it->getEnd())) ||
				((e >= it->getStart()) && (e <= it->getEnd())) ||
				((s <= it->getStart()) && (e >= it->getEnd())))
			correctAnswer.push_back(*it);
	}
	sort(correctAnswer.begin(), correctAnswer.end(), TestInterval::compare);
	
	// now use the tree
	actualAnswer = this->theTree->intersectingInterval(s,e);
	sort(actualAnswer.begin(), actualAnswer.end(), TestInterval::compare);
	
	// compare -- we print out some debugging info here if things go wrong..
	assert(actualAnswer == correctAnswer);
	cerr << "[passed]" << endl;
}


void 
TestIntervalTree::testEndPoints() {
	cerr << "\ttesting end points...";
	TestInterval interval = TestInterval(5.0,10.0);
	vector<TestInterval> intervals;
	intervals.push_back(interval);
	IntervalTree<TestInterval,double> tree = IntervalTree<TestInterval,double>(intervals, &getStartTest, &getEndTest);
	
	assert(tree.intersectingPoint(4.0).size() == 0);
	assert(tree.intersectingPoint(5.0).size() == 1);
	assert(tree.intersectingPoint(6.0).size() == 1);
	assert(tree.intersectingPoint(9.0).size() == 1);
	assert(tree.intersectingPoint(10.0).size() == 1);
	assert(tree.intersectingPoint(11.0).size() == 0);
	cerr << "[passed]" << endl;
}
	
void 
TestIntervalTree::testSquash() {
	cerr << "\ttesting squash...";
	vector<TestInterval> squashed = this->theTree->squash();
		
	sort(this->intervals.begin(), this->intervals.end(), TestInterval::compare);
	sort(squashed.begin(), squashed.end(), TestInterval::compare);
	assert(this->intervals == squashed);
	cerr << "[passed]" << endl;
}


*/


