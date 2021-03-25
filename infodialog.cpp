#include "infodialog.h"
#include "ui_infodialog.h"
#include <QWidget>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QFileInfo>
#include <QProcess>
#include <QGuiApplication>
#include <QDateTime>
#include <QTextStream>
#include <QThread>
#include <QtDebug>

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    qDebug()<< "InfoDialog constructor";
    ui->setupUi(this);

    queue = new Queue(this);
    launcher = new OrcaLauncher(this);

    writingtofilethread = new QThread(this);                                                        // создаю отдельный поток для записи .out файла из orca.exe
    connect(this, SIGNAL(destroyed()),
            writingtofilethread, SLOT(quit()));

    launcher->moveToThread(writingtofilethread);                                                    // перемещаю объект класса OrcaLauncher в отдельный поток
    writingtofilethread->start();

    connect(this, SIGNAL(launchOrca(Queue *)),
            launcher, SLOT(launchProgram(Queue *)));
    connect(launcher, SIGNAL(updateTable()),
            this, SLOT(updateTable()));
    connect(launcher, SIGNAL(programIsFinished()),
            this, SLOT(orcaIsFinished()));

    setWindowTitle("OrcaLauncher Queue status");
    QIcon *programIcon = new QIcon(":/new/prefix1/programIcon");
    setWindowIcon(*programIcon);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotCustomMenuRequested(QPoint)));
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->setStyleSheet("QTableView::item:selected { color: black; background: #e0f6ff; outline: none; border: none;}"
                                   "QTableView:focus {outline: none;}");
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    keyDelete = new QShortcut(this);
    keyDelete->setKey(Qt::Key_Delete);
    connect(keyDelete, SIGNAL(activated()), this, SLOT(slotShortcutDelete()));
}

void InfoDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();

    if (!(queue->status.contains(0) || queue->status.contains(1)))
    {
        event->accept();
        emit infoDialogIsClosing();
        queue->clearQueue();
    }
    else
        QMessageBox::warning(this, "Warning", QString("%1.inp is in progress!").arg(ui->tableWidget->item(queue->status.indexOf(1), 0)->text()));
}

InfoDialog::~InfoDialog()
{
    delete ui;
    launcher->~OrcaLauncher();
    qDebug()<< "InfoDialog destructor";
}

OrcaLauncher::OrcaLauncher(QObject *parent)
{
    process = new QProcess(this);
    qDebug()<< "OL constructor";
}

OrcaLauncher::~OrcaLauncher()
{
    qDebug()<< "OL destructor";
}

void OrcaLauncher::launchProgram(Queue *queue)  // метод, запускающий orca.exe
{
    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    QString orcaDir = settings->value("ORCA_PATH", "C:\\").toString();

    for (int i = 0; i < queue->fileList.size(); ++i)
    {
        queue->status[i] = 1;

        emit updateTable();

        QString outFilePath = queue->filePaths.at(i) + "/" + queue->fileNames.at(i) + ".out";

        QStringList arguments;
            arguments.append(queue->fileNames.at(i) + ".inp");

        process->setArguments(arguments);                                                           // устанавливаю параметр для запуска orca.exe
        process->setProgram(orcaDir);                                                               //
        process->setWorkingDirectory(queue->filePaths.at(i));                                              //

        QFile outFile(outFilePath);                                                                 //
        outFile.open(QIODevice::WriteOnly | QIODevice::Unbuffered);                                 // создаю и открываю для записи .out файл для orca.exe
        outFile.write(getHeaderText(queue->fileList.at(i)));                                                             // печатаю в файл дату и время запуска программы с текущим .inp файлом

        process->setProcessChannelMode(QProcess::MergedChannels);
        process->start();                                                                           // запускаю orca.exe
        queue->currentProcessID = process->processId();

        while(process->waitForReadyRead(2000000000))                                                          // печатаю в .out файл выдачу orca.exe
            outFile.write(process->readAll());

        outFile.close();                                                                            // закрываю файл для записи

        if (process->exitCode() == 0)
            queue->status[i] = 2;
        else
            queue->status[i] = 3;

        emit updateTable();
    }

    emit programIsFinished();
}

