#pragma once

#include <QMainWindow>
#include "ui_worldwindow.h"
#include "globals.h"
#include "file_functions.h"
#include "prunewindow.h"


class worldWindow : public QMainWindow
{
	Q_OBJECT

public:
	worldWindow(QWidget* parent = Q_NULLPTR);
	struct worldCoordinates {
		qint64 dim, x1, x2, z1, z2, blockX1, blockX2, blockZ1, blockZ2, chunkX1, chunkX2, chunkZ1, chunkZ2;
	};
	QString worldPath;
#define WORLDPATH worldPath;

private:
	Ui::worldWindow ui;
	worldWindow* worldwindow;
	void actionOpen();
	void actionExit();
	void closeEvent(QCloseEvent* event);
	//safe_areas getSafeAreas();
	worldCoordinates getIniDimensionCoords(QString type);
	worldCoordinates calculateChunks(worldCoordinates coords);
	void worldWindow::printCoords(worldCoordinates coords);

public slots:
	void receiveWorldDirectory(const QString worldDir);
	void loadPruneWorld(const pruneValues values);
	void onMenuActionTriggered();
	void actionAbout();
	void valueChanged();
	void inputChecked();
	void pruneWorld();
	void pruneWindowClosed();

signals:
	void sendPruneValues(const pruneValues, bool);
	void closingWindow();
	void exitAll();

};
