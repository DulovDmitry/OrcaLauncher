#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QSettings>


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

    void launchSublimeFromContextMenu(bool b);

    void launchOrca2aim(bool b);

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void slotCustomMenuRequested(QPoint pos);

signals:
    void sublLaunchSignal(int selectedRow);

    void deleteSelectedTask(int selectedRow);

    void killSelectedProcess();

    void infoDialogIsClosing();

protected slots:
    void closeEvent(QCloseEvent *event);

private:
    Ui::InfoDialog *ui;
    QSettings *settings;
    QStringList pathsOfFiles;
};

#endif // INFODIALOG_H
