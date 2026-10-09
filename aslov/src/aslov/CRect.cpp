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

CRect::CRect() { left = top = right = bottom = 0; }

CRect::CRect(CPoint p, CPoint s) {
  left = p.x;
  top = p.y;
  right = left + s.x;
  bottom = top + s.y;
}

CRect::CRect(int _left, int _top, int _right, int _bottom) {
  init(_left, _top, _right, _bottom);
}

const CRect& CRect::operator=(const CRect &r) {
  left = r.left;
  top = r.top;
  right = r.right;
  bottom = r.bottom;
  return *this;
}

void CRect::init(int _left, int _top, int _right, int _bottom) {
  left = _left;
  top = _top;
  right = _right;
  bottom = _bottom;
}

void CRect::join(const CRect &r) {
  if (r.left < left) {
    left = r.left;
  }
  if (r.top < top) {
    top = r.top;
  }
  if (r.right > right) {
    right = r.right;
  }
  if (r.bottom > bottom) {
    bottom = r.bottom;
  }
}

int CRect::width() const { return right - left; }
int CRect::height() const { return bottom - top; }
CPoint CRect::size() const { return {width(), height()}; }
CPoint CRect::centerPoint() const {
  return CPoint((left + right) / 2, (top + bottom) / 2);
}
CPoint CRect::topLeft() const { return CPoint(left, top); }

bool CRect::in(GdkEventButton *p) { return in(p->x, p->y); }
bool CRect::in(double x, double y) {
  return x >= left && x < right && y >= top && y < bottom;
}

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
