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
#include <cstring>
#include <limits.h>

void checkParser();

int main(int argc, char *argv[]) {
	gtk_init(&argc, &argv); //do not remove

	aslovInit(argv);

	checkParser();

}

void checkParser(){
	std::string s;
	int i,m=-1;
	bool b;
/*
	const std::string q[]={""," 1","2 "," 5 ","42"," -2"," +3","-4 "," +4","+8","-7"
			,"4333222111"
			,".23"
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
		b=parseString(a,i);
		if(b){
			s+="ok result="+std::to_string(i);
		}
		else{
			s+="error";
		}
		printl(s)
	}

	printi
*/
	{
		float d;
		const std::string q[]={"\n.12","\r-.3","\t+.4","+4","+4 "," -4 "," +4"
				,"-.7","+.7",".7","+8","-8","8"};
//		const std::string q[]={"",".1"," 2 ","-.7"};
		for(auto a:q){
			i=a.length();
			m=std::max(i,m);
		}
		for(auto a:q){
			s="parse ["+a+"]";
			for(i=0;i<m-int(a.length());i++){
				s+=' ';
			}
			b=parseString(a.c_str(),d);
			if(b){
				s+="ok result="+std::to_string(d);
			}
			else{
				s+="error";
			}
			printl(s)
		}

	}
}
