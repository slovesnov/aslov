/*
 * aslov.h
 *
 *  Created on: 03.06.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
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
#ifdef __GNUC__
#include <memory>//for type name
#include <cxxabi.h>//for type name this include file exists not for all compilers
#endif
#ifndef NOGTK
#include <gtk/gtk.h>
#include "CairoSurface.h" //CRect,CPoint
#include "Pixbuf.h"
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
#define g_printerr(...) fprintf(stderr,__VA_ARGS__)
#define G_DIR_SEPARATOR '\\'
#define G_DIR_SEPARATOR_S "\\"
#define G_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))
#endif

typedef std::vector<std::string> VString;
typedef std::map<std::string, std::string> MapStringString;
typedef std::pair<std::string, std::string> PairStringString;
typedef std::pair<double, double> PairDoubleDouble;

//format to string example format("%d %s",1234,"some")
std::string format(const char *f, ...);

template<typename A, typename ... B>
std::string formats(const std::string &separator, A const &a, B const &... b) {
	std::stringstream c;
	c << a;
	((c << separator << b), ...);
	return c.str();
}

template<typename ... A>
std::string formats(const char separator, A const &...a) {
	return formats(std::string(1, separator), a...);
}

template<typename ... A>
std::string formatz(A const &...a) {
	return formats("", a...);
}

//format to string example forma(1234,"some")
template<typename ... A>
std::string formata(A const &...a) {
	return formats(" ", a...);
}

#define forma formata

//output info to screen example printl(1234,"some")
#define printl printai
#define printel printeai

//output info to screen example println("%d %s",1234,"some")
#define println(...)  aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDOUT,format(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printeln(...)  aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDERR,format(__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printi println("")
#define printei printeln("")

enum class ASLOV_OUTPUT_TYPE {
	STDOUT, STDERR, FILE
};

void aslovPrintHelp(ASLOV_OUTPUT_TYPE t, const std::string &s, const char *f,
		const int l, const char *fu);

template<typename ... A>
void aslovPrints(ASLOV_OUTPUT_TYPE t, const std::string &separator,
		A const &... a) {
	if (t == ASLOV_OUTPUT_TYPE::STDERR) {
		g_printerr("%s", formats(separator, a...).c_str());
	} else {
		g_print("%s", formats(separator, a...).c_str());
	}
}

template<typename ... A>
void aslovPrints(ASLOV_OUTPUT_TYPE t, const char separator, A const &...a) {
	aslovPrints(t, std::string(1, separator), a...);
}

//prints('#',1,2,"ab") -> printf("1#2#ab") or prints("@@",1,2,"ab") -> printf("1@@2@@ab")
#define prints(...) aslovPrints(ASLOV_OUTPUT_TYPE::STDOUT,__VA_ARGS__);
//printa(1,2,"ab") -> printf("1 2 ab")
#define printa(...) aslovPrints(ASLOV_OUTPUT_TYPE::STDOUT," ",__VA_ARGS__);
//printz(1,2,"ab") -> printf("12ab")
#define printz(...) aslovPrints(ASLOV_OUTPUT_TYPE::STDOUT,"",__VA_ARGS__);

#define printes(...) aslovPrints(ASLOV_OUTPUT_TYPE::STDERR,__VA_ARGS__);
#define printea(...) aslovPrints(ASLOV_OUTPUT_TYPE::STDERR," ",__VA_ARGS__);
#define printez(...) aslovPrints(ASLOV_OUTPUT_TYPE::STDERR,"",__VA_ARGS__);

#define printsn(...) prints(__VA_ARGS__,"\n")
#define printan(...) printa(__VA_ARGS__,"\n")
#define printzn(...) printz(__VA_ARGS__,"\n")

#define printesn(...) printes(__VA_ARGS__,"\n")
#define printean(...) printea(__VA_ARGS__,"\n")
#define printezn(...) printez(__VA_ARGS__,"\n")

//adding to prints, printa, printz 'i' ah the end gives file, line, function info and '\n'
//printai ~ printl
#define printsi(...) aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDOUT,formats(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printai(...) aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDOUT,formata(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printzi(...) aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDOUT,formatz(__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printesi(...) aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDERR,formats(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printeai(...) aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDERR,formata(__VA_ARGS__),__FILE__,__LINE__,__func__);
#define printezi(...) aslovPrintHelp(ASLOV_OUTPUT_TYPE::STDERR,formatz(__VA_ARGS__),__FILE__,__LINE__,__func__);

/* https://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros
 * many modifications aslov
 * up to 8 variables printv(a1,a2,a3,a4,a5,a6,a7,a8)
 */

