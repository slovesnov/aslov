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
#define ONE_OF(a,id) oneOf(a,SIZE(a),id)
#define INDEX_OF_NO_CASE(a,id) indexOfNoCase(a,SIZE(a),id)

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
#define WRITE_CONFIG(T,V, ...) aslovWriteConfig(T,SIZE(T),V,##__VA_ARGS__);
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
void aslovWriteConfig(const std::string tags[],const int size,T && ... p){
	VString v;
    ([&] (auto & a)
    {
        	v.push_back(aslovToString(a));
    } (p), ...);

    assert(size==sizeof...(T));

	auto f = open(getConfigPath(), "w+");
	assert(f);
	for(int i=0;i<size;i++){
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
bool startsWith(const char *s, const char *begin);
bool startsWith(const char *s, const std::string &begin);
bool startsWith(const std::string& s, const char* begin);
bool endsWith(std::string const &s, std::string const &e);
std::string replaceAll(std::string subject, const std::string &from,
		const std::string &to);
VString split(const std::string& subject, const std::string& separator);
int countOccurence(const std::string& subject, const std::string& a);

int charIndex(const char *p, char c);
int indexOfNoCase(const char *a[], unsigned size, const char *item);
int indexOfNoCase(const char *a[], unsigned size, const std::string item);

bool cmpnocase(const std::string& a, const char* b);
bool cmpnocase(const char* a, const char* b);
bool cmp(const char* a, const char* b);
bool cmp(const std::string& a, const char* b);


const std::string localeToUtf8(const std::string &s);
const std::string utf8ToLocale(const std::string &s);

//convert localed "s" to lowercase in cp1251
std::string localeToLowerCase(const std::string &s, bool onlyRussainChars =
		false);
std::string utf8ToLowerCase(const std::string &s,
		bool onlyRussainChars = false);
#ifndef NOGTK
//TODO
std::string utfLowerCase(const std::string& s);
std::string utfLower(const std::string& s);
#endif
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

//BEGIN pixbuf/image functions
#ifndef NOGTK
void getPixbufWH(GdkPixbuf *p,int&w,int&h);
void free(GdkPixbuf*&p);

void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
		int height, int sourcex, int sourcey);

GdkPixbuf* pixbuf(const char* s);
GdkPixbuf* pixbuf(const std::string& s);
GdkPixbuf* pixbuf(std::string s, int x, int y, int width, int height);
GtkWidget* image(const char* s);
GtkWidget* image(const std::string& s);
GtkWidget* animatedImage(const char* s);
#endif
//END pixbuf/image functions

template<class T> typename std::vector<T>::const_iterator find(std::vector<T> const& v, const T& item) {
	return std::find(v.begin(),v.end(),item);
}

template<class T> typename std::vector<T>::iterator find(std::vector<T>& v, const T& item) {
	return std::find(v.begin(),v.end(),item);
}

template <typename Arg, typename... Args>
bool oneOfV(Arg const& arg, Args const&... args){
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

template<class T> bool oneOf(T const a[], int size,
		const T& item) {
	return indexOf(a, size, item) != -1;
}

template<class T> int indexOf(T const a[], int size, const T& item) {
	int i;
	for (i = 0; i < size; i++) {
		if (item == a[i]) {
			return i;
		}
	}
	return -1;
}

template<class T> int indexOf(std::vector<T> const& v, const T& item) {
	typename std::vector<T>::const_iterator it=find(v,item);
	return it==v.end()?-1:it-v.begin();
}

template<class T> bool oneOf(std::vector<T> const& v, const T& item) {
	return indexOf(v,item)!=-1;
}


#ifndef NOGTK
void addClass(GtkWidget *w, const gchar *s);
void removeClass(GtkWidget *w, const gchar *s);
void loadCSS();
void openURL(std::string url);
void destroy(cairo_t* p);
void destroy(cairo_surface_t * p);
#endif
int getNumberOfCores();



#endif /* ASLOV_H_ */
