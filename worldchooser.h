#pragma once

#include <QDialog>
#include "ui_dialog_worldChooser.h"

class worldChooser : public QDialog
{
	Q_OBJECT

public:
	worldChooser(QWidget* parent = Q_NULLPTR);

private:
	Ui::dialog_worldChooser ui;
	worldChooser* worldchooser;
	void closeEvent(QCloseEvent* event);
	
private slots:
	void itemChosen();
	void reject();

signals :
	void loadWorldSignal(QString);
	void closingChooserWindow();


};

