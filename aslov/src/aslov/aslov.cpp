/*
 * aslov.cpp
 *
 *  Created on: 03.06.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#include <cstring>
#include <cassert>

#ifdef NOGTK
#if NOGTK==0
#include <iconv.h>
#endif
#include <cstdarg>
#include <sys/stat.h>//getFileSize
#else
#include <glib/gstdio.h>
#endif

//_WIN32 for win32 & win64 for getNumberOfCores() function, for gtk & notgtk options
#ifdef _WIN32
#include <windows.h>
#elif G_OS_UNIX
#include <unistd.h>
#else
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#include "aslov.h"


std::string applicationName, applicationPath, workingDirectory;

//format to string example format("%d %s",1234,"some")
std::string format(const char *f, ...) {
	va_list a;
	va_start(a, f);
	size_t size = vsnprintf(nullptr, 0, f, a) + 1;
	va_end(a);
	std::string s;
	if (size > 1) {
		s.resize(size);
		va_start(a, f);
		vsnprintf(&s[0], size, f, a);
		va_end(a);
		s.resize(size - 1);
	}
	return s;
}

void aslovPrintHelp(bool toFile, const std::string &s, const char *f, const int l,
		const char *fu) {
	const char *p = strrchr(f, G_DIR_SEPARATOR);
	p = p ? p + 1 : f;
	if (toFile) {
		time_t t = time(NULL);
		tm *q = localtime(&t);
		FILE *w = openApplicationLog("a");
		fprintf(w, "%s %s:%d %s() %02d:%02d:%02d %02d.%02d.%d\n", s.c_str(), p,
				l, fu, q->tm_hour, q->tm_min, q->tm_sec, q->tm_mday,
				q->tm_mon + 1, q->tm_year + 1900);
		fclose(w);
	} else {
		g_print("%-40s %s:%d %s()\n", s.c_str(), p, l, fu);
	}
}

//BEGIN file functions
bool isDir(const char *path) {
#ifdef NOGTK
	struct stat s;
	if( stat(path,&s) == 0 ){
    	return s.st_mode & S_IFDIR;
	}
	else{
	    assert(0);
	    return false;
	}
#else
	GStatBuf b;
	if (g_stat(path, &b) != 0) {
		//error get file stat
	    assert(0);
	    return false;
	}

	return S_ISDIR(b.st_mode);
#endif
}

bool isDir(const std::string& path) {
	return isDir(path.c_str());
}

std::string getFileInfo(std::string path, FILEINFO fi) {
	//"c:\\slove.sno\\1\\rr" -> extension = ""
	std::size_t pos = path.rfind(G_DIR_SEPARATOR);
	if (fi == FILEINFO::DIRECTORY) {
		return path.substr(0, pos);
	}
	std::string name = path.substr(pos + 1);
	if (fi == FILEINFO::NAME) {
		return name;
	}
	pos = name.rfind('.');
	if (fi == FILEINFO::SHORT_NAME) {
		if (pos == std::string::npos) {
			return name;
		}
		else{
			return name.substr(0, pos);
		}
	}
	if (pos == std::string::npos) {
		return "";
	}
	std::string s = name.substr(pos + 1);
	if (fi == FILEINFO::LOWER_EXTENSION) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
			return std::tolower(c);
		});
	}
	return s;
}

int getFileSize(const std::string &path) {
#ifdef NOGTK
    struct stat b;
    int rc = stat(path.c_str(), &b);
    return rc == 0 ? b.st_size : -1;
#else
	GStatBuf b;
	g_stat(path.c_str(), &b);
	return b.st_size;
#endif
}

FILE* open(std::string path, const char *flags) {
#if !defined(NOGTK) || NOGTK==0
	auto s = utf8ToLocale(path);
#else
	auto s=path;
#endif
	return fopen(s.c_str(), flags);
}
//END file functions

//BEGIN application functions
void aslovInit(char const*const*argv) {
#if !defined(NOGTK) || NOGTK==0
	const std::string p = localeToUtf8(argv[0]);
#else
	const std::string p = argv[0];
#endif

	applicationPath = p;
	applicationName = getFileInfo(p, FILEINFO::SHORT_NAME);

	std::string s = getFileInfo(p, FILEINFO::DIRECTORY);
	for (std::string r : { "Release", "Debug" }) {
		if (endsWith(s, r)) {
			workingDirectory = s.substr(0, s.length() - r.length()-1);
			return;
		}
	}
	workingDirectory = s;
}

int getApplicationFileSize() {
	return getFileSize(applicationPath);
}

#ifdef NOGTK
const char* g_get_user_config_dir(){
	return workingDirectory.c_str();
}
#endif

FILE* openApplicationLog(const char *flags) {
	std::string p = g_get_user_config_dir()
			+ (G_DIR_SEPARATOR + applicationName) + ".log.txt";
	return open(p, flags);
}

std::string const& getApplicationName(){
	return applicationName;
}

std::string const& getApplicationPath(){
	return applicationPath;
}

std::string const& getWorkingDirectory(){
	return workingDirectory;
}

std::string getResourcePath(std::string name){
	return workingDirectory+G_DIR_SEPARATOR+applicationName+G_DIR_SEPARATOR+name;
}

std::string getImagePath(std::string name) {
	return getResourcePath("images/" + name);
}

//END application functions


//BEGIN config functions
std::string getConfigPath() {
	return g_get_user_config_dir() + (G_DIR_SEPARATOR + applicationName)
			+ ".cfg";
}

bool loadConfig(MapStringString&map){
	const int MAX_BUFF=4096;
	char b[MAX_BUFF];

	auto f = open(getConfigPath(), "r");
	if (!f) {//it's ok first time loading
		return false;
	}

	//order of strings in file is not important
	while (fgets(b, MAX_BUFF, f) ) {
		auto p=pairFromBuffer(b);
#ifndef NDEBUG
		if(map.find(p.first)!=map.end()){
			printl("warning duplicate key",p.first)
		}
#endif
		map.insert(p);
	}
	fclose(f);
	return true;

}

std::string aslovToString(std::string const& a){
	return a;
}

std::string aslovToString(const char* a){
	return a;
}

PairStringString pairFromBuffer(const char*b){
	char*w=strchr(b, '=');
	if(!w){
		return {"",""};
	}
	const char*p = w + 2;
	char*f;
	const char *search;
	const char s[] = "\r\n";
	for (search = s; *search != '\0'; search++) {
		f = strrchr(p, *search);
		if (f != NULL) {
			*f = 0;
		}
	}
	if(w>b && w[-1]==' '){
		w--;
	}
	return {std::string(b,w-b),std::string(p)};
}
//END config functions


//BEGIN string functions
std::string intToString(int v, char separator) { //format(1234567,3)="1 234 567"
	const int digits = 3;
	char b[16];
	std::string s;
	sprintf(b, "%d", v);
	int i;
	char *p;
	for (p = b, i = strlen(b) - 1; *p != 0; p++, i--) {
		s += *p;
		if (i % digits == 0 && i != 0) {
			s += separator;
		}
	}
	return s;
}

bool stringToInt(const std::string&d,int&v){
	return stringToInt(d.c_str(),v);
}

bool stringToInt(const char*d,int&v){
	char* p;
	long l = strtol(d, &p, 10);
	if (!*p) {
		v=l;
	}
	return !*p;
}

bool startsWith(const char *s, const char *begin) {
	return strncmp(s, begin, strlen(begin)) == 0;
}

bool startsWith(const char *s, const std::string &begin) {
	return startsWith(s, begin.c_str());
}

bool startsWith(const std::string& s, const char* begin){
	return startsWith(s.c_str(), begin);
}

bool endsWith(std::string const &s, std::string const &e) {
	auto i = e.length();
	return s.compare(s.length() - i, i, e) == 0;
}

std::string replaceAll(std::string subject, const std::string &from,
		const std::string &to) {
	size_t pos = 0;
	while ((pos = subject.find(from, pos)) != std::string::npos) {
		subject.replace(pos, from.length(), to);
		pos += to.length();
	}
	return subject;
}

VString split(const std::string& subject, const std::string& separator) {
	VString r;
	size_t pos, prev;
	for (prev = 0; (pos = subject.find(separator, prev)) != std::string::npos;
			prev = pos + separator.length()) {
		r.push_back(subject.substr(prev, pos - prev));
	}
	r.push_back(subject.substr(prev, subject.length()));
	return r;
}

int countOccurence(const std::string &subject, const std::string &a) {
	size_t pos = 0;
	int i = 0;
	while ((pos = subject.find(a, pos)) != std::string::npos) {
		pos += a.length();
		i++;
	}
	return i;
}

int charIndex(const char *p, char c) {
	return strchr(p, c) - p;
}

bool cmpnocase(const std::string& a, const char* b) {
	return cmpnocase(a.c_str(), b);
}

bool cmpnocase(const char* a, const char* b) {
	return strcasecmp(a, b) == 0;
}

bool cmp(const char* a, const char* b) {
	return strcmp(a, b) == 0;
}

bool cmp(const std::string& a, const char* b) {
	return cmp(a.c_str(), b);
}

#if NOGTK==0 //local function should be before localeToUtf8 & utf8ToLocale
std::string encodeIconv(const std::string& s, bool toUtf8) {
	std::string r;
	const char UTF8[]="UTF-8";
	const char CP1251[]="cp1251";
	iconv_t cd = toUtf8 ? iconv_open( UTF8, CP1251 ) : iconv_open( CP1251, UTF8 );
	if ((iconv_t) -1 == cd) {
		printl("error iconv_open");
		perror("iconv_open");
		return r;
	}

	size_t inbytesleft = s.length();
	char*in=new char[inbytesleft];
	strncpy(in,s.c_str(),inbytesleft);
	size_t outbytesleft = inbytesleft*2+1;
	char*out=new char[outbytesleft];
	char*outbuf=out;
	size_t ret = iconv(cd, &in, &inbytesleft, &outbuf, &outbytesleft);
	if ((size_t) -1 == ret) {
		printl("error iconv");
		perror("iconv");
		return r;
	}
	*outbuf=0;
	r=out;
	delete[]out;
	iconv_close(cd);
	return r;
}
#endif

#if !defined(NOGTK) || NOGTK==0

const std::string localeToUtf8(const std::string &s) {
#ifdef NOGTK
	return encodeIconv(s,true);
#else
	gchar *a = g_locale_to_utf8(s.c_str(), s.length(), NULL, NULL, NULL);
	std::string r(a);
	g_free(a);
	return r;
#endif
}

const std::string utf8ToLocale(const std::string &s) {
#ifdef NOGTK
	return encodeIconv(s,false);
#else
	gchar *a = g_locale_from_utf8(s.c_str(), s.length(), NULL, NULL, NULL);
	std::string r(a);
	g_free(a);
	return r;
#endif
}

std::string utf8ToLowerCase(const std::string &s,
		bool onlyRussainChars/*=false*/) {
#ifdef NOGTK
	return localeToUtf8(localeToLowerCase(utf8ToLocale(s), onlyRussainChars));
#else
	gchar*a=g_utf8_strdown(s.c_str(), s.length());
	std::string r(a);
	g_free(a);
	return r;
#endif
}
#endif

