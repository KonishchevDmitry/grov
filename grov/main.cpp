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


#include <cstdlib>
#include <cstdio>

#include <memory>

#include <boost/foreach.hpp>

#include <QtCore/QDir>
#include <QtCore/QSize>
#include <QtCore/QTextStream>

#include <QtGui/QApplication>
#include <QtGui/QIcon>

#include <mlib/gui/core.hpp>
#include <mlib/sys/system_signal_notifier.hpp>

#include <grov/client.hpp>
#include <grov/common.hpp>
#include <grov/main_window.hpp>

#include "main.hpp"


namespace grov { namespace {


	/// Parses and processes command line options.
	void	process_command_line_options(const QStringList& args);



	void process_command_line_options(const QStringList& args)
	{
		bool first_arg = true;
		bool debug = false;
		bool verbose = false;

		Q_FOREACH(const QString& arg, args)
		{
			if(first_arg)
			{
				first_arg = false;
				continue;
			}

			if(arg == "--debug")
				debug = true;
			else if(arg == "--help")
			{
				QTextStream stream(stdout);
				stream << _F(QApplication::tr(
						"Usage: %1 [OPTION]...\n"
						"\n"
						"Options:\n"
						"  --help     show this help message\n"
						"  --debug    turn on debug messages\n"
						"  --verbose  turn on verbose debug messages\n"
						"  --version  print application version and exit\n"
				), GROV_APP_UNIX_NAME) << endl;
				exit(EXIT_SUCCESS);
			}
			else if(arg == "--verbose")
				verbose = true;
			else if(arg == "--version")
			{
				QTextStream stream(stdout);
				stream
					<< QCoreApplication::applicationName()
					<< " "
					<< QCoreApplication::applicationVersion()
					<< endl;
				exit(EXIT_SUCCESS);
			}
			else
			{
				MLIB_W(_F( QApplication::tr("Invalid command line option '%1'"), arg ),
					_F( QApplication::tr("Please run '%1 --help' to view all available options."), GROV_APP_UNIX_NAME ) );
				exit(EXIT_FAILURE);
			}
		}

		if(debug)
		{
			if(verbose)
				m::set_debug_level(m::DEBUG_LEVEL_VERBOSE);
			else
				m::set_debug_level(m::DEBUG_LEVEL_ENABLED);
		}
		else if(verbose)
		{
			MLIB_W(QApplication::tr(
				"Invalid command line options: --verbose must be specified in conjunction with --debug." ));
			exit(EXIT_FAILURE);
		}
	#if GROV_DEVELOP_MODE
		else
		{
			m::set_debug_level(m::DEBUG_LEVEL_ENABLED);
			//m::set_debug_level(m::DEBUG_LEVEL_VERBOSE);
		}
	#endif
	}


}}


namespace grov {


QString get_app_home_dir(void)
{
	#ifdef Q_OS_UNIX
		return QDir::home().filePath("." GROV_APP_UNIX_NAME);
	#else
		return QDir::home().filePath(GROV_APP_NAME);
	#endif
}



QString get_user_agent(void)
{
	return QCoreApplication::applicationName() + '/' + QCoreApplication::applicationVersion();
}


}



int main(int argc, char* argv[])
{
	using namespace grov;

	std::auto_ptr<QApplication> app = m::gui::init(argc, argv, GROV_APP_NAME,
		m::get_version(GROV_VERSION_MAJOR, GROV_VERSION_MINOR, GROV_VERSION_PATCH) );

	// Processing command line arguments
	process_command_line_options(app->arguments());

	MLIB_D("Starting the application...");

	QString install_dir;

	// Getting installation directory -->
		try
		{
			install_dir = m::get_app_install_dir(GROV_APP_BIN_DIR);
		}
		catch(m::Exception& e)
		{
			MLIB_SW(QApplication::tr("Unable to determine application's installation directory"), EE(e));
		}
	// Getting installation directory <--

	// TODO: All code above does not show translated strings to the user.

	// Loading translations
	m::load_translations(
		install_dir.isEmpty() ? install_dir : QDir(install_dir).absoluteFilePath(GROV_APP_TRANSLATIONS_DIR),
		GROV_APP_UNIX_NAME
	);

	// Handle operating system signals
	m::sys::connect_end_work_system_signal(app.get(), SLOT(quit()));

	// Setting application icon -->
		if(!install_dir.isEmpty())
		{
			int sizes[] = { 8, 12, 16, 22, 24, 32, 48, 64, 128 };

			QIcon icon;

			BOOST_FOREACH(int size, sizes)
			{
				QString size_name = QString::number(size) + "x" + QString::number(size);

				icon.addFile(
					QDir(install_dir).absoluteFilePath(
						GROV_APP_ICONS_DIR "/" + size_name + "/apps/" GROV_APP_UNIX_NAME ".png" ),
					QSize(size, size)
				);
			}

			icon.addFile(
				QDir(install_dir).absoluteFilePath(
					GROV_APP_ICONS_DIR "/scalable/apps/" GROV_APP_UNIX_NAME ".svg" )
			);

			app->setWindowIcon(icon);
		}
	// Setting application icon <--

	// Running the application -->
	{
		int exit_code;

		try
		{
			Main_window main_window;
			m::gui::set_main_window(&main_window);

			main_window.show();
			exit_code = app->exec();

			MLIB_D("Destroying the main window...");
			m::gui::set_main_window(NULL);
		}
		catch(m::Exception& e)
		{
			MLIB_W(_F(QApplication::tr("Unable to start %1"), QCoreApplication::applicationName()), EE(e));
			return EXIT_FAILURE;
		}

		MLIB_D("Exiting with code %1...", exit_code);
		return exit_code;
	}
	// Running the application <--
}

