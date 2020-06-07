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

    void resetToZero();

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

signals:
    void sublLaunchSignal(int selectedRow);

private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
