/*
 * Pixbuf.cpp
 *
 *  Created on: 09.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef NOGTK

#include <cassert>

#include "aslov.h"
#include "Pixbuf.h"

Pixbuf::Pixbuf() {
	p=nullptr;
}

Pixbuf::~Pixbuf() {
	free();
}

Pixbuf::Pixbuf(const char *path) {
	p=nullptr;
	set(path);
}

Pixbuf::Pixbuf(const std::string &path) {
	p=nullptr;
	set(path);
}

void Pixbuf::free() {
	if (p) {
		g_object_unref(p);
		p = nullptr;
	}
}

int Pixbuf::width() const {
	return gdk_pixbuf_get_width(p);
}

int Pixbuf::height() const {
	return gdk_pixbuf_get_height(p);
}

CSize Pixbuf::size() const {
	return {width(),height()};
}

void Pixbuf::set(const char *path) {
	p=gdk_pixbuf_new_from_file(path, NULL);
	assert(p);
}

void Pixbuf::set(const std::string &path) {
	set(path.c_str());
}

#endif
