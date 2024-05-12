#include "globals.h"
#include <QString>
#include <QStandardPaths>

// ALL THE GLOBAL DEFINITIONS
QString app_title = "MBCE Prune Tool";

QString version = "3.0";
QString version_long = "v" + version + " for MCBE 1.18.31+";
QString tmp_dir = "tmp";
QString tmp_world_dir = tmp_dir + "/world";
QString data_dir = "data";
QString log_dir = "logs";
QString profile_dir = data_dir + "/profiles";

QString about_file = data_dir + "/about.txt";
QString options_file = data_dir + "/options.txt";

QString ini_file = data_dir + "/trim_settings.txt";
QString profile = "default";


// Default MC World Folder (Windows 10): %LOCALAPPDATA%\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\LocalState\games\com.mojang\minecraftWorlds
QString localappdata = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(0);
QString mc_world_dir = localappdata + "/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/com.mojang/minecraftWorlds/";

QString world_dir = "";
QString levelname_file = "levelname.txt";

QString levelicon_file = "world_icon.jpeg";
QString placeholder_icon = ":/MCBEAdmin/Resources/gfx/world_icon_fade.jpg";
QString leveldata_file = "level.dat";
QString leveldata_file_old = "level.dat_old";
QString leveldb_dir = "db";