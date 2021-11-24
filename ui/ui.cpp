#include "ui.h"
#include "ui_ui.h"

ui::ui(QWidget *parent)
    : QMainWindow(parent)
    , ui_p(new Ui::ui)
{
    ui_p->setupUi(this);
}

ui::~ui()
{
    delete ui_p;
}

