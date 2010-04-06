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


#ifndef GROV_HEADER_CLIENT_READER_NETWORK_TASK
#define GROV_HEADER_CLIENT_READER_NETWORK_TASK

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QTimer;

#include <src/common.hpp>

#include <src/client/reader.hxx>

#include "task.hpp"

#include "network_task.hxx"


namespace grov { namespace client { namespace reader {


/// Base class for all network tasks (which uses QNetworkReply) that we need to
/// process.
class Network_task: public Task
{
	Q_OBJECT

	public:
		Network_task(Reader* reader, QObject* parent = NULL);


	protected:
		/// Reader that created us.
		Reader*			reader;

		/// Number of failed requests.
		size_t			fails_count;

	private:
		/// Object through which we carry out the networking.
		QNetworkAccessManager*	manager;

		/// QNetworkReply that is processing at this moment.
		QNetworkReply*			current_reply;

		/// Timer to implement timeouts.
		QTimer*					timeout_timer;

		/// Error string if request failed.
		QString					reply_error;


	protected:
		/// Sends an HTTP GET request.
		void					get(const QString& url);

		/// Sends an HTTP POST request.
		void					post(const QString& url, const QString& data);

		/// Returns a QNetworkRequest object with the common HTTP headers
		/// and Cookies setted.
		///
		/// We create all requests by this method.
		virtual QNetworkRequest	prepare_request(const QString& url);

		/// get() or post() request finished.
		///
		/// If request failed, \a error will hold error string, otherwise -
		/// error.isEmpty() == true.
		///
		/// When request fails, the fails_count is incremented.
		virtual void			request_finished(const QString& error, const QByteArray& reply) = 0;

		/// This is convenient method for checking the \a error parameter of
		/// request_finished() method.
		///
		/// - If this is error and we already have too many tries, than it
		///   throws m::Exception with this error.
		/// - If this is error and we have free tries, it returns true.
		/// - Otherwise returns false.
		///
		/// @throw m::Exception.
		bool					throw_if_fatal_error(const QString& error);

		/// Returns true if we have too many request's fails and should stop
		/// useless tries.
		bool					to_many_tries(void);

	private:
		/// Starts processing the reply.
		void			process_reply(QNetworkReply* reply);


	private slots:
		/// Emits when we get a data.
		void	on_data_gotten(qint64 size, qint64 total_size);

		/// Emits when QNetworkReply finishes.
		void	on_finished(void);

		/// When timeout for task expires.
		void	on_timeout(void);

};


}}}

#endif

