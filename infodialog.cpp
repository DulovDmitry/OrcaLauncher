#include "infodialog.h"
#include "ui_infodialog.h"
#include <QWidget>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCloseEvent>

QStringList taskStatus;
int currentTask = 0;

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    currentTask = 0;

    setWindowTitle("OrcaLauncher Queue status");
    QIcon *programIcon = new QIcon(":/new/prefix1/programIcon");
    setWindowIcon(*programIcon);

}

void InfoDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();

    if (currentTask == 0)
    {
        event->accept();
    }
    else
    {
        if (QMessageBox::Yes == QMessageBox::question(this,
                                                      "Quit",
                                                      "All processes will be terminated and the queue will be cleared. Are you sure you want to quit?",
                                                      QMessageBox::Yes | QMessageBox::No))
        {
            event->accept();
        }
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

    ui->tableWidget->setFixedHeight(taskNames.size()*25 + 30);
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

        ui->tableWidget->setItem(i, 0, nametableitem);
        ui->tableWidget->setItem(i, 1, threadstableitem);
        ui->tableWidget->setItem(i, 2, statustableitem);

        ui->tableWidget->setRowHeight(i, 25);
    }

    ui->tableWidget->setColumnWidth(0, 194);
}

void InfoDialog::renewTable()                                                           // метод, который обновляет таблицу при завершении очередной задачи
{
    QTableWidgetItem *statustableitem = new QTableWidgetItem("In progress");                    // выполняемой задаче изменяем статус на "In progress"
    statustableitem->setBackgroundColor(QColor(255, 255, 0, 127));
    ui->tableWidget->setItem(currentTask, 2, statustableitem);

    if(currentTask)
    {
        QTableWidgetItem *statustableitem2 = new QTableWidgetItem("Completed");                 // выполненной задаче изменяем статус на "Completed"
        statustableitem2->setBackgroundColor(QColor(0, 255, 0, 127));
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
    ui->tableWidget->setItem(currentTask, 2, statustableitem);

    if(currentTask)
    {
        QTableWidgetItem *statustableitem2 = new QTableWidgetItem("Aborted");
        statustableitem2->setBackgroundColor(QColor(255, 0, 0, 127));
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

void InfoDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
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

void InfoDialog::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Delete %1.inp").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QString("Are you sure you want to delete %1.inp from queue?").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        if (ui->pushButton_2->isEnabled())
        {
            ui->tableWidget->removeRow(ui->tableWidget->currentRow());
            emit deleteSelectedTask(ui->tableWidget->currentRow() + 1);
        }
        else
            return;
    }
    else
    {
        return;
    }
}

void InfoDialog::on_pushButton_clicked()
{
    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Kill %1.inp").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
                                         QString("Are you sure you want to terminate %1.inp task?").arg(ui->tableWidget->itemAt(0, ui->tableWidget->currentRow())->text()),
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
