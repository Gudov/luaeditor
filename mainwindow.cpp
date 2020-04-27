#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
extern "C" {
#include "lua/lundump.h"
#include "lua/lobject.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
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
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		this->openLua(file);
	}
}

typedef struct LoadS {
  const char *s;
  size_t size;
} LoadS;

static const char *getS (lua_State *L, void *ud, size_t *size) {
  LoadS *ls = (LoadS *)ud;
  (void)L;
  if (ls->size == 0) return NULL;
  *size = ls->size;
  ls->size = 0;
  return ls->s;
}

extern Proto *lastLoadedProto;
void MainWindow::openLua(QFile &file)
{
	QByteArray raw = file.readAll();

	luaL_loadbuffer(L, raw.data(), raw.size(), "?");
	this->proto = lastLoadedProto;
}
