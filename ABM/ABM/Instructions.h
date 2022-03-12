#pragma once

#include <QMainWindow>
namespace Ui { class Instructions; };

class Instructions : public QMainWindow
{
	Q_OBJECT

public:
	Instructions(QWidget *parent = Q_NULLPTR);
	~Instructions();

private:
	Ui::Instructions *ui;
};
