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
#include <cstring>

std::string shortFileName(const char*file) {
	const char*p = strrchr(file, G_DIR_SEPARATOR);
	return p ? p + 1 : file;
}

int main(int argc, char *argv[]) {
//	gtk_init(&argc, &argv); //do not remove

	//aslovInit("C:\\slovesno\\eclipse\\gtktest\\Debu\\gtktest.exe");
	aslovInit(argv);

	std::string s;

	s=shortFileName(argv[0]);
	printl(s)

//	s=getFileName(argv[0],false);
//	printl(s)

/*
	MapStringString m;
	m.insert({"dictionary","en"});
	m.insert({"language","ru"});
	m.insert({"version","4.400000"});

	//printl(m["language"]);

	auto it=m.find("language");
	if(it==m.end()){
		printl("not found");
	}
	else{
		printl(it->second);
	}
*/

}
