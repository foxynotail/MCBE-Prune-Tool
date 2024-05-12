size_t calculate_db_size(leveldb::DB* db) {

	size_t totalKeys = 0;
	static char const spin_chars[] = "/-\\|";

	leveldb::ReadOptions readOptions;
	readOptions.decompress_allocator = new leveldb::DecompressAllocator();

	readOptions.verify_checksums = false;
	leveldb::Iterator* itr = db->NewIterator(readOptions);

	for (itr->SeekToFirst(); itr->Valid(); itr->Next()) {
		// Update progress
		if (totalKeys % 50000 == 0) {
			std::cout << " " << spin_chars[int(totalKeys / 50000) & 3] << " ";
			std::cout << "\r" << flush;
		}
		// End Update Progress

		totalKeys++;
	}
	delete itr;
	return totalKeys;
}

void parse_world(leveldb::DB* db, world_settings worldSettings, size_t totalKeys, std::vector<std::vector<char>>& dbKeys, bool scanOnly, std::ofstream& logFile) {

	leveldb::ReadOptions readOptions;
	readOptions.decompress_allocator = new leveldb::DecompressAllocator();

	readOptions.verify_checksums = false;
	leveldb::Iterator* itr = db->NewIterator(readOptions);
	size_t currentKey = 0;
	float lastProgress = 0;
	static char const spin_chars[] = "/-\\|";
	std::map<i_act, std::vector<char>> actorData;
	std::map<i_act, std::vector<char>> digestData;

	i_dim thisDimension;
	i_chnk thisX1;
	i_chnk thisX2;
	i_chnk thisZ1;
	i_chnk thisZ2;
	i_chnk nX1;
	i_chnk nX2;
	i_chnk nZ1;
	i_chnk nZ2;
	i_chnk eX1;
	i_chnk eX2;
	i_chnk eZ1;
	i_chnk eZ2;
	size_t keysDeleted = 0;
	size_t portalsDeleted = 0;
	size_t chunkDataFound = 0;
	size_t chunkDataMarked = 0;
	size_t digestDataFound = 0;
	int digestDataMarked = 0;
	size_t actorsFound = 0;

	for (itr->SeekToFirst(); itr->Valid(); itr->Next()) {

		// Update progress
		float progress = ceil(((float)currentKey / (float)totalKeys) * 100);
		if (progress > lastProgress) {
			std::cout << " |- Removing World data: ";
			std::cout << int(progress) << "% [" << currentKey << "/" << totalKeys << "]";
			std::cout << "\r";
			std::cout.flush();
			lastProgress = progress;
		}
		currentKey++;
		// End Update Progress

		thisDimension = worldSettings.dimension;
		thisX1 = worldSettings.x1;
		thisX2 = worldSettings.x2;
		thisZ1 = worldSettings.z1;
		thisZ2 = worldSettings.z2;

		if (worldSettings.dimension == -1) {
			nX1 = worldSettings.nx1;
			nX2 = worldSettings.nx2;
			nZ1 = worldSettings.nz1;
			nZ2 = worldSettings.nz2;
			eX1 = worldSettings.ex1;
			eX2 = worldSettings.ex2;
			eZ1 = worldSettings.ez1;
			eZ2 = worldSettings.ez2;
		}

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
			if (thisDimension == -1) {
				if (
					(dimension == 0 && (chunkX < thisX1 || chunkX >= thisX2 || chunkZ < thisZ1 || chunkZ >= thisZ2)) ||
					(dimension == 1 && (chunkX < nX1 || chunkX >= nX2 || chunkZ < nZ1 || chunkZ >= nZ2)) ||
					(dimension == 2 && (chunkX < eX1 || chunkX >= eX2 || chunkZ < eZ1 || chunkZ >= eZ2))
					) {
					inSafeArea = false;
				}
			}
			else if (dimension == thisDimension && (chunkX < thisX1 || chunkX >= thisX2 || chunkZ < thisZ1 || chunkZ >= thisZ2)) inSafeArea = false;

			if(inSafeArea == false) {
				dbKeys.push_back(thisKey);
				digestDataMarked++;
			} else {
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
			if (thisDimension == -1) {
				if (
					(dimension == 0 && (chunkX < thisX1 || chunkX >= thisX2 || chunkZ < thisZ1 || chunkZ >= thisZ2)) ||
					(dimension == 1 && (chunkX < nX1 || chunkX >= nX2 || chunkZ < nZ1 || chunkZ >= nZ2)) ||
					(dimension == 2 && (chunkX < eX1 || chunkX >= eX2 || chunkZ < eZ1 || chunkZ >= eZ2))
					) {
					inSafeArea = false;
				}
			}
			else if (dimension == thisDimension && (chunkX < thisX1 || chunkX >= thisX2 || chunkZ < thisZ1 || chunkZ >= thisZ2)) inSafeArea = false;
			if (inSafeArea == false) {
				dbKeys.push_back(thisKey);
				chunkDataMarked++;
			}
			continue;
		}
	}

	rLog(" |- Digest data found in the world: " + std::to_string(digestDataFound) + "                          ", logFile);
	rLog(" |- Digest data in this dimension, outside of safe area: " + std::to_string(digestDataMarked), logFile);
	rLog(" |- Chunk data found in the world: " + std::to_string(chunkDataFound), logFile);
	rLog(" |- Chunk data in this dimension, outside of safe area: " + std::to_string(chunkDataMarked), logFile);
	rLog(" |- Scanning database complete.", logFile);


	rLog("                                                           ", logFile);
	rLog("-> Scanning entity data.", logFile);
	std::cout << " |- Please wait..." << std::endl;

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
			std::cout << " |- Scanning entity data: ";
			std::cout << int(progress) << "% [" << i << "/" << actorDataSize << "]";
			std::cout << "\r";
			std::cout.flush();
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
		rLog(" |- Found " + std::to_string(lostActorsFound) + " lost entities in the world!                          ", logFile);
	}
	else {
		rLog(" |- No lost entities found.                                                      ", logFile);
	}
	// Then with the actors that remain..
	// Check for lost digest data
	// Iterate through the digest data
	// If no remaining actors match, then the digest reference is safe to delete
	// Strip that reference from the digest key value and put the value back in.
	rLog("                                                           ", logFile);
	rLog("-> Scanning entity digest values.", logFile);
	std::cout << " |- Please wait..." << std::endl;


	int c = 0;
	int digestDataSize = digestData.size();
	float cProgress = 0;
	int lostDigestFound = 0;

	std::map<i_act, std::vector<char>>::iterator dtr;
	for (dtr = begin(digestData); dtr != end(digestData); ++dtr) {

		// Update progress
		float progress = ceil(((float)c / (float)digestDataSize) * 100);
		if (progress > cProgress) {
			std::cout << " |- Scanning digest data: ";
			std::cout << int(progress) << "% [" << c << "/" << digestDataSize << "]";
			std::cout << "\r";
			std::cout.flush();
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
			std::cout << "Digest Lost!" << std::endl;
			lostDigestFound++;
			if (scanOnly == false) {
				std::string digestVal;
				std::string digestKey(dtr->second.begin(), dtr->second.end());
				leveldb::Status r = db->Get(leveldb::ReadOptions(), digestKey, &digestVal);
				std::cout << "Digest Read OK!" << std::endl;
				if (r.ok()) {
					size_t bytes = 8;
					std::string newDigestVal = "";
					for (int i = 0; i < digestVal.size(); i += bytes) {
						i_act actorID = bytestring_to_int(digestVal, i);
						if (actorID == digestActorID) digestVal.erase(i, bytes);
					}
					leveldb::Status d = db->Put(leveldb::WriteOptions(), digestKey, digestVal);
					if (d.ok()) {
						std::cout << "Digest Write OK!" << std::endl;
					}
					else {
						std::cout << "Digest Write Error!" << std::endl;
					}
				}
				else {
					std::cout << "Digest Read Error!" << std::endl;
				}
			}
		}
		c++;
	}
	if (lostDigestFound > 0) {
		rLog(" |- Found " + std::to_string(lostDigestFound) + " lost entity digest values!                          ", logFile);
	}
	else {
		rLog(" |- No lost entity digest values found.                                                      ", logFile);
	}

	if (scanOnly == false && dbKeys.size() > 0) {
		leveldb::WriteBatch batch;
		std::vector<std::vector<char>>::iterator it;
		for (int i = 0; i < dbKeys.size(); i++) {
			std::string dbKey(dbKeys[i].begin(), dbKeys[i].end());
			batch.Delete(dbKey);
			keysDeleted++;
		}
		leveldb::WriteOptions write_options;
		write_options.sync = true;
		leveldb::Status status = db->Write(leveldb::WriteOptions(), &batch);
		if (!status.ok()) {
			rLog("Error processing batch: " + status.ToString(), logFile);
		}
		rLog(" |- Deleted " + std::to_string(chunkDataMarked) + " chunk data keys from the world.", logFile);
		rLog(" |- Deleted " + std::to_string(digestDataMarked) + " digest data keys from the world.", logFile);
		rLog(" |- Deleted " + std::to_string(lostActorsFound) + " entity keys from world.", logFile);
	}
	else {
		rLog(" |- No Chunk, digest or entity keys to delete.", logFile);
	}
	delete itr;

	// Read portals key
	rLog("                                                           ", logFile);
	rLog("-> Removing portal data...", logFile);
	std::string portalKey = "portals";
	std::string portalVal;
	leveldb::Status s = db->Get(leveldb::ReadOptions(), portalKey, &portalVal);
	if (s.ok()) {
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
			if (thisDimension == -1) {
				if (
					(dimension == 0 && (chunkX < thisX1 || chunkX >= thisX2 || chunkZ < thisZ1 || chunkZ >= thisZ2)) ||
					(dimension == 1 && (chunkX < nX1 || chunkX >= nX2 || chunkZ < nZ1 || chunkZ >= nZ2)) ||
					(dimension == 2 && (chunkX < eX1 || chunkX >= eX2 || chunkZ < eZ1 || chunkZ >= eZ2))
					) {
					inSafeArea = false;
				}
			}
			else if (dimension == thisDimension && (chunkX < thisX1 || chunkX >= thisX2 || chunkZ < thisZ1 || chunkZ >= thisZ2)) inSafeArea = false;

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
			
			rLog(" |- Found " + std::to_string(portalsDeleted) + " portal records in this dimension, outside of the safe area.", logFile);
			if (scanOnly == false) {
				leveldb::WriteOptions write_options;
				write_options.sync = true;
				leveldb::Status p = db->Put(leveldb::WriteOptions(), portalKey, portalVal);
				if (!p.ok()) {
					rLog("Error processing portal batch: " + p.ToString(), logFile);
				}
				rLog(" |- Deleted " + std::to_string(portalsDeleted) + " portal records.", logFile);
			}
			else {
				rLog(" |- No portal data to remove.", logFile);
			}
		}
		else {
			rLog(" |- No portal data to remove.", logFile);
		}
	}

}


