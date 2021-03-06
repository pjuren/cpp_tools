/**
 * \section copyright Copyright Details
 * Copyright (C) 2010-2014 University of Southern California and Philip J. Uren
 *
 * \file  IntervalTree.hpp
 * \brief An Interval Tree is a data structure for quickly determining the set
 *        of intervals that intersect a given point or interval. These classes
 *        are templates with two parameters, T and R. T is the type of the
 *        objects being stored in the tree and R is the type of their indices
 *        (i.e. int, or long would be a common choice). In theory the only
 *        restrictions are that R is ordinal (specifically, must define +, -
 *        and /). T can be any type as long as the user can provide pointers to
 *        functions 'R getStart(T)' and 'R getEnd(T)' (trivial in most cases,
 *        and should be do-able with a functor).
 *
 * \authors Philip J. Uren
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

#ifndef INTERVALTREE_HPP_
#define INTERVALTREE_HPP_

// stl includes
#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <utility>

// local includes
#include "IntervalTreeNode.hpp"

/******************************************************************************
 * Class definitions and prototypes
 *****************************************************************************/

/**
 * \brief The actual IntervalTree class
 */
template <class T, class R>
class IntervalTree {
 public:
  IntervalTree();
  explicit IntervalTree(bool openEnded) : openEnded(openEnded) {}
  IntervalTree(const std::vector<T> &intervals, R (*getStart)(const T&),
               R (*getEnd)(const T&), const bool openEnded = false);
  IntervalTree(const IntervalTree<T, R> &t);
  ~IntervalTree();
  IntervalTree<T, R>& operator=(const IntervalTree<T, R>& other);
  void swap(IntervalTree<T, R>& other);

  // inspectors
  const std::vector<T> intersectingPoint(const R point) const;
  const std::vector<T> intersectingInterval(const R start, const R end) const;
  const std::vector<T> squash() const;
  const int size() const;
  const std::string toString() const;

  // constants
  static const bool OPEN_ENDED = true;

 private:
  IntervalTreeNode<T, R>* data;
  IntervalTree<T, R>* left;
  IntervalTree<T, R>* right;
  R (*getStart)(const T&);
  R (*getEnd)(const T&);
  bool openEnded;
};



/******************************************************************************
 * IntervalTree class implementation
 *****************************************************************************/

/**
 * \brief default constructor
 */
template <class T, class R>
IntervalTree<T, R>::IntervalTree() : data(NULL), left(NULL), right(NULL),
                                     getStart(NULL), getEnd(NULL) {;}

/**
 * \brief Constructor for IntervalTree.
 * \param intervals list of intervals, doesn't need to be sorted in any way.
 * \throws IntervalTreeError if no intervals are provided
 */
template <class T, class R>
IntervalTree<T, R>::IntervalTree(const std::vector<T> &intervals,
                                 R (*getStart)(const T&), R (*getEnd)(const T&),
                                 const bool openEnded) {
  this->left = NULL;
  this->right = NULL;
  this->getStart = getStart;
  this->getEnd = getEnd;
  this->openEnded = openEnded;
  IntervalComparator<T, R> startComp = IntervalComparator<T, R>(getStart);

  // because we're going to sort them, and want to force const on paramater
  std::vector<T> cIntervals = std::vector<T>(intervals);

  // can't build a tree with no intervals...
  if (intervals.size() <= 0)
    throw IntervalTreeError("Interval tree constructor got empty set of "
                            "intervals");

  // sort the list by start index (this is arbitrary and just lets us try to
  // split it evenly)
  sort(cIntervals.begin(), cIntervals.end(), startComp);

  // pick a mid-point and split the list
  T midInt = cIntervals[intervals.size() / 2];
  R mid = ((this->getEnd(midInt) - this->getStart(midInt)) / 2)
            + this->getStart(midInt);

  std::vector<T> here, lt, rt;
  for (typename std::vector<T>::iterator it = cIntervals.begin();
       it != cIntervals.end(); it++) {
    // place all intervals that end before <mid> into the left subtree
    if (this->getEnd(*it) < mid) {
      lt.push_back(*it);
    } else {
      if (this->getStart(*it) > mid) {
        // place all intervals that begin after <mid> into the right subtree
        rt.push_back(*it);
      } else {
        // everything else must overlap mid, so we keep it here
        here.push_back(*it);
      }
    }
  }

  if (here.size() <= 0) {
    std::ostringstream msg;
    msg << "fatal error: picked mid point at " << mid
        << " but this failed to intersect anything!";
    throw IntervalTreeError(msg.str().c_str());
  }

  if (lt.size() > 0) this->left = new IntervalTree(lt, getStart, getEnd,
                                                   openEnded);
  if (rt.size() > 0) this->right = new IntervalTree(rt, getStart, getEnd,
                                                    openEnded);
  this->data = new IntervalTreeNode<T, R>(here, mid, this->getStart,
                                          this->getEnd);
}

