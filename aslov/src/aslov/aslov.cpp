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
#include <cmath>
#include <random>
#include "aslov.h"

#ifdef NOGTK
#include <cstdarg>
#include <sys/stat.h>//getFileSize
#else
#include <glib/gstdio.h>
#endif

//after #include "aslov.h"
#ifdef NOTGK_WITH_ICONV
#include <iconv.h> //ciconv - error
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


static std::string applicationName, applicationPath;
#ifndef NOGTK
static std::string fontFamily;
static int fontHeight;
static cairo_font_slant_t fontSlant;
static cairo_font_weight_t fontWeight;
#endif
#ifdef _WIN32
static PairDoubleDouble scale;
#endif

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
void aslovInit(char const*const*argv,bool storeScaleFactor/*=false*/) {
#ifdef _WIN32
	if(storeScaleFactor){
		scale=aslovGetScaleFactor();
	}
#endif

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
			s = s.substr(0, s.length() - r.length()-1);
		}
	}
	//workingDirectory = s;
#ifndef NOGTK
	g_chdir(s.c_str());
	s=getWritableFilePath("");
	//2nd parameter. The mode argument is ignored on Windows
	g_mkdir(s.c_str(), 0);
#endif
}

int getApplicationFileSize() {
	return getFileSize(applicationPath);
}

FILE* openApplicationLog(const char *flags) {
	return open(getWritableFilePath("log.txt"), flags);
}

std::string const& getApplicationName(){
	return applicationName;
}

std::string getResourcePath(const std::string name){
	return applicationName+G_DIR_SEPARATOR+name;
}

std::string getImagePath(const std::string name) {
	return getResourcePath("images/" + name);
}

std::ifstream openResourceFileAsStream(const std::string name){
	std::ifstream f(getResourcePath(name));
	return f;
}

std::string getWritableFilePath(const std::string name){
#ifdef NOGTK
	return name;
#else
	return g_get_user_config_dir() +(G_DIR_SEPARATOR + applicationName)+G_DIR_SEPARATOR+ name;
#endif
}

#ifndef NOGTK
void writableFileSetContents(const std::string name,const std::string& s){
#ifndef NDEBUG
	gboolean b=
#endif
			g_file_set_contents(getWritableFilePath(name).c_str(), s.c_str(), s.length(), 0);
	assert(b);
}

const std::string writableFileGetContents(const std::string& name){
	return fileGetContent(getWritableFilePath(name));
}
#endif

const std::string fileGetContent(const std::string& path){
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}
//END application functions


//BEGIN config functions
#ifndef NOGTK
std::string getConfigPath() {
	return getWritableFilePath("config.txt");
}

bool loadConfig(MapStringString&map){
	std::string s=utf8ToLocale(getConfigPath());
	std::ifstream f(s);
	if(!f.is_open()){//it's ok first time loading
		return false;
	}

	//order of strings in file is not important
	while( std::getline( f, s ) ){
		auto p=pairFromBuffer(s);
#ifndef NDEBUG
		if(map.find(p.first)!=map.end()){
			printl("warning duplicate key",p.first)
		}
#endif
		map.insert(p);
	}
	return true;

}

