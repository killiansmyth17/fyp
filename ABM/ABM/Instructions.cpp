#include "Instructions.h"
#include "ui_Instructions.h"

Instructions::Instructions(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::Instructions)
{
	ui->setupUi(this);
}

Instructions::~Instructions()
{
	delete ui;
}
