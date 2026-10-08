/*
 * CRect.cpp
 *
 *  Created on: 03.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#ifndef NOGTK

#include "CRect.h"

std::string CRect::toString() const {
  return std::to_string(left) + "," + std::to_string(top) + " " +
         std::to_string(right) + "," + std::to_string(bottom) + " size" +
         std::to_string(width()) + "x" + std::to_string(height());
}

std::ostream &operator<<(std::ostream &os, const CRect &p) {
  return os << p.left << p.top << p.right << p.bottom;
}

std::istream &operator>>(std::istream &is, CRect &p) {
  return is >> p.left >> p.top >> p.right >> p.bottom;
}

#endif