PairStringString pairFromBuffer(const std::string&s){
	return pairFromBuffer(s.c_str());
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
#endif
//END config functions


//BEGIN string functions
bool startsWith(const char *s, const char *begin) {
	return strncmp(s, begin, strlen(begin)) == 0;
}

bool startsWith(const char *s, const std::string &begin) {
	return startsWith(s, begin.c_str());
}

bool startsWith(const std::string& s, const char* begin){
	return startsWith(s.c_str(), begin);
}

bool startsWith(const std::string& s, const std::string & begin){
	return startsWith(s.c_str(), begin.c_str());
}

bool endsWith(std::string const &s, std::string const &e) {
	auto i = e.length();
	auto l= s.length();
	if(l<i){
		return false;
	}
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

VString split(const std::string& subject, const char separator){
	return split(subject, std::string(1,separator));
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

int countOccurence(const std::string& subject, const char c) {
	return count_if(subject.begin(), subject.end(), [&c](char a) {return a==c;});
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

#ifdef NOTGK_WITH_ICONV //local function should be before localeToUtf8 & utf8ToLocale
std::string encodeIconv(const std::string& s, bool toUtf8) {
	std::string r;
	const char UTF8[]="UTF-8";
	/* use "iconv --list" under msys2 to view supported encodings
	 * empty string means current locale https://rdrr.io/r/base/iconv.html
	 */
	const char LOCAL[]="";
	//const char LOCAL[]="cp1251";
	iconv_t cd = toUtf8 ? iconv_open( UTF8, LOCAL ) : iconv_open( LOCAL, UTF8 );
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
		printl("error iconv ",s);
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

#ifndef NOTGK_WITHOUT_ICONV

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

std::string utf8ToLowerCase(const std::string &s
#ifdef NOGTK
		,bool onlyRussainChars/*=false*/
#endif
		) {
#ifdef NOGTK
	return localeToUtf8(localeToLowerCase(utf8ToLocale(s), onlyRussainChars));
#else
	gchar*a=g_utf8_strdown(s.c_str(), s.length());
	std::string r(a);
	g_free(a);
	return r;
#endif
}

std::string utf8ToUpperCase(const std::string &s
#ifdef NOGTK
		,bool onlyRussainChars/*=false*/
#endif
		){
#ifdef NOGTK
	assert(0);//TODO
	return "";
#else
	gchar*a=g_utf8_strup(s.c_str(), s.length());
	std::string r(a);
	g_free(a);
	return r;
#endif

}
#endif //#ifndef NOTGK_WITHOUT_ICONV

#ifndef NOGTK
std::string utf8Substring(const std::string &s, glong start_pos, glong end_pos){
	gchar *p = g_utf8_substring(s.c_str(), start_pos, end_pos);
	std::string r=p;
	g_free(p);
	return r;
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

GdkPixbuf* writablePixbuf(const char* s){
	std::string q=getWritableFilePath(s);
	return gdk_pixbuf_new_from_file(q.c_str(), NULL);
}

GdkPixbuf* writablePixbuf(const std::string& s){
	return writablePixbuf(s.c_str());
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

int indexOf(const char t,const std::string& v){
	auto i=v.find(t);
	return i==std::string::npos ? -1 : i;
}

#ifdef _WIN32
//Note this function should be called before gtk_init
PairDoubleDouble aslovGetScaleFactor(){
    auto activeWindow = GetActiveWindow();
    HMONITOR monitor = MonitorFromWindow(activeWindow, MONITOR_DEFAULTTONEAREST);

    // Get the logical width and height of the monitor
    MONITORINFOEX monitorInfoEx;
    monitorInfoEx.cbSize = sizeof(monitorInfoEx);
    GetMonitorInfo(monitor, &monitorInfoEx);
    auto cxLogical = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
    auto cyLogical = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;

    // Get the physical width and height of the monitor
    DEVMODE devMode;
    devMode.dmSize = sizeof(devMode);
    devMode.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    auto cxPhysical = devMode.dmPelsWidth;
    auto cyPhysical = devMode.dmPelsHeight;

    // Calculate the scaling factor
    return  {(double) cxPhysical / (double)cxLogical, ((double) cyPhysical / (double)cyLogical)};
}

PairDoubleDouble getScaleFactor(){
	return scale;
}
#endif

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

void loadCSS(std::string const &additionalData /*= ""*/){
	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;
	std::string s;

	provider = gtk_css_provider_new();
	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen,
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	std::ifstream t(applicationName+".css");
	std::stringstream buffer;
	buffer << t.rdbuf();
	s=buffer.str()+additionalData;
	gtk_css_provider_load_from_data(provider, s.c_str(), -1, NULL);
	g_object_unref(provider);
}

void openURL(std::string url) {
	/* gtk_show_uri_on_window works differ on my old notebook with windows7
	 * and new notebook with windows10
	 * gtk_show_uri_on_window open url/local file using windows word program on old notebook
	 * so cann't do works correctly on both notebooks using gtk_show_uri_on_window
	 * so use ShellExecute all variants works correctly
	 * ShellExecute(0, 0, "http://www.google.com", 0, 0 , SW_SHOW );
	 * ShellExecute(0, 0, "file:///C:\\slovesno\\a.txt", 0, 0 , SW_SHOW );
	 * ShellExecute(0, 0, "file:///C:/slovesno/a.txt", 0, 0 , SW_SHOW );
	 * ShellExecute(0, 0, "C:/slovesno/a.txt", 0, 0 , SW_SHOW );
	 * ShellExecute(0, 0, "C:\\slovesno\\a.txt", 0, 0 , SW_SHOW );
	 * ShellExecute(0, 0, "C:\\slovesno\\a.html", 0, 0 , SW_SHOW );
	 */
#ifdef _WIN32
	//printv(url)
	ShellExecute(0, 0, url.c_str(), 0, 0 , SW_SHOW );
#else
	gtk_show_uri_on_window(0, url.c_str(), gtk_get_current_event_time(), NULL);
#endif
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

std::string getBuildVersionString(bool _long){
	return getBuildString(_long)+", "+getVersionString(_long);
}

std::string getBuildString(bool _long) {
	/* Note date&time will be when compiler compile file which
	 * calls getBuildString, so it's time no last compilation time
	 */
	//__DATE__="Dec 15 2016" one needs "15 Dec 2016" or "15 December 2016"
	int i = _long ? 0 : __DATE__[4] == ' '; //day<10, avoid two spaces after 'build'
	const char *b = 0;
	if (_long) {
		const char *MONTH[] = { "January", "February", "March", "April", "May",
				"June", "July", "August", "September", "October", "November",
				"December" };

		for (auto a : MONTH) {
			if (strncasecmp(__DATE__, a, 3) == 0) {
				b = a;
				break;
			}
		}
	}
	return format("build %.*s %.*s %s %s", 2 - i,
	__DATE__ + 4 + i, _long ? strlen(b) : 3, _long ? b : __DATE__, __DATE__ + 7,
			__TIME__);
}

std::string getVersionString(bool _long) {
	return format("gcc%s %s, gtk%s %d.%d.%d", _long ? " version" : "",
	__VERSION__, _long ? " version" : "", GTK_MAJOR_VERSION,
	GTK_MINOR_VERSION,
	GTK_MICRO_VERSION);
}

void showHideWidget(GtkWidget *w,bool show){
	if(show){
		gtk_widget_show(w);
	}
	else{
		gtk_widget_hide(w);
	}
}

std::pair<double,double> getMonitorSize(bool millimeters/*=true*/){
	auto monitor = gdk_display_get_monitor(gdk_display_get_default(), 0);
	GdkRectangle r;
	gdk_monitor_get_geometry(monitor, &r);
	//not auto!
	double w = gdk_monitor_get_width_mm(monitor);
	double h = gdk_monitor_get_height_mm(monitor);
	if(!millimeters){
		w/=25.4;
		h/=25.4;
	}
	return {w,h};
}

double getMonitorDiagonal(bool millimeters/*=true*/){
	auto a=getMonitorSize(millimeters);
	return sqrt(a.first*a.first+a.second*a.second);
}

std::pair<double,double> getDPI(){
	auto monitor = gdk_display_get_monitor(gdk_display_get_default(), 0);
	GdkRectangle r;
	gdk_monitor_get_geometry(monitor, &r);
	auto w = gdk_monitor_get_width_mm(monitor);
	auto h = gdk_monitor_get_height_mm(monitor);
	return {r.width * 25.4 / w, r.height * 25.4 / h};
}

double getHorizontalDPI() {
	return getDPI().second;
}

double getVerticalDPI() {
	return getDPI().first;
}

void setFont(cairo_t *cr, const char *family, int height,
		cairo_font_slant_t slant, cairo_font_weight_t weight) {
	fontFamily=family;
	fontHeight=height;
	fontSlant=slant;
	fontWeight=weight;
	cairo_select_font_face(cr, family, slant, weight);
	cairo_set_font_size(cr, height);
}

void setFont(cairo_t *cr, int height){
	//if fontFamily="" need to call setFont(cairo_t *, const char *, int ,cairo_font_slant_t, cairo_font_weight_t ) at first
	fontHeight=height;
	cairo_set_font_size(cr, height);
}


void drawText(cairo_t *cr, std::string const &s, double x, double y,
		DRAW_TEXT optionx, DRAW_TEXT optiony) {
	const char *p = s.c_str();
	cairo_text_extents_t e;
	cairo_text_extents(cr, p, &e);

	//x_bearing && y_bearing<0
	x -= e.x_bearing;
	if (optionx != DRAW_TEXT_BEGIN) {
		x -= e.width * (optionx == DRAW_TEXT_END ? 1 : .5);
	}

	y -= e.y_bearing;
	if (optiony != DRAW_TEXT_BEGIN) {
		y -= e.height * (optiony == DRAW_TEXT_END ? 1 : .5);
	}
	cairo_move_to(cr, x, y);
	cairo_show_text(cr, p);
}

void drawMarkup(cairo_t *cr, std::string text, double x, double y,
		DRAW_TEXT optionx, DRAW_TEXT optiony){
	cairo_rectangle_int_t rect={int(x),int(y),0,0};
	drawMarkup(cr, text, rect, optionx, optiony);
}

void drawMarkup(cairo_t *cr, std::string text, cairo_rectangle_int_t rect,
		DRAW_TEXT optionx, DRAW_TEXT optiony) {
	int w, h;
	PangoLayout *layout = createPangoLayout(cr,text);
	pango_layout_get_pixel_size(layout, &w, &h);

	double px = rect.x;
	double py = rect.y;
	if (optionx!=DRAW_TEXT_BEGIN) {
		px += (rect.width - w) / (optionx==DRAW_TEXT_END?1:2);
	}
	if (optiony!=DRAW_TEXT_BEGIN) {
		py += (rect.height - h) /  (optiony==DRAW_TEXT_END?1:2);
	}

	cairo_move_to(cr, px, py);
	pango_cairo_update_layout(cr, layout);
	pango_cairo_show_layout(cr, layout);
	g_object_unref(layout);
}

PangoFontDescription* createPangoFontDescription(const PangoFontDescription*f,int height){
	PangoFontDescription* d=pango_font_description_copy(f);
	pango_font_description_set_absolute_size (d, height * PANGO_SCALE);
	return d;
}

PangoFontDescription* createPangoFontDescription(){
	std::string s=fontFamily+","+std::to_string(fontHeight);
	PangoFontDescription* d=pango_font_description_from_string(s.c_str());
	//https://www.cairographics.org/FAQ/
	pango_font_description_set_absolute_size (d, fontHeight * PANGO_SCALE);
	return d;
}

PangoLayout* createPangoLayout(cairo_t *cr,std::string text){
	PangoLayout *layout = pango_cairo_create_layout(cr);
	PangoFontDescription*desc = createPangoFontDescription();
	pango_layout_set_font_description(layout, desc);

	pango_layout_set_markup(layout, text.c_str(), -1);
	pango_font_description_free(desc);
	return layout;
}

#endif

int getNumberOfCores() {
#ifdef _WIN32 //sometimes this function is uses without using gtk library so use _WIN32 instead of G_OS_WIN32
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

double timeElapse(clock_t begin){
	return double(clock() - begin) / CLOCKS_PER_SEC;
}

std::string secondsToString(double seconds){
	int h,m,s,t=int(seconds);
	h = t/3600;
	t = t%3600;
	m = t/60;
	t = t%60;
	s = t;
	if(h>0){
		return format("%d:%02d:%02d",h,m,s);
	}
	else{
		return format("%d:%02d",m,s);
	}
}

std::string secondsToString(clock_t end,clock_t begin){
	return secondsToString(double(end - begin) / CLOCKS_PER_SEC);
}

std::string trim(const std::string& s) {
	std::string q = ltrim(s);
	return rtrim(q);
}

std::string ltrim(const std::string& s) {
	std::string::const_iterator it;
	for (it = s.begin(); it != s.end() && isspace(*it); it++)
		;
	return s.substr(it - s.begin());
}

std::string rtrim(const std::string& s) {
	std::string::const_reverse_iterator it;
	for (it = s.rbegin(); it != s.rend() && isspace(*it); it++)
		;
	return s.substr(0, s.length() - (it - s.rbegin()));
}

void setNumericLocale(){
	setlocale(LC_NUMERIC, "C");
}

std::string normalize(std::string const& s){
	std::string::size_type p,p1=0;
	if ((p= s.find('.')) == std::string::npos) {
		return s;
	}
	for(p1=s.length()-1;p1>p && s[p1]=='0';p1--);
		//"3.875000"->"3.875"
	if(s[p1]=='.'){
		p1--;
	}
	return s.substr(0,p1+1);
}

//https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c/9324796
double randomDouble(double from, double to) {
	thread_local static std::mt19937 gen(std::random_device { }());

	using dist_type = typename std::conditional
	<
	std::is_integral<double>::value
	, std::uniform_int_distribution<double>
	, std::uniform_real_distribution<double>
	>::type;

	thread_local static dist_type dist;

	return dist(gen, typename dist_type::param_type { from, to });
}

void preventThreadSleep(){
#ifdef _WIN32
	//prevents windows 10 threads sleep
	//stackoverflow.com/questions/34836406/how-to-prevent-windows-from-going-to-sleep-when-my-c-application-is-running
	SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif
}
