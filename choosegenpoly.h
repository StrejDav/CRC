#ifndef CHOOSEGENPOLY_H
#define CHOOSEGENPOLY_H

#include <QDialog>

namespace Ui {
class ChooseGenPoly;
}

class ChooseGenPoly : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseGenPoly(QWidget *parent = nullptr);
    ~ChooseGenPoly();

private:
    Ui::ChooseGenPoly *ui;
};

#endif // CHOOSEGENPOLY_H
