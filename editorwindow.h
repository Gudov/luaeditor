#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>

extern "C" {
#include "lua/lundump.h"
#include "lua/lobject.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace Ui {
class EditorWindow;
}

class EditorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit EditorWindow(QWidget *parent = nullptr);
	~EditorWindow();

	void init(Proto *pr);

private:
	void initCode(Proto *pr);
	void initConst(Proto *pr);

	Ui::EditorWindow *ui;
};

#endif // EDITORWINDOW_H