QByteArray OrcaLauncher::getHeaderText(QString inputFilePath)
{
    QString text;
    QDateTime current = QDateTime::currentDateTime();
    QString currentDateTime = current.toString("dd.MM.yyyy HH:mm:ss");

    text = "#---------------------------------------------------------------\n"
           "#\n"
           "#  File created by OrcaLauncher\n"
           "#  " + currentDateTime + "\n"
           "#  Input file: " + inputFilePath + "\n"
           "#\n"
           "#---------------------------------------------------------------\n";

    return text.toUtf8();
}

void InfoDialog::updateTable()
{
    int queueSize = queue->fileNames.size();

    setFixedHeight(queueSize*25 + 80);

    ui->tableWidget->setFixedHeight(queueSize*25 + 25);
    ui->tableWidget->setRowCount(queueSize);

    ui->pushButton_2->setGeometry(105, queueSize*25 + 55, 70, 20);
    ui->pushButton_2->setEnabled(false);

    ui->pushButton->setGeometry(185, queueSize*25 + 55, 70, 20);
    ui->pushButton->setEnabled(false);

    ui->moveUpButton->setGeometry(10, queueSize*25 + 55, 70, 20);
    ui->moveDownButton->setGeometry(55, queueSize*25 + 55, 70, 20);

    for (int i = 0; i < queueSize; ++i)                                                      // заполняем таблицу
    {
        QTableWidgetItem *nametableitem = new QTableWidgetItem(queue->fileNames.at(i));
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(queue->fileThread.at(i));
        QTableWidgetItem *statustableitem = new QTableWidgetItem(getTaskStatus(queue->status.at(i)));

        ui->tableWidget->setItem(i, 0, nametableitem);

        threadstableitem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 1, threadstableitem);

        statustableitem->setTextAlignment(Qt::AlignCenter);
        statustableitem->setBackgroundColor(getStatusColor(queue->status.at(i)));
        ui->tableWidget->setItem(i, 2, statustableitem);

        ui->tableWidget->setRowHeight(i, 25);
    }

}

void InfoDialog::moveWindowIntoCorner()
{
    QRect rec = QApplication::desktop()->screenGeometry();
    this->move(rec.width() - 356, rec.height() - queue->fileNames.size()*25 - 160);
}

void InfoDialog::updateWindowHeight()
{
    QRect rec = QApplication::desktop()->screenGeometry();
    this->move(this->geometry().x(), rec.height() - queue->fileNames.size()*25 - 160);
}

QString InfoDialog::getTaskStatus(int status)
{
    switch (status)
    {
    case 0: return "In queue";
    case 1: return "In progress";
    case 2: return "Completed";
    case 3: return "Aborted";
    }

    return QString();
}

QColor InfoDialog::getStatusColor(int status)
{
    switch (status)
    {
    case 0: return QColor(255, 255, 255);       // white
    case 1: return QColor(255, 255, 0, 127);    // light yellow
    case 2: return QColor(0, 255, 0, 127);      // light green
    case 3: return QColor(255, 0, 0, 127);      // light red
    }

    return QColor(255, 255, 255);
}

void InfoDialog::orcaIsFinished()
{
    emit orcaIsFinishedSignal();
}

void InfoDialog::launchProgram(Queue _queue)
{
    queue->clearQueue();
    queue->addElementsInQueue(_queue);

    updateTable();
    setWidgetStyle();
    moveWindowIntoCorner();

    emit launchOrca(queue);
}

void InfoDialog::addTasksToQueue(Queue _queue)
{
    queue->addElementsInQueue(_queue);
    updateTable();
    if(windowIsTooTall())
        updateWindowHeight();
}

void InfoDialog::launchSublimeFromContextMenu(bool b)
{
    int current_row = ui->tableWidget->currentRow();
    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    QString sublDir = settings->value("SUBL_PATH", "C:\\").toString();

    QStringList outFilePath = QStringList(queue->filePaths.at(current_row) + "/" + queue->fileNames.at(current_row) + ".out");

    QProcess *process = new QProcess(this);
    process->setArguments(outFilePath);
    process->setProgram(sublDir);

    if (sublDir.contains("subl.exe"))
        process->start();
    else
        QMessageBox::warning(this, "", "Please specify the path to the Sublime Text in the settings");
}