std::string localeToLowerCase(const std::string &s, bool onlyRussainChars) {
	typedef const unsigned char *cpuchar;
	cpuchar p;
	std::string q;
	for (p = cpuchar(s.c_str()); *p != 0; p++) {
		q += (*p >= 0xc0 && *p < 0xe0) ?
				(*p) + 0x20 : (onlyRussainChars ? *p : tolower(*p));
	}
	return q;
}
//END string functions

//BEGIN pixbuf functions
#ifndef NOGTK
void getPixbufWH(GdkPixbuf *p,int&w,int&h){
	w = gdk_pixbuf_get_width(p);
	h = gdk_pixbuf_get_height(p);
}

void getPixbufWH(const char*s,int&w,int&h){
	GdkPixbuf* p = pixbuf(s);
	assert(p);
	getPixbufWH(p, w, h);
	g_object_unref(p);
}

void getPixbufWH(std::string const& s,int&w,int&h){
	getPixbufWH(s.c_str(),w,h);
}


void free(GdkPixbuf*&p){
	if (p) {
		g_object_unref(p);
		p = 0;
	}
}

void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
		int height, int sourcex, int sourcey) {
	gdk_cairo_set_source_pixbuf(dest, source, destx - sourcex, desty - sourcey);
	cairo_rectangle(dest, destx, desty, width, height);
	cairo_fill(dest);
}

