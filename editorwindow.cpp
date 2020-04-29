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

enum Op {
	opCode,
	argA,
	argB,
	argC,
	argBc,
	argsBc
};

struct ItemOp {
	Op op;
	Instruction *instr;
};

Q_DECLARE_METATYPE(ItemOp);

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

		opCode->setData(QVariant::fromValue(ItemOp{Op::opCode, &pr->code[i]}));
		argA->setData(QVariant::fromValue(ItemOp{Op::argA, &pr->code[i]}));
		argB->setData(QVariant::fromValue(ItemOp{Op::argB, &pr->code[i]}));
		argC->setData(QVariant::fromValue(ItemOp{Op::argC, &pr->code[i]}));
		argBc->setData(QVariant::fromValue(ItemOp{Op::argBc, &pr->code[i]}));
		argsBc->setData(QVariant::fromValue(ItemOp{Op::argsBc, &pr->code[i]}));

		model->setItem(i, 1, opCode);
		model->setItem(i, 2, argA);
		model->setItem(i, 3, argB);
		model->setItem(i, 4, argC);
		model->setItem(i, 5, argBc);
		model->setItem(i, 6, argsBc);
	}

	connect(model, &QStandardItemModel::itemChanged, [](QStandardItem *item) {
		ItemOp itemOp = item->data().value<ItemOp>();
		switch (itemOp.op) {
			case Op::opCode:	SET_OPCODE(*itemOp.instr, item->text().toInt()); break;
			case Op::argA:		SETARG_A(*itemOp.instr, item->text().toInt()); break;
			case Op::argB:		SETARG_B(*itemOp.instr, item->text().toInt()); break;
			case Op::argC:		SETARG_C(*itemOp.instr, item->text().toInt()); break;
			case Op::argBc:		SETARG_Bx(*itemOp.instr, item->text().toInt()); break;
		default: break;
		}
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
