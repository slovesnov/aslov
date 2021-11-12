/*
 * CairoSurface.h
 *
 *  Created on: 07.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef NOGTK

#ifndef ASLOV_CAIROSURFACE_H_
#define ASLOV_CAIROSURFACE_H_

#include <gtk/gtk.h>
#include "CRect.h"

class CairoSurface {
	cairo_t *m_cairo;
	cairo_surface_t *m_surface;
	void init();
	void free();
public:
	CairoSurface();
	CairoSurface(int width, int height);
	CairoSurface(CSize const &size);
	CairoSurface & operator=(const CairoSurface&) = delete;
	CairoSurface(const CairoSurface&) = delete;

	void create(int width, int height);
	void create(CSize const &size);
	void create(std::string const &path);
	virtual ~CairoSurface();
	cairo_t* cairo();
	cairo_surface_t* surface();

	int width()const;
	int height()const;
	CSize size()const;

	void copy(CairoSurface &dest);
	void copy(CairoSurface &dest, CRect const &r);
	void copy(CairoSurface &dest, int destx, int desty, int width, int height);
	void copy(CairoSurface &dest, int destx, int desty, int width, int height,
			int sourcex, int sourcey);

/*
	cairo_t*& cairo1(){
		return m_cairo;
	}

	cairo_surface_t*& surface1(){
		return m_surface;
	}
*/

};

#endif /* ASLOV_CAIROSURFACE_H_ */
#endif
