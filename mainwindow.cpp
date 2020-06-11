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
#include <QFont>
#include <QIcon>
#include <QDateTime>
#include <QTextEdit>
#include <QThread>

QStringList fileList;
QStringList fileNames;
QStringList filePaths;
QStringList fileBodies;
QStringList fileThread;

QString lastDir;
QString orcaDir;
QString sublDir;
QString filter = "Orca input files (*.inp) ;; All files (*.*)";

QString aboutProgramText = "OrcaLauncher v1.1.2\n\n"
                           "This is an open source project designed to simplify commutication with ORCA quantum chemistry package\n\n"
                           "Author: Dmitry Dulov, dulov.dmitry@gmail.com";

int fileCounter = 0;
int maxThreads = 16;

qint64 pid;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    loadSettings();

    launcher = new OrcaLauncher();                                                                  // создаю объект класса OrcaLauncher
    connect(this, SIGNAL(orcaLauncherSignal()),                                                     // связываю сигнал класса MainWindow со слотом класса OrcaLauncher, который запускает orca.exe
            launcher, SLOT(launchProgram()));                                                       //
    connect(launcher, SIGNAL(programIsFinished()),
            this, SLOT(makeRunButtonAvaliable()));
    connect(launcher, SIGNAL(programIsFinished()),
            this, SLOT(showNormal()));

    infodialog = new InfoDialog();                                                                  // создаю объект класса InfoDialog
    connect(this, SIGNAL(initializeTableInInfoWindow(QStringList,QStringList,QStringList)),                     // связываю сигнал класса MainWindow со слотом класса InfoDialog, который создает таблицу в окне
            infodialog, SLOT(initializeTable(QStringList,QStringList,QStringList)));                            //
    connect(infodialog, SIGNAL(sublLaunchSignal(int)),
            this, SLOT(launchSubl(int)));
    connect(infodialog, SIGNAL(deleteSelectedTask(int)),
            this, SLOT(deleteFile(int)));
    connect(this, SIGNAL(destroyed(QObject*)),
            infodialog, SLOT(close()));
    connect(launcher, SIGNAL(renewTableInInfoWindow()),                                             // связываю сигнал класса OrcaLauncher со слотом класса InfoDialog, который обновляет данные таблицы
            infodialog, SLOT(renewTable()));                                                        //
    connect(launcher, SIGNAL(renewTableInInfoWindowWithError()),
            infodialog, SLOT(renewTableWithError()));
    connect(launcher, SIGNAL(programIsFinished()),
            infodialog, SLOT(resetToZero()));
    connect(infodialog, SIGNAL(killSelectedProcess()),
            this, SLOT(killCurrentTask()));

    writingtofilethread = new QThread(this);                                                        // создаю отдельный поток для записи .out файла из orca.exe
    connect(this, SIGNAL(destroyed()),
            writingtofilethread, SLOT(quit()));


    launcher->moveToThread(writingtofilethread);                                                    // перемещаю объект класса OrcaLauncher в отдельный поток
    writingtofilethread->start();

    setWindowTitle("OrcaLauncher");
    QIcon *programIcon = new QIcon("C:/Users/Dima/Documents/OrcaLauncher/programIcon.png");
    setWindowIcon(*programIcon);

    ui->tableWidget->setColumnWidth(0, 229);
    ui->tableWidget->setColumnWidth(1, 100);

    ui->plainTextEdit->setPlainText("\nPlease select an input file in the left window (Input files queue) to start editing");
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    settings->setValue("ORCA_PATH", orcaDir);
    settings->setValue("SUBL_PATH", sublDir);
    settings->setValue("LAST_DIR", lastDir);
}

void MainWindow::loadSettings()
{
    orcaDir = settings->value("ORCA_PATH", "C:\\").toString();
    sublDir = settings->value("SUBL_PATH", "C:\\").toString();
    lastDir = settings->value("LAST_DIR", "C:\\").toString();
}

OrcaLauncher::OrcaLauncher(QObject *parent)
{
    process = new QProcess(this);
}

