/* GF1 GUI - Version 1.0 for Debian Linux
   Copyright (c) 2017 Samuel Lourenço

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Please feel free to contact me via e-mail: samuel.fmlourenco@gmail.com */


// Includes
#include <QProcess>
#include "about.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

void MainWindow::on_comboBoxWave_currentIndexChanged(int index)
{
    QProcess sh;
    QString command;
    if (ui->actionAutoApply->isChecked())
    {
        if(index == 0)
            command = "gf1-sine";
        else if (index == 1)
            command = "gf1-tri";
        ui->textBrowserCommand->append("$ " + command);
        sh.setProcessChannelMode(QProcess::MergedChannels);
        sh.start("sh", QStringList() << "-c" << command);
        sh.waitForFinished();
        ui->textBrowserCommand->append(sh.readAll());
        sh.close();
    }
}

void MainWindow::on_doubleSpinBoxFreq_valueChanged(const QString &arg)
{
    QProcess sh;
    QString command;
    if (ui->actionAutoApply->isChecked())
    {
        command = "gf1-freq " + arg;
        ui->textBrowserCommand->append("$ " + command);
        sh.setProcessChannelMode(QProcess::MergedChannels);
        sh.start("sh", QStringList() << "-c" << command);
        sh.waitForFinished();
        ui->textBrowserCommand->append(sh.readAll());
        sh.close();
    }
}

void MainWindow::on_doubleSpinBoxAmp_valueChanged(const QString &arg)
{
    QProcess sh;
    QString command;
    if (ui->actionAutoApply->isChecked())
    {
        command = "gf1-amp " + arg;
        ui->textBrowserCommand->append("$ " + command);
        sh.setProcessChannelMode(QProcess::MergedChannels);
        sh.start("sh", QStringList() << "-c" << command);
        sh.waitForFinished();
        ui->textBrowserCommand->append(sh.readAll());
        sh.close();
    }
}

void MainWindow::on_pushButtonApply_clicked()
{
    QProcess sh;
    QString amp, command, freq, wave;
    amp = QString::number(ui->doubleSpinBoxAmp->value());
    freq = QString::number(ui->doubleSpinBoxFreq->value());
    if (ui->comboBoxWave->currentIndex() == 0)
        wave = "sine";
    else if (ui->comboBoxWave->currentIndex() == 1)
        wave = "tri";
    command = "gf1-" + wave + "&&gf1-freq " + freq + "&&gf1-amp " + amp;
    ui->textBrowserCommand->append("$ " + command);
    sh.setProcessChannelMode(QProcess::MergedChannels);
    sh.start("sh", QStringList() << "-c" << command);
    sh.waitForFinished();
    ui->textBrowserCommand->append(sh.readAll());
    sh.close();
}

void MainWindow::on_pushButtonClear_clicked()
{
    QProcess sh;
    ui->textBrowserCommand->append("$ gf1-clear");
    sh.setProcessChannelMode(QProcess::MergedChannels);
    sh.start("sh", QStringList() << "-c" << "gf1-clear");
    sh.waitForFinished();
    ui->textBrowserCommand->append(sh.readAll());
    sh.close();
}

void MainWindow::on_pushButtonReset_clicked()
{
    QProcess sh;
    QString command;
    if (ui->actionResetClear->isChecked())
        command = "gf1-reset&&sleep 1&&gf1-clear";
    else
        command = "gf1-reset";
    ui->textBrowserCommand->append("$ " + command);
    sh.setProcessChannelMode(QProcess::MergedChannels);
    sh.start("sh", QStringList() << "-c" << command);
    sh.waitForFinished();
    ui->textBrowserCommand->append(sh.readAll());
    sh.close();
}

void MainWindow::on_pushButtonStart_clicked()
{
    QProcess sh;
    ui->textBrowserCommand->append("$ gf1-start");
    sh.setProcessChannelMode(QProcess::MergedChannels);
    sh.start("sh", QStringList() << "-c" << "gf1-start");
    sh.waitForFinished();
    ui->textBrowserCommand->append(sh.readAll());
    sh.close();
}

void MainWindow::on_pushButtonStop_clicked()
{
    QProcess sh;
    ui->textBrowserCommand->append("$ gf1-stop");
    sh.setProcessChannelMode(QProcess::MergedChannels);
    sh.start("sh", QStringList() << "-c" << "gf1-stop");
    sh.waitForFinished();
    ui->textBrowserCommand->append(sh.readAll());
    sh.close();
}
