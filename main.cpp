#include <QtWidgets/QApplication>

#include "mcbeadmin.h"
#include "worldchooser.h"
#include "worldwindow.h"
#include "prunewindow.h"

int WinMain(int argc, char *argv[])
{

	QApplication a(argc, argv);
	MCBEAdmin w;
	w.show();
	return a.exec();

}