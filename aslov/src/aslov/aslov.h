/*
 * aslov.h
 *
 *  Created on: 03.06.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.rf.gd
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#ifdef __GNUC__
#include <cxxabi.h> //for type name this include file exists not for all compilers
#include <memory>   //for type name
#endif

#ifndef NOGTK
#include "CairoSurface.h" //CRect,CPoint
#include "Pixbuf.h"
#include <gtk/gtk.h>
#endif

#include <filesystem>
#include <iostream>
#include <source_location>

template <typename... Args>
void show_variables(std::ostream &os, std::string_view label, Args &&...args) {
  (
      [&os, &label](auto &&value) {
        size_t comma_pos = label.find(',');
        std::string_view current_name = (comma_pos != std::string_view::npos)
                                            ? label.substr(0, comma_pos)
                                            : label;

        // Удаляем лишние пробелы в начале и конце имени переменной для красоты
        size_t first = current_name.find_first_not_of(' ');
        if (first != std::string_view::npos) {
          size_t last = current_name.find_last_not_of(' ');
          current_name = current_name.substr(first, (last - first + 1));
        }

        // Выводим в переданный поток os вместо std::cout
        os << current_name << "=" << std::forward<decltype(value)>(value);

        if (comma_pos == std::string_view::npos) {
          os << ' ';
        } else {
          os << ", ";
          label = label.substr(comma_pos + 1);
          size_t next_non_space = label.find_first_not_of(' ');
          if (next_non_space != std::string_view::npos) {
            label = label.substr(next_non_space);
          }
        }
      }(std::forward<Args>(args)),
      ...);
}

template <typename... Args>
void print_variables(std::ostream &os, Args &&...args) {
  os << std::dec; // after possible ptr output
  ((os << std::forward<Args>(args) << " "), ...);
}

// pr("123",i,v);
#define pr(...)                                                                \
  print_variables(std::cout __VA_OPT__(, ) __VA_ARGS__);                       \
  pri

#define prs(...)                                                               \
  print_variables(std::cout __VA_OPT__(, ) __VA_ARGS__);                       \
  pri_short

#define pri_short                                                              \
  std::cout << std::filesystem::path(                                          \
                   std::source_location::current().file_name())                \
                   .filename()                                                 \
                   .string()                                                   \
            << ":" << std::source_location::current().line() << "\n";

#define pri prio(std::cout)

#define prio(os)                                                               \
  os << std::source_location::current().file_name() << ":"                     \
     << std::source_location::current().line() << " "                          \
     << std::source_location::current().function_name() << "\n";

// pr1("error {} {}", v[i], v[i + 1]);
#define pr1(fmt, ...)                                                          \
  std::cout << std::format(fmt " " __VA_OPT__(, ) __VA_ARGS__);                \
  pri

// prv("123",i,v);
#define prv(...)                                                               \
  show_variables(std::cout, #__VA_ARGS__, __VA_ARGS__);                        \
  pri

// output info  to log file printlo(1234,"some")
#define printlog(...)                                                          \
  {                                                                            \
    std::ofstream log_file(getWritableFilePath("log.txt"), std::ios::app);     \
    if (log_file.is_open()) {                                                  \
      print_variables(log_file __VA_OPT__(, ) __VA_ARGS__);                    \
      prio(log_file);                                                          \
    }                                                                          \
  }

#define printlogi printlog("")

extern std::mutex aslovcout_mutex;

#define prsync(...)                                                            \
  {                                                                            \
    std::lock_guard<std::mutex> lock(aslovcout_mutex);                         \
    prs(__VA_ARGS__);                                                          \
  }

#define prsynci                                                                \
  {                                                                            \
    std::lock_guard<std::mutex> lock(aslovcout_mutex);                         \
    pri_short;                                                                 \
  }

/* https://www.geeksforgeeks.org/c-macro-preprocessor-question-5/
 * default macro value is 0 so
 * #ifndef NOGTK => #if NOGTK==0 is true
 * #if NOGTK==0 ~ #if !defined(NOGTK) || NOGTK==0
 */
