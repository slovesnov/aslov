/*
 * CheckNewVersion.h
 *
 *  Created on: 14.02.2018
 *      Author: alexey slovesnov
 */

#ifndef CHECKNEWVERSION_H_
#define CHECKNEWVERSION_H_

#ifndef NOGTK

#include <string>
#include <gtk/gtk.h>

class CheckNewVersion {
	std::string m_versionUrl;
	double m_version;
	GThread* m_newVersionThread;
	GSourceFunc m_callback;
public:
	std::string m_message;
	void start(std::string versionUrl, double version,GSourceFunc callback);
	void routine();
	~CheckNewVersion();
};

#endif /* NOGTK */

#endif /* CHECKNEWVERSION_H_ */
