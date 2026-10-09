/*
 * aslov.cpp
 *
 *  Created on: 03.06.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#include "aslov.h"
#include <cmath>
#include <cstring>
#include <random>

#ifdef NOGTK
#include <cstdarg>
#include <sys/stat.h> //getFileSize
#include <thread>     //std::thread::hardware_concurrency()

#else
#include <glib/gstdio.h>
#endif

// after #include "aslov.h"
#ifdef NOTGK_WITH_ICONV
#include <iconv.h> //ciconv - error
#endif

//_WIN32 for win32 & win64 for openURL
#ifdef _WIN32
#include <windows.h>
#endif

static std::string applicationName, applicationPath;
std::mutex aslovcout_mutex;
#ifndef NOGTK
static std::string fontFamily;
static int fontHeight;
static cairo_font_slant_t fontSlant;
static cairo_font_weight_t fontWeight;
#endif
#ifdef _WIN32
static PairDoubleDouble scale;
#endif
static int aslovOutputWide = 40;

// BEGIN file functions
bool isDir(std::string_view path) {
  std::string p(path);
#ifdef NOGTK
  struct stat s;
  if (stat(p.c_str(), &s) == 0) {
    return s.st_mode & S_IFDIR;
  } else {
    assert(0);
    return false;
  }
#else
  GStatBuf b;
  if (g_stat(p.c_str(), &b) != 0) {
    // error get file stat
    assert(0);
    return false;
  }

  return S_ISDIR(b.st_mode);
#endif
}

std::string getFileInfo(std::string path, FILEINFO fi) {
  //"c:\\slove.sno\\1\\rr" -> extension = ""
  std::size_t pos = path.rfind(G_DIR_SEPARATOR);
  if (fi == FILEINFO::DIRECTORY) {
    return path.substr(0, pos);
  }

  //"./words.exe" also ok
  gchar *exe_name = g_path_get_basename(path.c_str());
  std::string name = exe_name;
  g_free(exe_name);

  // std::string name = path.substr(pos + 1);
  if (fi == FILEINFO::NAME) {
    return name;
  }
  pos = name.rfind('.');
  if (fi == FILEINFO::SHORT_NAME) {
    if (pos == std::string::npos) {
      return name;
    } else {
      return name.substr(0, pos);
    }
  }
  if (pos == std::string::npos) {
    return "";
  }
  std::string s = name.substr(pos + 1);
  if (fi == FILEINFO::LOWER_EXTENSION) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
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

FILE *open(std::string path, const char *flags) {
#if !defined(NOGTK) || NOGTK == 0
  auto s = utf8ToLocale(path);
#else
  auto s = path;
#endif
  return fopen(s.c_str(), flags);
}
// END file functions

// BEGIN application functions
void aslovInit(char const *const *argv, bool storeScaleFactor /*=false*/) {
#ifdef _WIN32
  if (storeScaleFactor) {
    scale = aslovGetScaleFactor();
  }
#endif

#if !defined(NOGTK) || NOGTK == 0
  const std::string p = localeToUtf8(argv[0]);
#else
  const std::string p = argv[0];
#endif

  applicationPath = p;
  applicationName = getFileInfo(p, FILEINFO::SHORT_NAME);

  std::string s = getFileInfo(p, FILEINFO::DIRECTORY);
  for (std::string r : {"Release", "Debug"}) {
    if (s.ends_with(r)) {
      s = s.substr(0, s.length() - r.length() - 1);
    }
  }
  // workingDirectory = s;
#ifndef NOGTK
  g_chdir(s.c_str());
  s = getWritableFilePath("");
  // 2nd parameter. The mode argument is ignored on Windows
  g_mkdir(s.c_str(), 0);
#endif
}

int getApplicationFileSize() { return getFileSize(applicationPath); }

// assume log.txt is uses only in debug mode, so can write in the same dir
void clearlog() { std::ofstream ofs("log.txt", std::ios::trunc); }

std::string const &getApplicationName() { return applicationName; }

std::string getResourcePath(const std::string_view name) {
  return applicationName + G_DIR_SEPARATOR + std::string(name);
}

std::string getImagePath(const std::string_view name) {
  return getResourcePath("images/" + std::string(name));
}

std::ifstream openResourceFileAsStream(const std::string_view name) {
  std::ifstream f(getResourcePath(name));
  return f;
}