void InfoDialog::launchOrca2aim(bool b)
{
    int current_row = ui->tableWidget->currentRow();
    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    QFileInfo fileInfo(settings->value("ORCA_PATH", "C:\\").toString());
    QString orca2aimDir = fileInfo.absolutePath() + "\\orca_2aim.exe";

    QStringList arguments;
    arguments.append(ui->tableWidget->item(current_row, 0)->text());

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(queue->filePaths.at(current_row));
    process->setArguments(arguments);
    process->setProgram(orca2aimDir);

    process->start();
    process->waitForFinished(30000);

    if(process->exitCode() == 0)
        QMessageBox::information(this, "Done!", QString("The %1.wfn has been created").arg(ui->tableWidget->item(current_row, 0)->text()));
    else
        QMessageBox::critical(this, "Error", "Something went wrong :(");
}

void InfoDialog::launchChemcraft(bool b)
{
    int current_row = ui->tableWidget->currentRow();
    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    QString chemcraftDir = settings->value("CHEMCRAFT_PATH", "C:\\").toString();

    QStringList arguments;
    arguments.append(ui->tableWidget->item(current_row, 0)->text() + ".out");

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(queue->filePaths.at(current_row));
    process->setArguments(arguments);
    process->setProgram(chemcraftDir);

    if (chemcraftDir.toLower().contains("chemcraft.exe"))
        process->start();
    else
        QMessageBox::warning(this, "", "Please specify the path to the Chemcraft in the settings");
}

void InfoDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if (!ui->pushButton_2->isEnabled())
        launchSublimeFromContextMenu(1);
}

void InfoDialog::on_tableWidget_cellClicked(int row, int column)
{
    if (queue->status.contains(0) || queue->status.contains(1))
    {
        ui->pushButton_2->setEnabled(queue->status[row] ? false : true);
        ui->pushButton->setEnabled(queue->status[row] == 1 ? true : false);
    }
}

void InfoDialog::on_pushButton_2_clicked()  //Delete task
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Delete %1.inp").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QString("Are you sure you want to delete %1.inp from queue?").arg(ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text()),
                                         QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes && ui->pushButton_2->isEnabled())
    {
        int row = ui->tableWidget->currentRow();
        queue->removeQueueElementAt(row);

        ui->tableWidget->removeRow(row);

        updateTable();

        ui->pushButton->setDisabled(true);
        ui->pushButton_2->setDisabled(true);
    }
    else
        return;
}

void InfoDialog::on_pushButton_clicked()    //Kill process
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Kill %1.inp").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QString("Are you sure you want to terminate %1.inp task?").arg(ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text()),
                                         QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes && ui->pushButton->isEnabled())
        killCurrentProcess();
    else
        return;
}

void InfoDialog::killCurrentProcess()
{
    QStringList argum = QStringList("/PID");
    argum.append(QString("%1").arg(queue->currentProcessID));
    argum.append("/F");
    argum.append("/T");

    QProcess *taskKillerProcess = new QProcess(this);
    taskKillerProcess->setArguments(argum);
    taskKillerProcess->setProgram("taskkill");

    taskKillerProcess->start();
}

