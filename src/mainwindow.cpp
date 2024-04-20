/* GF1 GUI - Version 3.1 for Debian Linux
   Copyright (c) 2017-2024 Samuel Lourenço

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
#include <QFileDialog>
#include <QMessageBox>
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
    filepath_ = QDir::homePath();
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

void MainWindow::on_actionLoad_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load Settings from File"), filepath_, tr("XML files (*.xml);;All files (*)"));
    if (!filename.isEmpty())  // Note that the previous dialog will return an empty string if the user cancels it
    {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not read from %1.\n\nPlease verify that you have read access to this file.").arg(QDir::toNativeSeparators(filename)));
        }
        else
        {
            QDomDocument document;
            bool error = !document.setContent(&file);
            file.close();
            if (error)
            {
                QMessageBox::critical(this, tr("Error"), tr("Invalid XML. The file might be corrupted or incomplete."));
            }
            else
            {
                QDomElement root = document.firstChildElement();
                if (root.tagName() != "settings" || root.attribute("target") != "GF1")
                {
                    QMessageBox::critical(this, tr("Error"), tr("The selected file is not a GF1 settings file."));
                }
                else
                {
                    int errorcount = implementSettings(root);
                    if (errorcount > 0)
                    {
                        QMessageBox::warning(this, tr("Warning"), tr("Found %1 setting(s) with invalid attribute values.\n\nPlease check your settings file for out-of-bounds or unsupported attribute values.").arg(errorcount));
                    }
                    filepath_ = filename;
                }
            }
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Settings to File"), filepath_, tr("XML files (*.xml);;All files (*)"));
    if (!filename.isEmpty())  // Note that the previous dialog will return an empty string if the user cancels it
    {
        QDomDocument document;
        QDomProcessingInstruction instruction = document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
        document.appendChild(instruction);
        QDomElement root = document.createElement("settings");
        root.setAttribute("target", "GF1");
        document.appendChild(root);
        if (ui->comboBoxWave->currentIndex() == 0)
            appendSetting(document, root, "waveform", "sinusoidal");
        else if (ui->comboBoxWave->currentIndex() == 1)
            appendSetting(document, root, "waveform", "triangular");
        appendSetting(document, root, "frequency", QString::number(ui->doubleSpinBoxFreq->value()));
        appendSetting(document, root, "amplitude", QString::number(ui->doubleSpinBoxAmp->value()));
        if (ui->checkBoxZload->isChecked())
            appendSetting(document, root, "50r_load", "on");
        else
            appendSetting(document, root, "50r_load", "off");
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not write to %1.\n\nPlease verify that you have write access to this file.").arg(QDir::toNativeSeparators(filename)));
        }
        else
        {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << document.toString();
            file.close();
            filepath_ = filename;
        }
    }
}

void MainWindow::on_actionSerial_triggered()
{
    Serial serial;
    serial.setSerialLineEditText(serialstr_);
    if (serial.exec() == QDialog::Accepted) {
        QString title = tr("GF1 Function Generator");
        serialstr_ = serial.serialLineEditText();
        if (!serialstr_.isEmpty()) {
            title.append(tr(" (S/N: %1)").arg(serialstr_));
        }
        this->setWindowTitle(title);
    }
}

void MainWindow::on_checkBoxZload_stateChanged()
{
    if (ui->checkBoxZload->isChecked())
    {
        ui->doubleSpinBoxAmp->setValue(ui->doubleSpinBoxAmp->value() / 2);
        ui->doubleSpinBoxAmp->setSingleStep(0.05);
        ui->doubleSpinBoxAmp->setMaximum(2.5);
    }
    else
    {
        ui->doubleSpinBoxAmp->setMaximum(5);
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
    QString amp, freq, wave, zload;
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

void MainWindow::appendSetting(QDomDocument &document, QDomElement &parent, const QString &name, const QString &value)
{
    QDomElement element = document.createElement("setting");
    element.setAttribute("value", value);
    element.setAttribute("name", name);
    parent.appendChild(element);
}

int MainWindow::implementSettings(const QDomElement &parent)
{
    QDomNodeList settings = parent.elementsByTagName("setting");
    int  cnt = settings.count(), errcnt = 0;
    for (int i = 0; i < cnt; ++i)  // Modifier settings
    {
        QDomNode node = settings.item(i);
        if (node.isElement())
        {
            QDomElement element = node.toElement();
            if(element.attribute("name").toLower() == "50r_load")
            {
                QString value = element.attribute("value").toLower();
                if (value == "on")
                    ui->checkBoxZload->setChecked(true);
                else if (value == "off")
                    ui->checkBoxZload->setChecked(false);
                else
                    ++errcnt;
            }
        }
    }
    for (int i = 0; i < cnt; ++i)  // Normal settings
    {
        QDomNode node = settings.item(i);
        if (node.isElement())
        {
            QDomElement element = node.toElement();
            QString name = element.attribute("name").toLower();
            if(name == "amplitude")
            {
                bool ok;
                double value = element.attribute("value").toDouble(&ok);
                if (ok && value >= ui->doubleSpinBoxAmp->minimum() && value <= ui->doubleSpinBoxAmp->maximum())
                    ui->doubleSpinBoxAmp->setValue(value);
                else
                    ++errcnt;
            }
            else if(name == "frequency")
            {
                bool ok;
                double value = element.attribute("value").toDouble(&ok);
                if (ok && value >= ui->doubleSpinBoxFreq->minimum() && value <= ui->doubleSpinBoxFreq->maximum())
                    ui->doubleSpinBoxFreq->setValue(value);
                else
                    ++errcnt;
            }
            else if(name == "waveform")
            {
                QString value = element.attribute("value").toLower();
                if (value == "sinusoidal")
                    ui->comboBoxWave->setCurrentIndex(0);
                else if (value == "triangular")
                    ui->comboBoxWave->setCurrentIndex(1);
                else
                    ++errcnt;
            }
        }
    }
    return errcnt;
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
