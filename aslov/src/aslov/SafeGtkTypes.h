#pragma once
#include <memory>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <pango/pango.h>
#include <glib.h>

template <typename T, auto FreeFunc>
struct GtkResourceDeleter {
    void operator()(T* resource) const {
        if (resource) {
            FreeFunc(resource);
        }
    }
};

template <typename T, auto FreeFunc>
using UniqueGtkResource = std::unique_ptr<T, GtkResourceDeleter<T, FreeFunc>>;

using SafeGRegex        = UniqueGtkResource<GRegex, g_regex_unref>;
using SafePangoFontDesc = UniqueGtkResource<PangoFontDescription, pango_font_description_free>;
using SafePixbuf        = UniqueGtkResource<GdkPixbuf, g_object_unref>;
