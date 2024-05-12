#pragma once

#include "prunewindow.h"
#include <qDebug>
#include <QCloseEvent>
#include <QFuture>
#include <QtConcurrent>
#include <QScrollbar>
#include "file_functions.h"
#include "prune.h"
#include <QStandardPaths>
#include <filesystem>
namespace fs = std::filesystem;

pruneWindow::pruneWindow(QWidget* parent)
	: QMainWindow()
{
	ui.setupUi(this);
	QWidget::setWindowTitle(APP_TITLE);
}

int pruneWindow::makeBackup(QString worldDir) {

	//connect(this, SIGNAL(sendPLog(QString)), this, SLOT(receivePLog(QString)));
	// Create new directory
	// Do we have permissions to make in minecraft folder?
	emit sendPLog("");
	emit sendPLog("->Creating Backup...");
	// Get original directory
	int lastSlash = worldDir.toStdString().rfind("/");
	QString worldDirFolder = worldDir.left(lastSlash + 1);
	QString worldFolderName = worldDir.mid(lastSlash + 1);
	//emit sendPLog(" |- Backup Root: " + worldDirFolder);
	emit sendPLog(" |- World Folder Name: " + worldFolderName);

	// Check if worldDirFolder is writable
	QFileInfo wDir(worldDirFolder);
	if (!wDir.isWritable()) {
		emit sendPLog(" |- Your world folder is not writable. Defaulting to user home directory.");
		emit sendPLog(" |- Consider running this app with Administrator Privalages to fix.");
		worldDirFolder = QStandardPaths::HomeLocation;
		//emit sendPLog(" |- Backup Folder is " + worldDirFolder);
	}

	int backupNum = 1;
	QString newWorldDir = worldFolderName + "-Backup[" + QString::number(backupNum) + "]";
	QString newWorldPath = worldDirFolder + "/" + newWorldDir;

	// Check if directory exists
	while (QDir(newWorldPath).exists()) {
		backupNum++;
		newWorldDir = worldFolderName + "-Backup[" + QString::number(backupNum) + "]";
		newWorldPath = worldDirFolder + "/" + newWorldDir;
	}
	//emit sendPLog(" |- Backup Dir: " + newWorldPath);
	//fs::create_directory(newWorldPath.toStdString());
	fs::path fsSrc = worldDir.toStdString();
	fs::path fsDst = newWorldPath.toStdString();
	emit sendPLog(" |- Copying Files...");
	file_functions::CopyRecursive(fsSrc, fsDst);
	emit sendPLog(" |- Backup saved at " + newWorldPath);

	// Need to update levelname.txt and level.dat to refect new backup name
	QString levelFile = "levelname.txt";
	QFile file(newWorldPath + "/" + levelFile);
	if (file.open(QIODevice::ReadWrite)) {
		QTextStream stream(&file);
		stream << newWorldDir << endl;
	}

	// Read & Update level.dat
	std::string levelPath = newWorldPath.toStdString();
	std::string levelDat = "level.dat";
	std::string levelDatFile = (levelPath + "/" + levelDat);
	std::ifstream in(levelDatFile);
	std::string contents((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	leveldb::Slice slice = contents;

	int offset = 8;
	fxNBT output;
	while (offset < slice.size()) {
		offset = fxnt_getNBT(slice, offset, output);
	}
	//output.print_map();
	std::string levelName;
	int valOffset = 0;
	int valLength = 0;
	int keyOffset = 0;
	int keyLength = 0;
	for (int i = 0; i < output.map.size(); ++i) {
		fxNBT map = output.map[i];
		if (map.tagname == "LevelName") {
			levelName = map.get_val();
			keyOffset = map.init_offset;
			keyLength = map.init_length;
		}
	}
	std::string newLevelName = newWorldDir.toStdString();
	int newlen = newLevelName.size();
	std::string sliceString = slice.ToString();
	auto id = int_to_byte(8);
	std::string tagname = "LevelName";
	auto tagLen = int_to_shortval(tagname.size());
	auto valLen = int_to_shortval(newLevelName.size());

	std::string newKey = id + tagLen + tagname + valLen + newLevelName;
	sliceString.replace(keyOffset, keyLength, newKey);
	ofstream fw(levelDatFile, std::ios::binary);
	if (fw.is_open())
	{
		fw << sliceString;
		fw.close();
	}
	else {
		pLog("There was an error saving updating the backup world name!");
	}

	emit sendPLog(" |- Backup Complete.");
	return 100;
}

void pruneWindow::receivePruneValues(pruneValues values, bool backup) {

	pLog("#################################################################");
	pLog("# Prune v" + prune_version);
	pLog("# ");
	pLog("# App by FoxyNoTail");
	pLog("# foxynotail.com");
	pLog("# @foxynotail");
	pLog("# (C) 2022, FoxyNoTail");
	pLog("############################################################");
	pLog("");
	pLog("Prune Program Started.");


	connect(this, SIGNAL(sendPLog(QString)), this, SLOT(receivePLog(QString)));
	connect(this, SIGNAL(sendProgress(int)), this, SLOT(receiveProgress(int)));
	connect(this, SIGNAL(sendProgressFormat(QString)), this, SLOT(receiveProgressFormat(QString)));
	connect(this, SIGNAL(sendProgressLabel(QString)), this, SLOT(receiveProgressLabel(QString)));

	if (backup == true) {
		QString worldDir = values.worldDir;
		QFuture<int> future = QtConcurrent::run(this, &pruneWindow::makeBackup, worldDir);
		QFutureWatcher<int> watcher;
		QEventLoop loop;
		// QueuedConnection is necessary in case the signal finished is emitted before the loop starts (if the task is already finished when setFuture is called)
		connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()), Qt::QueuedConnection);
		watcher.setFuture(future);
		loop.exec();
		//makeBackup(values.worldDir);
	}

	QFuture<int> future = QtConcurrent::run(this, &pruneWindow::pruneWorld, values);
	QFutureWatcher<int> watcher;
	QEventLoop loop;
	// QueuedConnection is necessary in case the signal finished is emitted before the loop starts (if the task is already finished when setFuture is called)
	connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()), Qt::QueuedConnection);
	watcher.setFuture(future);
	loop.exec();

	// Save log
	QString logContents = ui.console->toPlainText();
	std::string logString = logContents.toStdString();

	QDate date = QDate::currentDate();
	QString dateString = date.toString("yyyy-MM-dd");
	QTime time = QTime::currentTime();
	QString timeString = time.toString("HH-mm-ss");

	QString logName = "Prune " + dateString + " " + timeString + ".txt";

	std::string logFile = LOG_DIR.toStdString() + "/" + logName.toStdString();
	qDebug() << QString::fromStdString(logFile);
	ofstream fw(logFile, std::ofstream::out);
	if (fw.is_open())
	{
		fw << logString;
		fw.close();
		QString text = "Log file saved at: " + QString::fromStdString(logFile);
		pLog(text);
	}
	else {
		pLog("There was an error saving the log file!");
	}

	QMessageBox::information(this, "Prune Completed...", "The prune process is complete.\nYou can now safely exit the app.");
}

