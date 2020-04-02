#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStringList>
#include <QArrayData>
#include <QTextStream>
#include <QFileInfo>
#include <QFile>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QFont>
#include <QIcon>
#include <QDateTime>
#include <QTextEdit>

QStringList fileList;
QStringList fileNames;
QStringList filePaths;
QStringList fileBodies;
QStringList fileThread;

QString lastDir = "D:\\";
QString orcaDir;
QString filter = "Orca input files (*.inp) ;; All files (*.*)";

int fileCounter = 0;
int maxThreads = 16;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("OrcaLauncher");
    QIcon *programIcon = new QIcon("C:/Users/Dima/Documents/OrcaLauncher/programIcon.png");
    setWindowIcon(*programIcon);

    ui->tableWidget->setColumnWidth(0, 229);
    ui->tableWidget->setColumnWidth(1, 100);

    ui->plainTextEdit->setPlainText("\nPlease select an input file in the left window (Input files queue) to start editing");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::launchProgram()
{
    for (int i = 0; i < fileList.size(); ++i)
    {
        QString outFilePath = filePaths.at(i) + "/" + fileNames.at(i) + ".out";

        QProcess *process = new QProcess(this);

        QStringList arguments;
            arguments.append(fileNames.at(i) + ".inp");

        process->setArguments(arguments);
        process->setProgram(orcaDir);
        process->setWorkingDirectory(filePaths.at(i));

        QFile outFile(outFilePath);
        outFile.open(QFile::WriteOnly);
        QTextStream fout(&outFile);

        QDateTime current = QDateTime::currentDateTime();
        QString currentDateTime = current.toString("dd.MM.yyyy HH:mm:ss");
        fout << "This file was created by OrcaLauncher\n";
        fout << currentDateTime;

        process->setProcessChannelMode(QProcess::MergedChannels);
        process->start();

        while(process->waitForReadyRead())
            fout << process->readAll();

        outFile.flush();
        outFile.close();

        ui->statusBar->showMessage(fileNames.at(i) + " done!");
    }


}

void MainWindow::on_pushButton_clicked()                                                            // клик по кнопке Load Files
{
    QStringList loaded_file_names = QFileDialog::getOpenFileNames(this, "Select files", lastDir, filter);    // записываем имена+пути открытых файлов
    fileList.append(loaded_file_names);                                                                     // добавляем имена+пути открытых файлов в общий список

    if (loaded_file_names.isEmpty())
        return;

    ui->tableWidget->setRowCount(fileList.size());                                                          // создаем строки в таблице по количеству открытых файлов

    for (fileCounter; fileCounter < fileList.size(); ++fileCounter)                                 // главный цикл
    {
        QFileInfo fileInfo(fileList.at(fileCounter));                                                       //
        fileNames.append(fileInfo.baseName());                                                              // разделяем имя файла и его путь по отдельным спискам
        filePaths.append(fileInfo.absolutePath());                                                          //

        QFile file(fileList.at(fileCounter));
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream in(&file);
        QString text = in.readAll();
        fileBodies.append(text);
        file.close();

        int i = 1;
        while(!(text.contains(QString("PAL%1").arg(i)) || (text.contains(QString("nprocs %1\n").arg(i)) || text.contains(QString("nprocs %1 ").arg(i)))))
        {
            i++;
            if (i > maxThreads) break;
        }

        if (i == maxThreads+1)
            i = 1;

        fileThread.append(QString::number(i));

        //QListWidgetItem *listitem = new QListWidgetItem(fileNames.at(fileCounter));
        QTableWidgetItem *nametableitem = new QTableWidgetItem(fileNames.at(fileCounter));
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(fileCounter));

        ui->tableWidget->setItem(fileCounter, 0, nametableitem);
        ui->tableWidget->setItem(fileCounter, 1, threadstableitem);
        //ui->listWidget->addItem(listitem);

        ui->tableWidget->setRowHeight(fileCounter, 25);
    }

    ui->tableWidget->setColumnWidth(0, 214);                                                        // подстраиваем ширины колонок таблицы
    ui->tableWidget->setColumnWidth(1, 100);                                                        //

    lastDir = filePaths.last();                                                                     // запоминаем директорию последнего файла для удобства
}

void MainWindow::on_pushButton_2_clicked()                                                          // клик по кнопке Delete
{
    fileList.removeAt(ui->tableWidget->currentRow());
    fileNames.removeAt(ui->tableWidget->currentRow());
    filePaths.removeAt(ui->tableWidget->currentRow());
    fileBodies.removeAt(ui->tableWidget->currentRow());
    fileThread.removeAt(ui->tableWidget->currentRow());
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    ui->plainTextEdit->clear();
    fileCounter--;
}

void MainWindow::on_pushButton_3_clicked()                                                          // клик по кнопке Clear all
{
    fileList.clear();
    fileNames.clear();
    filePaths.clear();
    fileBodies.clear();
    fileThread.clear();
    ui->plainTextEdit->clear();

    for (int i = fileCounter; i >=0; i--)
    {
        ui->tableWidget->removeRow(i);
    }

    fileCounter = 0;
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)                              // двойной клик по ячейке в таблице
{
    ui->plainTextEdit->setPlainText(fileBodies.at(row));
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)                                    // клик по ячейке в таблице
{
    ui->plainTextEdit->setPlainText(fileBodies.at(row));
}