GdkPixbuf* pixbuf(const char* s){
	return gdk_pixbuf_new_from_file(getImagePath(s).c_str(), NULL);
}

GdkPixbuf* pixbuf(const std::string& s){
	return pixbuf(s.c_str());
}

GdkPixbuf* pixbuf(std::string s, int x, int y, int width,
		int height) {
	return gdk_pixbuf_new_subpixbuf(pixbuf(s), x, y, width, height);
}

GtkWidget* image(const char* s){
	return gtk_image_new_from_file(getImagePath(s).c_str());
}

GtkWidget* image(const std::string& s){
	return image(s.c_str());
}

GtkWidget* animatedImage(const char* s){
	return gtk_image_new_from_animation(
			gdk_pixbuf_animation_new_from_file(getImagePath(s).c_str(), 0));
}

#endif
//END pixbuf functions


//BEGIN 2 dimensional array functions
//END 2 dimensional array functions


int indexOfNoCase(const char *t,const char *v[], int size) {
	for (int i = 0; i < size; i++) {
		if (cmpnocase(v[i], t)) {
			return i;
		}
	}
	return -1;
}

int indexOfNoCase(const std::string t,const char *v[], int size ) {
	return indexOfNoCase(t.c_str(),v, size );
}


#ifndef NOGTK
void addClass(GtkWidget *w, const gchar *s) {
	GtkStyleContext *context;
	context = gtk_widget_get_style_context(w);
	gtk_style_context_add_class(context, s);
}