void OrcaLauncher::launchProgram()  // метод, запускающий orca.exe
{
    emit renewTableInInfoWindow();                                                              // обновляю данные в таблице в окне с информацией об очереди задач

    for (int i = 0; i < fileList.size(); ++i)
    {
        QString outFilePath = filePaths.at(i) + "/" + fileNames.at(i) + ".out";

        //QProcess *process = new QProcess(this);

        QStringList arguments;
            arguments.append(fileNames.at(i) + ".inp");

        process->setArguments(arguments);                                                           // устанавливаю параметр для запуска orca.exe
        process->setProgram(orcaDir);                                                               //
        process->setWorkingDirectory(filePaths.at(i));                                              //

        QFile outFile(outFilePath);                                                                 //
        outFile.open(QFile::WriteOnly);                                                             // создаю и открываю для записи .out файл для orca.exe
        QTextStream fout(&outFile);                                                                 //

        QDateTime current = QDateTime::currentDateTime();
        QString currentDateTime = current.toString("dd.MM.yyyy HH:mm:ss");                          // печатаю в файл дату и время запуска программы с текущим .inp файлом
        fout << "This file was created by OrcaLauncher\n";                                          //
        fout << currentDateTime;                                                                    //

        process->setProcessChannelMode(QProcess::MergedChannels);
        process->start();                                                                           // запускаю orca.exe
        pid = process->processId();
        while(process->waitForReadyRead())                                                          // печатаю в .out файл выдачу orca.exe
            fout << process->readAll();

        outFile.flush();
        outFile.close();                                                                            // закрываю файл для записи

        if (process->exitCode() == 0)
            emit renewTableInInfoWindow();
        else
            emit renewTableInInfoWindowWithError();
    }

    emit programIsFinished();
}

void MainWindow::on_pushButton_clicked()     // клик по кнопке Load Files
{
    QStringList loaded_file_names = QFileDialog::getOpenFileNames(this, "Add files in queue", lastDir, filter);    // записываем имена+пути открытых файлов
    fileList.append(loaded_file_names);                                                                     // добавляем имена+пути открытых файлов в общий список

    if (loaded_file_names.isEmpty())
        return;

    ui->tableWidget->setRowCount(fileList.size());                                                          // создаем строки в таблице по количеству открытых файлов

    for (fileCounter; fileCounter < fileList.size(); ++fileCounter)                             // главный цикл
    {
        QFileInfo fileInfo(fileList.at(fileCounter));                                                       //
        fileNames.append(fileInfo.baseName());                                                              // разделяем имя файла и его путь по отдельным спискам
        filePaths.append(fileInfo.absolutePath());                                                          //

        QFile file(fileList.at(fileCounter));                                                               //
        file.open(QFile::ReadOnly | QFile::Text);                                                           //
        QTextStream in(&file);                                                                              // Записываем содержимое отдельного файла в общий список
        QString text = in.readAll();                                                                        //
        fileBodies.append(text);                                                                            //
        file.close();

        int i = 1;
        while(!(text.contains(QString("PAL%1").arg(i)) || (text.contains(QString("nprocs %1\n").arg(i)) || text.contains(QString("nprocs %1 ").arg(i)))))
        {
            i++;
            if (i > maxThreads) break;
        }

        if (i == maxThreads+1)
            i = 1;

        fileThread.append(QString::number(i));                                                              // записываем в список информацию о количестве потоков, требуемом для отдельного файла

        QTableWidgetItem *nametableitem = new QTableWidgetItem(fileNames.at(fileCounter));                  //
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(fileCounter));              //
                                                                                                            // Заполняем таблицу именами файлов и количеством требуемых потоков
        ui->tableWidget->setItem(fileCounter, 0, nametableitem);                                            //
        ui->tableWidget->setItem(fileCounter, 1, threadstableitem);                                         //

        ui->tableWidget->setRowHeight(fileCounter, 25);
    }

    ui->tableWidget->setColumnWidth(0, 214);                                                        // подстраиваем ширины колонок таблицы
    ui->tableWidget->setColumnWidth(1, 100);                                                        //

    lastDir = filePaths.last();                                                                     // запоминаем директорию последнего файла для удобства
}

void MainWindow::on_pushButton_2_clicked()                                                          // клик по кнопке Delete
{
    fileList.removeAt(ui->tableWidget->currentRow());                                               // удаляем выбранный файл из таблицы и его информацию из всех списков
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
    fileList.clear();                                                                               // очищаем очередь задач полностью и очищаем все списки
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
    ui->plainTextEdit->setPlainText(fileBodies.at(row));                                            // перемещаем текст выбранного файла в окно редактора
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)                                    // клик по ячейке в таблице
{
    ui->plainTextEdit->setPlainText(fileBodies.at(row));                                            // перемещаем текст выбранного файла в окно редактора
}

