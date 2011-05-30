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


#include <QtCore/QRegExp>
#include <QtCore/QSet>

#include <QtXml/QDomDocument>

#include <grov/common.hpp>
#include <grov/common/feed.hpp>
#include <grov/common/feed_item.hpp>

#include "gr_xml_parser.hpp"


namespace grov { namespace client { namespace reader { namespace tasks {


namespace {
	/// If a feed labeled by name which contains this string we ignore it.
	const char* const GROV_IGNORE_LABEL_MARK = "[non-" GROV_APP_UNIX_NAME "]";
}



QDomDocument Gr_xml_parser::get_dom(const QByteArray& data)
{
	QDomDocument xml;
	QString error;
	int error_line;
	int error_column;

	if(!xml.setContent(data, false, &error, &error_line, &error_column))
		M_THROW(CSF(_F( "%1 at %2:%3.", error, error_line, error_column )));

	return xml;
}



Gr_feed_item_list Gr_xml_parser::reading_list(const QByteArray& data, QString* continuation_code)
{
	MLIB_DV("Parsing the reading list:");

	Gr_feed_item_list items;

	// Opening the XML document -->
		// Throws m::Exception
		QDomDocument xml = this->get_dom(data);
		QDomElement root = xml.documentElement();

		if(root.tagName() != "feed")
			M_THROW(tr("Invalid XML root element: '%1'."), root.tagName());
	// Opening the XML document <--

	// Continuation code -->
		if(continuation_code)
		{
			QDomNodeList codes = root.elementsByTagName("gr:continuation");

			if(codes.isEmpty())
				continuation_code->clear();
			else if(codes.size() > 1)
				M_THROW(tr("Reading list has several continuation codes."));
			else
			{
				*continuation_code = codes.item(0).toElement().text();
				MLIB_DV("Continuation code: '%1'.", *continuation_code);
				if(continuation_code->isEmpty())
					M_THROW(tr("Gotten empty continuation code."));
			}
		}
	// Continuation code <--

	// Entries -->
	{
		QDomNodeList entries = root.elementsByTagName("entry");

		for(int entry_id = 0; entry_id < entries.size(); entry_id++)
		{
			Gr_feed_item item;
			QDomNode entry = entries.item(entry_id);

			if(entry_id)
				MLIB_DV("");

			// Id -->
				item.gr_id = entry.firstChildElement("id").text();
				MLIB_DV("Id: '%1'.", item.gr_id);

				if(item.gr_id.isEmpty())
					M_THROW(tr("Gotten item with empty id."));
			// Id <--

			// Title and summary -->
			{
				item.url = entry.firstChildElement("link").attribute("href");
				MLIB_DV("URL: '%1'.", item.url);

				item.title = entry.firstChildElement("title").text();
				MLIB_DV("Title: '%1'.", item.title);

				item.summary = entry.firstChildElement("summary").text();
				//MLIB_DV("Summary: '%1'.", item.summary);

				if(item.summary.isEmpty())
				{
					item.summary = entry.firstChildElement("content").text();
					//MLIB_DV("Content: '%1'.", item.summary);
				}

				if(item.url.isEmpty() && item.title.isEmpty() && item.summary.isEmpty())
				{
					MLIB_SW(_F( tr("Gotten item [%1] with empty URL, title and summary. Skipping it."), item.gr_id ));
					continue;
				}

				if(item.title.isEmpty())
					item.title = tr("( No title )");
			}
			// Title and summary <--

			// Feed's id -->
				item.feed_gr_id = entry.firstChildElement("source").attribute("gr:stream-id");
				MLIB_DV("Feed id: '%1'.", item.feed_gr_id);

				if(item.feed_gr_id.isEmpty())
					M_THROW(tr("Gotten item with empty feed's id."));
			// Feed's id <--

			// Labels -->
			{
				QDomNodeList categories = entry.toElement().elementsByTagName("category");

				for(int id = 0; id < categories.size(); id++)
				{
					QString label = categories.item(id).toElement().attribute("label");

					if(label.isEmpty())
						continue;

					MLIB_DV("Label: '%1'.", label);

					if(label == "broadcast")
						item.broadcast = true;
					else if(label == "starred")
						item.starred = true;
				}

				MLIB_DV("Broadcast: %1.", item.broadcast);
				MLIB_DV("Starred: %1.", item.starred);
			}
			// Labels <--

			items << item;
		}
	}
	// Entries <--

	MLIB_DV("Reading list parsed.");

	return items;
}



QHash<QString, QString> Gr_xml_parser::stream_preference_list(const QByteArray& data)
{
	MLIB_DV("Parsing the stream preference list:");

	// Throws m::Exception
	QDomDocument xml = this->get_dom(data);

	QHash<QString, QString> orderings;
	QDomNode target_node;

	QRegExp user_feeds_regex("^user/\\d+/(.+)$");
	user_feeds_regex.setMinimal(true);

	// Getting target nodes -->
	{
		QDomElement root = xml.documentElement();
		if(root.tagName() != "object")
			M_THROW(tr("Invalid XML root element: '%1'."), root.tagName());

		QDomNodeList lists = root.elementsByTagName("object");
		for(int id = 0; id < lists.size(); id++)
		{
			QDomNode list = lists.item(id);

			if(list.toElement().attribute("name") == "streamprefs")
			{
				target_node = list.firstChild();
				break;
			}
		}
	}
	// Getting target nodes <--

	// Parsing preference nodes -->
		while(!target_node.isNull())
		{
			QDomElement target = target_node.toElement();

			if(target.tagName() == "list")
			{
				QString target_name = target.attribute("name");
				MLIB_DV("Target: '%1'.", target_name);

				QDomNode preference_node = target_node.firstChild();

				while(!preference_node.isNull())
				{
					QDomElement preference = preference_node.toElement();

					if(preference.tagName() == "object")
					{
						QString pref_name;
						QString pref_value;

						QDomNode prop_node = preference_node.firstChild();

						while(!prop_node.isNull())
						{
							QDomElement prop = prop_node.toElement();

							if(prop.tagName() == "string")
							{
								if(prop.attribute("name") == "id")
								{
									pref_name = prop.text();
									MLIB_DV("Name: '%1'.", pref_name);
								}
								else if(prop.attribute("name") == "value")
								{
									pref_value = prop.text();
									MLIB_DV("Value: '%1'.", pref_value);
								}
							}

							prop_node = prop_node.nextSibling();
						}

						if(pref_name == "subscription-ordering" && user_feeds_regex.indexIn(target_name) >= 0)
						{
							if(pref_value.size() % 8)
								M_THROW(tr("Gotten invalid '%1' preference value for %2: '%3'."),
									pref_name, target_name, pref_value);

							orderings[user_feeds_regex.cap(1)] = pref_value;
						}
					}

					preference_node = preference_node.nextSibling();
				}
			}

			target_node = target_node.nextSibling();

			MLIB_DV("");
		}
	// Parsing preference nodes <--

	MLIB_DV("The stream preference list parsed.");

	return orderings;
}



Gr_feed_list Gr_xml_parser::subscription_list(const QByteArray& data)
{
	MLIB_DV("Parsing the subscriptions list:");

	// Throws m::Exception
	QDomDocument xml = this->get_dom(data);

	Gr_feed_list feeds;
	QDomNode feed_node;

	// Getting feed nodes -->
	{
		QDomElement root = xml.documentElement();
		if(root.tagName() != "object")
			M_THROW(tr("Invalid XML root element: '%1'."), root.tagName());

		QDomNodeList lists = root.elementsByTagName("list");
		for(int id = 0; id < lists.size(); id++)
		{
			QDomNode list = lists.item(id);

			if(list.toElement().attribute("name") == "subscriptions")
			{
				feed_node = list.firstChild();
				break;
			}
		}
	}
	// Getting feed nodes <--

	// Parsing feed nodes -->
		while(!feed_node.isNull())
		{
			Gr_feed feed;
			bool ignore = false;
			QDomNode prop_node = feed_node.firstChild();

			while(!prop_node.isNull())
			{
				QDomElement prop = prop_node.toElement();

				if(prop.tagName() == "string")
				{
					if(prop.attribute("name") == "id")
					{
						feed.gr_id = prop.text();
						MLIB_DV("Id: '%1'.", feed.gr_id);
					}
					else if(prop.attribute("name") == "title")
					{
						feed.name = prop.text();
						MLIB_DV("Title: '%1'.", feed.name);
					}
					else if(prop.attribute("name") == "sortid")
					{
						feed.sort_id = prop.text();
						MLIB_DV("Sort id: '%1'.", feed.sort_id);
					}
				}
				else if(prop.tagName() == "list" && prop.attribute("name") == "categories")
				{
					QDomNodeList categories = prop.elementsByTagName("string");

					for(int id = 0; id < categories.size(); id++)
					{
						QDomElement category = categories.item(id).toElement();

						if(category.tagName() == "string" && category.attribute("name") == "label")
						{
							QString label = category.text();

							// TODO: Add to the README file
							if(
								// Marked to ignore by user
								label.contains(GROV_IGNORE_LABEL_MARK) ||

								// Subscriptions of Google Listen Android application
								label == "Listen Subscriptions"
							)
								ignore = true;

							if(label != "-") // Some special label
							{
								MLIB_DV("Label: '%1'.", label);
								feed.labels << label;
							}
						}
					}
				}

				prop_node = prop_node.nextSibling();
			}

			if(feed.name.isEmpty())
				feed.name = tr("( No title )");

			if(feed.gr_id.isEmpty())
				M_THROW(tr("Gotten subscription '%1' with no id."), feed.name);

			if(ignore)
				MLIB_DV("Skipping this feed.");
			else
				feeds << feed;

			feed_node = feed_node.nextSibling();

			MLIB_DV("");
		}
	// Parsing feed nodes <--

	MLIB_DV("Subscriptions list parsed.");

	return feeds;
}



QHash<QString, QString> Gr_xml_parser::tag_list(const QByteArray& data)
{
	MLIB_DV("Parsing the tag list:");

	QRegExp user_labels_regex("^user/\\d+/label/(.+)$");
	user_labels_regex.setMinimal(true);

	// Throws m::Exception
	QDomDocument xml = this->get_dom(data);

	QHash<QString, QString> sort_ids;
	QDomNode tag_node;

	// Getting tag nodes -->
	{
		QDomElement root = xml.documentElement();
		if(root.tagName() != "object")
			M_THROW(tr("Invalid XML root element: '%1'."), root.tagName());

		QDomNodeList lists = root.elementsByTagName("list");
		for(int id = 0; id < lists.size(); id++)
		{
			QDomNode list = lists.item(id);

			if(list.toElement().attribute("name") == "tags")
			{
				tag_node = list.firstChild();
				break;
			}
		}
	}
	// Getting tag nodes <--

	// Parsing tag nodes -->
		while(!tag_node.isNull())
		{
			QString name;
			QString sort_id;
			QDomNode prop_node = tag_node.firstChild();

			while(!prop_node.isNull())
			{
				QDomElement prop = prop_node.toElement();

				if(prop.tagName() == "string")
				{
					if(prop.attribute("name") == "id")
					{
						name = prop.text();
						MLIB_DV("Name: '%1'.", name);
					}
					else if(prop.attribute("name") == "sortid")
					{
						sort_id = prop.text();
						MLIB_DV("Sort id: '%1'.", sort_id);
					}
				}

				prop_node = prop_node.nextSibling();
			}

			if(name.isEmpty())
				M_THROW(tr("Gotten empty label name."));

			if(user_labels_regex.indexIn(name) >= 0 && !sort_id.isEmpty())
				sort_ids[sort_id] = user_labels_regex.cap(1);

			tag_node = tag_node.nextSibling();

			MLIB_DV("");
		}
	// Parsing tag nodes <--

	MLIB_DV("The tag list parsed.");

	return sort_ids;
}


}}}}

