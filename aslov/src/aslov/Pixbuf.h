/*
 * Pixbuf.h
 *
 *  Created on: 09.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef ASLOV_PIXBUF_H_
#define ASLOV_PIXBUF_H_

class Pixbuf {
public:
	Pixbuf();
	virtual ~Pixbuf();

	Pixbuf & operator=(const Pixbuf&) = delete;
	Pixbuf(const Pixbuf&) = delete;

};

#endif /* ASLOV_PIXBUF_H_ */