void removeClass(GtkWidget *w, const gchar *s) {
	GtkStyleContext *context;
	context = gtk_widget_get_style_context(w);
	gtk_style_context_remove_class(context, s);
}

void loadCSS(){
	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;

	provider = gtk_css_provider_new();
	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen,
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	std::string h = workingDirectory+G_DIR_SEPARATOR+applicationName+".css";
	gtk_css_provider_load_from_path(provider,h.c_str(), NULL);
	g_object_unref(provider);
}

void openURL(std::string url) {
	gtk_show_uri_on_window(0, url.c_str(), gtk_get_current_event_time(), NULL);
}

void destroy(cairo_t* p) {
	if (p) {
		cairo_destroy(p);
	}
}

void destroy(cairo_surface_t * p) {
	if (p) {
		cairo_surface_destroy(p);
	}
}

#endif

int getNumberOfCores() {
#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#elif G_OS_UNIX
	//tested ok
	return sysconf(_SC_NPROCESSORS_ONLN);
#else
	//Did not tested
	int nm[2];
	size_t len = 4;
	uint32_t count;

	nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
	sysctl(nm, 2, &count, &len, NULL, 0);

	if(count < 1) {
		nm[1] = HW_NCPU;
		sysctl(nm, 2, &count, &len, NULL, 0);
		if(count < 1) {count = 1;}
	}
	return count;
#endif
}
