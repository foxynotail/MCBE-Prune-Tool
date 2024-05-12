#pragma once

#include <QMainWindow>
#include "globals.h"
#include "file_functions.h"
#include "ui_mcbeadmin.h"
#include "worldchooser.h"
#include "worldwindow.h"

class MCBEAdmin : public QMainWindow
{
	Q_OBJECT

public:
	MCBEAdmin(QWidget *parent = Q_NULLPTR);

private:
	Ui::MCBEAdminClass ui;
	void closeEvent(QCloseEvent* event);

public slots:
	bool checkWorld(const QString worldFolder, const QString type);
	void chooseWorld();
	void browseWorld();
	void loadWorld(const QString worldDir);
	void worldChooserClosed();
	void worldWindowClosed();
	void openAbout();
	void exitAllMain();

signals: 
	void sendWorldDirectory(QString);
};