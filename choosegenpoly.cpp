#include "choosegenpoly.h"
#include "ui_choosegenpoly.h"

ChooseGenPoly::ChooseGenPoly(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseGenPoly)
{
    ui->setupUi(this);
}

ChooseGenPoly::~ChooseGenPoly()
{
    delete ui;
}
