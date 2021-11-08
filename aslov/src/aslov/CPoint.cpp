/*
 * CPoint.cpp
 *
 *  Created on: 08.11.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include "CPoint.h"

std::string CPoint::toString() const {
	return std::to_string(x) + "," + std::to_string(y);
}

std::ostream& operator<<(std::ostream& os, const CPoint& a){
    os << a.toString();
    return os;
}