void MainWindow::on_pushButton_4_clicked()                                                          // клик по кнопке Move UP
{                                                                                                   // последовательность действий, которая опускает выбранный файл на одну позицию вверх в очереди задач
    int current_row = ui->tableWidget->currentRow();

    if (current_row == -1) return;

    if (current_row > 0)
    {
        fileList.move(current_row, current_row-1);
        fileNames.move(current_row, current_row-1);
        fileThread.move(current_row, current_row-1);
        fileBodies.move(current_row, current_row-1);

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
{                                                                                                   // последовательность действий, которая опускает выбранный файл на одну позицию вниз в очереди задач
    int current_row = ui->tableWidget->currentRow();

    if (current_row == -1) return;

    if (current_row < fileCounter-1)
    {
        fileList.move(current_row, current_row+1);
        fileNames.move(current_row, current_row+1);
        fileThread.move(current_row, current_row+1);
        fileBodies.move(current_row, current_row+1);

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
}

void MainWindow::on_pushButton_9_clicked()                                                          // клик по книпке RUN
{
    if (!fileList.length())                                                         // проверка на наличие в очереди файлов
    {
        QMessageBox::warning(this, "", "Queue is empty");
        return;
    }

    if (orcaDir.length())                                                           // проверка на наличие пути к orca.exe
    {
        if (orcaDir.contains("orca.exe"))                                           // проверка на наличие пути именно к orca.exe
        {
            ui->pushButton_9->setEnabled(false);
            infodialog->show();                                                     // отображение информационного окна
            emit initializeTableInInfoWindow(fileNames, filePaths, fileThread);                // создание в нем таблицы
            emit orcaLauncherSignal();                                              // запуск метода, который запускает orca.exe
            this->showMinimized();                                                  // свертывание главного окна
            ui->plainTextEdit->clear();
        }
        else
            QMessageBox::warning(this, "", "Please specify the path to orca.exe in the settings");
    }
    else
        QMessageBox::warning(this, "", "Please specify the path to orca.exe in the settings");
}

void MainWindow::on_pushButton_6_clicked()                                                          // клик по кнопке Save
{                                                                                                   // последовательность действий, которая сохраняет содержимое окна редактора в выбраный файл
    int current_row = ui->tableWidget->currentRow(); 

    if (current_row > -1)
    {
        if (ui->plainTextEdit->toPlainText() != fileBodies.at(current_row))
        {
            QMessageBox::StandardButton confirmation;
            confirmation = QMessageBox::question(this,
                                                 QString("Save %1.inp").arg(fileNames.at(current_row)),
                                                 QString("Are you sure you want to change %1.inp?").arg(fileNames.at(current_row)),
                                                 QMessageBox::Yes|QMessageBox::No);
            if (confirmation == QMessageBox::Yes)
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

                if (i == maxThreads+1)
                    i = 1;

                fileThread.replace(current_row, QString::number(i));
                QTableWidgetItem *threadstableitem = new QTableWidgetItem(fileThread.at(current_row));
                ui->tableWidget->setItem(current_row, 1, threadstableitem);
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
    else
        QMessageBox::warning(this, "", "There is no file to save");
}

void MainWindow::on_pushButton_8_clicked()                                                          // клик по кнопке Save as new
{                                                                                                   // последовательность действий, которая сохраняет содержимое окна редактора в новый файл и добавляет этот файл в очередь задач
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

    if (i == maxThreads+1)
        i = 1;

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

void MainWindow::on_actionAbout_triggered()                                                         // клик по кнопке About
{
    QMessageBox::information(this, "About OrcaLauncher", aboutProgramText);
}

void MainWindow::on_actionSet_path_to_Sublime_Text_triggered()
{
    QString sublimeTextDir;
    sublimeTextDir = QFileDialog::getOpenFileName(this, "Set path to Sublime Text 3", sublDir, "Executable files *.exe");

    if (sublimeTextDir.contains("sublime_text.exe"))
    {
        QFileInfo sublDirInfo(sublimeTextDir);
        sublDir = sublDirInfo.absolutePath() + "/subl.exe";
    }
    else
        QMessageBox::warning(this, "", "This is not the Sublime Text");
}

void MainWindow::launchSubl(int selectedRow)
{
    QStringList outFilePath = QStringList(filePaths.at(selectedRow) + "/" + fileNames.at(selectedRow) + ".out");

    QProcess *process = new QProcess(this);
    process->setArguments(outFilePath);
    process->setProgram(sublDir);

    if (sublDir.contains("subl.exe"))
    {
        process->start();
    }
    else
    {
        QMessageBox::warning(this, "", "Please specify the path to the Sublime Text in the settings");
    }
}

void MainWindow::makeRunButtonAvaliable()
{
    ui->pushButton_9->setEnabled(true);
}

void MainWindow::deleteFile(int fileNumber)
{
    fileList.removeAt(fileNumber);
    fileNames.removeAt(fileNumber);
    filePaths.removeAt(fileNumber);
    fileBodies.removeAt(fileNumber);
    fileThread.removeAt(fileNumber);
    ui->tableWidget->removeRow(fileNumber);
    fileCounter--;
}

void MainWindow::killCurrentTask()
{
    QStringList argum = QStringList("/PID");
    argum.append(QString("%1").arg(pid));
    argum.append("/F");
    argum.append("/T");

    QProcess *taskKillerProcess = new QProcess(this);
    taskKillerProcess->setArguments(argum);
    taskKillerProcess->setProgram("taskkill");

    taskKillerProcess->start();
}