/**
 * \brief Copy constructor
 */
template <class T, class R>
IntervalTree<T, R>::IntervalTree(const IntervalTree<T, R> &t) {
  // deep copy data tree node
  if (t.data == NULL)
    this->data = NULL;
  else
    this->data = new IntervalTreeNode<T, R>(*(t.data));

  // deep copy left and right branches
  if (t.left == NULL)
    this->left = NULL;
  else
    this->left = new IntervalTree(*(t.left));
  if (t.right == NULL)
    this->right = NULL;
  else
    this->right = new IntervalTree(*(t.right));

  // function pointers can be copied shallow
  this->getStart = t.getStart;
  this->getEnd = t.getEnd;

  // open-ended is shallow
  this->openEnded = t.openEnded;
}

/**
 * \brief Destructor for IntervalTree
 */
template <class T, class R>
IntervalTree<T, R>::~IntervalTree() {
  if (this->data != NULL) delete this->data;
  if (this->left != NULL) delete this->left;
  if (this->right != NULL) delete this->right;
}

/**
 * \brief assignment operator; need to be careful here, since we have
 *				pointer members. Swap idiom should work for copy-assignment
 */
template <class T, class R>
IntervalTree<T, R>&
IntervalTree<T, R>::operator=(const IntervalTree<T, R>& other) {
  IntervalTree<T, R> tmp(other);
  this->swap(tmp);
  return *this;
}

/**
 * \brief swap the contents of this IntervalTree with another
 */
template <class T, class R>
void
IntervalTree<T, R>::swap(IntervalTree<T, R>& other) {
  std::swap(this->data, other.data);
  std::swap(this->left, other.left);
  std::swap(this->right, other.right);
  std::swap(this->getStart, other.getStart);
  std::swap(this->getEnd, other.getEnd);
  std::swap(this->openEnded, other.openEnded);
}

/**
 * \brief given a point, determine which set of intervals in the tree are
 *        intersected.
 * \param point the point of intersection to test against
 * \return vector of intersected intervals
 */
template <class T, class R>
const std::vector<T>
IntervalTree<T, R>::intersectingPoint(const R point) const {
  if (point > this->data->mid) {
    // we know all intervals in this->data begin before p (if they began
    // after p, they would have not included mid) we just need to find
    // those that end after p
    std::vector<T> endAfterP;
    for (auto it = this->data->ends.rbegin();
         it != this->data->ends.rend(); it++) {
      if (((!this->openEnded) && (this->getEnd(*it) >= point)) ||
          ((this->openEnded) && (this->getEnd(*it) > point))) {
        endAfterP.push_back(*it);
      } else {
        break;
      }
    }

    if (this->right != NULL) {
      std::vector<T> descendRight = this->right->intersectingPoint(point);
      for (typename std::vector<T>::iterator it = descendRight.begin();
           it != descendRight.end(); it++) {
        endAfterP.push_back(*it);
      }
    }
    return endAfterP;
  }
  if (point < this->data->mid) {
    // we know all intervals in this->data end after p (if they ended before
    // p, they would have not included mid) we just need to find those that
    // start before p
    std::vector<T> startBeforeP;
    for (typename std::vector<T>::iterator it = this->data->starts.begin();
         it != this->data->starts.end(); it++) {
      if (this->getStart(*it) <= point)
        startBeforeP.push_back(*it);
      else
        break;
    }

    if (this->left != NULL) {
      std::vector<T> descendLeft = this->left->intersectingPoint(point);
      for (typename std::vector<T>::iterator it = descendLeft.begin();
           it != descendLeft.end(); it++) {
        startBeforeP.push_back(*it);
      }
    }
    return startBeforeP;
  }

  // must be a perfect match, since point is neither > nor < mid
  assert(point == this->data->mid);
  return this->data->ends;
}

