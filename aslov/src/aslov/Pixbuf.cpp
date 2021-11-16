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

Pixbuf::Pixbuf(GdkPixbuf* pb){
	p=pb;
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
	free();
	p=gdk_pixbuf_new_from_file(path, NULL);
	assert(p);
}

void Pixbuf::set(const std::string &path) {
	set(path.c_str());
}

void Pixbuf::set(GdkPixbuf* pb){
	free();
	p=pb;
}


void Pixbuf::operator =(const char *path) {
	set(path);
}

void Pixbuf::operator =(const std::string &path) {
	set(path);
}

Pixbuf::operator GdkPixbuf*() {
	return p;
}

void Pixbuf::operator =(GdkPixbuf *pb) {
	set(pb);
}

void Pixbuf::createRGB(int width, int height) {
	free();
	p=gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width, height);
}

void Pixbuf::savePng(const std::string &path)const {
	gdk_pixbuf_save(p, path.c_str(), "png", NULL, NULL);
}

void Pixbuf::saveJpg(const std::string &path, int quality/*=100*/)const {
	gdk_pixbuf_save(p, path.c_str(), "jpeg", NULL, "quality",
			"100", NULL);
}

GdkPixbuf* Pixbuf::saturate(float f) const {
	GdkPixbuf*pb= gdk_pixbuf_copy(p);
	//desaturate image
	gdk_pixbuf_saturate_and_pixelate(p, pb, f, false);
	return pb;
}

#endif
