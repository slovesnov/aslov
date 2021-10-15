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
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#ifndef NOGTK
#include <gtk/gtk.h>
#endif

/* https://www.geeksforgeeks.org/c-macro-preprocessor-question-5/
 * default macro value is 0 so
 * #ifndef NOGTK => #if NOGTK==0 is true
 * #if NOGTK==0 ~ #if !defined(NOGTK) || NOGTK==0
 */
#ifdef NOGTK
	#if NOGTK==0
		#define NOTGK_WITH_ICONV
	#else
		#define NOTGK_WITHOUT_ICONV
	#endif
#endif

#define GP(a) gpointer(int64_t(a))
#define GP2INT(a) int(int64_t(a))
#define SIZE G_N_ELEMENTS
#define SIZEI(a) int(G_N_ELEMENTS(a))
#define INDEX_OF(id,a) indexOf(id,a,SIZEI(a))
#define ONE_OF(id,a) oneOf(id,a,SIZEI(a))
#define INDEX_OF_NO_CASE(id,a) indexOfNoCase(id,a,SIZEI(a))
#define JOIN(a) join(a,SIZEI(a))
#define JOINS(a,separator) join(a,SIZEI(a),separator)

#ifdef NOGTK
#define g_print printf
#define G_DIR_SEPARATOR '\\'
#define G_DIR_SEPARATOR_S "\\"
#define G_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))
#endif

typedef std::vector<std::string> VString;
typedef std::map<std::string, std::string> MapStringString;
typedef std::pair<std::string,std::string> PairStringString;


//format to string example format("%d %s",1234,"some")
std::string format(const char *f, ...);

template<typename A, typename ... B>
std::string formats(const std::string& separator,A const &a, B const &... b) {
	std::stringstream c;
	c << a;
	((c << separator << b), ...);
	return c.str();
}

template<typename A, typename ... B>
std::string formats(const char separator,A const &a, B const &... b) {
	return formats(std::string(1,separator),a,b...);
}

template<typename A, typename ... B>
std::string formatz(A const &a, B const &... b) {
	return formats("",a,b...);
}

//format to string example forma(1234,"some")
template<typename A, typename ... B>
std::string forma(A const &a, B const &... b) {
	return formats(" ",a,b...);
}

void aslovPrintHelp(bool toFile, const std::string &s, const char *f, const int l,
		const char *fu);

//output info to screen example println("%d %s",1234,"some")
#define println(...)  aslovPrintHelp(0,format(__VA_ARGS__),__FILE__,__LINE__,__func__);

//output info to screen example printl(1234,"some")
#define printl(...)  aslovPrintHelp(0,forma(__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printinfo println("")

template<typename A, typename ... B>
void aslovPrints(const std::string& separator,A const &a, B const &... b) {
	g_print("%s",formats(separator,a,b...).c_str());
}

template<typename A, typename ... B>
void aslovPrints(const char separator,A const &a, B const &... b) {
	aslovPrints(std::string(1,separator),a,b...);
}

//prints('#',1,2,"ab") -> printf("1#2#ab") or prints("@@",1,2,"ab") -> printf("1@@2@@ab")
#define prints(...) aslovPrints(__VA_ARGS__);
//printa(1,2,"ab") -> printf("1 2 ab")
#define printa(...) aslovPrints(" ",__VA_ARGS__);
//printz(1,2,"ab") -> printf("12ab")
#define printz(...) aslovPrints("",__VA_ARGS__);

//adding to prints, printa, printz 'n' at the end gives additional "\n"
#define printsn(...) prints(__VA_ARGS__,"\n");
#define printan(...) printa(__VA_ARGS__,"\n");
#define printzn(...) printz(__VA_ARGS__,"\n");

//adding to prints, printa, printz 'i' ah the end gives file, line, function info and '\n'
//printai ~ printl
#define printsi(...) aslovPrintHelp(0,formats(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printai(...) aslovPrintHelp(0,forma(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printzi(...) aslovPrintHelp(0,formatz(__VA_ARGS__),__FILE__,__LINE__,__func__);


//output info to log file printlog("%d %s",1234,"some")
#define printlog(...)  aslovPrintHelp(1,format(__VA_ARGS__),__FILE__,__LINE__,__func__);