#define ASLOV_PRINT_VARIABLE(a) printan(formatz(#a," = ",a))
#define ASLOV_PRINT_VARIABLE_I(a) printai(formatz(#a," = ",a))
#define ASLOV_PRINTE_VARIABLE(a) printean(formatz(#a," = ",a))
#define ASLOV_PRINTE_VARIABLE_I(a) printeai(formatz(#a," = ",a))

#define ASLOV_CONCATENATE(a, b)   a##b
#define ASLOV_FOR_EACH_NARG(...) ASLOV_FOR_EACH_NARG_(__VA_ARGS__, ASLOV_FOR_EACH_RSEQ_N())
#define ASLOV_FOR_EACH_NARG_(...) ASLOV_FOR_EACH_ARG_N(__VA_ARGS__)

#define ASLOV_FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define ASLOV_FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0
#define ASLOV_FOR_EACH_1(g,f,x,...) f(x)
#define ASLOV_FOR_EACH_2(g,f,x,...) g(x)ASLOV_FOR_EACH_1(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH_3(g,f,x,...) g(x)ASLOV_FOR_EACH_2(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH_4(g,f,x,...) g(x)ASLOV_FOR_EACH_3(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH_5(g,f,x,...) g(x)ASLOV_FOR_EACH_4(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH_6(g,f,x,...) g(x)ASLOV_FOR_EACH_5(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH_7(g,f,x,...) g(x)ASLOV_FOR_EACH_6(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH_8(g,f,x,...) g(x)ASLOV_FOR_EACH_7(g,f,__VA_ARGS__)
#define ASLOV_FOR_EACH(N,g,f,x,...) ASLOV_CONCATENATE(ASLOV_FOR_EACH_,N)(g,f, x, __VA_ARGS__)
#define ASLOV_PRINTV(g,f,...) ASLOV_FOR_EACH(ASLOV_FOR_EACH_NARG(__VA_ARGS__),g, f, __VA_ARGS__)
#define printv(...) ASLOV_PRINTV(ASLOV_PRINT_VARIABLE,ASLOV_PRINT_VARIABLE,__VA_ARGS__)
#define printvi(...) ASLOV_PRINTV(ASLOV_PRINT_VARIABLE,ASLOV_PRINT_VARIABLE_I,__VA_ARGS__)
#define printev(...) ASLOV_PRINTV(ASLOV_PRINTE_VARIABLE,ASLOV_PRINTE_VARIABLE,__VA_ARGS__)
#define printevi(...) ASLOV_PRINTV(ASLOV_PRINTE_VARIABLE,ASLOV_PRINTE_VARIABLE_I,__VA_ARGS__)

