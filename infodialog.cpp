#include "infodialog.h"
#include "ui_infodialog.h"
#include <QWidget>
#include <QDesktopWidget>

QStringList taskStatus;
int currentTask = 0;

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    currentTask = 0;

    setWindowTitle("OrcaLauncher Info");
    QIcon *programIcon = new QIcon("C:/Users/Dima/Documents/OrcaLauncher/programIcon.png");
    setWindowIcon(*programIcon);

}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::initializeTable(QStringList taskNames, QStringList taskThreads)
{
    QRect rec = QApplication::desktop()->screenGeometry();

    setFixedHeight(taskNames.size()*25 + 60);
    this->move(rec.width() - 366, rec.height() - taskNames.size()*25 - 100);

    ui->tableWidget->setFixedHeight(taskNames.size()*25 + 30);
    ui->tableWidget->setRowCount(taskNames.size());
    ui->tableWidget->setColumnWidth(0, 209);
    ui->tableWidget->setColumnWidth(1, 50);
    ui->tableWidget->setColumnWidth(2, 70);

    for (int i = 0; i < taskNames.size(); ++i)
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

void InfoDialog::renewTable()
{
    QTableWidgetItem *statustableitem = new QTableWidgetItem("In progress");
    statustableitem->setBackgroundColor(QColor(255, 255, 0, 127));
    ui->tableWidget->setItem(currentTask, 2, statustableitem);

    if(currentTask)
    {
        QTableWidgetItem *statustableitem2 = new QTableWidgetItem("Comlpeted");
        statustableitem2->setBackgroundColor(QColor(0, 255, 0, 127));
        ui->tableWidget->setItem(currentTask-1, 2, statustableitem2);
    }

    currentTask++;
}
