#pragma once

#include <QFileDialog>
#include <QMessageBox>

#include <iostream>
#include <fstream>
#include <sstream>

#include "mcbeadmin.h"

MCBEAdmin::MCBEAdmin(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.text_version->setText(VERSION_LONG);
	QWidget::setWindowTitle(APP_TITLE);
}

bool MCBEAdmin::checkWorld(QString worldFolder, QString type) {

	bool levelFound = file_functions::file_exists(worldFolder + "/" + LEVELNAME_FILE);
	
	if (levelFound == false) {

		if (type == "file") {
			QMessageBox::information(this, tr("ERROR:"), "World files not found in selected file.");
		}
		else {
			QMessageBox::information(this, tr("ERROR:"), "World files not found in selected folder.");
		}
		return false;
	}
	else {
		return true;
	}

}


// Load Worlds on PC (WINDOWS 10 only)
void MCBEAdmin::chooseWorld()
{
	// Need to change cursor to timer and disable main window until world chooser has loaded
	QApplication::setOverrideCursor(Qt::WaitCursor);
	this->setEnabled(false);

	worldChooser* worldchooser = new worldChooser(this);
	connect(worldchooser, SIGNAL(loadWorldSignal(QString)), this, SLOT(loadWorld(QString)));
	worldchooser->show();

	connect(worldchooser, SIGNAL(closingChooserWindow()), this, SLOT(worldChooserClosed()));

	// Need to change cursor to timer and disable main window until world chooser has loaded
	QApplication::restoreOverrideCursor();

}

// Open File Browser to find World File
void MCBEAdmin::browseWorld()
{

	// Default MC World Folder (Windows 10): %LOCALAPPDATA%\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\LocalState\games\com.mojang\minecraftWorlds
	// WORKS

	QString worldFolder = QFileDialog::getExistingDirectory(
		this,
		tr("Select World Folder..."),
		MC_WORLD_DIR,
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks
	);

	if (worldFolder.length() && checkWorld(worldFolder, "folder") == true) {

		// Now to open a new main window and send the worldFolder data to the new window
		loadWorld(worldFolder);
	}
}


void MCBEAdmin::loadWorld(QString worldDir) {

	worldWindow *worldwindow = new worldWindow();

	connect(this, SIGNAL(sendWorldDirectory(QString)), worldwindow, SLOT(receiveWorldDirectory(QString)));
	connect(worldwindow, SIGNAL(closingWindow()), this, SLOT(worldWindowClosed()));
	connect(worldwindow, SIGNAL(exitAll()), this, SLOT(exitAllMain()));

	worldwindow->show();

	emit sendWorldDirectory(worldDir);

	this->hide();
	
}

void MCBEAdmin::worldWindowClosed()
{
	this->show();
}


void MCBEAdmin::exitAllMain()
{

	this->close();
}

void MCBEAdmin::worldChooserClosed()
{
	this->setEnabled(true);
}

void MCBEAdmin::openAbout()
{
	QString about = file_functions::read_file(ABOUT_FILE);
	
	QString title = "About " + APP_TITLE;
	QString text = about;
	text += "<p>Version: " + VERSION_LONG + "</p>";
					
	QMessageBox::about(this, title, text);
}


void MCBEAdmin::closeEvent(QCloseEvent* event)
{
	// Delete temp data
	//QDir(TMP_DIR).removeRecursively();

}