/*
 #define ASLOV_PRINT_VARIABLE(a) printan(formatz(#a," = ",a))
 #define ASLOV_PRINT_VARIABLE_I(a) printai(formatz(#a," = ",a))

 #define ASLOV_CONCATENATE(a, b)   a##b
 #define ASLOV_FOR_EACH_NARG(...) ASLOV_FOR_EACH_NARG_(__VA_ARGS__, ASLOV_FOR_EACH_RSEQ_N())
 #define ASLOV_FOR_EACH_NARG_(...) ASLOV_FOR_EACH_ARG_N(__VA_ARGS__)

 #define ASLOV_FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
 #define ASLOV_FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0
 #define ASLOV_FOR_EACH_1(f,x,...) f(x)
 #define ASLOV_FOR_EACH_2(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_1(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH_3(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_2(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH_4(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_3(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH_5(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_4(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH_6(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_5(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH_7(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_6(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH_8(f,x,...) ASLOV_PRINT_VARIABLE(x)ASLOV_FOR_EACH_7(f,__VA_ARGS__)
 #define ASLOV_FOR_EACH(N,f,x,...) ASLOV_CONCATENATE(ASLOV_FOR_EACH_,N)(f, x, __VA_ARGS__)
 #define ASLOV_PRINTV(f,...) ASLOV_FOR_EACH(ASLOV_FOR_EACH_NARG(__VA_ARGS__), f, __VA_ARGS__)
 #define printv(...) ASLOV_PRINTV(ASLOV_PRINT_VARIABLE,__VA_ARGS__)
 #define printvi(...) ASLOV_PRINTV(ASLOV_PRINT_VARIABLE_I,__VA_ARGS__)
 */
//output info to log file printlog("%d %s",1234,"some")
#define printlog(...)  aslovPrintHelp(ASLOV_OUTPUT_TYPE::FILE,format(__VA_ARGS__),__FILE__,__LINE__,__func__);

//output info  to log file printlo(1234,"some")
#define printlo(...)  aslovPrintHelp(ASLOV_OUTPUT_TYPE::FILE,forma(__VA_ARGS__),__FILE__,__LINE__,__func__);

#define printlogi printlog("")

//BEGIN file functions
enum class FILEINFO {
	NAME, EXTENSION, LOWER_EXTENSION, DIRECTORY, SHORT_NAME
};
bool isDir(const char *path);
bool isDir(const std::string &path);
std::string getFileInfo(std::string path, FILEINFO fi);
int getFileSize(const std::string &path);
FILE* open(std::string path, const char *flags);
//END file functions

//BEGIN application functions
//call aslovInit before gtk_init just first in main() function if you need to use ScaleFactor
void aslovInit(char const *const*argv, bool storeScaleFactor = false);
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
void writableFileSetContents(const std::string name, const std::string &s);
const std::string writableFileGetContents(const std::string &name);
#endif
const std::string fileGetContent(const std::string &path, bool binary = false);

//END application functions

//BEGIN config functions
#ifndef NOGTK
std::string getConfigPath();
std::string getConfigPathLocaled();
bool loadConfig(MapStringString &map);
#define WRITE_CONFIG(T,V, ...) aslovWriteConfig(T,SIZE(T),V,##__VA_ARGS__);

template<typename ... T>
void aslovWriteConfig(const std::string tags[], const int size, T &&... p) {
	assert(size==sizeof...(T));
	std::ofstream f(getConfigPathLocaled());
	assert(f.is_open());
	int i = 0;
([&] (auto & a) {
			f<<tags[i++]<<" = "<<a<<"\n";
		}(p), ...);
}

PairStringString pairFromBuffer(const std::string &s);
PairStringString pairFromBuffer(const char *b);
#endif
//END config functions

//BEGIN string functions
std::string timeToString(const char *format, bool toLowerCase = false);

template<typename T>
std::string toString(T t, char separator = ' ', int digits = 3) {
//std::fixed to prevents scientific notation t=1234567.890123 b=1.23457e +06
std::stringstream c;
c << std::fixed << t;
std::string s, e, b = c.str();
std::string::size_type p, p1;
p = b.find('.');
if (p != std::string::npos) {
	for (p1 = b.length() - 1; p1 > p && b[p1] == '0'; p1--)
		; //"3.875000"->"3.875"
	if (p != p1) { //"1.000" -> "1"
		e = b.substr(p, p1 - p + 1);
	}
	b = b.substr(0, p);
}
bool negative = std::is_signed<T>::value && t < 0;
unsigned i = b.length() - 1;
for (char a : b) {
	s += a;
	if (i % digits == 0 && i != 0 && (!negative || i != b.length() - 1)) {
		s += separator;
	}
	i--;
}
return s + e;
}

