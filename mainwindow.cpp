#include "mainwindow.h"
#include "ui_mainwindow.h"

void ErrorPopUp(const QString message)
{
    QMessageBox errorBox;
    errorBox.setText(message);
    errorBox.setFixedSize(500,200);
    errorBox.exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(1000, 600));
    crc = CRC();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_Encode_clicked()
{
    bool isPositiveNumber;
    bool genPolyFound;

    const std::string message = ui->inputEncode_Codeword->text().toStdString();
    const size_t k = message.length();
    size_t r = ui->inputEncode_RDegree->text().toUInt(&isPositiveNumber, 10);

    if (std::regex_search(message, std::regex("[^1|0]"), std::regex_constants::match_any))
        ErrorPopUp("You must insert message only in a binary format!");
    else if (!isPositiveNumber)
        ErrorPopUp("Degree of generating polynomial must be a positive integer!");
    else
    {
        if (r == 0)
        {
            try {
                r = crc.FindHammingCode(k);
            }  catch (const std::exception &e) {
                ErrorPopUp(e.what());
                return;
            }
        }

        std::vector<Polynomial> generatingPolynomials;

        try {
            generatingPolynomials = crc.ReturnGeneratingPolynomials(k, r, genPolyFound);
        }  catch (const std::exception &e) {
            ErrorPopUp(e.what());
            return;
        }


        QDialog chooseGenPolyDialog;
        chooseGenPolyDialog.setWindowTitle("Found generating polynomials");
        //chooseGenPolyDialog.setModal(true);
        QBoxLayout radioBtns(QBoxLayout::Direction::TopToBottom, &chooseGenPolyDialog);
        QPushButton *acceptGenPoly = new QPushButton("Ok");

        radioBtns.addWidget(new QLabel("Please choose a generating polynomial: "));

        std::vector<QRadioButton *> radioBtnsPolys;
        for (const auto &poly: generatingPolynomials)
        {
            QRadioButton *temp = new QRadioButton(QString::fromStdString(crc.PrettifyPolynomialVector(poly) + " (" + std::string(poly.begin(), poly.end()) + ")"),
                                                  &chooseGenPolyDialog);
            radioBtns.addWidget(temp);
            radioBtnsPolys.push_back(temp);
        }
        radioBtns.addWidget(acceptGenPoly);
        chooseGenPolyDialog.exec();
    }
}
