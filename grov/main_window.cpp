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


#include <mlib/gui/messages.hpp>
#include <mlib/gui/message_box.hpp>

#include <grov/client.hpp>
#include <grov/common.hpp>

#include "main_window/about_dialog.hpp"

#include "main_window.hpp"
#include "ui_main_window.h"


namespace grov {


Main_window::Main_window(QWidget *parent)
:
	QMainWindow(parent),
	ui(new Ui::Main_window),
	progress_dialog(new m::gui::Message_box(QMessageBox::Information, "", "", QMessageBox::Cancel, this))
{
    ui->setupUi(this);

	// Client -->
		// Throws m::Exception
		this->client = new Client(this);

		connect(this->client, SIGNAL(mode_changed(Client::Mode)),
			this, SLOT(mode_changed(Client::Mode)) );
	// Client <--

	// Viewer -->
		ui->viewer->connect_to_parent(this->client,
			ui->show_feed_list, ui->go_to_item_page,
			ui->star, ui->share
		);

		connect(ui->next_unread_feed_or_label, SIGNAL(activated()),
			ui->viewer, SLOT(go_to_next_unread_feed_or_label()) );

		connect(ui->previous_unread_feed_or_label, SIGNAL(activated()),
			ui->viewer, SLOT(go_to_previous_unread_feed_or_label()) );

		connect(ui->next_item, SIGNAL(activated()),
			ui->viewer, SLOT(go_to_next_item()) );

		connect(ui->previous_item, SIGNAL(activated()),
			ui->viewer, SLOT(go_to_previous_item()) );
	// Viewer <--

	// Progress dialog -->
		this->progress_dialog->setWindowFlags(
			this->progress_dialog->windowFlags() & ~Qt::WindowCloseButtonHint );

		connect(this->progress_dialog, SIGNAL(buttonClicked(QAbstractButton *)),
			this->client, SLOT(cancel_current_task()) );
	// Progress dialog <--

	this->mode_changed(this->client->current_mode());
}



Main_window::~Main_window()
{
}



void Main_window::mode_changed(Client::Mode mode)
{
	// Menus -->
		ui->go_offline->setVisible(mode == Client::MODE_NONE);
		ui->discard_all_offline_data->setVisible(mode == Client::MODE_OFFLINE);
		ui->flush_offline_data->setVisible(mode == Client::MODE_OFFLINE);

		ui->view_menu->setEnabled(mode == Client::MODE_OFFLINE);
		ui->show_feed_list->setEnabled(mode == Client::MODE_OFFLINE);
		ui->feed_menu->setEnabled(mode == Client::MODE_OFFLINE);
		ui->item_menu->setEnabled(mode == Client::MODE_OFFLINE);
	// Menus <--

	ui->viewer->setVisible(mode == Client::MODE_OFFLINE);
	ui->viewer->select_no_feed();

	// Progress dialog -->
	{
		bool show_progress = false;
		QString progress_title;
		QString progress_text;

		switch(mode)
		{
			case Client::MODE_GOING_OFFLINE:
				progress_title = tr("Going offline");
				progress_text = tr("Please wait while all needed data is being downloaded...");
				show_progress = true;
				break;

			case Client::MODE_GOING_NONE:
				progress_title = tr("Flushing offline data");
				progress_text = tr("Please wait while all offline data data is being flushed...");
				show_progress = true;
				break;

			default:
				break;
		}

		if(show_progress)
		{
			this->progress_dialog->set_title(progress_title);
			this->progress_dialog->set_message(progress_text);
			this->progress_dialog->show();
		}
		else
			this->progress_dialog->hide();
	}
	// Progress dialog <--
}



void Main_window::on_about_activated(void)
{
	main_window::About_dialog about;
	about.exec();
}



void Main_window::on_discard_all_offline_data_activated(void)
{
#if !GROV_DEVELOP_MODE
	bool is = m::gui::yes_no_message(this, tr("Discard all offline data"),
		tr("Are you sure want to discard all offline data?") );

	if(is)
#endif
		this->client->discard_offline_data();
}



void Main_window::on_flush_offline_data_activated(void)
{
	this->client->flush_offline_data();
}



void Main_window::on_go_offline_activated(void)
{
	this->client->go_offline();
}


}

