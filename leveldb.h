#ifndef LEVELDB_H
#define LEVELDB_H

#define LEVELDB_PLATFORM_WINDOWS
#pragma comment(lib,"leveldb.lib")
#define DLLX __declspec( dllimport )

#include <QString>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "leveldb/db.h"
#include "leveldb/zlib_compressor.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/cache.h"
#include "leveldb/decompress_allocator.h"
#include "leveldb/write_batch.h"

int readDB(QString dbdir, qint64 dbSize);
#endif