#ifdef NOGTK
#if NOGTK == 0
#define NOTGK_WITH_ICONV
#else
#define NOTGK_WITHOUT_ICONV
#endif
#endif

// #define GP GINT_TO_POINTER
// #define GP2INT GPOINTER_TO_INT

#define SIZE G_N_ELEMENTS
#define SIZEI(a) int(G_N_ELEMENTS(a))
#define INDEX_OF_NO_CASE(id, a) indexOfNoCase(id, a, SIZEI(a))
#define JOIN(a) join(a, SIZEI(a))
#define JOINS(a, separator) join(a, SIZEI(a), separator)

#ifdef NOGTK
#define g_print printf
#define g_printerr(...) fprintf(stderr, __VA_ARGS__)
#define G_DIR_SEPARATOR '\\'
#define G_DIR_SEPARATOR_S "\\"
#define G_N_ELEMENTS(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

using VString = std::vector<std::string>;
using MapStringString = std::map<std::string, std::string>;
// using PairStringString = std::pair<std::string, std::string>;
using PairDoubleDouble = std::pair<double, double>;

// format to string example format("%d %s",1234,"some")
std::string format(const char *f, ...);

// #define forma formata
// auto s=format("%d %s",12,"ab")	12 ab
// auto s=formats(",",12,"ab",2.3)	12,ab,2.3
// auto s=formatz(12,"ab",2.3)	12ab2.3
// auto s=formata(12,"ab",2.3)	12 ab 2.3

// BEGIN file functions
enum class FILEINFO { NAME, EXTENSION, LOWER_EXTENSION, DIRECTORY, SHORT_NAME };
bool isDir(const char *path);
bool isDir(const std::string &path);
std::string getFileInfo(std::string path, FILEINFO fi);
int getFileSize(const std::string &path);
FILE *open(std::string path, const char *flags);
// END file functions

// BEGIN application functions
// call aslovInit before gtk_init just first in main() function if you need to
// use ScaleFactor
void aslovInit(char const *const *argv, bool storeScaleFactor = false);
int getApplicationFileSize();
FILE *openApplicationLog(const char *flags);
void clearlog();
std::string getLogPath();
std::string const &getApplicationName();
std::string getResourcePath(const std::string name);
std::string getImagePath(const std::string name);
std::ifstream openResourceFileAsStream(const std::string name);
std::string getWritableFilePath(const std::string name);
#ifndef NOGTK
// writable resource+log+cfg files are places in same dir and should started
// with application name
void writableFileSetContents(const std::string name, const std::string &s);
const std::string writableFileGetContents(const std::string &name);
#endif
const std::string fileGetContent(const std::string &path, bool binary = false);
bool filePutContent(const std::string &path, const std::string &content,
                    bool binary = false);

// END application functions

// BEGIN string functions
std::string timeToString(const char *format, bool toLowerCase = false);

