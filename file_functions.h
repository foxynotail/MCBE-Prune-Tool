#ifndef FILE_FUNCIONS_H
#define FILE_FUNCIONS_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <string>
#include "globals.h";

namespace fs = std::filesystem;

class file_functions
{
public:
	static safe_areas read_profile(QString filepath);
	static QString read_file(QString filepath);
	static bool dir_exists(QString dir);
	static bool file_exists(QString path);
	static bool write_file(QString filepath, QString data = "");
	static qint64 dirSize(QString dirPath);
	static QString formatSize(qint64 size);
	static qint64 read_ini_int(QString section, QString item, QString filepath);
	static QString read_ini_str(QString section, QString item, QString filepath);
	static void write_ini_val(QString section, QString item, QString value, QString filepath);
	static void CopyRecursive(const fs::path& src, const fs::path& target) noexcept;
};
#endif

