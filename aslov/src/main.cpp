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

std::string getLowerFileExtension(std::string file) { //[in] full path file name, returns file extension
	std::size_t pos = file.rfind('.');
	//version 4.0 return pos==std::string::npos ? "" : file.substr(pos+1);

	if (pos == std::string::npos) {
		return "";
	}
	else {
		std::string s = "";
		for (const char*p = file.c_str() + pos + 1; *p != 0; p++) {
			s += ::tolower(*p);
		}
		return s;
	}
}


std::string getFileName(std::string filepath, bool withExtension) {
	std::size_t pos = filepath.rfind(G_DIR_SEPARATOR);
	std::string s =
			pos == std::string::npos ? filepath : filepath.substr(pos + 1);
	if (withExtension) {
		return s;
	}
	pos = s.rfind('.');
	return pos == std::string::npos ? s : s.substr(0, pos);
}

int main(int argc, char *argv[]) {
//	gtk_init(&argc, &argv); //do not remove

	//aslovInit("C:\\slovesno\\eclipse\\gtktest\\Debu\\gtktest.exe");
	aslovInit(argv);

	std::string s;

	s=getFileName(argv[0],true);
	printl(s)

	s=getFileName(argv[0],false);
	printl(s)

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
