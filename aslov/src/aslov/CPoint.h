/*
 * CPoint.h
 *
 *       Created on: 17.09.2014
 *           Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef NOGTK

#ifndef CPOINT_H_
#define CPOINT_H_

#include <gtk/gtk.h>
#include <ostream>

class CPoint {
public:
	int x, y;
	CPoint() {
		x = y = 0;
	}
	CPoint(int _x, int _y) {
		x = _x;
		y = _y;
	}
#if GTK_MAJOR_VERSION==3
	CPoint(GdkEventButton *p) {
		x = p->x;
		y = p->y;
	}
#endif
	void operator+=(const CPoint &p) {
		x += p.x;
		y += p.y;
	}
	void operator-=(const CPoint &p) {
		x -= p.x;
		y -= p.y;
	}

	bool operator==(const CPoint &p) const {
		return x == p.x && y == p.y;
	}

	std::string toString() const;

};

std::ostream& operator<<(std::ostream &os, const CPoint &a);

#endif /* CPOINT_H_ */
#endif
