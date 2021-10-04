/*
 * main.cpp
 *
 *       Created on: 01.09.2015
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2015-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#include "aslov/aslov.h"
//#include <fstream>


int main(int argc, char *argv[]) {
//	gtk_init(&argc, &argv); //do not remove

	//aslovInit("C:\\slovesno\\eclipse\\gtktest\\Debu\\gtktest.exe");
	aslovInit(argv[0]);


	printl("applicationName",applicationName)
	printl("applicationPath",applicationPath)
	printl("workingDirectory",workingDirectory)

}
