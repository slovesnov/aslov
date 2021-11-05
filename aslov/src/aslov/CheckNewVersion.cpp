/*
 * CheckNewVersion.cpp
 *
 *  Created on: 14.02.2018
 *      Author: alexey slovesnov
 */
#ifndef NOGTK

#include "aslov.h"
#include "CheckNewVersion.h"

static gpointer check_new_version_thread(gpointer d) {
	((CheckNewVersion*)(d))->routine();
	return NULL;
}

void CheckNewVersion::start(std::string versionUrl, double version,GSourceFunc callback) {
	m_versionUrl = versionUrl;
	m_version = version;
	m_callback=callback;
	m_newVersionThread = g_thread_new("", check_new_version_thread, gpointer(this));
}

void CheckNewVersion::routine() {
	GFile *f = g_file_new_for_uri(m_versionUrl.c_str()); //f is always not null
	gsize length;
	char *content = NULL;
	if (g_file_load_contents(f, NULL, &content, &length, NULL, NULL)) {
		std::string s(content, length); //content is not null terminated so create std::string
		setNumericLocale();//dot interpret as decimal separator
		if (std::stod(s) > m_version) {
			m_message = localeToUtf8(s);
			gdk_threads_add_idle(m_callback, NULL);
		}
	}
	g_free(content);
	g_object_unref(f);
}

CheckNewVersion::~CheckNewVersion() {
	g_thread_join(m_newVersionThread);
}
#endif /* NOGTK */
