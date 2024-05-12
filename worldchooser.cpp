#pragma once

#include "mcbeadmin.h"
#include "worldchooser.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QStandardPaths>
#include <QDirIterator>
#include <QFile>
#include <QString>
#include <qDebug>
#include <QVector>
#include <QMessageBox>
#include <QVectorIterator>
#include <QDateTime>
#include <QIcon>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextStream>
#include "leveldb.h"

#include <iostream>
#include <sstream>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <filesystem>
#include <Windows.h>

bool worldSort(QString& a, QString& b) {

	QStringList partsA = a.split("|");
	QStringList partsB= b.split("|");

	QString unixtimeA = partsA[0];
	QString unixtimeB = partsB[0];

	uint numA = unixtimeA.toInt();
	uint numB = unixtimeB.toInt();

	return numA > numB;
}


class ListDelegate : public QStyledItemDelegate {
public:
	ListDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}

	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {

		QRect r = option.rect;

		QPen fontPen(QColor::fromRgb(208, 209, 212), 1, Qt::SolidLine);

		if (option.state & QStyle::State_Selected)
		{
			painter->setBrush(QColor(138, 182, 222));
			painter->drawRect(r);

		}
		else if(option.state & QStyle::State_MouseOver)
		{
			painter->setBrush(QColor(90, 91, 92));
			painter->drawRect(r);

		}
		else
		{
			//BACKGROUND ALTERNATING COLORS
			painter->setBrush((index.row() % 2) ? QColor(72, 73, 74) : QColor(55, 55, 55));
			painter->drawRect(r);
		}

		painter->setPen(fontPen);

		//GET TITLE, DESCRIPTION AND ICON
		QIcon ic = QIcon(index.data(Qt::DecorationRole).toString());
		QString title = index.data(Qt::DisplayRole).toString();
		QString description = index.data(Qt::UserRole + 1).toString();
		QString date = index.data(Qt::UserRole + 2).toString();

		int imageSpace = 10;
		if (!ic.isNull())
		{
			//ICON
			QSize iconSize = ic.actualSize(r.size());
			iconSize.scale(120, 64, Qt::KeepAspectRatio);
			ic = ic.pixmap(iconSize);

			r = option.rect.adjusted(4, 4, -4, -4);
			ic.paint(painter, r, Qt::AlignVCenter | Qt::AlignLeft);
			imageSpace = 124;
		}

		//World
		r = option.rect.adjusted(imageSpace, 10, -10, -10);
		painter->setFont(QFont("Mojangles", 10, QFont::DemiBold));
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignTop | Qt::AlignLeft, title, &r);

		//Last Modified Date
		r = option.rect.adjusted(imageSpace, 10, -10, -10);
		painter->setFont(QFont("Mojangles", 9, QFont::Normal));
		painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom | Qt::AlignRight, description, &r);
		
		//File Size
		//r = option.rect.adjusted(imageSpace, 10, -10, -10);
		//painter->setFont(QFont("Mojangles", 9, QFont::Normal));
		//painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignBottom | Qt::AlignRight, date, &r);

	}

	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
		return QSize(124, 72);
	}
};

