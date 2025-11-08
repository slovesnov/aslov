/*
 * main.cpp
 *
 *       Created on: 01.09.2015
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2015-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.rf.gd
 */

#include "aslov/aslov.h"
//#include <cstring>
//#include <limits.h>

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv); //do not remove
	aslovInit(argv);

	printlog("%d",1)
	printlo(1)
	printlogi
//	auto a=getDPI();
//	printl(a.first,a.second);
	int a=1;
	std::string s="df";
	printl(join(a,s));
	printl(join(3,"sd"));
	VString v={"d","3"};
	printl(joinV(v,"#"));

}
