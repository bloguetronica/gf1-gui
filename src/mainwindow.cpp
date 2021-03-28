/* GF1 GUI - Version 2.0 for Debian Linux
   Copyright (c) 2017-2018 Samuel Lourenço

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation, either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <https://www.gnu.org/licenses/>.


   Please feel free to contact me via e-mail: samuel.fmlourenco@gmail.com */


// Includes
#include <QProcess>
#include "about.h"
#include "mainwindow.h"
#include "serial.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButtonClear->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    About about;
    about.exec();
}

void MainWindow::on_actionSerial_triggered()
{
    Serial serial;
    serial.setSerialLineEditText(serialstr_);
    if (serial.exec() == QDialog::Accepted)
    {
        serialstr_ = serial.serialLineEditText();
        if (serialstr_ == "")
            this->setWindowTitle("GF1 Function Generator");
        else
            this->setWindowTitle("GF1 Function Generator (S/N: " + serialstr_ + ")");
    }
}

void MainWindow::on_checkBoxZload_stateChanged()
{
    if (ui->checkBoxZload->isChecked())
    {
        ui->doubleSpinBoxAmp->setValue(ui->doubleSpinBoxAmp->value() / 2);
        ui->doubleSpinBoxAmp->setSingleStep(0.05);
        ui->doubleSpinBoxAmp->setMaximum(4);
    }
    else
    {
        ui->doubleSpinBoxAmp->setMaximum(8);
        ui->doubleSpinBoxAmp->setSingleStep(0.1);
        ui->doubleSpinBoxAmp->setValue(ui->doubleSpinBoxAmp->value() * 2);
    }
}

void MainWindow::on_comboBoxWave_currentIndexChanged(int index)
{
    if (ui->actionAutoApply->isChecked())
    {
        if(index == 0)
            execute(("gf1-sine " + serialstr_).simplified());
        else if (index == 1)
            execute(("gf1-tri " + serialstr_).simplified());
    }
}

void MainWindow::on_doubleSpinBoxAmp_valueChanged(const QString &arg)
{
    if (ui->actionAutoApply->isChecked())
    {
        QString zload;
        if (ui->checkBoxZload->isChecked())
            zload = "50";
        execute(("gf1-amp" + zload + " " + arg + " " + serialstr_).simplified());
    }
}

void MainWindow::on_doubleSpinBoxFreq_valueChanged(const QString &arg)
{
    if (ui->actionAutoApply->isChecked())
    {
        execute(("gf1-freq " + arg + " " + serialstr_).simplified());
    }
}

void MainWindow::on_pushButtonApply_clicked()
{
    QString amp, command, freq, wave, zload;
    amp = QString::number(ui->doubleSpinBoxAmp->value());
    freq = QString::number(ui->doubleSpinBoxFreq->value());
    if (ui->comboBoxWave->currentIndex() == 0)
        wave = "sine";
    else if (ui->comboBoxWave->currentIndex() == 1)
        wave = "tri";
    if (ui->checkBoxZload->isChecked())
        zload = "50";
    execute(("gf1-" + wave + " " + serialstr_ + " && gf1-freq " + freq + " " + serialstr_ + " && gf1-amp" + zload + " "  + amp + " " + serialstr_).simplified());
}

void MainWindow::on_pushButtonClear_clicked()
{
    execute(("gf1-clear " + serialstr_).simplified());
}

void MainWindow::on_pushButtonReset_clicked()
{
    if (ui->actionResetClear->isChecked())
        execute(("gf1-reset " + serialstr_ + " && sleep 2 && gf1-clear " + serialstr_).simplified());
    else
        execute(("gf1-reset " + serialstr_).simplified());;
}

void MainWindow::on_pushButtonStart_clicked()
{
    execute(("gf1-start " + serialstr_).simplified());
}

void MainWindow::on_pushButtonStop_clicked()
{
    execute(("gf1-stop " + serialstr_).simplified());
}

void MainWindow::execute(const QString &command)
{
    QProcess sh;
    ui->textBrowserCommand->append("$ " + command);
    sh.setProcessChannelMode(QProcess::MergedChannels);
    sh.start("sh", QStringList() << "-c" << command);
    sh.waitForFinished();
    QString result = sh.readAll();
    result.chop(1);
    ui->textBrowserCommand->append(result);
    sh.close();
}