void MainWindow::on_pushButton_4_clicked()                                                          // клик по кнопке Move UP
{
    int current_row = ui->tableWidget->currentRow();

    if (current_row > 0)
    {
        fileList.move(current_row, current_row-1);
        fileNames.move(current_row, current_row-1);
        fileThread.move(current_row, current_row-1);

        for (int i = 0; i < fileCounter; i++)
        {
            QTableWidgetItem *nametableitem = new QTableWidgetItem(fileNames.at(i));
            QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(i));

            ui->tableWidget->setItem(i, 0, nametableitem);
            ui->tableWidget->setItem(i, 1, threadstableitem);
        }

        ui->tableWidget->selectRow(current_row-1);
    }

}

void MainWindow::on_pushButton_5_clicked()                                                          // клик по кнопке Move DOWN
{
    int current_row = ui->tableWidget->currentRow();

    if (current_row < fileCounter-1)
    {
        fileList.move(current_row, current_row+1);
        fileNames.move(current_row, current_row+1);
        fileThread.move(current_row, current_row+1);

        for (int i = 0; i < fileCounter; i++)
        {
            QTableWidgetItem *nametableitem = new QTableWidgetItem(fileNames.at(i));
            QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(i));

            ui->tableWidget->setItem(i, 0, nametableitem);
            ui->tableWidget->setItem(i, 1, threadstableitem);
        }

        ui->tableWidget->selectRow(current_row+1);
    }
}

void MainWindow::on_actionSet_Orca_directory_triggered()
{
    orcaDir = QFileDialog::getOpenFileName(this, "Set path to orca.exe", "D:\\", "Executable files *.exe");

    // save path to config file
}

void MainWindow::on_pushButton_9_clicked()                                                          // клик по книпке RUN
{
    if (!fileList.length())
    {
        QMessageBox::warning(this, "", "Queue is empty");
        return;
    }

    if (orcaDir.length())
    {
        if (orcaDir.contains("orca.exe"))
        {
            ui->statusBar->showMessage("run");
            launchProgram();
        }
        else
            QMessageBox::warning(this, "", "Please specify the path to orca.exe in the settings");
    }
    else
        QMessageBox::warning(this, "", "Please specify the path to orca.exe in the settings");
}

void MainWindow::on_pushButton_6_clicked()                                                          // клик по кнопке Save
{
    int current_row = ui->tableWidget->currentRow();

    if (current_row > -1)
    {
        QFile file(fileList.at(current_row));
        file.open(QFile::WriteOnly);
        QTextStream out(&file);
        QString actualText = ui->plainTextEdit->toPlainText();
        fileBodies.replace(current_row, actualText);
        out << actualText;
        file.flush();
        file.close();

        int i = 1;
        while(!(actualText.contains(QString("PAL%1").arg(i)) || (actualText.contains(QString("nprocs %1\n").arg(i)) || actualText.contains(QString("nprocs %1 ").arg(i)))))
        {
            i++;
            if (i > maxThreads) break;
        }

        fileThread.replace(current_row, QString::number(i));
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(current_row));
        ui->tableWidget->setItem(current_row, 1, threadstableitem);
    }
    else
        QMessageBox::warning(this, "", "There is no file to save");
}

void MainWindow::on_pushButton_8_clicked()                                                          // клик по кнопке Save as new
{
    QString actualText = ui->plainTextEdit->toPlainText();

    QString savingFileName = QFileDialog::getSaveFileName(this, "Save file as", lastDir, filter);

    if(savingFileName.isEmpty() || savingFileName.isNull())
        return;

    QFile file(savingFileName);
    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    out << actualText;
    file.flush();
    file.close();

    fileList.append(savingFileName);
    fileBodies.append(actualText);

    QFileInfo fileInfo(fileList.at(fileCounter));
    fileNames.append(fileInfo.baseName());
    filePaths.append(fileInfo.absolutePath());

    int i = 1;
    while(!(actualText.contains(QString("PAL%1").arg(i)) || (actualText.contains(QString("nprocs %1\n").arg(i)) || actualText.contains(QString("nprocs %1 ").arg(i)))))
    {
        i++;
        if (i > maxThreads) break;
    }
    fileThread.append(QString::number(i));

    ui->tableWidget->setRowCount(fileList.size());

    QTableWidgetItem *nametableitem = new QTableWidgetItem(fileNames.at(fileCounter));
    QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(fileCounter));

    ui->tableWidget->setItem(fileCounter, 0, nametableitem);
    ui->tableWidget->setItem(fileCounter, 1, threadstableitem);

    ui->tableWidget->setRowHeight(fileCounter, 25);
    ui->tableWidget->selectRow(fileCounter);

    fileCounter++;
}
