/*
 * CairoSurface.h
 *
 *  Created on: 07.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef ASLOV_CAIROSURFACE_H_
#define ASLOV_CAIROSURFACE_H_

#ifndef NOGTK
#include <gtk/gtk.h>
#include "CSize.h"

class CairoSurface {
	cairo_t *m_cairo;
	cairo_surface_t *m_surface;
	void init();
	void free();
public:
	CairoSurface();
	CairoSurface(int width, int height);
	CairoSurface(CSize const &size);
	void create(int width, int height);
	void create(CSize const &size);
	void create(std::string const &path);
	virtual ~CairoSurface();
	cairo_t* cairo();
	cairo_surface_t* surface();

	int width()const;
	int height()const;
	CSize size()const;
/*
	cairo_t*& cairo1(){
		return m_cairo;
	}

	cairo_surface_t*& surface1(){
		return m_surface;
	}
*/

};
#endif //NOGTK

#endif /* ASLOV_CAIROSURFACE_H_ */
