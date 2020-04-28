#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include "editorwindow.h"

extern "C" {
#include "lua/lundump.h"
#include "lua/lobject.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

extern Proto *lastLoadedProto;
}

Q_DECLARE_METATYPE(Proto*);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	L = luaL_newstate();
	lastLoadedProto = nullptr;

	connect(this->ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileInput);
	connect(this->ui->actionSave_as, &QAction::triggered, this, &MainWindow::openFileSave);
	connect(this->ui->actionSave, &QAction::triggered, this, &MainWindow::fileSave);
	connect(this->ui->treeView, &QTreeView::clicked, this, &MainWindow::openEditor);
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

static QStandardItem *recusiveProtoAdd(Proto *pr) {
	QStandardItem *item = new QStandardItem;
	for (int i = 0; i < pr->sizep; i++) {
		QStandardItem *child = recusiveProtoAdd(pr->p[i]);
		child->setText( QString("%1").arg(i) );
		item->appendRow(child);
	}
	item->setEditable(false);
	item->setData(QVariant::fromValue(pr));
	return item;
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

	if(!lastLoadedProto) {
		QMessageBox msg;
		msg.setText(lua_tostring(L, -1));
		msg.exec();
		return;
	}
	this->fileName = fileName;
	this->proto = lastLoadedProto;
	this->ui->actionSave->setEnabled(true);
	this->ui->actionSave_as->setEnabled(true);

	QStandardItemModel *model = new QStandardItemModel;//(proto->sizep, 1);
	QStandardItem *item = recusiveProtoAdd(proto);
	item->setText("Root");
	model->appendRow(item);

	model->setHorizontalHeaderItem( 0, new QStandardItem( "" ) );
	this->ui->treeView->setModel(model);
}

void MainWindow::openEditor(const QModelIndex &index)
{
	QStandardItem *item = ((QStandardItemModel*)this->ui->treeView->model())->itemFromIndex(index);
	Proto *pr = item->data().value<Proto*>();
	EditorWindow *win = new EditorWindow;
	win->init(pr);
	win->show();
}

void MainWindow::openFileSave()
{
	this->fileName = QFileDialog::getSaveFileName(this,
		tr("save lua"), "C:\\war2", tr("*.lua"));
	this->fileSave();
}

static int writer (lua_State *L, const void* b, size_t size, void* B) {
	(void)L;
	auto vec = (std::vector<uint8_t>*)B;
	auto data = ((uint8_t*)b);
	for(size_t i = 0; i < size; i++) {
		vec->push_back(data[i]);
	}
	//luaL_addlstring((luaL_Buffer*) B, (const char *)b, size);
	return 0;
}

void MainWindow::fileSave()
{
	std::vector<uint8_t> buff;
	luaU_dump(L, this->proto, writer, &buff, 0);

	FILE *f = fopen(fileName.toUtf8().data(), "wb");
	fwrite(buff.data(), 1, buff.size(), f);
	fclose(f);
}
