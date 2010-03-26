/**************************************************************************
*                                                                         *
*   grov - Google Reader offline viewer                                   *
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


#ifndef GROV_HEADER_READER_TASK
#define GROV_HEADER_READER_TASK

#include <src/common.hpp>

#include "task.hxx"


namespace grov { namespace reader {


/// Base class for all tasks that we need to process.
class Task: public QObject
{
	Q_OBJECT

	public:
		Task(QObject* parent = NULL);


	private:
		/// Was task cancelled.
		bool			is_cancelled;


	public:
		/// Returns true if task was cancelled.
		bool			cancelled(void);

		/// Processes the task.
		virtual void	process(void) = 0;


	signals:
		/// This signal tasks emits when processing fails.
		void	error(const QString& message);


	public slots:
		/// Cancels the task.
		void			cancel(void);

};


}}

#endif