std::string getWritableFilePath(const std::string_view name) {
#ifdef NOGTK
  return name;
#else
  return g_get_user_config_dir() + (G_DIR_SEPARATOR + applicationName) +
         G_DIR_SEPARATOR + std::string(name);
#endif
}

#ifndef NOGTK
void writableFileSetContents(const std::string name, const std::string &s) {
#ifndef NDEBUG
  gboolean b =
#endif
      g_file_set_contents(getWritableFilePath(name).c_str(), s.c_str(),
                          s.length(), 0);
  assert(b);
}

const std::string writableFileGetContents(const std::string &name) {
  return fileGetContent(getWritableFilePath(name));
}
#endif

const std::string fileGetContent(const std::string &path,
                                 bool binary /*=false*/) {
  // in non binary mode change "\r\n" to "\n"
  auto m = binary ? std::ios::binary : std::ios::in;
#ifdef NOTGK_WITHOUT_ICONV
  std::ifstream t(path, m);
#else
  std::ifstream t(utf8ToLocale(path), m);
#endif
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

bool filePutContent(const std::string &path, const std::string &content,
                    bool binary /*=false*/) {
  auto m = binary ? (std::ios::out | std::ios::binary | std::ios::trunc)
                  : (std::ios::out | std::ios::trunc);

#ifdef NOTGK_WITHOUT_ICONV
  std::ofstream t(path, m);
#else
  std::ofstream t(utf8ToLocale(path), m);
#endif

  if (!t.is_open()) {
    return false;
  }

  t << content;

  return t.good();
}

// END application functions

// BEGIN config functions
#ifndef NOGTK
std::string getConfigPath() { return getWritableFilePath("config.txt"); }

std::string getConfigPathLocaled() { return utf8ToLocale(getConfigPath()); }

bool loadConfig(MapStringString &map) {
  const std::string delimiter = " = ";
  size_t pos;
  std::string s = getConfigPathLocaled();
  std::ifstream f(s);
  if (!f.is_open()) { // it's ok first time loading
    return false;
  }

  // order of strings in file is not important
  while (std::getline(f, s)) {
    pos = s.find(delimiter);
    assert(pos != std::string::npos);
    auto key = s.substr(0, pos);
    auto value = s.substr(pos + delimiter.length());
    if (map.find(key) != map.end()) {
      assert(("error duplicate key" + key).c_str());
    }
    map.insert({key, value});
  }
  return true;
}

std::string getSystemLanguage() {
  const gchar *const *languages = g_get_language_names();
  if (languages && languages[0]) {
    std::string primary_lang = languages[0];
    if (primary_lang.length() >= 2) {
      return primary_lang.substr(0, 2);
    }
    return primary_lang;
  }
  return "en";
}

#endif
// END config functions

// BEGIN string functions
std::string timeToString(const char *format, bool toLowerCase /*=false*/) {
  const int size = 100;
  char s[size];
  auto t = std::time(nullptr);
  std::strftime(s, size, format, std::localtime(&t));
  if (toLowerCase) {
    std::transform(s, s + size, s, tolower);
  }
  return s;
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

VString split(const std::string &subject, const std::string &separator) {
  VString r;
  size_t pos, prev;
  for (prev = 0; (pos = subject.find(separator, prev)) != std::string::npos;
       prev = pos + separator.length()) {
    r.push_back(subject.substr(prev, pos - prev));
  }
  r.push_back(subject.substr(prev, subject.length()));
  return r;
}

VString split(const std::string &subject, const char separator) {
  return split(subject, std::string(1, separator));
}
#ifndef NOGTK
VString splitr(const std::string &subject, const std::string &regex) {
  VString v;
  auto fields = g_regex_split_simple(regex.c_str(), subject.c_str(),
                                     G_REGEX_DEFAULT, G_REGEX_MATCH_DEFAULT);
  guint i;
  for (i = 0; i < g_strv_length(fields); i++) {
    v.push_back(fields[i]);
  }
  g_strfreev(fields);
  return v;
}
#endif

int countOccurence(const std::string &subject, const std::string &a) {
  size_t pos = 0;
  int i = 0;
  while ((pos = subject.find(a, pos)) != std::string::npos) {
    pos += a.length();
    i++;
  }
  return i;
}

int countOccurence(const std::string &subject, const char c) {
  return count_if(subject.begin(), subject.end(),
                  [&c](char a) { return a == c; });
}

bool cmpnocase(std::string_view a, std::string_view b) {
  if (a.length() != b.length())
    return false;
  return strncasecmp(a.data(), b.data(), a.length()) == 0;
}

bool cmp(std::string_view a, std::string_view b) { return a == b; }
bool contains(std::string_view a, std::string_view b) {
  return a.find(b) != std::string_view::npos;
}
bool contains(std::string_view a, const char b) {
  return a.find(b) != std::string_view::npos;
}

bool cmp(const char *a, const char *b) { return strcmp(a, b) == 0; }

bool cmp(const std::string &a, const char *b) { return cmp(a.c_str(), b); }

bool contains(const std::string &a, const char b) {
  return a.find(b) != std::string::npos;
}

bool contains(const std::string &a, const char *b) {
  return a.find(b) != std::string::npos;
}

bool contains(const std::string &a, const std::string &b) {
  return a.find(b) != std::string::npos;
}

#ifdef NOTGK_WITH_ICONV // local function should be before localeToUtf8 &
                        // utf8ToLocale
std::string encodeIconv(const std::string &s, bool toUtf8) {
  std::string r;
  const char UTF8[] = "UTF-8";
  /* use "iconv --list" under msys2 to view supported encodings
   * empty string means current locale https://rdrr.io/r/base/iconv.html
   */
  const char LOCAL[] = "";
  // const char LOCAL[]="cp1251";
  iconv_t cd = toUtf8 ? iconv_open(UTF8, LOCAL) : iconv_open(LOCAL, UTF8);
  if ((iconv_t)-1 == cd) {
    printl("error iconv_open");
    perror("iconv_open");
    return r;
  }

  size_t inbytesleft = s.length();
  char *in = new char[inbytesleft];
  strncpy(in, s.c_str(), inbytesleft);
  size_t outbytesleft = inbytesleft * 2 + 1;
  char *out = new char[outbytesleft];
  char *outbuf = out;
  size_t ret = iconv(cd, &in, &inbytesleft, &outbuf, &outbytesleft);
  if ((size_t)-1 == ret) {
    printl("error iconv ", s);
    perror("iconv");
    return r;
  }
  *outbuf = 0;
  r = out;
  delete[] out;
  iconv_close(cd);
  return r;
}
#endif

#ifndef NOTGK_WITHOUT_ICONV

const std::string localeToUtf8(const std::string &s) {
#ifdef NOGTK
  return encodeIconv(s, true);
#else
  gchar *a = g_locale_to_utf8(s.c_str(), s.length(), NULL, NULL, NULL);
  std::string r(a);
  g_free(a);
  return r;
#endif
}

const std::string utf8ToLocale(const std::string &s) {
#ifdef NOGTK
  return encodeIconv(s, false);
#else
  gchar *a = g_locale_from_utf8(s.c_str(), s.length(), NULL, NULL, NULL);
  std::string r(a);
  g_free(a);
  return r;
#endif
}

std::string utf8ToLowerCase(const std::string &s
#ifdef NOGTK
                            ,
                            bool onlyRussainChars /*=false*/
#endif
) {
#ifdef NOGTK
  return localeToUtf8(localeToLowerCase(utf8ToLocale(s), onlyRussainChars));
#else
  gchar *a = g_utf8_strdown(s.c_str(), s.length());
  std::string r(a);
  g_free(a);
  return r;
#endif
}

std::string utf8ToUpperCase(const std::string &s
#ifdef NOGTK
                            ,
                            bool onlyRussainChars /*=false*/
#endif
) {
#ifdef NOGTK
  assert(0); // TODO
  return "";
#else
  gchar *a = g_utf8_strup(s.c_str(), s.length());
  std::string r(a);
  g_free(a);
  return r;
#endif
}
#endif // #ifndef NOTGK_WITHOUT_ICONV

#ifndef NOGTK
std::string utf8Substring(const std::string &s, glong start_pos,
                          glong end_pos) {
  gchar *p = g_utf8_substring(s.c_str(), start_pos, end_pos);
  std::string r = p;
  g_free(p);
  return r;
}
#endif

std::string localeToLowerCase(const std::string &s, bool onlyRussainChars) {
  typedef const unsigned char *cpuchar;
  cpuchar p;
  std::string q;
  for (p = cpuchar(s.c_str()); *p != 0; p++) {
    q += (*p >= 0xc0 && *p < 0xe0) ? (*p) + 0x20
                                   : (onlyRussainChars ? *p : tolower(*p));
  }
  return q;
}

// END string functions

// BEGIN pixbuf functions
#ifndef NOGTK

void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
          int height, int sourcex, int sourcey) {
  gdk_cairo_set_source_pixbuf(dest, source, destx - sourcex, desty - sourcey);
  cairo_rectangle(dest, destx, desty, width, height);
  cairo_fill(dest);
}

GdkPixbuf *pixbuf(std::string_view s) {
  return gdk_pixbuf_new_from_file(getImagePath(s).c_str(), nullptr);
}

GdkPixbuf *pixbuf(std::string_view s, int x, int y, int width, int height) {
  return gdk_pixbuf_new_subpixbuf(pixbuf(s), x, y, width, height);
}

GdkPixbuf *writablePixbuf(std::string_view s) {
  return gdk_pixbuf_new_from_file(getWritableFilePath(s).c_str(), nullptr);
}

GtkWidget *image(std::string_view s) {
  return gtk_image_new_from_file(getImagePath(s).c_str());
}

#ifdef USE_ANIMATED_IMAGE
GtkWidget *animatedImage(const char *s) {
  return gtk_image_new_from_animation(
      gdk_pixbuf_animation_new_from_file(getImagePath(s).c_str(), 0));
}
#endif
#endif
// END pixbuf functions

int indexOf(const char t, std::string_view v) {
  auto pos = v.find(t);
  return (pos != std::string_view::npos) ? static_cast<int>(pos) : -1;
}

bool oneOf(const char t, std::string_view v) {
  return v.find(t) != std::string_view::npos;
}

#ifdef _WIN32
// Note this function should be called before gtk_init
PairDoubleDouble aslovGetScaleFactor() {
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
  return {(double)cxPhysical / (double)cxLogical,
          ((double)cyPhysical / (double)cyLogical)};
}

PairDoubleDouble getScaleFactor() { return scale; }
#endif

#ifndef NOGTK

void addClass(GtkWidget *w, std::string_view s) {
  GtkStyleContext *context;
  context = gtk_widget_get_style_context(w);
  std::string q(s);
  gtk_style_context_add_class(context, q.c_str());
}

void removeClass(GtkWidget *w, std::string_view s) {
  GtkStyleContext *context;
  context = gtk_widget_get_style_context(w);
  std::string q(s);
  gtk_style_context_remove_class(context, q.c_str());
}

void addRemoveClass(GtkWidget *w, std::string_view s, bool add) {
  if (add) {
    addClass(w, s);
  } else {
    removeClass(w, s);
  }
}

void loadCSS(std::string const &additionalData /*= ""*/) {
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
  std::string s;

  provider = gtk_css_provider_new();
  display = gdk_display_get_default();
  screen = gdk_display_get_default_screen(display);
  gtk_style_context_add_provider_for_screen(
      screen, GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  std::ifstream t(applicationName + ".css");
  std::stringstream buffer;
  buffer << t.rdbuf();
  s = buffer.str() + additionalData;
  gtk_css_provider_load_from_data(provider, s.c_str(), -1, NULL);
  g_object_unref(provider);
}

void openURL(std::string url) {
  /* gtk_show_uri_on_window works differ on my old notebook with windows7
   * and new notebook with windows10
   * gtk_show_uri_on_window open url/local file using windows word program on
   * old notebook so cann't do works correctly on both notebooks using
   * gtk_show_uri_on_window so use ShellExecute all variants works correctly
   * ShellExecute(0, 0, "http://www.google.com", 0, 0 , SW_SHOW );
   * ShellExecute(0, 0, "file:///C:\\slovesno\\a.txt", 0, 0 , SW_SHOW );
   * ShellExecute(0, 0, "file:///C:/slovesno/a.txt", 0, 0 , SW_SHOW );
   * ShellExecute(0, 0, "C:/slovesno/a.txt", 0, 0 , SW_SHOW );
   * ShellExecute(0, 0, "C:\\slovesno\\a.txt", 0, 0 , SW_SHOW );
   * ShellExecute(0, 0, "C:\\slovesno\\a.html", 0, 0 , SW_SHOW );
   */
#ifdef _WIN32
  // pr(url)
  ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#else
  gtk_show_uri_on_window(0, url.c_str(), gtk_get_current_event_time(), NULL);
#endif
}

// void destroy(cairo_t *p) {
//   if (p) {
//     cairo_destroy(p);
//   }
// }

// void destroy(cairo_surface_t *p) {
//   if (p) {
//     cairo_surface_destroy(p);
//   }
// }

std::string getBuildVersionString(bool _long) {
  return getBuildString(_long) + ", " + getVersionString(_long);
}

std::string getBuildString(bool _long) {
  std::string user_locale = std::setlocale(LC_TIME, nullptr);
  std::setlocale(LC_TIME, "C");
  std::tm t = {};
  std::istringstream ss(__DATE__);
  ss >> std::get_time(&t, "%b %d %Y"); // %b matches "Oct"

  std::string result = "";
  char day_buf[4];
  char month_year_buf[64];

  std::strftime(day_buf, sizeof(day_buf), "%e", &t);
  const char *month_token = _long ? "%B %Y" : "%b %Y";
  std::strftime(month_year_buf, sizeof(month_year_buf), month_token, &t);

  std::string day_str(day_buf);
  if (!day_str.empty() && day_str[0] == ' ') {
    day_str.erase(0, 1);
  }
  result = day_str + " " + month_year_buf;
  std::setlocale(LC_TIME, user_locale.c_str());
  return "build " + result + " " + __TIME__;
}

std::string getVersionString(bool _long) {
  return std::format("gcc{} {}, gtk{} {}.{}.{}", _long ? " version" : "",
                     __VERSION__, _long ? " version" : "", GTK_MAJOR_VERSION,
                     GTK_MINOR_VERSION, GTK_MICRO_VERSION);
}

void showHideWidget(GtkWidget *w, bool show) {
  if (show) {
    gtk_widget_show(w);
  } else {
    gtk_widget_hide(w);
  }
}

void clearContainer(GtkWidget *w) {
  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(w));
  for (iter = children; iter != NULL; iter = g_list_next(iter)) {
    gtk_widget_destroy(GTK_WIDGET(iter->data));
  }
  g_list_free(children);
}

PairDoubleDouble getMonitorSize(bool millimeters /*=true*/) {
  auto monitor = gdk_display_get_monitor(gdk_display_get_default(), 0);
  GdkRectangle r;
  gdk_monitor_get_geometry(monitor, &r);
  // not auto!
  double w = gdk_monitor_get_width_mm(monitor);
  double h = gdk_monitor_get_height_mm(monitor);
  if (!millimeters) {
    w /= 25.4;
    h /= 25.4;
  }
  return {w, h};
}

double getMonitorDiagonal(bool millimeters /*=true*/) {
  auto a = getMonitorSize(millimeters);
  return sqrt(a.first * a.first + a.second * a.second);
}

PairDoubleDouble getDPI() {
  auto monitor = gdk_display_get_monitor(gdk_display_get_default(), 0);
  GdkRectangle r;
  gdk_monitor_get_geometry(monitor, &r);
  auto w = gdk_monitor_get_width_mm(monitor);
  auto h = gdk_monitor_get_height_mm(monitor);
  return {r.width * 25.4 / w, r.height * 25.4 / h};
}

double getHorizontalDPI() { return getDPI().second; }

double getVerticalDPI() { return getDPI().first; }

void setFont(cairo_t *cr, const char *family, int height,
             cairo_font_slant_t slant, cairo_font_weight_t weight) {
  fontFamily = family;
  fontHeight = height;
  fontSlant = slant;
  fontWeight = weight;
  cairo_select_font_face(cr, family, slant, weight);
  cairo_set_font_size(cr, height);
}

void setFont(cairo_t *cr, int height) {
  // if fontFamily="" need to call setFont(cairo_t *, const char *, int
  // ,cairo_font_slant_t, cairo_font_weight_t ) at first
  fontHeight = height;
  cairo_set_font_size(cr, height);
}

void drawText(cairo_t *cr, std::string const &s, double x, double y,
              DRAW_TEXT optionx, DRAW_TEXT optiony) {
  const char *p = s.c_str();
  cairo_text_extents_t e;
  cairo_text_extents(cr, p, &e);

  // x_bearing && y_bearing<0
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
                DRAW_TEXT optionx, DRAW_TEXT optiony) {
  cairo_rectangle_int_t rect = {int(x), int(y), 0, 0};
  drawMarkup(cr, text, rect, optionx, optiony);
}

void drawMarkup(cairo_t *cr, std::string text, cairo_rectangle_int_t rect,
                DRAW_TEXT optionx, DRAW_TEXT optiony) {
  int w, h;
  PangoLayout *layout = createPangoLayout(cr, text);
  pango_layout_get_pixel_size(layout, &w, &h);

  double px = rect.x;
  double py = rect.y;
  if (optionx != DRAW_TEXT_BEGIN) {
    px += (rect.width - w) / (optionx == DRAW_TEXT_END ? 1 : 2);
  }
  if (optiony != DRAW_TEXT_BEGIN) {
    py += (rect.height - h) / (optiony == DRAW_TEXT_END ? 1 : 2);
  }

  cairo_move_to(cr, px, py);
  pango_cairo_update_layout(cr, layout);
  pango_cairo_show_layout(cr, layout);
  g_object_unref(layout);
}

PangoFontDescription *createPangoFontDescription(const PangoFontDescription *f,
                                                 int height) {
  PangoFontDescription *d = pango_font_description_copy(f);
  pango_font_description_set_absolute_size(d, height * PANGO_SCALE);
  return d;
}

PangoFontDescription *createPangoFontDescription() {
  std::string s = fontFamily + "," + std::to_string(fontHeight);
  PangoFontDescription *d = pango_font_description_from_string(s.c_str());
  // https://www.cairographics.org/FAQ/
  pango_font_description_set_absolute_size(d, fontHeight * PANGO_SCALE);
  return d;
}

PangoLayout *createPangoLayout(cairo_t *cr, std::string text) {
  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *desc = createPangoFontDescription();
  pango_layout_set_font_description(layout, desc);

  pango_layout_set_markup(layout, text.c_str(), -1);
  pango_font_description_free(desc);
  return layout;
}
#endif

double timeElapse(clock_t begin) {
  return double(clock() - begin) / CLOCKS_PER_SEC;
}

std::string secondsToString(double seconds) {
  int t = static_cast<int>(seconds);
  int h = t / 3600;
  int m = (t % 3600) / 60;
  int s = t % 60;

  if (h > 0) {
    return std::format("{}:{:02}:{:02}", h, m, s);
  } else {
    return std::format("{}:{:02}", m, s);
  }
}

std::string secondsToString(clock_t end, clock_t begin) {
  return secondsToString(double(end - begin) / CLOCKS_PER_SEC);
}

std::string secondsToString(clock_t begin) {
  return secondsToString(timeElapse(begin));
}

std::string trim(const std::string &s) {
  std::string q = ltrim(s);
  return rtrim(q);
}

std::string ltrim(const std::string &s) {
  std::string::const_iterator it;
  for (it = s.begin(); it != s.end() && isspace(*it); it++)
    ;
  return s.substr(it - s.begin());
}

std::string rtrim(const std::string &s) {
  std::string::const_reverse_iterator it;
  for (it = s.rbegin(); it != s.rend() && isspace(*it); it++)
    ;
  return s.substr(0, s.length() - (it - s.rbegin()));
}

std::string normalize(std::string const &s) {
  std::string::size_type p, p1 = 0;
  if ((p = s.find('.')) == std::string::npos) {
    return s;
  }
  for (p1 = s.length() - 1; p1 > p && s[p1] == '0'; p1--)
    ;
  //"3.875000"->"3.875"
  if (s[p1] == '.') {
    p1--;
  }
  return s.substr(0, p1 + 1);
}

// https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c/9324796
double randomDouble(double from, double to) {
  thread_local static std::mt19937 gen(std::random_device{}());

  using dist_type =
      typename std::conditional<std::is_integral<double>::value,
                                std::uniform_int_distribution<double>,
                                std::uniform_real_distribution<double>>::type;

  thread_local static dist_type dist;

  return dist(gen, typename dist_type::param_type{from, to});
}

void preventThreadSleep() {
#ifdef _WIN32
  // prevents windows 10 threads sleep
  // stackoverflow.com/questions/34836406/how-to-prevent-windows-from-going-to-sleep-when-my-c-application-is-running
  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED |
                          ES_AWAYMODE_REQUIRED);
#endif
}

void aslovSetOutputWidth(int width) { aslovOutputWide = width; }
