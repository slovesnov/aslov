/*
 * CRect.h
 *
 *       Created on: 17.09.2014
 *           Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */
#pragma once
#ifndef NOGTK

#include "CPoint.h"

class CRect {
public:
  int left;
  int top;
  int right;
  int bottom;

  CRect();
  CRect(CPoint p, CPoint s);
  CRect(int _left, int _top, int _right, int _bottom);
  const CRect &operator=(const CRect &r);

  void init(int _left, int _top, int _right, int _bottom);
  void join(const CRect &r);

  int width() const;
  int height() const;
  CPoint size() const;
  CPoint centerPoint() const;
  CPoint topLeft() const;

  bool in(GdkEventButton *p);
  bool in(double x, double y);

  std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const CRect &p);
std::istream &operator>>(std::istream &is, CRect &p);

#endif