worldChooser::worldChooser(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QVector<QString> worlds;

	QDirIterator it(MC_WORLD_DIR);
	while (it.hasNext()) {

		// DETERMINE IF FOLDER CONTAINS WORLD FILES
		QString worldPath = it.next();
		QString worldFolder = worldPath;

		QStringList pathParts = worldFolder.split("/");
		worldFolder = pathParts.last();

		// LOOK IN FOLDER TO SEE IF IT CONTAINS WORLD FILES

		if (worldFolder != "." && worldFolder != "..") {

			QString relativeLevelFile = MC_WORLD_DIR + worldFolder + "/" + LEVELNAME_FILE;

			// Get file info of levelname.txt
			qDebug() << "Working on " << relativeLevelFile;

			QFileInfo fileInfo(relativeLevelFile);
			bool isWorld = fileInfo.exists() && fileInfo.isFile();

			//bool isWorld = file_functions::file_exists(relativeLevelFile);
			//bool isWorld = true;

			if (isWorld == true) {

				QFile file(relativeLevelFile);
				if (!file.open(QFile::ReadOnly | QFile::Text)) break;
				QTextStream in(&file);
				QString worldName = in.readAll();
				fileInfo.setFile(file);
				QDateTime created = fileInfo.lastModified();
				uint unixtime = created.toSecsSinceEpoch();
				QString lastModified = created.toString(Qt::SystemLocaleShortDate);
				QString s = QString::number(unixtime);

				// Need faster way to get directory size
				// MCC Toolchest is reading level.dat file to get Creative / Survival tag, level name & potentially Directory Size too
				//qint64 size = file_functions::dirSize(worldPath);
				//QString worldSize = file_functions::formatSize(size);				
				QString worldSize = "0";

				// How to read level.dat file?
				QString levelDat = MC_WORLD_DIR + worldFolder + "/level.dat";
				QFile levelFile(levelDat);
				if (!levelFile.open(QFile::ReadOnly | QFile::Text)) break;
				QTextStream levelIn(&levelFile);
				QString levelData = levelIn.readAll();


				levelFile.close();

				// Create string for vector
				QString worldData = s + "|" + worldName + "|" + worldFolder + "|" + worldSize + "|" + lastModified;
				worlds.push_back(worldData);
			}
		}
	}

	qSort(worlds.begin(), worlds.end(), worldSort);

	QVector<QString> images;
	QVector<QString> worldDirectories;
	ui.worldList->setIconSize(QSize(100, 50));

	QVectorIterator<QString> v(worlds);

	QString worldsFolder = MC_WORLD_DIR;

	while (v.hasNext()) {

		QString value = v.next();
		QStringList parts = value.split("|");
		QString world = parts[1];
		QString worldFolder = parts[2];
		QString worldSize = parts[3];
		QString lastModified = parts[4];

		QListWidgetItem *item = new QListWidgetItem();

		//QString description = world + "\n" + worldSize + "\n" + lastModified;

		QString worldIcon = worldsFolder + worldFolder + "/" + LEVELICON_FILE;
		bool iconExists = file_functions::file_exists(worldIcon);
		if (iconExists == false) {
			worldIcon = PLACEHOLDER_ICON;
		} 
		QFile file(worldIcon);
		if (!file.open(QIODevice::ReadOnly)) {
			worldIcon = ":/MCBEAdmin/Resources/gfx/world_icon_fade.jpg";
		}

		item->setIcon(QIcon(worldIcon));
		item->setData(Qt::DecorationRole, worldIcon);
		item->setData(Qt::DisplayRole, world);
		item->setData(Qt::UserRole + 1, lastModified);
		item->setData(Qt::UserRole + 2, worldSize);
		item->setData(Qt::UserRole + 3, worldsFolder + worldFolder);

		Qt::WindowFlags flags = windowFlags();
		flags |= Qt::CustomizeWindowHint;
		flags &= ~Qt::WindowContextHelpButtonHint;
		flags &= ~Qt::WindowSystemMenuHint;
		flags &= ~Qt::WindowMinMaxButtonsHint;
		setWindowFlags(flags);

		ui.worldList->addItem(item);

	}
	ui.worldList->setItemDelegate(new ListDelegate(ui.worldList));

}

void worldChooser::itemChosen()
{

	QVariant itemData = ui.worldList->currentItem()->data(Qt::UserRole + 3);
	QString worldDirectory = itemData.value<QString>();
	//qDebug() << worldDirectory;

	// Now to open a new main window and send the worldDirectory data to the new window

	// Also need to consider how we do this with the file chooser too 
	// MCBEAdmin::openWorld()
	
	emit loadWorldSignal(worldDirectory);
	this->close();

}

void worldChooser::reject() {
	emit closingChooserWindow();
	this->close();
}


void worldChooser::closeEvent(QCloseEvent* event)
{
	emit closingChooserWindow();
}