void update_coordinates(int offsetX, int offsetZ, std::string& value) {

	int offset = 0;
	fxNBT output;
	while (offset < value.size()) {
		offset = fxnt_getNBT(value, offset, output);
	}
	//output.print_map();

	int mapSize = output.map.size();

	for (int i = 0; i < mapSize; i++) {

		fxNBT item = output.map[i];

		bool found = false;
		int ioffset = item.val_offset;
		int ilength = item.length;
		int newVal;
		std::string ivalue = item.get_val();

		if (item.type == "value" && item.tagname == "pairx") {
			found = true;
			newVal = std::stoi(ivalue) + (offsetX * 16);
			//std::cout << "PairX: " << ivalue << std::endl;
		}
		else if (item.type == "value" && item.tagname == "pairz") {
			found = true;
			newVal = std::stoi(ivalue) + (offsetZ * 16);
			//std::cout << "PairZ: " << ivalue << std::endl;
		}
		else if (item.type == "value" && item.tagname == "x") {
			found = true;
			newVal = std::stoi(ivalue) + (offsetX * 16);
			//std::cout << "X: " << ivalue << std::endl;
		}
		else if (item.type == "value" && item.tagname == "z") {
			found = true;
			newVal = std::stoi(ivalue) + (offsetZ * 16);
			//std::cout << "Z: " << ivalue << std::endl;
		}
		if (found == true) {
			//std::cout << "Len: " << ilength << std::endl;
			//std::cout << "NewVal: " << newVal << std::endl;
			auto newValStr = int_to_bytestring(newVal);
			//std::cout << "NBTVal: " << newValStr << std::endl;
			//int checkX = get_intval(newValStr, 0);
			//std::cout << "CheckVal: " << checkX << std::endl;
			//std::cout << "CheckLen: " << newValStr.size() << std::endl;
			value.replace(ioffset, ilength, newValStr);
		}
	}
	/*
	int checkOffset = 0;
	fxNBT checkOutput;
	while (checkOffset < value.size()) {
		checkOffset = fxnt_getNBT(value, checkOffset, checkOutput);
	}
	checkOutput.print_map();
	system("pause");
	*/

}
