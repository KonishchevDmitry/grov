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


#ifndef GROV_HEADER_MAIN_WINDOW_VIEWER
#define GROV_HEADER_MAIN_WINDOW_VIEWER

class QAction;
class QMenu;
class QUrl;

#include <QtGui/QWidget>

#include <grov/common.hpp>
#include <grov/common/feed_item.hpp>

#include <grov/client/storage.hxx>


namespace grov { namespace main_window {


namespace Ui {
	class Viewer;
}


/// A widget that displays all ours offline data.
///
/// This viewer is not works itself after construction. You must call
/// connect_to_parent() method to start its work.
class Viewer: public QWidget
{
	Q_OBJECT

	public:
		Viewer(QWidget *parent = 0);
		~Viewer(void);


	private:
		/// Qt Designer-generated widgets.
		Ui::Viewer*			ui;

		/// Action for showing the feed list.
		QAction*			show_feed_list_action;

		/// Action for going to item's page.
		QAction*			go_to_page_action;

		/// Action for starring an item.
		QAction*			star_action;

		/// Action for sharing an item.
		QAction*			share_action;


		/// All offline data.
		client::Storage*	storage;


		/// Item that is being displayed at this time or invalid item if not
		/// item is being displayed at this time.
		Db_feed_item		current_item;

		/// A position that the splitter had last time when we collapsed it.
		int					last_splitter_pos;


	public:
		/// Connects viewer to a parent widget and the storage, so it will
		/// display its data.
		void	connect_to_parent(client::Storage* storage, QAction* show_feed_list_action, QAction* go_to_page_action, QAction* star_action, QAction* share_action);

		/// Sets selection in feed tree to none.
		void	select_no_feed(void);

	private:
		/// Sets current_item to invalid item.
		void	reset_current_item(void);

		/// Sets current feed item.
		void	set_current_item(const Db_feed_item& item);

		/// Displays "There is no more unread items" message instead of item.
		void	set_no_more_items(void);

		/// Makes widgets that displays share flag display \a shared flag.
		void	set_share_flag_to(bool shared);

		/// Makes widgets that displays star flag display \a starred flag.
		void	set_star_flag_to(bool starred);


	signals:
		/// Emitted when user selects an item.
		void	item_selected(bool valid);


	public slots:
		/// Called when user clicks "Next feed item" button (or when he changes
		/// current feed or label).
		///
		/// @param source_changed - is this a feed or label changed event.
		void	go_to_next_item(bool source_changed = false);

		/// Called when user clicks "Next unread feed or label" button.
		void	go_to_next_unread_feed_or_label(void);

		/// Called when user clicks "Previous feed item" button.
		void	go_to_previous_item(void);

		/// Called when user clicks "Previous unread feed or label" button.
		void	go_to_previous_unread_feed_or_label(void);

	private slots:
		/// Called when user selects a feed.
		void	feed_selected(Big_id id);

		/// Called when user clicks "Go to item's page" button.
		void	go_to_item_page(void);

		/// Called when user selects an item.
		void	item_selected_cb(bool valid);

		/// Called when user selects a label.
		void	label_selected(Big_id id);

		/// Called when user clicks a URL.
		void	link_clicked(const QUrl& qurl);

		/// Called when user shares/unshares an item.
		void	on_share_toggled(bool state);

		/// Called when user stars/unstars an item.
		void	on_star_toggled(bool state);

		/// Sets items view widget to "Please select a label or a feed" state.
		void	set_no_selected_feed(void);

		/// Called when user clicks "Show feed list" button.
		void	show_feed_list_toggle_cb(void);

		/// Called when somebody moves splitter to a new position.
		void	splitter_moved_cb(int pos);
};


}}

#endif

