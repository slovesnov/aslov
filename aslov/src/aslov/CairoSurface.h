/*
 * CairoSurface.h
 *
 *  Created on: 07.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#pragma once
#ifndef NOGTK

#include "CRect.h"
#include "SafeGtkTypes.h"

class CairoSurface {
  SafeCairo m_cairo;
  SafeCairoSurface m_surface;

public:
  CairoSurface();
  CairoSurface(int width, int height);
  CairoSurface(CPoint const &size);
  CairoSurface &operator=(const CairoSurface &) = delete;
  CairoSurface(const CairoSurface &) = delete;

  void create(int width, int height);
  void create(CPoint const &size);
  void create(std::string const &path);

  int width() const;
  int height() const;
  CPoint size() const;

  void copy(CairoSurface &dest);
  void copy(CairoSurface &dest, CRect const &r);
  void copy(CairoSurface &dest, int destx, int desty, int width, int height);
  void copy(CairoSurface &dest, int destx, int desty, int width, int height,
            int sourcex, int sourcey);
  void copyToCairo(cairo_t *cr, int destx, int desty, int width, int height,
                   int sourcex, int sourcey);

  operator cairo_t *();
  operator cairo_surface_t *();
  void savePng(std::string const &path) const;

  GdkPixbuf *toPixbuf(int startx, int starty, int width, int height);
  GdkPixbuf *toPixbuf();

};

#endif