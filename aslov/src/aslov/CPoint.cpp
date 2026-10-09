/*
 * CPoint.cpp
 *
 *  Created on: 08.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#ifndef NOGTK

#include "CPoint.h"

CPoint::CPoint() { x = y = 0; }
CPoint::CPoint(int _x, int _y) {
  x = _x;
  y = _y;
}
CPoint::CPoint(GdkEventButton *p) {
  x = p->x;
  y = p->y;
}
void CPoint::operator+=(const CPoint &p) {
  x += p.x;
  y += p.y;
}
void CPoint::operator-=(const CPoint &p) {
  x -= p.x;
  y -= p.y;
}
bool CPoint::operator==(const CPoint &p) const { return x == p.x && y == p.y; }
bool CPoint::operator!=(const CPoint &p) const { return !(operator==(p)); }

std::string CPoint::toString() const {
  return std::to_string(x) + "," + std::to_string(y);
}

std::ostream &operator<<(std::ostream &os, const CPoint &p) {
  return os << p.x << " " << p.y;
}

std::istream &operator>>(std::istream &is, CPoint &p) {
  return is >> p.x >> p.y;
}

#endif