void pruneWindow::receivePLog(QString text) {
	pLog(text);
}
void pruneWindow::receiveProgress(int progress) {
	int val = progress;
	ui.progressBar->setValue(val);
	//qDebug() << "Progress: " << progress;
}
void pruneWindow::receiveProgressFormat(QString text) {
	ui.progressBar->setFormat(text);
	//qDebug() << "Progress Text: " << text;
}
void pruneWindow::receiveProgressLabel(QString text) {
	ui.progressLabel->setText(text);
	//qDebug() << "Progress Label: " << text;
}
void pruneWindow::closeEvent(QCloseEvent* event) {
	emit closingPruneWindow();
}
void pruneWindow::printToConsole(QString text) {
	QString temp = ui.console->toPlainText();
	temp += text + "\n";
	ui.console->setPlainText(temp);
	ui.console->moveCursor(QTextCursor::End);
	ui.console->ensureCursorVisible();
}
void pruneWindow::pLog(QString text) {
	printToConsole(text);
	qDebug() << text;
}

int pruneWindow::pruneWorld(pruneValues values) {
	using namespace std;
	bool scanOnly = false;

	leveldb::Status worldDBStatus;
	leveldb::DB* db;

	// Start timer to calculate process time
	clock_t start_s = clock();

	emit sendPLog("");
	emit sendPLog("-> Starting Process...");

	// Setup standard world options
	emit sendPLog("-> Setting LevelDB options...");
	leveldb::Options options = world_options();

	// Open Source World DB
	QString worldDB = values.worldDir + "/db";
	int lastSlash = values.worldDir.toStdString().rfind("/");
	QString worldFolder = values.worldDir.mid(lastSlash + 1);

	emit sendPLog(" |- Loading " + worldFolder + " world...");

	worldDBStatus = leveldb::DB::Open(options, worldDB.toStdString(), &db);

	if (!worldDBStatus.ok()) {
		emit sendPLog(" |- Opening Source World : " + worldFolder + " failed.");
	}
	else {
		emit sendPLog(" |- Source World database opened successfully");
	}

	
	if (values.overworld.trim == true || values.nether.trim == true || values.end.trim == true) {

		world_settings worldSettings;
		worldSettings.trimOverworld = values.overworld.trim;
		worldSettings.ox1 = values.overworld.x1;
		worldSettings.ox2 = values.overworld.x2;
		worldSettings.oz1 = values.overworld.z1;
		worldSettings.oz2 = values.overworld.z2;

		worldSettings.trimNether = values.nether.trim;
		worldSettings.nx1 = values.nether.x1;
		worldSettings.nx2 = values.nether.x2;
		worldSettings.nz1 = values.nether.z1;
		worldSettings.nz2 = values.nether.z2;

		worldSettings.trimEnd = values.end.trim;
		worldSettings.ex1 = values.end.x1;
		worldSettings.ex2 = values.end.x2;
		worldSettings.ez1 = values.end.z1;
		worldSettings.ez2 = values.end.z2;

		emit sendPLog("");
		if (worldSettings.trimOverworld == true) {
			emit sendPLog("-> Overworld Dimension Coordinates...");
			emit sendPLog(" |- Block Coordinates: " + QString::number(worldSettings.ox1 * 16) + "," + QString::number(worldSettings.oz1 * 16) + " to " + QString::number(worldSettings.ox2 * 16) + "," + QString::number(worldSettings.oz2 * 16));
			emit sendPLog(" |- Chunk Coordinates: " + QString::number(worldSettings.ox1) + "," + QString::number(worldSettings.oz1) + " to " + QString::number(worldSettings.ox2) + "," + QString::number(worldSettings.oz2));
		}

		if (worldSettings.trimNether == true) {
			emit sendPLog("-> Nether Dimension Coordinates...");
			emit sendPLog(" |- Block Coordinates: " + QString::number(worldSettings.nx1 * 16) + "," + QString::number(worldSettings.nz1 * 16) + " to " + QString::number(worldSettings.nx2 * 16) + "," + QString::number(worldSettings.nz2 * 16));
			emit sendPLog(" |- Chunk Coordinates: " + QString::number(worldSettings.nx1) + "," + QString::number(worldSettings.nz1) + " to " + QString::number(worldSettings.nx2) + "," + QString::number(worldSettings.nz2));
		}

		if (worldSettings.trimEnd == true) {
			emit sendPLog("-> End Dimension Coordinates...");
			emit sendPLog(" |- Block Coordinates: " + QString::number(worldSettings.ex1 * 16) + "," + QString::number(worldSettings.ez1 * 16) + " to " + QString::number(worldSettings.ex2 * 16) + "," + QString::number(worldSettings.ez2 * 16));
			emit sendPLog(" |- Chunk Coordinates: " + QString::number(worldSettings.ex1) + "," + QString::number(worldSettings.ez1) + " to " + QString::number(worldSettings.ex2) + "," + QString::number(worldSettings.ez2));
		}

		// Calculate database sizes (threaded)
		emit sendPLog("");
		emit sendPLog("-> Calculating Database Size.");
		emit sendPLog(" | -Please wait...");
		size_t totalKeys;
		if (inDev == false) {
			totalKeys = calculate_db_size(db);
			emit sendPLog(" |- Source Database has " + QString::number(totalKeys) + " keys to process.");
		}
		else {
			totalKeys = 5000000;
		}
		emit sendPLog("");
		emit sendPLog("-> Scanning database...");
		emit sendPLog(" |- Please wait...");
		parse_world(db, worldSettings, totalKeys, scanOnly);

	}

	if (values.optimize == true) optimize_world(db);

	emit sendPLog("");
	emit sendPLog("->Process Complete.");

	emit sendProgressLabel("Done.");

	emit sendPLog(" |- Closing Database...");
	delete db;

	emit sendPLog(" |- Done.");
	// TIMER
	// the code you wish to time goes here
	clock_t stop_s = clock();
	float clocktime = (stop_s - start_s) / 1000;

	if (clocktime > 60) {
		clocktime = clocktime / 60;
		std::stringstream ss;
		ss << dec << setprecision(2) << clocktime;
		std::string time = ss.str();
		emit sendPLog("\nProcess took " + QString::fromStdString(time) + " minutes to complete");
	}
	else {
		std::stringstream ss;
		ss << dec << setprecision(2) << clocktime;
		std::string time = ss.str();
		emit sendPLog("\nProcess took " + QString::fromStdString(time) + " seconds to complete");
	}
	return 100;

}
int pruneWindow::calculate_db_size(leveldb::DB* db) {

	emit sendProgressLabel("[1/6] Calculating Database Size...");
	int totalKeys = 0;
	const char* dots[5];
	dots[0] = ".";
	dots[1] = "..";
	dots[2] = "...";
	dots[3] = "....";
	dots[4] = ".....";
	int dot_no = 0;
	leveldb::ReadOptions readOptions;
	readOptions.decompress_allocator = new leveldb::DecompressAllocator();

	readOptions.verify_checksums = false;
	leveldb::Iterator* itr = db->NewIterator(readOptions);

	emit sendProgressFormat("Scanning...");
	emit sendProgress(100);
	for (itr->SeekToFirst(); itr->Valid(); itr->Next()) {
		if (totalKeys % 10000 == 0) {
			dot_no++;
			if (dot_no == 5) dot_no = 0;
			sendProgressFormat(dots[dot_no]);
		}
		totalKeys++;
	}
	delete itr;

	emit sendProgressFormat("%p%");
	emit sendProgress(0);

	return totalKeys;
}