//output info  to log file printlo(1234,"some")
#define printlo(...)  aslovPrintHelp(1,forma(__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printloginfo printlog("")

//BEGIN file functions
enum class FILEINFO {
	NAME, EXTENSION, LOWER_EXTENSION, DIRECTORY, SHORT_NAME
};
bool isDir(const char *path);
bool isDir(const std::string& path);
std::string getFileInfo(std::string path, FILEINFO fi);
int getFileSize(const std::string &path);
FILE* open(std::string path, const char *flags);
//END file functions

//BEGIN application functions
void aslovInit(char const*const* argv);
int getApplicationFileSize();
FILE* openApplicationLog(const char *flags);
std::string const& getApplicationName();
//std::string const& getApplicationPath();
//std::string const& getWorkingDirectory();
std::string getResourcePath(const std::string name);
std::string getImagePath(const std::string name);
std::ifstream openResourceFileAsStream(const std::string name);
std::string getWritableFilePath(const std::string name);
#ifndef NOGTK
//writable resource+log+cfg files are places in same dir and should started with application name
void writableFileSetContents(const std::string name,const std::string& s);
const std::string writableFileGetContents(const std::string& name);
#endif
const std::string fileGetContent(const std::string& path);

//END application functions

//BEGIN config functions
#ifndef NOGTK
std::string getConfigPath();
bool loadConfig(MapStringString&map);
#define WRITE_CONFIG(T,V, ...) aslovWriteConfig(T,SIZE(T),V,##__VA_ARGS__);

template <typename ... T>
void aslovWriteConfig(const std::string tags[],const int size,T && ... p){
    assert(size==sizeof...(T));
	std::ofstream f(getConfigPath());
	assert(f.is_open());
    int i=0;
    ([&] (auto & a){
		f<<tags[i++]<<" = "<<a<<"\n";
    } (p), ...);
}

PairStringString pairFromBuffer(const std::string&s);
PairStringString pairFromBuffer(const char*b);
#endif
//END config functions

//BEGIN string functions
std::string intToString(int v, char separator=' ');

//v is changed only if parse is valid for all functions
bool stringToInt(const std::string &d, int &v, int radix = 10);
bool stringToInt(const char *d, int &v, int radix = 10);
bool stringToUnsigned(const std::string &d, unsigned &v, int radix = 10);
bool stringToUnsigned(const char *d, unsigned &v, int radix = 10);
bool stringToLL(const std::string &d, long long &v, int radix = 10);
bool stringToLL(const char *d, long long &v, int radix = 10);

//stringParse("0xff",i,16), stringParse("ff",i,16), stringParse("+0xff",i,16) ok
bool stringParse(const std::string &d, int &v, int radix = 10);
bool stringParse(const char *d, int &v, int radix = 10);
bool stringParse(const std::string &d, unsigned &v, int radix = 10);
bool stringParse(const char *d, unsigned &v, int radix = 10);
bool stringParse(const std::string &d, long long &v, int radix = 10);
bool stringParse(const char *d, long long &v, int radix = 10);

bool startsWith(const char *s, const char *begin);
bool startsWith(const char *s, const std::string &begin);
bool startsWith(const std::string& s, const char* begin);
bool startsWith(const std::string& s, const std::string & begin);
bool endsWith(std::string const &s, std::string const &e);
std::string replaceAll(std::string subject, const std::string &from,
		const std::string &to);
VString split(const std::string& subject, const std::string& separator);
VString split(const std::string& subject, const char separator=' ');
int countOccurence(const std::string& subject, const std::string& a);
int countOccurence(const std::string& subject, const char c);

bool cmpnocase(const std::string& a, const char* b);
bool cmpnocase(const char* a, const char* b);
bool cmp(const char* a, const char* b);
bool cmp(const std::string& a, const char* b);

#ifndef NOTGK_WITHOUT_ICONV
const std::string localeToUtf8(const std::string &s);
const std::string utf8ToLocale(const std::string &s);

std::string utf8ToLowerCase(const std::string &s,
		bool onlyRussainChars = false);
#endif
//convert localed string to lowercase
std::string localeToLowerCase(const std::string &s, bool onlyRussainChars =
		false);

