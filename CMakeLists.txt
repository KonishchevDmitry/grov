cmake_minimum_required(VERSION 2.6)


# Application's properties -->
	set(APP_NAME Grov)
	set(APP_UNIX_NAME grov)
	set(APP_AUTHOR "Dmitry Konishchev")

	set(APP_VERSION_MAJOR 0)
	set(APP_VERSION_MINOR 1)
	set(APP_VERSION_PATCH 0)
	set(APP_VERSION "0.1")

	if(UNIX)
		set(APP_BIN_DIR bin)
		set(APP_ICONS_DIR share/icons/hicolor)
		set(APP_TRANSLATIONS_DIR share/${APP_UNIX_NAME}/translations)
	else()
		set(APP_BIN_DIR .)
		set(APP_ICONS_DIR icons)
		set(APP_TRANSLATIONS_DIR translations)
	endif()
# Application's properties <--


# Build type -->
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Release)
	endif()

	string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} Debug DEVELOP_BUILD)
# Build type <--


# Configuring MLib -->
	set(MLIB_APP_TRANSLATIONS_DIR ${APP_TRANSLATIONS_DIR})

	set(MLIB_APP_QT_MIN_VERSION 4.5)
	set(MLIB_APP_QT_MODULES QtCore QtGui QtNetwork QtSql QtWebkit QtXml)
	set(MLIB_ENABLE_ALIASES TRUE)

	set(MLIB_DEBUG_MODE TRUE)

	if(DEVELOP_BUILD)
		set(MLIB_DEVELOP_MODE TRUE)
	endif()

	include(mlib/mlib.cmake)
# Configuring MLib <--


# Includes
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})


# Sub-projects
add_subdirectory(mlib)
add_subdirectory(${APP_UNIX_NAME})


# CPack -->
	set(CPACK_PACKAGE_NAME ${APP_UNIX_NAME})
	set(CPACK_PACKAGE_VENDOR ${APP_AUTHOR})

	set(CPACK_PACKAGE_VERSION_MAJOR ${APP_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${APP_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH ${APP_VERSION_PATCH})

	set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Google Reader offline viewer")

	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
	set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")


	set(CPACK_SOURCE_PACKAGE_FILE_NAME "${APP_UNIX_NAME}-${APP_VERSION}")
	set(CPACK_SOURCE_GENERATOR TGZ TBZ2)
	set(CPACK_SOURCE_IGNORE_FILES
		"/debian"
		"/grov/.vim"
		"/grov/cscope.files"
		"/grov/cscope.out"
		"/grov/tags"
		"/TODO"
		"/\\\\.git"
		"/\\\\.gitignore"
	)

	include(CPack)
# CPack <--
