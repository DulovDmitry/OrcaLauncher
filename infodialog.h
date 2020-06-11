#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>


namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = 0);
    ~InfoDialog();

public slots:
    void initializeTable(QStringList taskNames, QStringList taskPaths, QStringList taskThreads);

    void renewTable();

    void renewTableWithError();

    void resetToZero();

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

signals:
    void sublLaunchSignal(int selectedRow);

    void deleteSelectedTask(int selectedRow);

    void killSelectedProcess();

private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