template <typename T,typename ...P>
std::string joinS(const std::string separator,T&& t,P&& ... p){
	std::stringstream c;
	c << t;
	((c << separator << p), ...);
	return c.str();
}

template <typename T,typename ...P>
std::string joinS(const char separator,T&& t,P&& ... p){
	return joinS(std::string(1,separator),t,p...);
}

//separator can be default so use as 2nd parameter. It differs from other functions arguments order.
template<typename T>
std::string joinV(std::vector<T> const &v, const char separator=' '){
	std::stringstream c;
	bool f=true;
	for(auto&a:v){
		if(f){
			f=false;
		}
		else{
			c<<separator;
		}
		c<<a;
	}
	return c.str();
}

//separator can be default so use as 2nd parameter. It differs from other functions arguments order.
template<typename T>
std::string join(T const v[], int size,const char separator=' ') {
	std::stringstream c;
	for(int i=0;i<size;i++){
		if(i){
			c<<separator;
		}
		c<<v[i];
	}
	return c.str();
}


//(T&& t,P&& ... p) two ampersands conflict with std::string join(VString const &v, const char separator=' ');
template <typename T,typename ...P>
std::string join(T& t,P& ... p){
	return joinS(" ",t,p...);
}

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
void getPixbufWH(const char*s,int&w,int&h);
void getPixbufWH(std::string const&s,int&w,int&h);
void free(GdkPixbuf*&p);

void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
		int height, int sourcex, int sourcey);

GdkPixbuf* pixbuf(const char* s);
GdkPixbuf* pixbuf(const std::string& s);
GdkPixbuf* pixbuf(std::string s, int x, int y, int width, int height);
GdkPixbuf* writablePixbuf(const char* s);
GdkPixbuf* writablePixbuf(const std::string& s);

GtkWidget* image(const char* s);
GtkWidget* image(const std::string& s);
GtkWidget* animatedImage(const char* s);
#endif
//END pixbuf/image functions

template<class T> typename std::vector<T>::const_iterator find(const T& t,std::vector<T> const& v) {
	return std::find(v.begin(),v.end(),t);
}

template<class T> typename std::vector<T>::iterator find(const T& t,std::vector<T>& v) {
	return std::find(v.begin(),v.end(),t);
}

//cann't use indexOf name because sometimes compiler cann't deduce
template <class T, class... V>
int indexOfV(T const& t,V const&... v) {
	auto l = {v...};
    auto i=std::find(std::begin(l),std::end(l),t);
    return i==std::end(l)?-1:i-std::begin(l);
}

//template <class T, class... V>
//int indexOf(T const& t,V const&... v) {
//	auto l = {v...};
//    auto i=std::find(std::begin(l),std::end(l),t);
//    return i==std::end(l)?-1:i-std::begin(l);
//}

template<class T> int indexOf(const T& t,const T v[], int size) {
	int i = std::find(v, v + size, t)-v;
	return i == size ? -1 : i;
}

template<class T> int indexOf(const T& t,std::vector<T> const& v) {
	typename std::vector<T>::const_iterator it=find(t,v);
	return it==v.end()?-1:it-v.begin();
}

int indexOfNoCase(const char *t,const char *v[], int size );
int indexOfNoCase(const std::string t,const char *v[], int size);
int indexOf(const char t,const std::string& v);

//template <class T, class... V>bool oneOfV(T const& t, V const&... v){
//	return ((t== v)|| ...);
//	//return indexOfV(t,v...)!=-1;
//}
template <class T, class... V>bool oneOf(T const& t, V const&... v){
	return ((t== v)|| ...);
	//return indexOfV(t,v...)!=-1;
}

template<class T> bool oneOf(const T &t, T const v[], int size) {
	return indexOf(t,v, size) != -1;
}

template<class T> bool oneOf(const T& t,std::vector<T> const& v) {
	return indexOf(t,v) != -1;
}

#ifndef NOGTK
void addClass(GtkWidget *w, const gchar *s);
void removeClass(GtkWidget *w, const gchar *s);
void loadCSS(std::string const &additionalData = "");
void openURL(std::string url);
void destroy(cairo_t* p);
void destroy(cairo_surface_t * p);
#endif
int getNumberOfCores();
double timeElapse(clock_t begin);


#endif /* ASLOV_H_ */
