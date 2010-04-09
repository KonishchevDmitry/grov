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


#ifndef GROV_HEADER_CLIENT_STORAGE
#define GROV_HEADER_CLIENT_STORAGE

#include <memory>

#include <boost/scoped_ptr.hpp>

class QSqlDatabase;
class QSqlQuery;

#include <QtCore/QMap>
#include <QtCore/QSet>

#include <src/common.hpp>
#include <src/common/feed.hxx>
#include <src/common/feed_item.hpp>
#include <src/common/feed_tree.hxx>

#include "storage.hxx"


namespace grov { namespace client {


/// Represents a storage which stores all feeds' items for offline viewing.
class Storage: public QObject
{
	Q_OBJECT

	private:
		/// Source from which items are being gotten at this moment.
		enum Current_source {
			/// No source has been setted yet.
			SOURCE_NONE,

			/// A feed with id this->id.
			SOURCE_FEED,

			/// A label with id this->id.
			SOURCE_LABEL,
		};


	public:
		/// Class for throwing in get_next_item() and get_previous_item().
		class No_more_items {};

		/// Class for throwing in get_next_item() and get_previous_item().
		class No_selected_items {};


	public:
		/// @throw m::Exception
		Storage(QObject* parent = NULL);
		~Storage(void);


	private:
		/// Database for offline access to the feeds' items.
		boost::scoped_ptr<QSqlDatabase>	db;


		/// Id of the fake "broadcast feed".
		Big_id						broadcast_feed_id;

		/// Id of the fake "starred feed".
		Big_id						starred_feed_id;


		// Current source.
		Current_source				current_source;

		/// Current source's id.
		Big_id						current_source_id;

		/// Current query that user processes on the database.
		std::auto_ptr<QSqlQuery>	current_query;

		/// Items, that have been read in the current query.
		QSet<Big_id>				current_query_read_cache;

		/// Items, that have been starred/unstarred in the current query.
		QMap<Big_id, bool>			current_query_star_cache;


		/// Cache of items' ids that needs to be marked as read.
		QList<Big_id>				readed_items_cache;


	public:
		/// Adds feeds to the storage.
		///
		/// @throw m::Exception.
		void					add_feeds(const Gr_feed_list& feeds);

		/// Adds items to the storage.
		///
		/// @throw m::Exception.
		void					add_items(const Gr_feed_item_list& items);

		/// Returns current feed tree.
		///
		/// @throw m::Exception.
		Feed_tree				get_feed_tree(void);

		/// Returns next feeds' item.
		///
		/// @throw m::Exception, No_more_items, No_selected_items.
		Db_feed_item			get_next_item(void);

		/// Returns previous feeds' item.
		///
		/// @throw m::Exception, No_more_items, No_selected_items.
		Db_feed_item			get_previous_item(void);

		/// Returns user changes that he made to the items.
		///
		/// @throw m::Exception.
		Changed_feed_item_list	get_user_changes(void);

		/// Marks item as read.
		///
		/// @throw m::Exception.
		void					mark_as_read(const Db_feed_item& item);

		/// Marks user changes as flushed to Google Reader.
		///
		/// @throw m::Exception.
		void					mark_changes_as_flushed(Changed_feed_item_list::const_iterator begin, Changed_feed_item_list::const_iterator end);

		/// Sets current source to a feed with id == \a id.
		void					set_current_source_to_feed(Big_id id);

		/// Sets current source to a feed with id == \a id.
		void					set_current_source_to_label(Big_id id);

		/// Adds or removes star from item.
		///
		/// @throw m::Exception.
		void					star(Big_id id, bool is);

	protected:
		/// Deletes all storage's data.
		///
		/// @throw m::Exception.
		void			clear(void);

		/// Checks whether storage has any items.
		///
		/// @throw m::Exception.
		bool			has_items(void);

	private:
		/// Checks whether we can work with this database format version.
		///
		/// @throw m::Exception, No_more_items.
		void			check_db_format_version(void);

		/// Deletes all cached data.
		void			clear_cache(void);

		/// Creates a query that will be used to display items requested by
		/// user.
		///
		/// @throw m::Exception.
		void			create_current_query(void);

		/// Creates all necessary tables in the database.
		///
		/// @throw m::Exception.
		void			create_db_tables(void);

		/// Flushs all cached data.
		///
		/// @throw m::Exception, No_more_items.
		void			flush_cache(void);

		/// Gets some valuable information from the database needed to continue
		/// work after its opening.
		///
		/// @throw m::Exception.
		void			get_db_info(void);

		/// Returns item corresponding to current_query.
		///
		/// @throw m::Exception, No_more_items.
		Db_feed_item	get_item(bool next);

		/// Executes a query.
		///
		/// @throw m::Exception.
		void			exec(QSqlQuery& query);

		/// Executes a query.
		///
		/// @throw m::Exception.
		QSqlQuery		exec(const QString& query_string);

		/// Executes a query and calls query.next().
		///
		/// If query.next() returns false, throws m::Exception.
		///
		/// @throw m::Exception.
		void			exec_and_next(QSqlQuery& query);

		/// Executes a query and calls query.next().
		///
		/// If query.next() returns false, throws m::Exception.
		///
		/// @throw m::Exception.
		QSqlQuery		exec_and_next(const QString& query_string);

		/// Initialize the empty database.
		///
		/// Adds fake feeds, etc.
		///
		/// @throw m::Exception.
		void			init_empty_database(void);

		/// Prepares SQL query for execution.
		///
		/// @throw m::Exception.
		QSqlQuery		prepare(const QString& string);

		/// Resets current internal state - current position for
		/// get_next_item() and get_previous_item(), etc.
		void			reset(void);


	signals:
		/// Emitted when feeds tree changed.
		void	feed_tree_changed(void);

		/// Called when an item is marked as read/unread.
		void	item_marked_as_read(const QList<Big_id>& feed_ids, bool read);
};


}}

#endif


