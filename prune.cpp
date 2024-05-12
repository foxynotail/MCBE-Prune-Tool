#include <QtWidgets/QApplication>
#include "prune.h"

using namespace std;




void pruneWorld::closeEvent(QCloseEvent* event)
{
	// Delete temp data

}



pruneWorld::pruneWorld(int argc, char* argv[]) {

	std::ofstream logFile = create_log();

	bool commandLine = false;
	bool scanOnly = false;

	std::cout << "#################################################################" << std::endl;
	std::cout << "# Prune v" << prune_version << std::endl;
	std::cout << "# Trim the chunks around your selected coordinates." << std::endl;
	std::cout << "# For Minecraft Bedrock Edition 1.18.31+" << std::endl;
	std::cout << "# " << std::endl;
	std::cout << "# Instructions & Info in the README.txt file" << std::endl;
	std::cout << "# " << std::endl;
	std::cout << "# Script by FoxyNoTail" << std::endl;
	std::cout << "# foxynotail.com" << std::endl;
	std::cout << "# @foxynotail" << std::endl;
	std::cout << "# (C) 2022, FoxyNoTail" << std::endl;
	std::cout << "############################################################" << std::endl;
	std::cout << std::endl;
	rLog("Prune Program Started.", logFile);

	// For this script, we need to run it from a command line without prompts...
	// If there are arguments, then don't ask questions!
	// Arguments 
	//		1 - Dimension		-1				[-1] All, [0] Overworld, [1] Nether, [2] End)
	//		2 - Coords			x1,z1,x2,z2		
	//		3 - WorldDir		"C:\MyWorld\"

	world_settings worldSettings{};
	std::string worldDB;

	if (argc > 1) {

		commandLine = true;
		rLog("-> Using command line data...", logFile);


		// Using command line
		std::string dimension = argv[1];
		worldSettings.dimension = std::stoi(dimension);
		std::string coords = (std::string)argv[2];
		coords += ",";

		vector<string> coordVec;

		int vecLen = 4;
		if (worldSettings.dimension == -1) vecLen = 12;

		for (int i = 0; i < vecLen; ++i) {
			int index = coords.find(",");
			if (index != std::string::npos) {
				coordVec.push_back(trim(coords.substr(0, index)));
				coords = coords.substr(index + 1);
			}
			else {
				rLog("Error: Invalid Coordinates.", logFile);
				exit;
			}
		}

		block_dimensions input;

		input.x1 = std::stoi(coordVec[0]);
		input.x2 = std::stoi(coordVec[1]);
		input.z1 = std::stoi(coordVec[2]);
		input.z2 = std::stoi(coordVec[3]);
		block_dimensions output = calculate_chunks(input);

		worldSettings.x1 = output.x1;
		worldSettings.x2 = output.x2;
		worldSettings.z1 = output.z1;
		worldSettings.z2 = output.z2;
		if (worldSettings.dimension == -1) {

			input.x1 = std::stoi(coordVec[4]);
			input.x2 = std::stoi(coordVec[5]);
			input.z1 = std::stoi(coordVec[6]);
			input.z2 = std::stoi(coordVec[7]);
			output = calculate_chunks(input);

			worldSettings.nx1 = output.x1;
			worldSettings.nx2 = output.x2;
			worldSettings.nz1 = output.z1;
			worldSettings.nz2 = output.z2;

			input.x1 = std::stoi(coordVec[8]);
			input.x2 = std::stoi(coordVec[9]);
			input.z1 = std::stoi(coordVec[10]);
			input.z2 = std::stoi(coordVec[11]);
			output = calculate_chunks(input);

			worldSettings.ex1 = output.x1;
			worldSettings.ex2 = output.x2;
			worldSettings.ez1 = output.z1;
			worldSettings.ez2 = output.z2;
		}

		std::string worldDir = argv[3];
		worldDB = worldDir + "/db";
		// LOAD LEVEL.DAT AND GET WORLD NAME
		std::string levelFile = worldDir + "/levelname.txt";
		bool worldExists = file_exists(levelFile);

		rLog("-> Checking world files...", logFile);

		if (worldExists == 1) {

			rLog(" |- World Exists.", logFile);
			std::ifstream infile(levelFile);
			std::string worldName;

			if (getline(infile, worldName)) {
				rLog(" |- World Name: " + worldName, logFile);
			}
			infile.close();
		}
		else {
			rLog(" |- ERROR: World Data Not Found!", logFile);
			system("pause");
			exit;
		}

	}
	else {

		commandLine = false;

		worldDB = get_world_directory(logFile);
		get_world_settings(worldSettings, logFile);
		int worldStatus = worldSettings.status;
		if (worldStatus == 0) {
			system("pause");
			exit;
		}

	}

	std::string dimensionName;
	switch (worldSettings.dimension) {
	case -1: {
		dimensionName = "All Dimensions";
		break;
	}
	case 0: {
		dimensionName = "The Overworld";
		break;
	}
	case 1: {
		dimensionName = "The Nether";
		break;
	}
	case 2: {
		dimensionName = "The End";
		break;
	}
	}

	rLog(" |- Dimension: " + dimensionName, logFile);
	if (worldSettings.dimension == -1) {
		rLog("-> Overworld Dimension Coordinates...", logFile);
		rLog(" |- Block Coordinates: " + std::to_string(worldSettings.x1 * 16) + "," + std::to_string(worldSettings.z1 * 16) + " to " + std::to_string(worldSettings.x2 * 16) + "," + std::to_string(worldSettings.z2 * 16), logFile);
		rLog(" |- Chunk Coordinates: " + std::to_string(worldSettings.x1) + "," + std::to_string(worldSettings.z1) + " to " + std::to_string(worldSettings.x2) + "," + std::to_string(worldSettings.z2), logFile);
		rLog("-> Nether Dimension Coordinates...", logFile);
		rLog(" |- Block Coordinates: " + std::to_string(worldSettings.nx1 * 16) + "," + std::to_string(worldSettings.nz1 * 16) + " to " + std::to_string(worldSettings.nx2 * 16) + "," + std::to_string(worldSettings.nz2 * 16), logFile);
		rLog(" |- Chunk Coordinates: " + std::to_string(worldSettings.nx1) + "," + std::to_string(worldSettings.nz1) + " to " + std::to_string(worldSettings.nx2) + "," + std::to_string(worldSettings.nz2), logFile);
		rLog("-> End Dimension Coordinates...", logFile);
		rLog(" |- Block Coordinates: " + std::to_string(worldSettings.ex1 * 16) + "," + std::to_string(worldSettings.ez1 * 16) + " to " + std::to_string(worldSettings.ex2 * 16) + "," + std::to_string(worldSettings.ez2 * 16), logFile);
		rLog(" |- Chunk Coordinates: " + std::to_string(worldSettings.ex1) + "," + std::to_string(worldSettings.ez1) + " to " + std::to_string(worldSettings.ex2) + "," + std::to_string(worldSettings.ez2), logFile);
	}
	else {
		rLog(" |- Block Coordinates: " + std::to_string(worldSettings.x1 * 16) + "," + std::to_string(worldSettings.z1 * 16) + " to " + std::to_string(worldSettings.x2 * 16) + "," + std::to_string(worldSettings.z2 * 16), logFile);
		rLog(" |- Chunk Coordinates: " + std::to_string(worldSettings.x1) + "," + std::to_string(worldSettings.z1) + " to " + std::to_string(worldSettings.x2) + "," + std::to_string(worldSettings.z2), logFile);
	}

	if (commandLine == false) {
		// CONFIRM
		std::cout << "\n#####################################################################################" << std::endl;
		std::cout << "Are you sure you want to Prune these coordinates from " << dimensionName << "?" << std::endl;
		std::cout << "-> Y = Yes, N = No, S = Scan Only" << std::endl;
		std::cout << "#####################################################################################" << std::endl;
		std::cout << "Confirm: ";
		std::string confirm;
		std::cin >> confirm;

		if (confirm == "Y" || confirm == "y") {
			rLog("Cloning Confirmed...", logFile);
			scanOnly = false;
		}
		else if (confirm == "S" || confirm == "s") {
			rLog("Scan Only  Confirmed...", logFile);
			scanOnly = true;
		}
		else {
			rLog("Process Aborted.", logFile);
			rLog("Done.", logFile);
			system("pause");
			exit;
		}
	}

	rLog("                                                           ", logFile);
	rLog("-> Starting Process...", logFile);

	// Start timer to calculate process time
	clock_t start_s = clock();

	// Setup standard world options
	leveldb::Options options = world_options(logFile);

	// Open Source World DB
	rLog(" |- Loading " + worldDB + "...", logFile);
	leveldb::Status worldDBStatus;
	leveldb::DB* db;

	worldDBStatus = leveldb::DB::Open(options, worldDB, &db);

	if (!worldDBStatus.ok()) {
		rLog(" |- Opening Source World : " + worldDB + " failed.", logFile);

		system("pause");
		exit;
	}
	else {
		rLog(" |- Source World database opened successfully", logFile);
	}

	// Calculate database sizes (threaded)
	rLog("                                                           ", logFile);
	rLog("-> Calculating Database Size.", logFile);
	std::cout << " |- Please wait..." << std::endl;
	size_t totalKeys;
	if (inDev == false) {
		std::future<size_t> dbSize = std::async(&calculate_db_size, db);

		totalKeys = dbSize.get();
		rLog(" |- Source Database has " + std::to_string(totalKeys) + " keys to process.", logFile);
	}
	else {
		totalKeys = 5163306;
	}

	rLog("                                                           ", logFile);
	rLog("-> Scanning database...", logFile);
	std::cout << " |- Please wait..." << std::endl;
	std::vector<std::vector<char>> dbKeys;

	parse_world(db, worldSettings, totalKeys, dbKeys, scanOnly, logFile);

	rLog("                                                           ", logFile);
	rLog("->Process Complete.", logFile);


	// ######################################################
	// End Script & Close Database

	rLog(" |- Closing Database...", logFile);
	delete db;
	rLog(" |- Done.", logFile);

	// TIMER
	// the code you wish to time goes here
	clock_t stop_s = clock();
	float clocktime = (stop_s - start_s) / 1000;

	if (clocktime > 60) {
		clocktime = clocktime / 60;
		std::stringstream ss;
		ss << dec << setprecision(2) << clocktime;
		std::string time = ss.str();
		rLog("\nProcess took " + time + " minutes to complete", logFile);
	}
	else {
		std::stringstream ss;
		ss << dec << setprecision(2) << clocktime;
		std::string time = ss.str();
		rLog("\nProcess took " + time + " seconds to complete", logFile);
	}

	if (commandLine == false) {
		system("pause");
	}
	exit;

}