/* parseString("0xff",i,16), parseString("ff",i,16), parseString("+0xff",i,16) ok
 * t is changed only if parse is valid
 */
template<class T>
bool parseString(const char *d, T &t, int radix = 10) {
/* strtol("") is ok so check whether empty string
 * strtol(" 4") "\r4", "\n4", "\t4" is ok so check for space
 * */
if (!d || *d == 0 || isspace(*d)) {
	return false;
}
/*strtoul("-1") is ok*/
if (std::is_unsigned<T>::value && *d == '-') {
	return false;
}
char *p;
T a;

/* if correct parse then errno is not changed
 * so set errno=0
 */
errno = 0;
if (std::is_same_v<T, long>
		|| (std::is_same_v<T, int> && sizeof(int) == sizeof(long))) {
	a = strtol(d, &p, radix);
} else if (std::is_same_v<T, unsigned long>
		|| (std::is_same_v<T, unsigned> && sizeof(int) == sizeof(long))) {
	a = strtoul(d, &p, radix);
} else if (std::is_same_v<T, int64_t>
		|| (std::is_same_v<T, int> && sizeof(int) == sizeof(int64_t))) {
	a = strtoll(d, &p, radix);
} else if (std::is_same_v<T, uint64_t>
		|| (std::is_same_v<T, unsigned> && sizeof(int) == sizeof(int64_t))) {
	a = strtoull(d, &p, radix);
} else if (std::is_same_v<T, float>) {
	a = strtof(d, &p);
} else if (std::is_same_v<T, double>) {
	a = strtod(d, &p);
} else {
	assert(0);
	return false;
}
/*errno!=0 - out of range, *p!=0 - not full string recognized*/
bool b = errno == 0 && *p == 0;
if (b) {
	t = a;
}
return b;
}

template<class T>
bool parseString(std::string const &s, T &t, int radix = 10) {
return parseString(s.c_str(), t, radix);
}

bool startsWith(const char *s, const char *begin);
bool startsWith(const char *s, const std::string &begin);
bool startsWith(const std::string &s, const char *begin);
bool startsWith(const std::string &s, const std::string &begin);
bool endsWith(std::string const &s, std::string const &e);
std::string replaceAll(std::string subject, const std::string &from,
	const std::string &to);
VString split(const std::string &subject, const std::string &separator);
VString split(const std::string &subject, const char separator = ' ');
#ifndef NOGTK
VString splitr(const std::string &subject, const std::string &regex);
#endif

int countOccurence(const std::string &subject, const std::string &a);
int countOccurence(const std::string &subject, const char c);

bool cmpnocase(const std::string &a, const char *b);
bool cmpnocase(const char *a, const char *b);
bool cmp(const char *a, const char *b);
bool cmp(const std::string &a, const char *b);
bool contains(const std::string &a, const char b);
bool contains(const std::string &a, const char *b);
bool contains(const std::string &a, const std::string &b);

#ifndef NOTGK_WITHOUT_ICONV
const std::string localeToUtf8(const std::string &s);
const std::string utf8ToLocale(const std::string &s);

std::string utf8ToLowerCase(const std::string &s
#ifdef NOGTK
		,bool onlyRussainChars = false
#endif
	);
std::string utf8ToUpperCase(const std::string &s
#ifdef NOGTK
		,bool onlyRussainChars = false
#endif
	);
#endif

#ifndef NOGTK
std::string utf8Substring(const std::string &s, glong start_pos, glong end_pos);
#endif
//convert localed string to lowercase
std::string localeToLowerCase(const std::string &s, bool onlyRussainChars =
	false);

template<typename T, typename ...P>
std::string joinS(const std::string separator, T &&t, P &&... p) {
std::stringstream c;
c << t;
((c << separator << p), ...);
return c.str();
}

template<typename T, typename ...P>
std::string joinS(const char separator, T &&t, P &&... p) {
return joinS(std::string(1, separator), t, p...);
}

