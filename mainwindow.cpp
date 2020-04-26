#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	connect(this->ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileInput);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFileInput()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open lua"), "C:\\war2", tr("*.lua"));

	QFile file(fileName);
	if (file.exists()) {
		this->openLua(file);
	}
}

void MainWindow::openLua(QFile &file)
{

}
