/*
 * CheckNewVersion.h
 *
 *  Created on: 14.02.2018
 *      Author: alexey slovesnov
 */

#pragma once
#ifndef NOGTK

#include <string>
#include <gtk/gtk.h>

class CheckNewVersion {
	std::string m_version;
	GThread *m_newVersionThread;
	GSourceFunc m_callback;
public:
	std::string m_message;
	void start(std::string version, GSourceFunc callback);
	void routine();
};

#endif /* NOGTK */