//separator can be default so use as 2nd parameter. It differs from other functions arguments order.
template<typename T>
std::string joinV(std::vector<T> const &v, std::string const &separator = " ") {
std::stringstream c;
bool f = true;
for (auto const &a : v) {
	if (f) {
		f = false;
	} else {
		c << separator;
	}
	c << a;
}
return c.str();
}

template<typename T>
std::string joinV(std::vector<T> const &v, const char separator) {
return joinV(v, std::string(1, separator));
}

//separator can be default so use as 2nd parameter. It differs from other functions arguments order.
template<typename T>
std::string join(T const v[], int size, const char separator = ' ') {
std::stringstream c;
for (int i = 0; i < size; i++) {
	if (i) {
		c << separator;
	}
	c << v[i];
}
return c.str();
}

//(T&& t,P&& ... p) two ampersands conflict with std::string join(VString const &v, const char separator=' ');
template<typename T, typename ...P>
std::string join(T &t, P &... p) {
return joinS(" ", t, p...);
}

//END string functions

//BEGIN 2 dimensional array functions
template<class T> T** create2dArray(int dimension1, int dimension2) {
T **p = new T*[dimension1];
for (int i = 0; i < dimension1; i++) {
	p[i] = new T[dimension2];
}
return p;
}

template<class T> void delete2dArray(T **p, int dimension1) {
for (int i = 0; i < dimension1; i++) {
	delete[] p[i];
}
delete[] p;
}
//END 2 dimensional array functions

//BEGIN pixbuf/image functions
#ifndef NOGTK

void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
	int height, int sourcex, int sourcey);

GdkPixbuf* pixbuf(const char *s);
GdkPixbuf* pixbuf(const std::string &s);
GdkPixbuf* pixbuf(std::string s, int x, int y, int width, int height);
GdkPixbuf* writablePixbuf(const char *s);
GdkPixbuf* writablePixbuf(const std::string &s);

GtkWidget* image(const char *s);
GtkWidget* image(const std::string &s);
GtkWidget* animatedImage(const char *s);
#endif
//END pixbuf/image functions

template<class T> typename std::vector<T>::const_iterator find(const T &t,
	std::vector<T> const &v) {
return std::find(v.begin(), v.end(), t);
}

template<class T> typename std::vector<T>::iterator find(const T &t,
	std::vector<T> &v) {
return std::find(v.begin(), v.end(), t);
}

//cann't use indexOf name because sometimes compiler cann't deduce
template<class T, class ... V>
int indexOfV(T const &t, V const &... v) {
auto l = { v... };
auto i = std::find(std::begin(l), std::end(l), t);
return i == std::end(l) ? -1 : i - std::begin(l);
}

//template <class T, class... V>
//int indexOf(T const& t,V const&... v) {
//	auto l = {v...};
//    auto i=std::find(std::begin(l),std::end(l),t);
//    return i==std::end(l)?-1:i-std::begin(l);
//}

template<class T> int indexOf(const T &t, const T v[], int size) {
int i = std::find(v, v + size, t) - v;
return i == size ? -1 : i;
}

template<class T> int indexOf(const T &t, std::vector<T> const &v) {
typename std::vector<T>::const_iterator it = find(t, v);
return it == v.end() ? -1 : it - v.begin();
}

int indexOfNoCase(const char *t, const char *v[], int size);
int indexOfNoCase(const std::string t, const char *v[], int size);
/* also works with int indexOf(const char t,const char* p);
 * in this case last \0 symbols isn't matched because const char* -> string
 * and string hasn't terminal zero symbol. This is good.
 */
int indexOf(const char t, const std::string &v);

bool oneOf(char const &t, const std::string &v);
bool oneOf(char const &t, char const *v);

//template <class T, class... V>bool oneOfV(T const& t, V const&... v){
//	return ((t== v)|| ...);
//	//return indexOfV(t,v...)!=-1;
//}
template<class T, class ... V> bool oneOf(T const &t, V const &... v) {
return ((t== v)|| ...);
 //return indexOfV(t,v...)!=-1;
}

template<class T> bool oneOf(const T &t, T const v[], int size) {
return indexOf(t, v, size) != -1;
}

