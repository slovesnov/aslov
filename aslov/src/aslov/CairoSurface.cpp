/*
 * CairoSurface.cpp
 *
 *  Created on: 07.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include <cassert>
#include "CairoSurface.h"

#ifndef NOGTK
void CairoSurface::init() {
	m_cairo = nullptr;
	m_surface = nullptr;
}

void CairoSurface::free() {
	if (m_cairo) {
		cairo_destroy(m_cairo);
		m_cairo=nullptr;
	}
	if (m_surface) {
		cairo_surface_destroy(m_surface);
		m_surface=nullptr;
	}
}

CairoSurface::CairoSurface() {
	init();
}

CairoSurface::CairoSurface(int width, int height) {
	init();
	create(width, height);
}

CairoSurface::CairoSurface(const CSize &size) {
	CairoSurface(size.cx, size.cy);
}

CairoSurface::~CairoSurface() {
	free();
}

void CairoSurface::create(int width, int height) {
	free();
	m_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	m_cairo = cairo_create(m_surface);
}

void CairoSurface::create(const CSize &size) {
	create(size.cx, size.cy);
}

cairo_t* CairoSurface::cairo() {
	return m_cairo;
}

cairo_surface_t* CairoSurface::surface() {
	return m_surface;
}

void CairoSurface::create(const std::string &path) {
	//from https://www.cairographics.org/manual/cairo-PNG-Support.html
	//should be utf8 path
	free();
	m_surface=cairo_image_surface_create_from_png(path.c_str());
	m_cairo = cairo_create(m_surface);
}

int CairoSurface::width() const {
	return cairo_image_surface_get_width(m_surface);
}

int CairoSurface::height() const {
	return cairo_image_surface_get_height(m_surface);
}

CSize CairoSurface::size() const {
	return {width(),height()};
}

#endif //NOGTK
