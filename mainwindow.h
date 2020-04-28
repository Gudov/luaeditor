#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
extern "C" {
#include "lua/lua.h"
#include "lua/lundump.h"
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openFileInput();
	void openFileSave();
	void fileSave();

	void openEditor(const QModelIndex &index);

private:
	void openLua(QString fileName);

    Ui::MainWindow *ui;
	lua_State *L;
	Proto *proto;
	QString fileName;
};
#endif // MAINWINDOW_H
