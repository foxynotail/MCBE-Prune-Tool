#ifndef GLOBALS_H
#define GLOBALS_H

#define LEVELDB_PLATFORM_WINDOWS
#pragma comment(lib,"leveldb.lib")
#define DLLX __declspec( dllimport )

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <regex>
#include <map>
#include <sstream>
#include <set>
#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <variant>
#include <vector>
#include <optional>
#include <cassert>
#include <thread>
#include <future>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fstream>

#include "leveldb/db.h"
#include "leveldb/zlib_compressor.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/cache.h"
#include "leveldb/decompress_allocator.h"
#include "leveldb/write_batch.h"

#include <QTGlobal>
#include <QtWidgets/QApplication>
#include <QString>
#include <QTextStream>
#include <QTime>
#include <QDate>


// ALL THE GLOBAL DECLARATIONS

// don't use #include <QString> here, instead do this:

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

// that way you aren't compiling QString into every header file you put this in...
// aka faster build times.

extern QString app_title;

extern QString version;
extern QString version_long;

extern QString tmp_dir;
extern QString tmp_world_dir;
extern QString data_dir;
extern QString log_dir;
extern QString profile_dir;

extern QString about_file;
extern QString profile_file;
extern QString options_file;

extern QString localappdata;

extern QString mc_world_dir;
extern QString world_dir;
extern QString levelname_file;
extern QString leveldata_file;
extern QString leveldata_file_old;
extern QString leveldb_dir;
extern QString levelicon_file;
extern QString placeholder_icon;

extern QString ini_file;
extern QString profile;

extern QString worldname;

#define APP_TITLE app_title

#define VERSION version
#define VERSION_LONG version_long

#define TMP_DIR tmp_dir
#define TMP_WORLD_DIR tmp_world_dir
#define DATA_DIR data_dir
#define LOG_DIR log_dir
#define PROFILE_DIR profile_dir

#define ABOUT_FILE about_file

#define MC_WORLD_DIR mc_world_dir
#define LEVELNAME_FILE levelname_file
#define LEVELDATA_FILE leveldata_file
#define LEVELDATA_FILE_OLD leveldata_file_old
#define LEVELDB_DIR leveldb_dir
#define LEVELICON_FILE levelicon_file
#define PLACEHOLDER_ICON placeholder_icon

#define INI_FILE ini_file
#define PROFILE profile

#define WORLDNAME worldname

struct pruneValues {
	QString worldDir{};
	bool optimize{};
	struct Overworld {
		bool trim{};
		qint64 x1{}, x2{}, z1{}, z2{};
	};
	struct Nether {
		bool trim{};
		qint64 x1{}, x2{}, z1{}, z2{};
	};
	struct End {
		bool trim{};
		qint64 x1{}, x2{}, z1{}, z2{};
	};
	Overworld overworld{};
	Nether nether{};
	End end{};
};

typedef int64_t i_act;
typedef int32_t i_chnk;
typedef int32_t i_dim;
typedef int16_t i_tag;
typedef int16_t i_sci;
int max8 = 999;

struct world_settings {
	bool trimOverworld, trimNether, trimEnd;
	int status, lenX, lenZ, offsetX, offsetZ;
	i_dim dimension;
	i_chnk x1, z1, x2, z2;
	i_chnk ox1, ox2, oz1, oz2, nx1, nx2, nz1, nz2, ex1, ex2, ez1, ez2;
};

typedef std::map<int, std::vector<int>> safe_area;
typedef std::map<int, std::map<int, std::vector<int>>> safe_areas;

//typedef std::map<int, int> safe_area;			 // int, coord
//typedef std::map<int, safe_area> safe_area_dim;  // int, coords
//typedef std::map<int, safe_area_dim> safe_areas; // dimension <int, coords>

// Dimension, Row, Coords
// 0, 0, 0 = x1
// 0, 0, 1 = x2
// 0, 0, 2 = z1
// 0, 0, 3 = z2
// 0, 1, 0 = x1
// 0, 

struct block_dimensions {
	i_chnk x1, x2, z1, z2;
};

#endif // GLOBALS_H