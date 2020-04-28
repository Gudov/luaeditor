#include "editorwindow.h"
#include "ui_editorwindow.h"

#include <QLayoutItem>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QTextItem>
#include <QFrame>

extern "C" {
#include "lua/lundump.h"
#include "lua/lobject.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lua/lopcodes.h"
}

Q_DECLARE_METATYPE(Instruction*);

EditorWindow::EditorWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::EditorWindow)
{
	ui->setupUi(this);
}

EditorWindow::~EditorWindow()
{
	delete ui;
}

void EditorWindow::init(Proto *pr)
{
	QStandardItemModel *model = new QStandardItemModel(pr->sizecode, 7);

	for (int i = 0; i < pr->sizecode; i++) {
		Instruction instr = pr->code[i];
		QStandardItem *code = new QStandardItem(
					QString(luaP_opnames[GET_OPCODE( instr )])
			);
		model->setItem(i, 0, code);

		QStandardItem *opCode = new QStandardItem( QString("%1").arg(GET_OPCODE(instr)) );
		QStandardItem *argA = new QStandardItem( QString("%1").arg(GETARG_A(instr)) );
		QStandardItem *argB = new QStandardItem( QString("%1").arg(GETARG_B(instr)) );
		QStandardItem *argC = new QStandardItem( QString("%1").arg(GETARG_C(instr)) );
		QStandardItem *argBc = new QStandardItem( QString("%1").arg(GETARG_Bx(instr)) );
		QStandardItem *argsBc = new QStandardItem( QString("%1").arg(GETARG_sBx(instr)) );

		model->setItem(i, 1, opCode);
		model->setItem(i, 2, argA);
		model->setItem(i, 3, argB);
		model->setItem(i, 4, argC);
		model->setItem(i, 5, argBc);
		model->setItem(i, 6, argsBc);
	}

	connect(model, &QStandardItemModel::itemChanged, [this](QStandardItem *item) {

	});

	this->ui->tableView->setModel(model);
	this->ui->tableView->setColumnWidth(0, 70);
	this->ui->tableView->setColumnWidth(1, 10);
	this->ui->tableView->setColumnWidth(2, 10);
	this->ui->tableView->setColumnWidth(3, 10);
	this->ui->tableView->setColumnWidth(4, 10);
	this->ui->tableView->setColumnWidth(5, 10);
	this->ui->tableView->setColumnWidth(6, 10);
}
