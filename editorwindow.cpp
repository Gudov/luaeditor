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
#include "lua/ltm.h"
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

struct ItemConst {
	TValue *c;
	QStandardItem *typeItem;
};

Q_DECLARE_METATYPE(ItemOp);
Q_DECLARE_METATYPE(ItemConst);

void EditorWindow::init(Proto *pr)
{
	this->initCode(pr);
	this->initConst(pr);
}

void EditorWindow::initCode(Proto *pr)
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


void EditorWindow::initConst(Proto *pr)
{
	QStandardItemModel *model = new QStandardItemModel(pr->sizecode, 7);
	for (int i = 0; i < pr->sizek; i++) {
		TValue *v = &pr->k[i];
		QStandardItem *type = new QStandardItem( lua_typename(nullptr, v->tt) );
		QStandardItem *value = new QStandardItem;
		QString valueText;
		switch(v->tt) {
			case LUA_TNUMBER: valueText = QString("%L1").arg(v->value.n); break;
			case LUA_TSTRING: valueText = QString(getstr((TString*)v->value.p)); break;
			case LUA_TBOOLEAN: valueText = QString("%1").arg(v->value.b, 16); break;
		default:
			valueText = "wrong type";
		}
		value->setText(valueText);

		if (valueText != "wrong type") {
			value->setData(QVariant::fromValue(ItemConst{v, type}));
		} else {
			value->setData(QVariant::fromValue(ItemConst{nullptr, nullptr}));
		}
		type->setData(QVariant::fromValue(ItemConst{nullptr, nullptr}));

		model->setItem(i, 0, type);
		model->setItem(i, 1, value);
	}

	connect(model, &QStandardItemModel::itemChanged, this, [](QStandardItem *item) {
		if(item->data().value<ItemConst>().typeItem == nullptr) {
			return;
		}

		ItemConst itConst = item->data().value<ItemConst>();

		int type = -1;
		for(int i = 0; i < 7; i++) {
			if( itConst.typeItem->text() == luaT_typenames[i] ) {
				type = i;
				break;
			}
		}

		if (type == -1) {
			itConst.typeItem->setText("type error");
			return;
		}

		switch(type) {
		case LUA_TNUMBER: setnvalue(itConst.c, item->text().toFloat()); break;
		case LUA_TSTRING: {
				QString *s = new QString(item->text());
				itConst.c->tt = LUA_TSTRING;
				itConst.c->value.p = s->toUtf8().data();
			};
			break;
		case LUA_TBOOLEAN:
			itConst.c->tt = LUA_TBOOLEAN;
			itConst.c->value.b = item->text().toUInt(nullptr, 16);
			break;
		}
	});

	this->ui->tableViewConst->setModel(model);
}