template <typename T>
std::string toString(T t, char separator = ' ', int digits = 3) {
  // std::fixed to prevents scientific notation t=1234567.890123 b=1.23457e +06
  std::stringstream c;
  c << std::fixed << t;
  std::string s, e, b = c.str();
  std::string::size_type p, p1;
  p = b.find('.');
  if (p != std::string::npos) {
    for (p1 = b.length() - 1; p1 > p && b[p1] == '0'; p1--)
      ;            //"3.875000"->"3.875"
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

/* parseString("0xff",i,16), parseString("ff",i,16), parseString("+0xff",i,16)
 * ok t is changed only if parse is valid
 */
template <class T> bool parseString(const char *d, T &t, int radix = 10) {
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
  if (std::is_same_v<T, long> ||
      (std::is_same_v<T, int> && sizeof(int) == sizeof(long))) {
    a = strtol(d, &p, radix);
  } else if (std::is_same_v<T, unsigned long> ||
             (std::is_same_v<T, unsigned> && sizeof(int) == sizeof(long))) {
    a = strtoul(d, &p, radix);
  } else if (std::is_same_v<T, int64_t> ||
             (std::is_same_v<T, int> && sizeof(int) == sizeof(int64_t))) {
    a = strtoll(d, &p, radix);
  } else if (std::is_same_v<T, uint64_t> ||
             (std::is_same_v<T, unsigned> && sizeof(int) == sizeof(int64_t))) {
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

template <class T>
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
                            ,
                            bool onlyRussainChars = false
#endif
);
std::string utf8ToUpperCase(const std::string &s
#ifdef NOGTK
                            ,
                            bool onlyRussainChars = false
#endif
);
#endif

#ifndef NOGTK
std::string utf8Substring(const std::string &s, glong start_pos, glong end_pos);
#endif
// convert localed string to lowercase
std::string localeToLowerCase(const std::string &s,
                              bool onlyRussainChars = false);

template <typename T, typename... P>
std::string joinS(const std::string separator, T &&t, P &&...p) {
  std::stringstream c;
  c << t;
  ((c << separator << p), ...);
  return c.str();
}

template <typename T, typename... P>
std::string joinS(const char separator, T &&t, P &&...p) {
  return joinS(std::string(1, separator), t, p...);
}

template <typename T>
  requires std::convertible_to<T, std::string_view>
inline std::string joinV(const std::vector<T> &v,
                         std::string_view separator = " ") {
  if (v.empty())
    return {};

  size_t total_size = separator.size() * (v.size() - 1);
  for (const auto &s : v) {
    total_size += std::string_view(s).size();
  }

  std::string result;
  result.reserve(total_size);

  result += v[0];
  for (size_t i = 1; i < v.size(); ++i) {
    result += separator;
    result += v[i];
  }
  return result;
}

template <typename T>
  requires(!std::convertible_to<T, std::string_view>)
inline std::string joinV(const std::vector<T> &v,
                         std::string_view separator = " ") {
  if (v.empty())
    return {};
  std::stringstream c;
  c << v[0];
  for (size_t i = 1; i < v.size(); ++i) {
    c << separator << v[i];
  }
  return c.str();
}

template <typename T>
inline std::string joinV(const std::vector<T> &v, char separator) {
  return joinV(v, std::string(1, separator));
}

// separator can be default so use as 2nd parameter. It differs from other
// functions arguments order.
template <typename T>
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

template <typename T, std::size_t N>
std::string join(const std::array<T, N> &v, const char separator = ' ') {
  std::stringstream c;
  for (std::size_t i = 0; i < N; i++) {
    if (i) {
      c << separator;
    }
    c << v[i];
  }
  return c.str();
}

template <typename T> struct is_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_array_v = is_std_array<T>::value;

template <typename First, typename... P>
  requires(!std::is_array_v<std::remove_cvref_t<First>> &&
           !is_std_array_v<std::remove_cvref_t<First>>)
std::string join(First &&first, P &&...p) {
  return joinS(" ", std::forward<First>(first), std::forward<P>(p)...);
}
// END string functions

// BEGIN 2 dimensional array functions
template <typename T>
std::vector<std::vector<T>> create2dArray(size_t rows, size_t cols,
                                          const T &initial_value = T()) {
  return std::vector<std::vector<T>>(rows, std::vector<T>(cols, initial_value));
}
// END 2 dimensional array functions

// BEGIN pixbuf/image functions
#ifndef NOGTK

void copy(GdkPixbuf *source, cairo_t *dest, int destx, int desty, int width,
          int height, int sourcex, int sourcey);

GdkPixbuf *pixbuf(const char *s);
GdkPixbuf *pixbuf(const std::string &s);
GdkPixbuf *pixbuf(std::string s, int x, int y, int width, int height);
GdkPixbuf *writablePixbuf(const char *s);
GdkPixbuf *writablePixbuf(const std::string &s);

GtkWidget *image(const char *s);
GtkWidget *image(const std::string &s);
#ifdef USE_ANIMATED_IMAGE
GtkWidget *animatedImage(const char *s);
#endif
#endif
// END pixbuf/image functions

template <class T>
typename std::vector<T>::const_iterator find(const T &t,
                                             std::vector<T> const &v) {
  return std::find(v.begin(), v.end(), t);
}

template <class T>
typename std::vector<T>::iterator find(const T &t, std::vector<T> &v) {
  return std::find(v.begin(), v.end(), t);
}

// cann't use indexOf name because sometimes compiler cann't deduce
template <class T, class... V> int indexOfV(T const &t, V const &...v) {
  auto l = {v...};
  auto i = std::find(std::begin(l), std::end(l), t);
  return i == std::end(l) ? -1 : i - std::begin(l);
}

// #include <type_traits>

template <class T, class... Args> int indexOf(T const &t, Args const &...v) {
  auto is_equal = [](const auto &a, const auto &b) {
    if constexpr (std::is_convertible_v<decltype(a), std::string_view> &&
                  std::is_convertible_v<decltype(b), std::string_view>) {
      return std::string_view(a) == std::string_view(b);
    } else {
      return a == b;
    }
  };

  int index = 0;
  int found_index = -1;

  ((is_equal(t, v) ? (found_index = index, false) : (++index, true)) && ...);

  return found_index;
}

/*
template <class T, class... Args> int indexOf(T const &t, Args const &...v) {
  // Create an initializer_list containing 't' and all elements of 'v'.
  // This guarantees the list is never empty, enabling robust type deduction.
  auto l = {t, v...};

  // Search for 't' starting from the second element (the beginning of 'v...').
  auto it = std::find(std::next(std::begin(l)), std::end(l), t);

  // If found, calculate the index relative to 'v...' by subtracting 1.
  return it == std::end(l) ? -1 : (it - std::begin(l) - 1);
}*/

template <class T> int indexOf(const T &t, std::initializer_list<T> v) {
  auto it = std::find(v.begin(), v.end(), t);
  return it == v.end() ? -1 : std::distance(v.begin(), it);
}

template <class T, std::size_t N>
int indexOf(const T &t, const std::array<T, N> &v) {
  auto it = std::find(v.begin(), v.end(), t);
  return it == v.end() ? -1 : static_cast<int>(std::distance(v.begin(), it));
}

template <class T, class U, std::size_t N>
int indexOf(const U &t, const T (&v)[N]) {
  const T *it = std::find(v, v + N, t);
  int i = static_cast<int>(it - v);
  return i == N ? -1 : i;
}

template <class T> int indexOf(const T &t, std::vector<T> const &v) {
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

template <class T> bool oneOf(const T &t, std::initializer_list<T> v) {
  return indexOf(t, v) != -1;
}

template <class T, std::size_t N>
int oneOf(const T &t, const std::array<T, N> &v) {
  return indexOf(t, v) != -1;
}

bool oneOf(char const &t, const std::string &v);
bool oneOf(char const &t, char const *v);

// template <class T, class... V>bool oneOfV(T const& t, V const&... v){
//	return ((t== v)|| ...);
//	//return indexOfV(t,v...)!=-1;
// }
template <class T, class... V> bool oneOf(T const &t, V const &...v) {
  return ((t == v) || ...);
  // return indexOfV(t,v...)!=-1;
}

template <class T, class U, std::size_t N>
bool oneOf(const U &t, const T (&v)[N]) {
  return indexOf(t, v) != -1;
}

template <class T> bool oneOf(const T &t, std::vector<T> const &v) {
  return indexOf(t, v) != -1;
}

// BEGIN config functions
#ifndef NOGTK
std::string getConfigPath();
std::string getConfigPathLocaled();
bool loadConfig(MapStringString &map);

template <std::size_t N, typename... T>
bool readConfig(const std::string (&tags)[N], T &...p) {
  static_assert(N == sizeof...(T),
                "Number of arguments should match the number of tags");
#ifndef NDEBUG
  for (const auto &tag : tags) {
    assert(tag.find(' ') == std::string::npos && "Tag cannot contain spaces");
    assert(tag.find('=') == std::string::npos &&
           "Tag cannot contain '=' character");
  }
#endif
  MapStringString m;
  if (!loadConfig(m)) {
    return false;
  }

  int index = -1;
  auto identify_and_process = [&](auto &&arg) -> bool {
    index++;
    auto it = m.find(tags[index]);
    if (it == m.end()) {
#ifndef NDEBUG
      pr("error cann't find tag");
#endif
      return false;
    }
    auto &v = it->second;
    using ActualType = std::decay_t<decltype(arg)>;

    if constexpr (std::is_same_v<ActualType, PangoFontDescription *>) {
      PangoFontDescription *desc =
          pango_font_description_from_string(v.c_str());
      if (desc) {
        arg = desc;
      } else {
#ifndef NDEBUG
        pr("error cann't parse to PangoFontDescription, string=", v);
#endif
        return false;
      }
    } else {
      if (!(std::istringstream(v) >> arg)) {
#ifndef NDEBUG
        pr("error cann't parse string", v, "argument", index);
#endif
        return false;
      }
    }
    return true;
  };

  return (identify_and_process(p) && ...);
}

template <std::size_t N, typename... T>
void writeConfig(const std::string (&tags)[N], T &&...p) {
  static_assert(N == sizeof...(T),
                "Number of arguments should match the number of tags");
#ifndef NDEBUG
  for (const auto &tag : tags) {
    assert(tag.find(' ') == std::string::npos && "Tag cannot contain spaces");
    assert(tag.find('=') == std::string::npos &&
           "Tag cannot contain '=' character");
  }
#endif
  std::ofstream f(
      getConfigPathLocaled(),
      std::ios::out |
          std::ios::binary); // binary f << "\n"; output only \n without \r
  assert(f.is_open());

  int i = 0;
  (
      [&](auto &&a) { // Используем auto&& для универсальности
        using PureType = std::decay_t<decltype(a)>;
        f << tags[i++] << " = ";
        if constexpr (std::is_same_v<PureType, PangoFontDescription *>) {
          assert(a);
          char *font_str = pango_font_description_to_string(a);
          f << font_str;
          g_free(font_str);
        } else {
          f << a;
        }
        f << "\n";
      }(std::forward<T>(p)),
      ...);
}

std::string getSystemLanguage();
#endif
// END config functions

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
void clearContainer(GtkWidget *w);
// int getContainerIndex(GtkWidget *container, GtkWidget *w);
// millimeters or inches
PairDoubleDouble getMonitorSize(bool millimeters = true);
double getMonitorDiagonal(bool millimeters = true);
PairDoubleDouble getDPI();
double getHorizontalDPI();
double getVerticalDPI();

enum DRAW_TEXT { DRAW_TEXT_BEGIN, DRAW_TEXT_CENTER, DRAW_TEXT_END };
// setFont(cr, "Times New Roman", 22,CAIRO_FONT_SLANT_NORMAL,
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
PangoFontDescription *createPangoFontDescription(const PangoFontDescription *f,
                                                 int height);
PangoFontDescription *createPangoFontDescription();
PangoLayout *createPangoLayout(cairo_t *cr, std::string text);
#endif
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
template <class T> std::string aslovTypeName() {
  typedef typename std::remove_reference<T>::type TR;
  std::unique_ptr<char, void (*)(void *)> own(
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
#define OBJECT_TYPE(v) aslovTypeName<decltype(v)>()
#endif /* __GNUC__ */

void aslovSetOutputWidth(int width);
