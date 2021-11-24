#ifndef UI_H
#define UI_H

#include <QMainWindow>
#include <QDialog>
#include "..\src\crc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ui; }
QT_END_NAMESPACE

class ui : public QMainWindow
{
    Q_OBJECT

public:
    ui(QWidget *parent = nullptr);
    ~ui();

private:
    Ui::ui *ui_p;
};
#endif // UI_H
