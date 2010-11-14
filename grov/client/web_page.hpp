/**************************************************************************
*                                                                         *
*   Grov - Google Reader offline viewer                                   *
*                                                                         *
*   Copyright (C) 2010, Dmitry Konishchev                                 *
*   http://konishchevdmitry.blogspot.com/                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
*   GNU General Public License for more details.                          *
*                                                                         *
**************************************************************************/


#ifndef GROV_HEADER_CLIENT_WEB_PAGE
#define GROV_HEADER_CLIENT_WEB_PAGE

#include <QtWebKit/QWebPage>

#include <grov/common.hpp>

#include "web_page.hxx"


namespace grov { namespace client {


/// Changes QWebPage behavior for our needs.
class Web_page: public QWebPage
{
	Q_OBJECT

	public:
		Web_page(QObject* parent = NULL);

	// QWebPage interface -->
		public slots:
			bool		shouldInterruptJavaScript(void);

		protected:
			QWebPage*	createWindow(WebWindowType type);
			void		javaScriptAlert(QWebFrame* frame, const QString& msg);
			bool		javaScriptConfirm(QWebFrame* frame, const QString& msg);
			bool		javaScriptPrompt(QWebFrame* frame, const QString& msg, const QString& defaultValue, QString* result);
	// QWebPage interface <--
};


}}

#endif

