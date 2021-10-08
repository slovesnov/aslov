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

void checkParser();

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv); //do not remove

	aslovInit(argv);

	VString v{"1","2","3"};

	std::string s;
	s=joinV(v);
	printl("["+s+']')

//	s=join('#', "dictionary","language","version");
//	printl("["+s+']')

#if ASNOGTK>=0
	printinfo
#else
	printinfo
#endif

	auto q=localeToUtf8("24");
	printl("["+q+']')
	//checkParser();

/*
	auto d=g_date_time_new_now_local();
	printl( g_date_time_is_daylight_savings(d));

	println("%02d:%02d:%02d",
	g_date_time_get_hour(d),g_date_time_get_minute(d),g_date_time_get_second(d) );
*/


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

void checkParser(){
	std::string s;
	int i,m=-1;
	bool b;
	const std::string q[]={""," 1","2 "," 5 ","42"," -2"," +3","-4 "," +4","+8","-7"
			,"4333222111"
			,std::to_string(INT_MIN)
			,std::to_string(INT_MAX)
	};
	for(auto a:q){
		i=a.length();
		m=std::max(i,m);
	}
	for(auto a:q){
		s="parse ["+a+"]";
		for(i=0;i<m-int(a.length());i++){
			s+=' ';
		}
		b=stringToInt(a,i);
		if(b){
			s+="ok result="+std::to_string(i);
		}
		else{
			s+="error";
		}
		printl(s)
	}
}
