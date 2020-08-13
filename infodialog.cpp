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

QStringList taskStatus;
QStringList numberOfSubtask;
int currentTask = 0;
int currentSubtask = 0;

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    currentTask = 0;

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

}

void InfoDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();

    if (currentTask == 0)
    {
        event->accept();
        emit infoDialogIsClosing();
    }
    else
    {
        QMessageBox::warning(this, "Warning", QString("%1.inp is in progress!").arg(ui->tableWidget->item(currentTask - 1, 0)->text()));
    }
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::initializeTable(QStringList taskNames, QStringList taskPaths, QStringList taskThreads)        // метод, который создает таблицу в информационном окне
{
    QRect rec = QApplication::desktop()->screenGeometry();

    setFixedHeight(taskNames.size()*25 + 80);
    this->move(rec.width() - 366, rec.height() - taskNames.size()*25 - 160);                        // помещаем окно в правый нижний угол

    ui->tableWidget->setFixedHeight(taskNames.size()*25 + 25);
    ui->tableWidget->setRowCount(taskNames.size());
    ui->tableWidget->setColumnWidth(0, 209);
    ui->tableWidget->setColumnWidth(1, 50);
    ui->tableWidget->setColumnWidth(2, 70);                                                         // настраиваем внешний вид таблицы
    ui->pushButton_2->setGeometry(10, taskNames.size()*25 + 55, 70, 20);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setGeometry(90, taskNames.size()*25 + 55, 70, 20);
    ui->pushButton->setEnabled(false);

    for (int i = 0; i < taskNames.size(); ++i)                                                      // заполняем таблицу
    {
        QTableWidgetItem *nametableitem = new QTableWidgetItem(taskNames.at(i));
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(taskThreads.at(i));
        QTableWidgetItem *statustableitem = new QTableWidgetItem("In queue");

        threadstableitem->setTextAlignment(Qt::AlignCenter);
        statustableitem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(i, 0, nametableitem);
        ui->tableWidget->setItem(i, 1, threadstableitem);
        ui->tableWidget->setItem(i, 2, statustableitem);

        ui->tableWidget->setRowHeight(i, 25);
    }

    ui->tableWidget->setColumnWidth(0, 194);

    pathsOfFiles = taskPaths;
}

void InfoDialog::renewTable()                                                           // метод, который обновляет таблицу при завершении очередной задачи
{    
    QTableWidgetItem *statustableitem = new QTableWidgetItem("In progress");                    // выполняемой задаче изменяем статус на "In progress"
    statustableitem->setBackgroundColor(QColor(255, 255, 0, 127));
    statustableitem->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(currentTask, 2, statustableitem);

    if(currentTask)
    {
        QTableWidgetItem *statustableitem2 = new QTableWidgetItem("Completed");                 // выполненной задаче изменяем статус на "Completed"
        statustableitem2->setBackgroundColor(QColor(0, 255, 0, 127));
        statustableitem2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(currentTask-1, 2, statustableitem2);
    }

    currentTask++;
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setEnabled(false);
}

void InfoDialog::renewTableWithError()
{
    QTableWidgetItem *statustableitem = new QTableWidgetItem("In progress");
    statustableitem->setBackgroundColor(QColor(255, 255, 0, 127));
    statustableitem->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(currentTask, 2, statustableitem);

    if(currentTask)
    {
        QTableWidgetItem *statustableitem2 = new QTableWidgetItem("Aborted");
        statustableitem2->setBackgroundColor(QColor(255, 0, 0, 127));
        statustableitem2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(currentTask-1, 2, statustableitem2);
    }

    currentTask++;
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setEnabled(false);
}

void InfoDialog::resetToZero()
{
    currentTask = 0;
}

void InfoDialog::launchSublimeFromContextMenu(bool b)
{
    emit sublLaunchSignal(ui->tableWidget->currentRow());
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
    process->setWorkingDirectory(pathsOfFiles.at(current_row));
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
    process->setWorkingDirectory(pathsOfFiles.at(current_row));
    process->setArguments(arguments);
    process->setProgram(chemcraftDir);

    process->start();
    //process->waitForFinished(30000);
}

void InfoDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if (!ui->pushButton_2->isEnabled())
        sublLaunchSignal(row);
}

void InfoDialog::on_tableWidget_cellClicked(int row, int column)
{
    if (currentTask)
    {
        if (row == currentTask - 1)
            ui->pushButton->setEnabled(true);
        else
            ui->pushButton->setEnabled(false);

        if (row > currentTask - 1)
            ui->pushButton_2->setEnabled(true);
        else
            ui->pushButton_2->setEnabled(false);
    }
}

void InfoDialog::on_pushButton_2_clicked()  //Delete task
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Delete %1.inp").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QString("Are you sure you want to delete %1.inp from queue?").arg(ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text()),
                                         QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        if (ui->pushButton_2->isEnabled())
        {
            int row = ui->tableWidget->currentRow();

            emit deleteSelectedTask(row);
            ui->tableWidget->removeRow(row);
            pathsOfFiles.removeAt(row);

            ui->pushButton->setDisabled(true);
            ui->pushButton_2->setDisabled(true);
        }
        else
            return;
    }
    else
    {
        return;
    }
}

void InfoDialog::on_pushButton_clicked()    //Kill process
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Kill %1.inp").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QString("Are you sure you want to terminate %1.inp task?").arg(ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text()),
                                         QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        if (ui->pushButton->isEnabled())
            emit killSelectedProcess();
        else
            return;
    }
    else
    {
        return;
    }
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
