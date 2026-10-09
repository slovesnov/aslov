/*
 * CairoSurface.cpp
 *
 *  Created on: 07.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#ifndef NOGTK

#include "CairoSurface.h"
#include "Pixbuf.h"
#include <cassert>


CairoSurface::CairoSurface() {}

CairoSurface::CairoSurface(int width, int height) { create(width, height); }

CairoSurface::CairoSurface(const CPoint &size) { CairoSurface(size.x, size.y); }

void CairoSurface::create(int width, int height) {
  m_surface.reset(
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height));
  m_cairo.reset(cairo_create(m_surface.get()));
}

void CairoSurface::create(const CPoint &size) { create(size.x, size.y); }

void CairoSurface::create(const std::string &path) {
  // from https://www.cairographics.org/manual/cairo-PNG-Support.html
  // should be utf8 path
  m_surface.reset(cairo_image_surface_create_from_png(path.c_str()));
  m_cairo.reset(cairo_create(m_surface.get()));
}

int CairoSurface::width() const {
  return cairo_image_surface_get_width(m_surface.get());
}

int CairoSurface::height() const {
  return cairo_image_surface_get_height(m_surface.get());
}

CPoint CairoSurface::size() const { return {width(), height()}; }

void CairoSurface::copy(CairoSurface &dest) {
  cairo_t *d = dest;
  cairo_set_source_surface(d, m_surface.get(), 0, 0);
  cairo_paint(d);
}

void CairoSurface::copy(CairoSurface &dest, const CRect &r) {
  copy(dest, r.left, r.top, r.width(), r.height(), r.left, r.top);
}

void CairoSurface::copy(CairoSurface &dest, int destx, int desty, int width,
                        int height) {
  copy(dest, destx, desty, width, height, destx, desty);
}

void CairoSurface::copy(CairoSurface &dest, int destx, int desty, int width,
                        int height, int sourcex, int sourcey) {
  cairo_t *d = dest;
  copyToCairo(d, destx, desty, width, height, sourcex, sourcey);
  //	cairo_set_source_surface(d, m_surface, destx - sourcex, desty -
  //sourcey); 	cairo_rectangle(d, destx, desty, width, height); 	cairo_fill(d);
}

void CairoSurface::copyToCairo(cairo_t *cr, int destx, int desty, int width,
                               int height, int sourcex, int sourcey) {
  cairo_set_source_surface(cr, m_surface.get(), destx - sourcex, desty - sourcey);
  cairo_rectangle(cr, destx, desty, width, height);
  cairo_fill(cr);
}

CairoSurface::operator cairo_t *() { return m_cairo.get(); }

CairoSurface::operator cairo_surface_t *() { return m_surface.get(); }

void CairoSurface::savePng(const std::string &path) const {
  cairo_surface_write_to_png(m_surface.get(), path.c_str());
}

GdkPixbuf *CairoSurface::toPixbuf(int startx, int starty, int width,
                                  int height) {
  return gdk_pixbuf_get_from_surface(m_surface.get(), startx, starty, width, height);
}

GdkPixbuf *CairoSurface::toPixbuf() {
  return toPixbuf(0, 0, width(), height());
}

#endif // NOGTK
