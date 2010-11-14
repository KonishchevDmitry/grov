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


#include <boost/foreach.hpp>

#include <QtGui/QAction>

#include <QtWebKit/QWebFrame>

#include <grov/common.hpp>

#include "web_page.hpp"


namespace grov { namespace client {


Web_page::Web_page(QObject* parent)
:
	QWebPage(parent)
{
	WebAction disabled_web_actions[] = {
		QWebPage::OpenLink,						// Open the current link
		QWebPage::OpenLinkInNewWindow,			// Open the current link in a new window
		QWebPage::OpenFrameInNewWindow,			// Replicate the current frame in a new window
		QWebPage::DownloadLinkToDisk,			// Download the current link to the disk
		QWebPage::OpenImageInNewWindow,			// Open the highlighted image in a new window
		QWebPage::DownloadImageToDisk,			// Download the highlighted image to the disk
		QWebPage::CopyImageToClipboard,			// Copy the highlighted image to the clipboard
		QWebPage::Back,							// Navigate back in the history of navigated links
		QWebPage::Forward,						// Navigate forward in the history of navigated links
		QWebPage::Stop,							// Stop loading the current page
		QWebPage::Reload,						// Reload the current page
		QWebPage::Cut,							// Cut the content currently selected into the clipboard
		QWebPage::Paste,						// Paste content from the clipboard
		QWebPage::Undo,							// Undo the last editing action
		QWebPage::Redo,							// Redo the last editing action
		QWebPage::MoveToNextChar,				// Move the cursor to the next character
		QWebPage::MoveToPreviousChar,			// Move the cursor to the previous character
		QWebPage::MoveToNextWord,				// Move the cursor to the next word
		QWebPage::MoveToPreviousWord,			// Move the cursor to the previous word
		QWebPage::MoveToNextLine,				// Move the cursor to the next line
		QWebPage::MoveToPreviousLine,			// Move the cursor to the previous line
		QWebPage::MoveToStartOfLine,			// Move the cursor to the start of the line
		QWebPage::MoveToEndOfLine,				// Move the cursor to the end of the line
		QWebPage::MoveToStartOfBlock,			// Move the cursor to the start of the block
		QWebPage::MoveToEndOfBlock,				// Move the cursor to the end of the block
		QWebPage::MoveToStartOfDocument,		// Move the cursor to the start of the document
		QWebPage::MoveToEndOfDocument,			// Move the cursor to the end of the document
		QWebPage::SelectNextChar,				// Select to the next character
		QWebPage::SelectPreviousChar,			// Select to the previous character
		QWebPage::SelectNextWord,				// Select to the next word
		QWebPage::SelectPreviousWord,			// Select to the previous word
		QWebPage::SelectNextLine,				// Select to the next line
		QWebPage::SelectPreviousLine,			// Select to the previous line
		QWebPage::SelectStartOfLine,			// Select to the start of the line
		QWebPage::SelectEndOfLine,				// Select to the end of the line
		QWebPage::SelectStartOfBlock,			// Select to the start of the block
		QWebPage::SelectEndOfBlock,				// Select to the end of the block
		QWebPage::SelectStartOfDocument,		// Select to the start of the document
		QWebPage::SelectEndOfDocument,			// Select to the end of the document
		QWebPage::DeleteStartOfWord,			// Delete to the start of the word
		QWebPage::DeleteEndOfWord,				// Delete to the end of the word
		QWebPage::SetTextDirectionDefault,		// Set the text direction to the default direction
		QWebPage::SetTextDirectionLeftToRight,	// Set the text direction to left-to-right
		QWebPage::SetTextDirectionRightToLeft,	// Set the text direction to right-to-left
		QWebPage::ToggleBold,					// Toggle the formatting between bold and normal weight
		QWebPage::ToggleItalic,					// Toggle the formatting between italic and normal style
		QWebPage::ToggleUnderline,				// Toggle underlining
		QWebPage::InspectElement,				// Show the Web Inspector with the currently highlighted HTML element
		QWebPage::InsertParagraphSeparator,		// Insert a new paragraph
		QWebPage::InsertLineSeparator			// Insert a new line
	};

	BOOST_FOREACH(WebAction action, disabled_web_actions)
		this->action(action)->setVisible(false);
}



QWebPage* Web_page::createWindow(WebWindowType type)
{
	return NULL;
}



void Web_page::javaScriptAlert(QWebFrame* frame, const QString& msg)
{
}



bool Web_page::javaScriptConfirm(QWebFrame* frame, const QString& msg)
{
	return false;
}



bool Web_page::javaScriptPrompt(QWebFrame* frame, const QString& msg, const QString& defaultValue, QString* result)
{
	return false;
}



bool Web_page::shouldInterruptJavaScript(void)
{
	MLIB_SW(_F(
		tr("A JavaScript program at '%1' is running for a long period of time. Interrupting it."),
		this->mainFrame()->url().toString()
	));
	return true;
}


}}