void InfoDialog::slotCustomMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QIcon *orcaIcon = new QIcon(":/new/prefix1/orcaIcon");
    QIcon *sublimeIcon = new QIcon(":/new/prefix1/sublimeIcon");
    QIcon *chemcraftIcon = new QIcon(":/new/prefix1/chemcraftIcon");

    QAction *makeWfnFile = new QAction(*orcaIcon, "Make " + ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text() + ".wfn (orca_2aim)", this);
    QAction *launchSublime = new QAction(*sublimeIcon, "Open in Sublime Text 3", this);
    QAction *launchChemcraft = new QAction(*chemcraftIcon, "Open in Chemcraft", this);

    connect(launchSublime, SIGNAL(triggered(bool)),
            this, SLOT(launchSublimeFromContextMenu(bool)));
    connect(makeWfnFile, SIGNAL(triggered(bool)),
            this, SLOT(launchOrca2aim(bool)));
    connect(launchChemcraft, SIGNAL(triggered(bool)),
            this, SLOT(launchChemcraft(bool)));

    on_tableWidget_cellClicked(ui->tableWidget->currentRow(), ui->tableWidget->currentColumn());

    if (ui->pushButton->isEnabled())
        makeWfnFile->setDisabled(true);

    if (ui->pushButton_2->isEnabled())
    {
        launchSublime->setDisabled(true);
        launchChemcraft->setDisabled(true);
        makeWfnFile->setDisabled(true);
    }

    menu->addAction(launchSublime);
    menu->addAction(launchChemcraft);
    menu->addAction(makeWfnFile);
    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
}

bool InfoDialog::windowIsTooTall()
{
    int a = this->geometry().y() + this->geometry().height();
    return (a > QApplication::desktop()->screenGeometry().height() - 40) ? true : false;
}

void InfoDialog::setWidgetStyle()
{
    ui->tableWidget->setColumnWidth(0, 194);
    ui->tableWidget->setColumnWidth(1, 50);
    ui->tableWidget->setColumnWidth(2, 70);

    ui->pushButton_2->setFixedSize(70, 20);
    ui->pushButton->setFixedSize(70, 20);

    ui->moveUpButton->setIcon(QIcon(":/new/prefix1/arrowUp"));
    ui->moveUpButton->setFixedSize(40, 20);
    ui->moveUpButton->setFocusPolicy(Qt::NoFocus);

    ui->moveDownButton->setIcon(QIcon(":/new/prefix1/arrowDown"));
    ui->moveDownButton->setFixedSize(40, 20);
    ui->moveDownButton->setFocusPolicy(Qt::NoFocus);
}

void InfoDialog::on_moveDownButton_clicked()
{
    int currentTask = queue->status.indexOf(1);
    int currentRow = ui->tableWidget->currentRow();

    if (currentTask == -1) return;
    if (currentRow <= currentTask) return;

    if(currentRow < ui->tableWidget->rowCount() - 1)
    {
        queue->interchangeElementsAt(currentRow, currentRow + 1);
        interchangeTableRows(currentRow, currentRow + 1);
        ui->tableWidget->selectRow(currentRow + 1);
    }
}

void InfoDialog::on_moveUpButton_clicked()
{
    int currentTask = queue->status.indexOf(1);
    int currentRow = ui->tableWidget->currentRow();

    if (currentTask == -1) return;
    if (currentRow <= currentTask) return;

    if(currentRow > currentTask + 1)
    {
        queue->interchangeElementsAt(currentRow, currentRow - 1);
        interchangeTableRows(currentRow, currentRow - 1);
        ui->tableWidget->selectRow(currentRow - 1);
    }
}

void InfoDialog::interchangeTableRows(int row1, int row2)
{
    QTableWidgetItem *name1 = ui->tableWidget->takeItem(row1, 0);
    QTableWidgetItem *threads1 = ui->tableWidget->takeItem(row1, 1);

    QTableWidgetItem *name2 = ui->tableWidget->takeItem(row2, 0);
    QTableWidgetItem *threads2 = ui->tableWidget->takeItem(row2, 1);

    ui->tableWidget->setItem(row1, 0, name2);
    ui->tableWidget->setItem(row1, 1, threads2);

    ui->tableWidget->setItem(row2, 0, name1);
    ui->tableWidget->setItem(row2, 1, threads1);
}

void InfoDialog::slotShortcutDelete()
{
    if (ui->pushButton_2->isEnabled())
        on_pushButton_2_clicked();
}

void InfoDialog::on_tableWidget_cellActivated(int row, int column)
{
    on_tableWidget_cellClicked(row, column);
}

void InfoDialog::on_tableWidget_cellEntered(int row, int column)
{
    on_tableWidget_cellClicked(row, column);
}

void InfoDialog::on_tableWidget_cellPressed(int row, int column)
{
    on_tableWidget_cellClicked(row, column);
}