int pruneWindow::optimize(leveldb::DB* db) {
	db->CompactRange(NULL, NULL);
	db->CompactRange(NULL, NULL);

	emit sendProgressLabel("Finished Optimizing...");
	emit sendProgressFormat("Optimized");
	emit sendProgress(100);

	return 100;
}

void pruneWindow::optimize_world(leveldb::DB* db) {

	emit sendProgressLabel("Optimizing World... This can take a long time!");
	int totalKeys = 0;
	const char* dots[5];
	dots[0] = ".";
	dots[1] = "..";
	dots[2] = "...";
	dots[3] = "....";
	dots[4] = ".....";
	int dot_no = 0;

	emit sendProgress(0);
	QFuture<int> future = QtConcurrent::run(this, &pruneWindow::optimize, db);
	QFutureWatcher<int> watcher;
	QEventLoop loop;

	while(future.isRunning()) {
		dot_no++;
		if (dot_no == 5) dot_no = 0;
		sendProgressFormat(dots[dot_no]);
		_sleep(100);
	}

	// QueuedConnection is necessary in case the signal finished is emitted before the loop starts (if the task is already finished when setFuture is called)
	connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()), Qt::QueuedConnection);
	watcher.setFuture(future);
	loop.exec();

}

void pruneWindow::parse_world(leveldb::DB* db, world_settings worldSettings, size_t totalKeys, bool scanOnly) {

	emit sendProgressLabel("[2/6] Scanning Database...");
	emit sendProgressFormat("%p%");
	emit sendProgress(0);

	leveldb::ReadOptions readOptions;
	readOptions.decompress_allocator = new leveldb::DecompressAllocator();

	readOptions.verify_checksums = false;
	leveldb::Iterator* itr = db->NewIterator(readOptions);
	size_t currentKey = 0;
	float lastProgress = 0;
	static char const spin_chars[] = "/-\\|";
	std::map<i_act, std::vector<char>> actorData;
	std::map<i_act, std::vector<char>> digestData;

	i_dim oDim, nDim, eDim;
	i_chnk oX1, oX2, oZ1, oZ2, nX1, nX2, nZ1, nZ2, eX1, eX2, eZ1, eZ2;
	size_t keysDeleted = 0;
	size_t portalsDeleted = 0;
	size_t chunkDataFound = 0;
	size_t chunkDataMarked = 0;
	size_t digestDataFound = 0;
	int digestDataMarked = 0;
	size_t actorsFound = 0;
	std::vector<std::vector<char>> dbKeys;

	for (itr->SeekToFirst(); itr->Valid(); itr->Next()) {

		// Update progress
		float progress = ceil(((float)currentKey / (float)totalKeys) * 100);
		if (progress > lastProgress) {
			sendProgress(progress);
			lastProgress = progress;
		}
		currentKey++;
		// End Update Progress

		oDim = worldSettings.trimOverworld;
		nDim = worldSettings.trimNether;
		eDim = worldSettings.trimEnd;
		oX1 = worldSettings.ox1;
		oX2 = worldSettings.ox2;
		oZ1 = worldSettings.oz1;
		oZ2 = worldSettings.oz2;
		nX1 = worldSettings.nx1;
		nX2 = worldSettings.nx2;
		nZ1 = worldSettings.nz1;
		nZ2 = worldSettings.nz2;
		eX1 = worldSettings.ex1;
		eX2 = worldSettings.ex2;
		eZ1 = worldSettings.ez1;
		eZ2 = worldSettings.ez2;

		// Get Keys
		leveldb::Slice k = itr->key();
		std::string dbkey = k.ToString();
		if (
			k.starts_with("AutonomousEntities")
			|| k.starts_with("BiomeData")
			|| k.starts_with("Nether")
			|| k.starts_with("Overworld")
			|| k.starts_with("TheEnd")
			|| k.starts_with("scoreboard")
			|| k.starts_with("mobevents")
			|| k.starts_with("schedulerWT")
			|| k.starts_with("LevelChunkMetaDataDictionary")
			|| k.starts_with("VILLAGE_")
			|| k.starts_with("map_")
			|| k.starts_with("~local_player")
			|| k.starts_with("player_")
			|| k.starts_with("tickingarea_")
			|| k.starts_with("portals")
			|| k.starts_with("game_flatworldlayers")
			|| k.starts_with("structuretemplate_")
			) {
			//std::cout << "Ignore Key." << std::endl;
			continue;
		}
		else if (k.starts_with("actorprefix")) {
			// Actor
			i_act actorID = bytestring_to_int(k, 11);
			auto thisKey = std::vector<char>(k.data(), k.data() + k.size());
			actorData[actorID] = thisKey;
		}
		else if (k.starts_with("digp")) {
			digestDataFound++;
			// Digest
			// Get Chunk Info
			i_chnk chunkX = get_intval(k, 4);
			i_chnk chunkZ = get_intval(k, 8);
			// Get Dimension
			i_dim dimension = 0;
			if (k.size() > 12) dimension = get_intval(k, 12);
			auto thisKey = std::vector<char>(k.data(), k.data() + k.size());

			// Mark for deletion if in dimension & outside safe area
			bool inSafeArea = true;
			if ((oDim == true && dimension == 0 && (chunkX < oX1 || chunkX >= oX2 || chunkZ < oZ1 || chunkZ >= oZ2)) ||
				(nDim == true && dimension == 1 && (chunkX < nX1 || chunkX >= nX2 || chunkZ < nZ1 || chunkZ >= nZ2)) ||
				(eDim == true && dimension == 2 && (chunkX < eX1 || chunkX >= eX2 || chunkZ < eZ1 || chunkZ >= eZ2))
				) 
			{
				inSafeArea = false;
			}

			if (inSafeArea == false) {
				dbKeys.push_back(thisKey);
				digestDataMarked++;
			}
			else {
				// If not marked then get actor IDs from digest
				auto v = itr->value();
				auto value = v.ToString();
				int bytes = 8;
				for (int i = 0; i < value.size(); i += bytes) {
					i_act actorID = bytestring_to_int(value, i);
					digestData[actorID] = thisKey;
				}
			}
			continue;
		}
		else if (k.size() == 9 || k.size() == 10 || k.size() == 13 || k.size() == 14) {
			auto thisKey = std::vector<char>(k.data(), k.data() + k.size());
			// Chunk Data
			// Get Chunk Info
			i_chnk chunkX = get_intval(k, 0);
			i_chnk chunkZ = get_intval(k, 4);
			// Get Dimension
			i_dim dimension;
			if (k.size() == 9 || k.size() == 10) dimension = 0;
			else dimension = get_intval(k, 8);

			chunkDataFound++;

			bool inSafeArea = true;
			if ((oDim == true && dimension == 0 && (chunkX < oX1 || chunkX >= oX2 || chunkZ < oZ1 || chunkZ >= oZ2)) ||
				(nDim == true && dimension == 1 && (chunkX < nX1 || chunkX >= nX2 || chunkZ < nZ1 || chunkZ >= nZ2)) ||
				(eDim == true && dimension == 2 && (chunkX < eX1 || chunkX >= eX2 || chunkZ < eZ1 || chunkZ >= eZ2))
				)
			{
				inSafeArea = false;
			}
			if (inSafeArea == false) {
				dbKeys.push_back(thisKey);
				chunkDataMarked++;
			}
			continue;
		}
	}

	emit sendPLog(" |- Digest data found in the world: " + QString::number(digestDataFound) + "                          ");
	emit sendPLog(" |- Digest data in this dimension, outside of safe area: " + QString::number(digestDataMarked));
	emit sendPLog(" |- Chunk data found in the world: " + QString::number(chunkDataFound));
	emit sendPLog(" |- Chunk data in this dimension, outside of safe area: " + QString::number(chunkDataMarked));
	emit sendPLog(" |- Scanning database complete.");

	emit sendProgress(0);
	emit sendProgressLabel("[3/6] Scanning Entity Data...");
	emit sendProgressFormat("%p%");

	emit sendPLog("");
	emit sendPLog("-> Scanning entity data.");
	emit sendPLog(" |- Please wait...");

	int i = 0;
	int actorDataSize = actorData.size();
	float lProgress = 0;
	int matches = 0;
	int lostActorsFound = 0;
	std::vector<std::vector<char>> safeActors;

	// Find matching actor data against digest data
	std::map<i_act, std::vector<char>>::iterator atr;
	for (atr = begin(actorData); atr != end(actorData); ++atr) {

		// Update progress
		float progress = ceil(((float)i / (float)actorDataSize) * 100);
		if (progress > lProgress) {
			sendProgress(progress);
			lProgress = progress;
		}
		// End Update Progress

		i_act actorID = atr->first;
		auto thisKey = std::vector<char>(atr->second.data(), atr->second.data() + atr->second.size());

		bool found = false;
		std::map<i_act, std::vector<char>>::iterator dtr;
		dtr = digestData.find(actorID);
		if (dtr != digestData.end()) {
			// Match
			// Actor data found in this digest so we need to keep this actor
			// Store actor details so we can find lost digest data later
			safeActors.push_back(thisKey);
			matches++;
			found = true;
		}
		if (found == false) {
			// Lost actor!
			// No match for this actor data in the digest files
			// This is a lost actor so delete it
			lostActorsFound++;
			dbKeys.push_back(thisKey);
		}
		i++;
	}
	if (lostActorsFound > 0) {
		emit sendPLog(" |- Found " + QString::number(lostActorsFound) + " lost entities in the world!");
	}
	else {
		emit sendPLog(" |- No lost entities found. ");
	}
	// Then with the actors that remain..
	// Check for lost digest data
	// Iterate through the digest data
	// If no remaining actors match, then the digest reference is safe to delete
	// Strip that reference from the digest key value and put the value back in.
	emit sendPLog("");
	emit sendPLog("-> Scanning entity digest values.");
	emit sendPLog(" |- Please wait...");


	emit sendProgress(0);
	emit sendProgressLabel("[4/6] Scanning Digest Data...");
	emit sendProgressFormat("%p%");

	int c = 0;
	int digestDataSize = digestData.size();
	float cProgress = 0;
	int lostDigestFound = 0;

	std::map<i_act, std::vector<char>>::iterator dtr;
	for (dtr = begin(digestData); dtr != end(digestData); ++dtr) {

		// Update progress
		float progress = ceil(((float)c / (float)digestDataSize) * 100);
		if (progress > cProgress) {
			sendProgress(progress);
			cProgress = progress;
		}
		// End Update Progress

		i_act digestActorID = dtr->first;
		bool found = false;

		std::map<i_act, std::vector<char>>::iterator atr;
		atr = actorData.find(digestActorID);
		if (atr != actorData.end()) {
			// This digest value is usefull so dont remove it
			found = true;
		}
		if (found == false) {
			// No actors current were found in this digest data
			// This digest doesn't reference any actors left in this world
			// Strip this from the digest key
			//std::cout << "Digest Lost!" << std::endl;
			lostDigestFound++;
			if (scanOnly == false) {
				std::string digestVal;
				std::string digestKey(dtr->second.begin(), dtr->second.end());
				leveldb::Status r = db->Get(leveldb::ReadOptions(), digestKey, &digestVal);
				//std::cout << "Digest Read OK!" << std::endl;
				if (r.ok()) {
					size_t bytes = 8;
					std::string newDigestVal = "";
					for (int i = 0; i < digestVal.size(); i += bytes) {
						i_act actorID = bytestring_to_int(digestVal, i);
						if (actorID == digestActorID) digestVal.erase(i, bytes);
					}
					leveldb::Status d = db->Put(leveldb::WriteOptions(), digestKey, digestVal);
					if (d.ok()) {
						//std::cout << "Digest Write OK!" << std::endl;
					}
					else {
						//std::cout << "Digest Write Error!" << std::endl;
					}
				}
				else {
					//std::cout << "Digest Read Error!" << std::endl;
				}
			}
		}
		c++;
	}
	if (lostDigestFound > 0) {
		emit sendPLog(" |- Found " + QString::number(lostDigestFound) + " lost entity digest values!");
	}
	else {
		emit sendPLog(" |- No lost entity digest values found.");
	}

	emit sendPLog("");
	emit sendPLog("-> Deleting data.");
	emit sendPLog(" |- Please wait...");


	emit sendProgress(0);
	emit sendProgressLabel("[5/6] Deleting Data...");
	emit sendProgressFormat("%p%");

	if (scanOnly == false && dbKeys.size() > 0) {
		leveldb::WriteBatch batch;
		std::vector<std::vector<char>>::iterator it;
		float dProgress = 0;

		for (int i = 0; i < dbKeys.size(); i++) {
			std::string dbKey(dbKeys[i].begin(), dbKeys[i].end());
			batch.Delete(dbKey);
			keysDeleted++;
			float progress = ceil(((float)i / (float)dbKeys.size()) * 100);
			if (progress > dProgress) {
				sendProgress(progress);
				dProgress = progress;
			}

		}
		leveldb::WriteOptions write_options;
		write_options.sync = true;
		leveldb::Status status = db->Write(leveldb::WriteOptions(), &batch);
		if (!status.ok()) {
			emit sendPLog("Error deleting database keys!");
		}
		else {
			emit sendPLog(" |- Deleted " + QString::number(chunkDataMarked) + " chunk data keys from the world.");
			emit sendPLog(" |- Deleted " + QString::number(digestDataMarked) + " digest data keys from the world.");
			emit sendPLog(" |- Deleted " + QString::number(lostActorsFound) + " entity keys from world.");
		}
	}
	else {
		emit sendPLog(" |- No Chunk, digest or entity keys to delete.");
	}
	delete itr;

	emit sendProgress(0);
	emit sendProgressLabel("[6/6] Scanning Portal Data...");
	emit sendProgressFormat("%p%");

	// Read portals key
	std::string portalKey = "portals";
	std::string portalVal;
	leveldb::Status s = db->Get(leveldb::ReadOptions(), portalKey, &portalVal);
	if (s.ok()) {
		emit sendPLog("");
		emit sendPLog("-> Removing portal data...");
		// Portal key exists so get data
		// Get list of portals in this dimension by iterating through NBT data
		int poffset = 0;
		fxNBT output;
		while (poffset < portalVal.size()) {
			poffset = fxnt_getNBT(portalVal, poffset, output);
		}
		//output.print_map();

		int workingOffset = 0;

		fxNBT::portalData portals = output.find_portal_data();

		for (int i = 0; i < portals.map.size(); i++) {

			fxNBT::portalData portal = portals.map[i];

			i_dim dimension = std::stoi(portal.dim);
			i_chnk chunkX = std::stoi(portal.x) / 16;
			i_chnk chunkZ = std::stoi(portal.z) / 16;
			//std::cout << "Portal D: " << portalDim << " X: " << portal.x << " Z: " << portal.z << " CX: " << portalChunkX << " CZ: " << portalChunkZ << std::endl;
			bool inSafeArea = true;

			if ((oDim == true && dimension == 0 && (chunkX < oX1 || chunkX >= oX2 || chunkZ < oZ1 || chunkZ >= oZ2)) ||
				(nDim == true && dimension == 1 && (chunkX < nX1 || chunkX >= nX2 || chunkZ < nZ1 || chunkZ >= nZ2)) ||
				(eDim == true && dimension == 2 && (chunkX < eX1 || chunkX >= eX2 || chunkZ < eZ1 || chunkZ >= eZ2))
				)
			{
				inSafeArea = false;
			}

			if (inSafeArea == false) {
				portalsDeleted++;
				int recordOffset = portal.init_offset - workingOffset;
				int recordLength = portal.init_length + 1;

				portalVal.erase(recordOffset, recordLength);
				// Move the offset to account for change in length
				workingOffset += recordLength;
			}
		}
		if (portalsDeleted > 0) {

			// Update list
			fxNBT::portalData list = output.find_portal_list();
			int listValue = list.entries;
			int newVal = listValue - portalsDeleted;
			auto newValStr = int_to_bytestring(newVal);
			portalVal.replace(list.val_offset, list.val_length, newValStr);

			// Check
			/*
			int loffset = 0;
			fxNBT loutput;
			while (loffset < portalVal.size()) {
				loffset = fxnt_getNBT(portalVal, loffset, loutput);
			}
			loutput.print_map();
			system("pause");
			*/

			emit sendPLog(" |- Found " + QString::number(portalsDeleted) + " portal records in this dimension, outside of the safe area.");
			if (scanOnly == false) {
				leveldb::WriteOptions write_options;
				write_options.sync = true;
				leveldb::Status p = db->Put(leveldb::WriteOptions(), portalKey, portalVal);
				if (!p.ok()) {
					emit sendPLog("Error deleting portal data!");
				}
				emit sendPLog(" |- Deleted " + QString::number(portalsDeleted) + " portal records.");
			}
			else {
				emit sendPLog(" |- No portal data to remove.");
			}
		}
		else {
			emit sendPLog(" |- No portal data to remove.");
		}
	}
	emit sendProgress(100);
	emit sendProgressLabel("Done...");
	emit sendProgressFormat("");
}