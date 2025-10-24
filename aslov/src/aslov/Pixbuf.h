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

#ifndef ASLOV_PIXBUF_H_
#define ASLOV_PIXBUF_H_

#include <gtk/gtk.h>
#include <string>
#include "CPoint.h"

class Pixbuf {
	GdkPixbuf *p;
	void free();
public:
	Pixbuf();
	Pixbuf(const char *path);
	Pixbuf(std::string const &path);
	Pixbuf(GdkPixbuf *pb);
	void set(const char *path);
	void set(std::string const &path);
	void set(GdkPixbuf *pb);

	void operator=(const char *path);
	void operator=(std::string const &path);
	void operator=(GdkPixbuf *pb);

	operator GdkPixbuf*();

	virtual ~Pixbuf();

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

#endif /* ASLOV_PIXBUF_H_ */
#endif
