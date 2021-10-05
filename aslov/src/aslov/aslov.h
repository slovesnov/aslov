/*
 * aslov.h
 *
 *  Created on: 03.06.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef ASLOV_H_
#define ASLOV_H_

#include <cassert>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#ifndef NOGTK
#include <gtk/gtk.h>
#endif

#define GP(a) gpointer(int64_t(a))
#define GP2INT(a) int(int64_t(a))
#define SIZE G_N_ELEMENTS
#define SIZEI(a) int(G_N_ELEMENTS(a))
#define INDEX_OF(a,id) indexOf(a,SIZEI(a),id)
#define IN_ARRAY(array,item) (INDEX_OF(array,item)!=-1)
#ifdef NOGTK
#define g_print printf
#define G_DIR_SEPARATOR '\\'
#define G_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))
#endif

typedef std::vector<std::string> VString;
typedef std::map<std::string, std::string> MapStringString;
typedef std::pair<std::string,std::string> PairStringString;


//format to string example format("%d %s",1234,"some")
std::string format(const char *f, ...);

//format to string example forma(1234,"some")
template<typename Arg, typename ... Args>
std::string forma(Arg const &arg, Args const &... args) {
	std::stringstream c;
	c << arg;
	((c << ' ' << args), ...);
	return c.str();
}

void aslovPrintHelp(bool toFile, const std::string &s, const char *f, const int l,
		const char *fu);

//output info to screen example println("%d %s",1234,"some")
#define println(f, ...)  aslovPrintHelp(0,format(f,##__VA_ARGS__),__FILE__,__LINE__,__func__);

//output info to screen example printl(1234,"some")
#define printl(f, ...)  aslovPrintHelp(0,forma(f,##__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printinfo println("")

//output info to log file printlog("%d %s",1234,"some")
#define printlog(f, ...)  aslovPrintHelp(1,format(f,##__VA_ARGS__),__FILE__,__LINE__,__func__);

//output info  to log file printlo(1234,"some")
#define printlo(f, ...)  aslovPrintHelp(1,forma(f,##__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printloginfo printlog("")

//BEGIN file functions
enum class FILEINFO {
	NAME, EXTENSION, LOWER_EXTENSION, DIRECTORY, SHORT_NAME
};
#ifndef NOGTK
bool isDir(const char *url);
bool isDir(const std::string& s);
#endif
std::string getFileInfo(std::string filepath, FILEINFO fi);
int getFileSize(const std::string &path);
FILE* open(std::string filepath, const char *flags);
//END file functions

//BEGIN application functions
void aslovInit(char const*const* argv);
int getApplicationFileSize();
FILE* openApplicationLog(const char *flags);
std::string const& getApplicationName();
std::string const& getApplicationPath();
std::string const& getWorkingDirectory();
std::string getResourcePath(std::string name);
std::string getImagePath(std::string name);
//END application functions

//BEGIN config functions
std::string getConfigPath();
bool loadConfig(MapStringString&map);
//#define WRITE_CONFIG(T,V) static_assert(SIZE(T)==SIZE(V));writeConfig(T,V,SIZE(T));
//helper for writeConfigV
std::string aslovToString(std::string const& a);
//helper for writeConfigV
std::string aslovToString(const char* a);

//helper for writeConfigV
template <typename T>
std::string aslovToString(T t){
	return std::to_string(t);
}

//https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
template <typename ... T>
void writeConfig(const std::string tags[],T && ... p){
	VString v;
    ([&] (auto & a)
    {
        	v.push_back(aslovToString(a));
    } (p), ...);

	auto f = open(getConfigPath(), "w+");
	if (!f) {
		println("error");
		assert(0);
		return;
	}

	for(int i=0;i<int(v.size());i++){
		fprintf(f,"%s = %s\n",tags[i].c_str(),v[i].c_str());
	}
	fclose(f);

}

PairStringString pairFromBuffer(const char*b);
//END config functions


//BEGIN string functions
std::string intToString(int v, char separator);
bool stringToInt(const std::string&d,int&v);
bool stringToInt(const char*d,int&v);
bool startsWith(const char *buff, const char *begin);
bool startsWith(const char *buff, const std::string &begin);
bool endsWith(std::string const &s, std::string const &e);
std::string replaceAll(std::string subject, const std::string &from,
		const std::string &to);
int charIndex(const char *p, char c);

const std::string localeToUtf8(const std::string &s);
const std::string utf8ToLocale(const std::string &s);

//convert localed "s" to lowercase in cp1251
std::string localeToLowerCase(const std::string &s, bool onlyRussainChars =
		false);
std::string utf8ToLowerCase(const std::string &s,
		bool onlyRussainChars = false);

//END string functions


//BEGIN 2 dimensional array functions
template<class T>T** create2dArray(int dimension1, int dimension2){
	T **p = new T*[dimension1];
	for (int i = 0; i < dimension1; i++) {
		p[i] = new T[dimension2];
	}
	return p;
}

template<class T>void delete2dArray(T **p, int dimension1){
	for (int i = 0; i < dimension1; i++) {
		delete[] p[i];
	}
	delete[] p;
}
//END 2 dimensional array functions

//BEGIN pixbuf functions
#ifndef NOGTK
void getPixbufWH(GdkPixbuf *p,int&w,int&h);
void free(GdkPixbuf*&p);
void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
		int height, int sourcex, int sourcey);
#endif
//END pixbuf functions

template <typename Arg, typename... Args>
bool oneOf(Arg const& arg, Args const&... args){
	return ((arg== args)|| ...);
}

template<class T>
bool oneOf(T const& v,std::vector<T> const& t ) {
	return std::find(begin(t), end(t), v) != end(t);
}

//cann't use indexOf name because sometimes compiler cann't deduce
template <typename Arg, typename... Args>
int indexOfV(Arg const& t,Args const&... args) {
	auto l = {args...};
    auto i=std::find(std::begin(l),std::end(l),t);
    return i==std::end(l)?-1:i-std::begin(l);
}

template<class T> int indexOf(const T a[], const unsigned aSize,
		const T e) {
	unsigned i = std::find(a, a + aSize, e) - a;
	return i == aSize ? -1 : i;
}

#ifndef NOGTK
void addClass(GtkWidget *w, const gchar *s);
void removeClass(GtkWidget *w, const gchar *s);
void loadCSS();
void openURL(std::string url);
#endif
int getNumberOfCores();


#endif /* ASLOV_H_ */
