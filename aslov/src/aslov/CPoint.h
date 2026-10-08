/*
 * CPoint.h
 *
 *       Created on: 17.09.2014
 *           Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#pragma once
#ifndef NOGTK

#include <gtk/gtk.h>
#include <iostream>

class CPoint {
public:
  int x, y;
  CPoint() { x = y = 0; }
  CPoint(int _x, int _y) {
    x = _x;
    y = _y;
  }
#if GTK_MAJOR_VERSION == 3
  CPoint(GdkEventButton *p) {
    x = p->x;
    y = p->y;
  }
#endif
  void operator+=(const CPoint &p) {
    x += p.x;
    y += p.y;
  }
  void operator-=(const CPoint &p) {
    x -= p.x;
    y -= p.y;
  }

  bool operator==(const CPoint &p) const { return x == p.x && y == p.y; }

  bool operator!=(const CPoint &p) const { return !(operator==(p)); }

  std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const CPoint &p);
std::istream &operator>>(std::istream &is, CPoint &p);

#endif
