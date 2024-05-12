#pragma once

#include "worldwindow.h"
#include <qDebug>
#include <QCloseEvent>
#include <QWidget>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "file_functions.h"

worldWindow::worldWindow(QWidget* parent)
	: QMainWindow()
{
	ui.setupUi(this);
	ui.statusbar->showMessage(VERSION_LONG);
	QWidget::setWindowTitle(APP_TITLE);

	// Connect all menu options!

	// menuFile
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	// menuEdit
	connect(ui.actionCut, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionCopy, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionPaste, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionPreferences, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	// menuTools
	connect(ui.actionPrune, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionDeleteChunks, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionDeleteRegions, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetOverworld, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetEnd, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetNether, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetWorld, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionRemovePortals, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	connect(ui.actionOptimize, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionPendingTicks, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetVillages, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetPlayers, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetPlayerLocations, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	
	connect(ui.actionHostileMobs, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionPassiveMobs, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionRemoveEntities, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionRemoveBlocks, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionResetDragon, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	connect(ui.actionWorldSpawn, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionEnableAchievements, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	connect(ui.actionOPS, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionMobDespawn, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	// menuProfile
	connect(ui.actionProfileSelect, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionProfileSave, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));

	// menuHelp
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
	connect(ui.actionCheckUpdates, SIGNAL(triggered()), this, SLOT(onMenuActionTriggered()));
}


void worldWindow::receiveWorldDirectory(QString worldDir) {

	// Get levelname
	worldPath = worldDir;
	QString worldname = file_functions::read_file(worldDir + "/" + LEVELNAME_FILE);
	
	// Set window title
	QString windowTitle = APP_TITLE + " - " + WORLDNAME;
	QWidget::setWindowTitle(windowTitle);

	
	safe_areas safeAreas = file_functions::read_profile(PROFILE_DIR + "/" + PROFILE + ".txt");


	QVBoxLayout* overworldRows = qobject_cast<QVBoxLayout*>(ui.overworldRows->layout());
	QVBoxLayout* netherRows = qobject_cast<QVBoxLayout*>(ui.netherRows->layout());
	QVBoxLayout* endRows = qobject_cast<QVBoxLayout*>(ui.endRows->layout());
	QHBoxLayout* newRow = new QHBoxLayout();
	QLabel* newLabel = new QLabel("TEST");
	QLineEdit* newX1 = new QLineEdit("A");
	QLineEdit* newX2 = new QLineEdit("B");
	newRow->insertWidget(0, newX1);
	newRow->insertWidget(1, newX2);
	newRow->insertWidget(4, newLabel);
	//layout->addLayout(newRow);

	
	// Overworld Safe Areas
	for (int i = 0; i < safeAreas.size(); ++i) {

		safe_area safeArea = safeAreas[i];
		// safe_area = dim, vector<int>
		safe_area::iterator it;
		
		for (it = safeArea.begin(); it != safeArea.end(); ++it) {

			int dim = it->first;
			int x1 = it->second[0];
			int x2 = it->second[1];
			int z1 = it->second[2];
			int z2 = it->second[3];
			QString widgetLabel;
			QHBoxLayout* newRow = new QHBoxLayout();
			QLineEdit* newX1 = new QLineEdit(QString::number(x1));
			QLineEdit* newX2 = new QLineEdit(QString::number(x2));
			QLineEdit* newZ1 = new QLineEdit(QString::number(z1));
			QLineEdit* newZ2 = new QLineEdit(QString::number(z2));
			newRow->insertWidget(0, newX1);
			newRow->insertWidget(1, newX2);
			newRow->insertWidget(2, newZ1);
			newRow->insertWidget(3, newZ2);

			switch (dim) {
			case 0:

				// Overworld
				widgetLabel = "overworldRow" + QString::number(i);

				break;
			case 1:

				// Nether
				widgetLabel = "netherRow" + QString::number(i);
				break;
			case 2:

				// End
				widgetLabel = "endRow" + QString::number(i);
				break;

			}

			QLabel* newLabel = new QLabel(widgetLabel);
			newRow->insertWidget(4, newLabel);
			overworldRows->addSpacing(50);
			overworldRows->addLayout(newRow);
			overworldRows->addStretch(1);
			setLayout(overworldRows);
			
		}
	}
	
		
	
	/*
	// Get values from data/trim_settings.ini and send to window
	worldWindow::worldCoordinates coords = getIniDimensionCoords("Overworld");

	QString x1Str = QString::number(coords.x1);
	QString x2Str = QString::number(coords.x2);
	QString z1Str = QString::number(coords.z1);
	QString z2Str = QString::number(coords.z2);

	ui.overworldX1->setText(x1Str);
	ui.overworldX2->setText(x2Str);
	ui.overworldZ1->setText(z1Str);
	ui.overworldZ2->setText(z2Str);
	coords.dim = 0;
	coords = worldWindow::calculateChunks(coords);
	worldWindow::printCoords(coords);

	coords = getIniDimensionCoords("Nether");

	x1Str = QString::number(coords.x1);
	x2Str = QString::number(coords.x2);
	z1Str = QString::number(coords.z1);
	z2Str = QString::number(coords.z2);

	ui.netherX1->setText(x1Str);
	ui.netherX2->setText(x2Str);
	ui.netherZ1->setText(z1Str);
	ui.netherZ2->setText(z2Str);
	coords.dim = 1;
	coords = worldWindow::calculateChunks(coords);
	worldWindow::printCoords(coords);

	coords = getIniDimensionCoords("End");

	x1Str = QString::number(coords.x1);
	x2Str = QString::number(coords.x2);
	z1Str = QString::number(coords.z1);
	z2Str = QString::number(coords.z2);

	ui.endX1->setText(x1Str);
	ui.endX2->setText(x2Str);
	ui.endZ1->setText(z1Str);
	ui.endZ2->setText(z2Str);
	coords.dim = 2;
	coords = worldWindow::calculateChunks(coords);
	worldWindow::printCoords(coords);

	// Get Trim values from ini file
	QString optimize = file_functions::read_ini_str("Options", "optimize", INI_FILE);
	QString oTrim = file_functions::read_ini_str("Overworld", "trim", INI_FILE);
	QString nTrim = file_functions::read_ini_str("Nether", "trim", INI_FILE);
	QString eTrim = file_functions::read_ini_str("End", "trim", INI_FILE);

	if (optimize == "Yes") ui.optimizeCheck->setCheckState(Qt::Checked);
	else ui.optimizeCheck->setCheckState(Qt::Unchecked);

	if (oTrim == "Yes") ui.overworldCheck->setCheckState(Qt::Checked);
	else ui.overworldCheck->setCheckState(Qt::Unchecked);

	if (nTrim == "Yes") ui.netherCheck->setCheckState(Qt::Checked);
	else ui.netherCheck->setCheckState(Qt::Unchecked);

	if (eTrim == "Yes") ui.endCheck->setCheckState(Qt::Checked);
	else ui.endCheck->setCheckState(Qt::Unchecked);
	*/
}

worldWindow::worldCoordinates worldWindow::calculateChunks(worldWindow::worldCoordinates coords) {

	worldWindow::worldCoordinates output = coords;

	if (coords.x1 > coords.x2) {
		qint64 x3 = coords.x1;
		coords.x1 = coords.x2;
		coords.x2 = x3;
	}

	if (coords.z1 > coords.z2) {
		qint64 z3 = coords.z1;
		coords.z1 = coords.z2;
		coords.z2 = z3;
	}

	qint64 x1Chunks = ceil(coords.x1 / 16);
	qint64 x2Chunks = ceil(coords.x2 / 16);
	qint64 z1Chunks = ceil(coords.z1 / 16);
	qint64 z2Chunks = ceil(coords.z2 / 16);

	qint64 x1Blocks = x1Chunks * 16;
	qint64 x2Blocks = x2Chunks * 16;
	qint64 z1Blocks = z1Chunks * 16;
	qint64 z2Blocks = z2Chunks * 16;

	output.blockX1 = x1Blocks;
	output.blockX2 = x2Blocks;
	output.blockZ1 = z1Blocks;
	output.blockZ2 = z2Blocks;
	output.chunkX1 = x1Chunks;
	output.chunkX2 = x2Chunks;
	output.chunkZ1 = z1Chunks;
	output.chunkZ2 = z2Chunks;
	return output;

}
/*
safe_areas worldWindow::getSafeAreas() {

	safe_areas output;
	//file_functions::read_profile(PROFILE_DIR + "/" + PROFILE + ".txt");
	return output;

}
*/
worldWindow::worldCoordinates worldWindow::getIniDimensionCoords(QString type) {

	worldWindow::worldCoordinates output;

	output.x1 = file_functions::read_ini_int(type, "x1", INI_FILE);
	output.x2 = file_functions::read_ini_int(type, "x2", INI_FILE);
	output.z1 = file_functions::read_ini_int(type, "z1", INI_FILE);
	output.z2 = file_functions::read_ini_int(type, "z2", INI_FILE);

	return output;
}

void worldWindow::printCoords(worldWindow::worldCoordinates coords) {
	
	QString x1ChunksStr = QString::number(coords.chunkX1);
	QString x2ChunksStr = QString::number(coords.chunkX2);
	QString z1ChunksStr = QString::number(coords.chunkZ1);
	QString z2ChunksStr = QString::number(coords.chunkZ2);

	QString x1BlocksStr = QString::number(coords.blockX1);
	QString x2BlocksStr = QString::number(coords.blockX2);
	QString z1BlocksStr = QString::number(coords.blockZ1);
	QString z2BlocksStr = QString::number(coords.blockZ2);

	QString xCoords = x1BlocksStr + " to " + x2BlocksStr;
	QString zCoords = z1BlocksStr + " to " + z2BlocksStr;
	QString coordString = "Chunks (" + x1ChunksStr + "," + z1ChunksStr + " to " + x2ChunksStr + "," + z2ChunksStr + ")";

	switch(coords.dim) {
	case 0 :
		ui.overworldX1->setText(QString::number(coords.x1));
		ui.overworldX2->setText(QString::number(coords.x2));
		ui.overworldZ1->setText(QString::number(coords.z1));
		ui.overworldZ2->setText(QString::number(coords.z2));
		//ui.overworldXCoords->setText(xCoords);
		//ui.overworldZCoords->setText(zCoords);
		ui.overworldCoords->setText(coordString);
		break;
	case 1:
		ui.netherX1->setText(QString::number(coords.x1));
		ui.netherX2->setText(QString::number(coords.x2));
		ui.netherZ1->setText(QString::number(coords.z1));
		ui.netherZ2->setText(QString::number(coords.z2));
		//ui.netherXCoords->setText(xCoords);
		//ui.netherZCoords->setText(zCoords);
		ui.netherCoords->setText(coordString);
		break;
	case 2:
		ui.endX1->setText(QString::number(coords.x1));
		ui.endX2->setText(QString::number(coords.x2));
		ui.endZ1->setText(QString::number(coords.z1));
		ui.endZ2->setText(QString::number(coords.z2));
		//ui.endXCoords->setText(xCoords);
		//ui.endZCoords->setText(zCoords);
		ui.endCoords->setText(coordString);
		break;	
	}
	
}


void worldWindow::valueChanged() {
	
	worldWindow::worldCoordinates coords;
	coords.x1 = qint64((ui.overworldX1->text()).toLongLong());
	coords.x2 = qint64((ui.overworldX2->text()).toLongLong());
	coords.z1 = qint64((ui.overworldZ1->text()).toLongLong());
	coords.z2 = qint64((ui.overworldZ2->text()).toLongLong());
	coords.dim = 0;
	coords = worldWindow::calculateChunks(coords);
	worldWindow::printCoords(coords);
	file_functions::write_ini_val("Overworld", "x1", ui.overworldX1->text(), INI_FILE);
	file_functions::write_ini_val("Overworld", "x2", ui.overworldX2->text(), INI_FILE);
	file_functions::write_ini_val("Overworld", "z1", ui.overworldZ1->text(), INI_FILE);
	file_functions::write_ini_val("Overworld", "z2", ui.overworldZ2->text(), INI_FILE);

	coords.x1 = qint64((ui.netherX1->text()).toLongLong());
	coords.x2 = qint64((ui.netherX2->text()).toLongLong());
	coords.z1 = qint64((ui.netherZ1->text()).toLongLong());
	coords.z2 = qint64((ui.netherZ2->text()).toLongLong());
	coords.dim = 1;
	coords = worldWindow::calculateChunks(coords);
	worldWindow::printCoords(coords);
	file_functions::write_ini_val("Nether", "x1", ui.netherX1->text(), INI_FILE);
	file_functions::write_ini_val("Nether", "x2", ui.netherX2->text(), INI_FILE);
	file_functions::write_ini_val("Nether", "z1", ui.netherZ1->text(), INI_FILE);
	file_functions::write_ini_val("Nether", "z2", ui.netherZ2->text(), INI_FILE);

	coords.x1 = qint64((ui.endX1->text()).toLongLong());
	coords.x2 = qint64((ui.endX2->text()).toLongLong());
	coords.z1 = qint64((ui.endZ1->text()).toLongLong());
	coords.z2 = qint64((ui.endZ2->text()).toLongLong());
	coords.dim = 2;
	coords = worldWindow::calculateChunks(coords);
	worldWindow::printCoords(coords);
	file_functions::write_ini_val("End", "x1", ui.endX1->text(), INI_FILE);
	file_functions::write_ini_val("End", "x2", ui.endX2->text(), INI_FILE);
	file_functions::write_ini_val("End", "z1", ui.endZ1->text(), INI_FILE);
	file_functions::write_ini_val("End", "z2", ui.endZ2->text(), INI_FILE);
	
}

void worldWindow::inputChecked() {
	
	QString checkedVal;
	int checked = ui.overworldCheck->checkState();
	if (checked == 0) {
		checkedVal = "No";
	} else {
		checkedVal = "Yes";
	}
	file_functions::write_ini_val("Overworld", "trim", checkedVal, INI_FILE);
	checked = ui.netherCheck->checkState();
	if (checked == 0) {
		checkedVal = "No";
	}
	else {
		checkedVal = "Yes";
	}
	file_functions::write_ini_val("Nether", "trim", checkedVal, INI_FILE);
	checked = ui.endCheck->checkState();
	if (checked == 0) {
		checkedVal = "No";
	}
	else {
		checkedVal = "Yes";
	}
	file_functions::write_ini_val("End", "trim", checkedVal, INI_FILE);
	checked = ui.optimizeCheck->checkState();
	if (checked == 0) {
		checkedVal = "No";
	}
	else {
		checkedVal = "Yes";
	}
	file_functions::write_ini_val("Options", "optimize", checkedVal, INI_FILE);
	
}

void worldWindow::pruneWorld() {

	
	pruneValues values;
	values.worldDir = WORLDPATH;

	worldWindow::worldCoordinates coords;
	coords.x1 = qint64((ui.overworldX1->text()).toLongLong());
	coords.x2 = qint64((ui.overworldX2->text()).toLongLong());
	coords.z1 = qint64((ui.overworldZ1->text()).toLongLong());
	coords.z2 = qint64((ui.overworldZ2->text()).toLongLong());
	coords.dim = 0;
	coords = worldWindow::calculateChunks(coords);
	values.overworld.x1 = coords.chunkX1;
	values.overworld.x2 = coords.chunkX2;
	values.overworld.z1 = coords.chunkZ1;
	values.overworld.z2 = coords.chunkZ2;
	if (ui.overworldCheck->checkState() == 0) values.overworld.trim = false; else values.overworld.trim = true;
	file_functions::write_ini_val("Overworld", "x1", QString::number(coords.blockX1), INI_FILE);
	file_functions::write_ini_val("Overworld", "x2", QString::number(coords.blockX2), INI_FILE);
	file_functions::write_ini_val("Overworld", "z1", QString::number(coords.blockZ1), INI_FILE);
	file_functions::write_ini_val("Overworld", "z2", QString::number(coords.blockZ2), INI_FILE);

	coords.x1 = qint64((ui.netherX1->text()).toLongLong());
	coords.x2 = qint64((ui.netherX2->text()).toLongLong());
	coords.z1 = qint64((ui.netherZ1->text()).toLongLong());
	coords.z2 = qint64((ui.netherZ2->text()).toLongLong());
	coords.dim = 1;
	coords = worldWindow::calculateChunks(coords);
	values.nether.x1 = coords.chunkX1;
	values.nether.x2 = coords.chunkX2;
	values.nether.z1 = coords.chunkZ1;
	values.nether.z2 = coords.chunkZ2;
	if (ui.netherCheck->checkState() == 0) values.nether.trim = false; else values.nether.trim = true;
	file_functions::write_ini_val("Nether", "x1", QString::number(coords.blockX1), INI_FILE);
	file_functions::write_ini_val("Nether", "x2", QString::number(coords.blockX2), INI_FILE);
	file_functions::write_ini_val("Nether", "z1", QString::number(coords.blockZ1), INI_FILE);
	file_functions::write_ini_val("Nether", "z2", QString::number(coords.blockZ2), INI_FILE);

	coords.x1 = qint64((ui.endX1->text()).toLongLong());
	coords.x2 = qint64((ui.endX2->text()).toLongLong());
	coords.z1 = qint64((ui.endZ1->text()).toLongLong());
	coords.z2 = qint64((ui.endZ2->text()).toLongLong());
	coords.dim = 2;
	coords = worldWindow::calculateChunks(coords);
	values.end.x1 = coords.chunkX1;
	values.end.x2 = coords.chunkX2;
	values.end.z1 = coords.chunkZ1;
	values.end.z2 = coords.chunkZ2;
	if (ui.endCheck->checkState() == 0) values.end.trim = false; else values.end.trim = true;
	file_functions::write_ini_val("End", "x1", QString::number(coords.blockX1), INI_FILE);
	file_functions::write_ini_val("End", "x2", QString::number(coords.blockX2), INI_FILE);
	file_functions::write_ini_val("End", "z1", QString::number(coords.blockZ1), INI_FILE);
	file_functions::write_ini_val("End", "z2", QString::number(coords.blockZ2), INI_FILE);

	if (ui.optimizeCheck->checkState() == 0) values.optimize = false; else values.optimize = true;

	if (values.overworld.trim == false && values.nether.trim == false && values.end.trim == false && values.optimize == false) {
		QMessageBox::warning(this, "No dimensions selected...", "You must select at least one dimension to trim or enable the optimize feature to just optimize the world.");
	}
	else if (values.overworld.trim == false && values.nether.trim == false && values.end.trim == false && values.optimize == true) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Optimize Only?", "You haven't selected any dimensions to trim.\nDo you just want to optimize the world?", QMessageBox::Yes | QMessageBox::No))
		{
			loadPruneWorld(values);
		}

	}
	else {
		loadPruneWorld(values);
	}
	
}

void worldWindow::loadPruneWorld(pruneValues values) {

	bool backup = false;
	if (QMessageBox::Yes == QMessageBox::question(this, "Backup World?", "Do you want to make a backup before starting?", QMessageBox::Yes | QMessageBox::No))
	{
		backup = true;
	}


	QApplication::setOverrideCursor(Qt::WaitCursor);
	this->setEnabled(false);


	pruneWindow* prunewindow = new pruneWindow(this);
	connect(this, SIGNAL(sendPruneValues(pruneValues, bool)), prunewindow, SLOT(receivePruneValues(pruneValues, bool)));
	prunewindow->show();

	connect(prunewindow, SIGNAL(closingPruneWindow()), this, SLOT(pruneWindowClosed()));

	// Need to change cursor to timer and disable main window until world chooser has loaded
	QApplication::restoreOverrideCursor();

	this->hide();

	emit sendPruneValues(values, backup);

}


void worldWindow::onMenuActionTriggered()
{
	auto action = qobject_cast<QAction*>(QObject::sender())->objectName();
	auto actionText = qobject_cast<QAction*>(QObject::sender())->text();

	if (action == "actionOpen") {
		actionOpen();
	}
	else if (action == "actionExit") {
		actionExit();
	}
	else if (action == "actionAbout") {
		actionAbout();
	}
	else {
		QMessageBox::information(this, tr("Error:"), "This feature has not been implemented yet");
	}
}

void worldWindow::actionOpen()
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Confirm Close Window", "Opening a new world will undo any unsaved changes.<br>Are you sure you want to start over?", QMessageBox::Yes | QMessageBox::No))
	{
		emit closingWindow();
		this->close();
	}
}


void worldWindow::actionExit()
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Confirm Exit", "Are you sure you wish to exit?<br>Any unsaved changes will be lost!", QMessageBox::Yes | QMessageBox::No))
	{
		emit exitAll();
		this->close();
	}

}

void worldWindow::actionAbout()
{
	QString about = file_functions::read_file(ABOUT_FILE);	
	QString title = "About " + APP_TITLE;
	QString text = about;
	text += "<p>Version: " + VERSION_LONG + "</p>";
	
	QMessageBox::about(this, title, text);
	
}


void worldWindow::closeEvent(QCloseEvent* event)
{
	// Delete temp data
	//QDir(TMP_DIR).removeRecursively();

}

void worldWindow::pruneWindowClosed()
{
	//QDir(TMP_DIR).removeRecursively();
}