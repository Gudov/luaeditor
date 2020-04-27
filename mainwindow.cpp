#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
extern "C" {
#include "lua/lundump.h"
#include "lua/lobject.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

extern Proto *lastLoadedProto;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	L = luaL_newstate();

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
		this->openLua(fileName);
	}
}

void MainWindow::openLua(QString fileName)
{
	FILE *f = fopen(fileName.toUtf8().data(), "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *data = (char*)malloc(fsize + 1);
	fread(data, 1, fsize, f);
	fclose(f);

	luaL_loadbuffer(L, data, fsize, "?");

	this->proto = lastLoadedProto;
}
