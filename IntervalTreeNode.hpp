/**
 * \file  IntervalTreeNoe.hpp
 * \brief TODO
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

#ifndef INTERVALTREENODE_HPP_
#define INTERVALTREENODE_HPP_

// stl includes
#include <algorithm>
#include <utility>
#include <string>
#include <vector>


/******************************************************************************
 * Class definitions and prototypes
 *****************************************************************************/

/**
 * \brief class for interval tree errors
 */
class IntervalTreeError: public std::exception {
 public:
  explicit IntervalTreeError(const char* msg) { this->msg = msg; }
  virtual const char* what() const throw() { return this->msg; }
 private:
  const char* msg;
};

/**
 * \brief Functor for use in sorting Intervals of type <T> using a comparison
 *        function provided at construction time
 */
template <class T, class R>
class IntervalComparator {
 public:
  explicit IntervalComparator(R (*compFunc)(const T&)) {
    this->compFunc = compFunc;
  }
  bool operator()(T i1, T i2) {
    return this->compFunc(i1) < this->compFunc(i2);
  }
 private:
  R (*compFunc)(const T&);
};

/**
 * \brief Stores a set of intervals sorted by start and end
 */
template <class T, class R>
class IntervalTreeNode {
 public:
  IntervalTreeNode();
  IntervalTreeNode(const std::vector<T>, const double mid,
                   R (*getStart)(const T&), R (*getEnd)(const T&));
  IntervalTreeNode(const IntervalTreeNode<T, R> &n);
  ~IntervalTreeNode();
  IntervalTreeNode<T, R>& operator=(const IntervalTreeNode<T, R>& other);
  void swap(IntervalTreeNode<T, R>& other);
  std::string toString();

  std::vector<T> starts;
  std::vector<T> ends;
  double mid;
 private:
  R (*getStart)(const T&);
  R (*getEnd)(const T&);
};


/******************************************************************************
 * IntervalTreeNode class implementation
 *****************************************************************************/
/**
 * \brief default constructor
 */
template <class T, class R>
IntervalTreeNode<T, R>::IntervalTreeNode() : mid(0), getStart(NULL),
                                             getEnd(NULL),
                                             starts(std::vector<T>()),
                                             ends(std::vector<T>())  {;}

/**
 * \brief IntervalTreeNode constructor
 * \param intervals TODO
 */
template <class T, class R>
IntervalTreeNode<T, R>::IntervalTreeNode(const std::vector<T> intervals,
                                         const double mid,
                                         R (*getStart)(const T&),
                                         R (*getEnd)(const T&)) {
  this->getStart = getStart;
  this->getEnd = getEnd;
  IntervalComparator<T, R> startComp = IntervalComparator<T, R>(getStart);
  IntervalComparator<T, R> endComp = IntervalComparator<T, R>(getEnd);
  this->starts = std::vector<T> (intervals);
  sort(this->starts.begin(), this->starts.end(), startComp);
  this->ends = std::vector<T> (intervals);
  sort(this->ends.begin(), this->ends.end(), endComp);
  this->mid = mid;
}

/**
 * \brief Copy constructor
 */
template <class T, class R>
IntervalTreeNode<T, R>::IntervalTreeNode(const IntervalTreeNode<T, R> &n) {
  // shallow copy is fine here
  this->starts = n.starts;
  this->ends = n.ends;
  this->mid = n.mid;
  this->getStart = n.getStart;
  this->getEnd = n.getEnd;
}

/**
 * \brief destructor
 */
template <class T, class R>
IntervalTreeNode<T, R>::~IntervalTreeNode() {
  // nothing special to do..
}

/**
* \brief assignment; have to be careful here, since we have pointer members.
*/
template <class T, class R>
IntervalTreeNode<T, R>&
IntervalTreeNode<T, R>::operator=(const IntervalTreeNode<T, R>& other) {
  IntervalTreeNode<T, R> tmp(other);
  this->swap(tmp);
  return *this;
}

/**
* \brief swap the contents of this node with another one.
*/
template <class T, class R>
void
IntervalTreeNode<T, R>::swap(IntervalTreeNode<T, R>& other) {
  std::swap(this->mid, other.mid);
  this->starts.swap(other.starts);
  this->ends.swap(other.ends);
  std::swap(this->getStart, other.getStart);
  std::swap(this->getEnd, other.getEnd);
}

/**
 * \brief return a string representation of an IntervalTreeNode
 */
template <class T, class R>
std::string
IntervalTreeNode<T, R>::toString() {
  std::ostringstream s;
  s << "mid: " << this->mid << std::endl;
  s << "intervals sorted by start:" << std::endl;
  for (typename std::vector<T>::iterator it = this->starts.begin();
       it != this->starts.end(); it++) {
    s << "(" << this->getStart(*it) << " - " << this->getEnd(*it) << ")"
      << std::endl;
  }
  s << "intervals sorted by end:" << std::endl;
  for (typename std::vector<T>::iterator it = this->ends.begin();
       it != this->ends.end(); it++) {
      s << "(" << this->getStart(*it) << " - " << this->getEnd(*it) << ")"
        << std::endl;
  }
  return s.str();
}

#endif  // INTERVALTREENODE_HPP_
