#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QPushButton>

#include <regex>
#include "choosegenpoly.h"
#include "src/crc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_Encode_clicked();

private:
    Ui::MainWindow *ui;
    CRC crc;
};
#endif // MAINWINDOW_H