/**
 * \brief given an interval, determine which set of intervals in the tree are
 *        intersected.
 * \param start TODO
 * \param end TODO
 * \return: vector of intersected intervals
 */
template <class T, class R>
const std::vector<T>
IntervalTree<T, R>::intersectingInterval(const R start, const R end) const {
  // find all intervals in this node that intersect start and end
  std::vector<T> intersecting;
  for (typename std::vector<T>::iterator it = this->data->starts.begin();
       it != this->data->starts.end(); it++) {
      if (this->openEnded) {
        if (((this->getStart(*it) >= start) && (this->getStart(*it) < end)) ||
              ((this->getEnd(*it) > start) && (this->getEnd(*it) < end)) ||
              ((start >= this->getStart(*it) && start < this->getEnd(*it))) ||
              ((end > this->getStart(*it)) && (end < this->getEnd(*it)))) {
            intersecting.push_back(*it);
        }
      } else {
        if (((this->getStart(*it) >= start) && (this->getStart(*it) <= end)) ||
              ((this->getEnd(*it) >= start) && (this->getEnd(*it) <= end)) ||
              ((start >= this->getStart(*it) && start <= this->getEnd(*it))) ||
              ((end >= this->getStart(*it)) && (end <= this->getEnd(*it)))) {
            intersecting.push_back(*it);
        }
      }
  }

  // process left subtree (if we have one) if the requested interval begins
  // before mid
  if ((this->left != NULL) && (start <= this->data->mid)) {
    std::vector<T> descendLeft = this->left->intersectingInterval(start, end);
    for (typename std::vector<T>::iterator it = descendLeft.begin();
         it != descendLeft.end(); it++) {
      intersecting.push_back(*it);
    }
  }

  // process right subtree (if we have one) if the requested interval
  // ends after mid
  if ((this->right != NULL) && (end >= this->data->mid)) {
    std::vector<T> desendRight = this->right->intersectingInterval(start, end);
    for (typename std::vector<T>::iterator it = desendRight.begin();
      it != desendRight.end(); it++) {
      intersecting.push_back(*it);
    }
  }

  return intersecting;
}

/**
 * \brief squash the tree -- i.e. return a vector of all items in the tree
 * \note this is not destructive, the original tree remains
 */
template <class T, class R>
const std::vector<T>
IntervalTree<T, R>::squash() const {
  std::vector<T> res;
  res.insert(res.end(), this->data->starts.begin(), this->data->starts.end());
  if (this->left != NULL) {
    std::vector<T> l = this->left->squash();
    res.insert(res.end(), l.begin(), l.end());
  }
  if (this->right != NULL) {
    std::vector<T> r = this->right->squash();
    res.insert(res.end(), r.begin(), r.end());
  }
  return res;
}

/**
 * \brief get the number of items in the tree.
 *
 * This is a little expensive at the moment, since it requires a full traversal
 * of the tree
 */
template <class T, class R>
const int
IntervalTree<T, R>::size() const {
  int res = this->data->starts.size();
  if (this->left != NULL) res += this->left->size();
  if (this->right != NULL) res += this->right->size();
  return res;
}

/**
 * \brief return a string representation of an IntervalTree
 */
template <class T, class R>
const std::string
IntervalTree<T, R>::toString() const {
  std::string left = "<EMPTY>", right = "<EMPTY>";
  if (this->left != NULL) left = this->left->toString();
  if (this->right != NULL) right = this->right->toString();
  return this->data->toString() + "\n** left ** " + left +
         "\n** right ** " + right;
}

#endif  // INTERVALTREE_HPP_
