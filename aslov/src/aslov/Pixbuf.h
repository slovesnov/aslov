/*
 * Pixbuf.h
 *
 *  Created on: 09.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#ifndef NOGTK

#pragma once

#include "CPoint.h"
#include "SafeGtkTypes.h"

class Pixbuf {
	SafePixbuf p;
public:
	Pixbuf();
	Pixbuf(std::string_view path);
	Pixbuf(GdkPixbuf *pb);
	void set(std::string_view path);
	void set(GdkPixbuf *pb);

	void operator=(std::string_view path);
	void operator=(GdkPixbuf *pb);

	operator GdkPixbuf*();

	Pixbuf& operator=(const Pixbuf&) = delete;
	Pixbuf(const Pixbuf&) = delete;

	int width() const;
	int height() const;
	CPoint size() const;

	void createRGB(int width, int height);
	void savePng(std::string const &path) const;
	void saveJpg(std::string const &path, int quality = 100) const;
	GdkPixbuf* saturate(float f) const;
};

#endif
