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
#include <windows.h> //getNumberOfCores
#ifdef NOGTK
#include <iconv.h>
#include <cstdarg>
#include <sys/stat.h>//getFileSize
#else
#include <glib/gstdio.h>
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
#ifndef NOGTK
bool isDir(const char *url) {
	GStatBuf b;
	if (g_stat(url, &b) != 0) {
		//error get file stat
		return false;
	}

	return S_ISDIR(b.st_mode);
}

bool isDir(const std::string& s) {
	return isDir(s.c_str());
}
#endif

std::string getFileInfo(std::string filepath, FILEINFO fi) {
	//"c:\\slove.sno\\1\\rr" -> extension = ""
	std::size_t pos = filepath.rfind(G_DIR_SEPARATOR);
	if (fi == FILEINFO::DIRECTORY) {
		return filepath.substr(0, pos);
	}
	std::string name = filepath.substr(pos + 1);
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

FILE* open(std::string filepath, const char *flags) {
	auto s = utf8ToLocale(filepath);
	return fopen(s.c_str(), flags);
}
//END file functions

//BEGIN application functions
void aslovInit(char const*const*argv) {
	const std::string p = localeToUtf8(argv[0]);
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


bool startsWith(const char *buff, const char *begin) {
	return strncmp(buff, begin, strlen(begin)) == 0;
}

bool startsWith(const char *buff, const std::string &begin) {
	return startsWith(buff, begin.c_str());
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

int charIndex(const char *p, char c) {
	return strchr(p, c) - p;
}

#ifdef NOGTK //local function should be before localeToUtf8 & utf8ToLocale
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

std::string utf8ToLowerCase(const std::string &s,
		bool onlyRussainChars/*=false*/) {
	return localeToUtf8(localeToLowerCase(utf8ToLocale(s), onlyRussainChars));
}
//END string functions

//BEGIN pixbuf functions
#ifndef NOGTK
void getPixbufWH(GdkPixbuf *p,int&w,int&h){
	w = gdk_pixbuf_get_width(p);
	h = gdk_pixbuf_get_height(p);
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

GtkWidget* image(const char* s){
	return gtk_image_new_from_file(getImagePath(s).c_str());
}

GtkWidget* image(const std::string& s){
	return image(s.c_str());
}

#endif
//END pixbuf functions


//BEGIN 2 dimensional array functions
//END 2 dimensional array functions

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
#endif

int getNumberOfCores() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
