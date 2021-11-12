/*
 * Pixbuf.h
 *
 *  Created on: 09.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef NOGTK

#ifndef ASLOV_PIXBUF_H_
#define ASLOV_PIXBUF_H_

#include <gtk/gtk.h>
#include <string>
#include "CSize.h"

class Pixbuf {
	GdkPixbuf*p;
	void free();
public:
	Pixbuf();
	Pixbuf(const char* path);
	Pixbuf(std::string const &path);
	void set(const char* path);
	void set(std::string const &path);

	virtual ~Pixbuf();

	Pixbuf & operator=(const Pixbuf&) = delete;
	Pixbuf(const Pixbuf&) = delete;

	int width()const;
	int height()const;
	CSize size()const;
};

#endif /* ASLOV_PIXBUF_H_ */
#endif
