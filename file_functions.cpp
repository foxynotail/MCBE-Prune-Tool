#include "file_functions.h"
#include <windows.h>

QString file_functions::read_file(QString filepath)
{
	qDebug() << "Reading File: " << filepath;

	QString data = "";

	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	while (!in.atEnd()) {
		data += in.readLine();
	}

	file.close();

	return data;
}

bool file_functions::dir_exists(QString dir)
{
	qDebug() << "Checking Directory Exists: " << dir;
	bool dirExists = QDir(dir).exists();
	return dirExists;

}

bool file_functions::file_exists(QString path)
{
	//qDebug() << "Checking File Exists: " << path;
	QFileInfo fileInfo(path);
	bool fileExists = fileInfo.exists() && fileInfo.isFile();
	return fileExists;

}


bool file_functions::write_file(QString filename, QString data)
{
	// TODO
	return false;
}


qint64 file_functions::dirSize(QString dirPath) {
	qint64 size = 0;
	QDir dir(dirPath);
	//calculate total size of current directories' files
	QDir::Filters fileFilters = QDir::Files;
	for (QString filePath : dir.entryList(fileFilters)) {
		QFileInfo fi(dir, filePath);
		size += fi.size();
	}
	//add size of child directories recursively
	QDir::Filters dirFilters = QDir::Dirs | QDir::NoDotAndDotDot;
	for (QString childDirPath : dir.entryList(dirFilters))
		size += dirSize(dirPath + QDir::separator() + childDirPath);
	return size;
}

QString file_functions::formatSize(qint64 size) {
	QStringList units = { "Bytes", "KB", "MB", "GB", "TB", "PB" };
	int i;
	double outputSize = size;
	for (i = 0; i < units.size() - 1; i++) {
		if (outputSize < 1024) break;
		outputSize = outputSize / 1024;
	}
	return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
}

qint64 file_functions::read_ini_int(QString section, QString item, QString filepath) {

	std::string sectionS = section.toStdString();
	std::string itemS = item.toStdString();
	std::string filepathS = filepath.toStdString();
	std::wstring sectionT = std::wstring(sectionS.begin(), sectionS.end());
	std::wstring itemT = std::wstring(itemS.begin(), itemS.end());
	std::wstring filepathT = std::wstring(filepathS.begin(), filepathS.end());
	int result = GetPrivateProfileInt(sectionT.c_str(), itemT.c_str(), 0, filepathT.c_str());
	return result;
}

void file_functions::write_ini_val(QString section, QString item, QString value, QString filepath) {

	std::string sectionS = section.toStdString();
	std::string itemS = item.toStdString();
	std::string filepathS = filepath.toStdString();
	std::string valueS = value.toStdString();
	std::wstring sectionT = std::wstring(sectionS.begin(), sectionS.end());
	std::wstring itemT = std::wstring(itemS.begin(), itemS.end());
	std::wstring filepathT = std::wstring(filepathS.begin(), filepathS.end());
	std::wstring valueT = std::wstring(valueS.begin(), valueS.end());
	WritePrivateProfileString(sectionT.c_str(), itemT.c_str(), valueT.c_str(), filepathT.c_str());

}
QString file_functions::read_ini_str(QString section, QString item, QString filepath) {

	std::string sectionS = section.toStdString();
	std::string itemS = item.toStdString();
	std::string filepathS = filepath.toStdString();
	std::wstring sectionT = std::wstring(sectionS.begin(), sectionS.end());
	std::wstring itemT = std::wstring(itemS.begin(), itemS.end());
	std::wstring filepathT = std::wstring(filepathS.begin(), filepathS.end());

	std::string def = "NULL";
	std::wstring defaultT = std::wstring(def.begin(), def.end());
	
	WCHAR output[100];
	DWORD size;
	size = 100;

	GetPrivateProfileString(sectionT.c_str(), itemT.c_str(), defaultT.c_str(), output, size, filepathT.c_str());
	std::wstring ws(output);
	std::string s = std::string(ws.begin(), ws.end());
	return QString::fromStdString(s);
}

void file_functions::CopyRecursive(const fs::path & src, const fs::path & target) noexcept
{
	try
	{
		fs::copy(src, target, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	}
	catch (std::exception& e)
	{
		QString error = QString::fromStdString(e.what());
		qDebug() << "File Copy Error: " << error;
	}
}

safe_areas file_functions::read_profile(QString filepath) {

	safe_areas output;

	QString line = "";

	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	int i = 0;
	while (!in.atEnd()) {
		line += in.readLine();
		QRegExp rx("(\\,)");
		QStringList query = line.split(rx);
		int dim = query.at(0).toInt();
		int x1 = query.at(1).toInt();
		int x2 = query.at(2).toInt();
		int z1 = query.at(3).toInt();
		int z2 = query.at(4).toInt();

		safe_area thisLine;

		thisLine[dim].push_back(x1);
		thisLine[dim].push_back(x2);
		thisLine[dim].push_back(z1);
		thisLine[dim].push_back(z2);

		output[i] = thisLine;
		i++;
	}

	file.close();

	return output;


}