template<class T> bool oneOf(const T &t, T v[], int size) {
return indexOf(t, v, size) != -1;
}

template<class T> bool oneOf(const T &t, std::vector<T> const &v) {
return indexOf(t, v) != -1;
}

#ifdef _WIN32
/* this function counts scale factor immediately
 * gtk_init spoil this function so to use stored scale factor call
 * aslovInit(argv0,true) first in main and later call getScaleFactor();
 */
PairDoubleDouble aslovGetScaleFactor();
PairDoubleDouble getScaleFactor();
#endif

#ifndef NOGTK
void addClass(GtkWidget *w, const gchar *s);
void addClass(GtkWidget *w, const std::string &s);
void removeClass(GtkWidget *w, const gchar *s);
void removeClass(GtkWidget *w, const std::string &s);
void addRemoveClass(GtkWidget *w, const gchar *s, bool add);
void addRemoveClass(GtkWidget *w, const std::string &s, bool add);
void loadCSS(std::string const &additionalData = "");
void openURL(std::string url);
void destroy(cairo_t *p);
void destroy(cairo_surface_t *p);
std::string getBuildVersionString(bool _long);
std::string getBuildString(bool _long);
std::string getVersionString(bool _long);
void showHideWidget(GtkWidget *w, bool show);
//millimeters or inches
PairDoubleDouble getMonitorSize(bool millimeters = true);
double getMonitorDiagonal(bool millimeters = true);
PairDoubleDouble getDPI();
double getHorizontalDPI();
double getVerticalDPI();

enum DRAW_TEXT {
DRAW_TEXT_BEGIN, DRAW_TEXT_CENTER, DRAW_TEXT_END
};
//setFont(cr, "Times New Roman", 22,CAIRO_FONT_SLANT_NORMAL,
//		CAIRO_FONT_WEIGHT_NORMAL);
void setFont(cairo_t *cr, const char *family, int height,
cairo_font_slant_t slant, cairo_font_weight_t weight);
void setFont(cairo_t *cr, int height);
void drawText(cairo_t *cr, std::string const &s, double x, double y,
DRAW_TEXT optionx, DRAW_TEXT optiony);
void drawMarkup(cairo_t *cr, std::string text, double x, double y,
DRAW_TEXT optionx, DRAW_TEXT optiony);
void drawMarkup(cairo_t *cr, std::string text, cairo_rectangle_int_t rect,
DRAW_TEXT optionx, DRAW_TEXT optiony);
PangoFontDescription* createPangoFontDescription(const PangoFontDescription *f,
int height);
PangoFontDescription* createPangoFontDescription();
PangoLayout* createPangoLayout(cairo_t *cr, std::string text);

#endif
int getNumberOfCores();
double timeElapse(clock_t begin);
std::string secondsToString(double seconds);
std::string secondsToString(clock_t end, clock_t begin);
std::string secondsToString(clock_t begin);
std::string trim(const std::string &s);
std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
void setNumericLocale();
void setAllLocales();
//"1.23000" -> "1.23", "1.00" -> "1", "123" -> "123"
std::string normalize(std::string const &s);
double randomDouble(double from, double to);
void preventThreadSleep();

#ifdef __GNUC__
template<class T> std::string aslovTypeName() {
typedef typename std::remove_reference<T>::type TR;
std::unique_ptr<char, void (*)(void*)> own(
	abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
	std::free);
std::string r = own != nullptr ? own.get() : typeid(TR).name();
if (std::is_const<TR>::value)
r += " const";
if (std::is_volatile<TR>::value)
r += " volatile";
if (std::is_lvalue_reference<T>::value)
r += "&";
else if (std::is_rvalue_reference<T>::value)
r += "&&";
return r;
}

/* int i;
 * std::string s=OBJECT_TYPE(i); //s="int"
 * */
#define OBJECT_TYPE(v)   aslovTypeName<decltype(v)>()
#endif /* __GNUC__ */

void aslovSetOutputWidth(int width);

#endif /* ASLOV_H_ */
