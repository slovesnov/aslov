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
  CPoint();
  CPoint(int _x, int _y);
  CPoint(GdkEventButton *p);
  void operator+=(const CPoint &p);
  void operator-=(const CPoint &p);
  bool operator==(const CPoint &p) const;
  bool operator!=(const CPoint &p) const;
  std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const CPoint &p);
std::istream &operator>>(std::istream &is, CPoint &p);

#endif
