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
    crc = new CRC;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete crc;
    delete chooseGenPolyDialog;
}

void MainWindow::on_button_Encode_clicked()
{
    bool isValidR;

    const std::string message = ui->inputEncode_Codeword->text().toStdString();
    const size_t k = message.length();
    size_t r = ui->inputEncode_RDegree->text().toUInt(&isValidR, 10);

    if (std::regex_search(message, std::regex("[^1|0]"), std::regex_constants::match_any) || k == 0)
        ErrorPopUp("You must insert message in a binary format only!");
    else if (!isValidR)
        ErrorPopUp("Degree of generating polynomial must be a positive integer!");
    else
    {
        if (r == 0)
        {
            try {
                r = crc->FindHammingCode(k);
            }  catch (const std::exception &e) {
                ErrorPopUp(e.what());
                return;
            }
        }

        try {
            generatingPolynomials = {};
            generatingPolynomials = crc->ReturnGeneratingPolynomials(k, r);
        }  catch (const std::exception &e) {
            ErrorPopUp(e.what());
            return;
        }

        chooseGenPolyDialog = new QDialog;
        genPolyLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom, chooseGenPolyDialog);
        QPushButton *acceptGenPoly = new QPushButton("Ok", chooseGenPolyDialog);

        genPolyRBtnsPolys = {};

        chooseGenPolyDialog->setWindowTitle("Found generating polynomials");
        connect(acceptGenPoly, &QPushButton::released, this, &MainWindow::GenPolyAccepted);
        genPolyLayout->addWidget(new QLabel("Please choose a generating polynomial: "));

        for (const auto &poly: generatingPolynomials)
        {
            QRadioButton *temp = new QRadioButton(QString::fromStdString(crc->PrettifyPolynomialVector(poly) + " (" + std::string(poly.begin(), poly.end()) + ")"),
                                                  chooseGenPolyDialog);
            genPolyLayout->addWidget(temp);
            genPolyRBtnsPolys.push_back(temp);
        }
        genPolyRBtnsPolys[0]->setChecked(true);
        genPolyLayout->addWidget(acceptGenPoly);
        chooseGenPolyDialog->exec();
        ui->outputEncode_Encoded->setText(QString::fromStdString(crc->Encode(message, *genPoly)));

    }
}

void MainWindow::GenPolyAccepted()
{
    for (size_t i = 0; i < genPolyRBtnsPolys.size(); i++)
    {
        bool chckd = genPolyRBtnsPolys[i]->isChecked();
        if (chckd)
        {
            genPoly = &generatingPolynomials[i];
            break;
        }
        delete genPolyRBtnsPolys[i];
    }

    chooseGenPolyDialog->accept();
}

void MainWindow::on_button_Decode_clicked()
{
    const std::string message = ui->inputDecode_Codeword->text().toStdString();
    const QString QgenPoly = ui->inputDecode_GeneratingPolynomial->text();
    const size_t n = message.length();
    const size_t r = QgenPoly.length();

    if (std::regex_search(message, std::regex("[^1|0]"), std::regex_constants::match_any) || std::regex_search(QgenPoly.toStdString(), std::regex("[^1|0]"), std::regex_constants::match_any) || n == 0 || r == 0)
        ErrorPopUp("You must insert message and generating polynomial in a binary format only!");
    else if (r > n)
        ErrorPopUp("Generating polynomial cannot be longer than encoded message!");
    else
    {
        Polynomial l_genPoly = crc->ConvertToVector(QgenPoly.toStdString());

        try
        {
            std::tuple<std::string, bool, size_t> decodedTuple = crc->Decode(message, l_genPoly);
            const std::string correctedMessage = std::get<0>(decodedTuple);
            const bool error = std::get<1>(decodedTuple);
            const std::string errorPos = std::to_string(std::get<2>(decodedTuple));

            const std::string decodedMessage = correctedMessage.substr(0, n - r + 1);
            std::string additionalInfo;

            if (error)
            {
                additionalInfo = "Decoded message contains an error at position x<sup>" + errorPos + "</sup>.<br>" +
                        "<br>It was corrected from:<br>&nbsp;&nbsp;&nbsp;&nbsp;" + message +
                        "<br>To:<br>&nbsp;&nbsp;&nbsp;&nbsp;" + correctedMessage;
            }
            else
            {
                additionalInfo = "Decoded message does not contain any errors.";
            }

            ui->outputDecode_Decoded->setText(QString::fromStdString(decodedMessage));
            ui->outputDecode_AdditionalInfo->setText(QString::fromStdString(additionalInfo));
        }
        catch (const std::exception &e)
        {
            ErrorPopUp(e.what());
            return;
        }

    }
}

