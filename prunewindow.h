#pragma once

#include <QMainWindow>
#include "ui_prunewindow.h"
#include "globals.h"
#include "file_functions.h"


class pruneWindow : public QMainWindow
{
	Q_OBJECT

public:
	pruneWindow(QWidget* parent = Q_NULLPTR);
	void pLog(QString text);
	int calculate_db_size(leveldb::DB* db);
	void parse_world(leveldb::DB* db, world_settings worldSettings, size_t totalKeys, bool scanOnly);
	void optimize_world(leveldb::DB* db);
	int optimize(leveldb::DB* db);
	int makeBackup(QString);
	
private:
	Ui::pruneWindow ui;
	pruneWindow* prunewindow;
	void closeEvent(QCloseEvent* event);
	void printToConsole(QString text);
	int pruneWorld(pruneValues values);

public slots:
	void receivePruneValues(const pruneValues values, bool backup);
	void receivePLog(const QString text);
	void receiveProgress(const int progress);
	void receiveProgressFormat(const QString text);
	void receiveProgressLabel(const QString text);

signals:
	void sendPLog(const QString);
	void sendProgress(const int);
	void sendProgressFormat(const QString);
	void sendProgressLabel(const QString);
	void closingPruneWindow();
	void exitAll();

};