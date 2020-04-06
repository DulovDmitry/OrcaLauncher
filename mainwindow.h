#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "infodialog.h"

namespace Ui {
class MainWindow;
}

class OrcaLauncher : public QObject
{
    Q_OBJECT

public:
    explicit OrcaLauncher(QObject *parent = 0);

public slots:
    void launchProgram();

signals:
    void renewTableInInfoWindow();

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_actionSet_Orca_directory_triggered();

    void on_pushButton_9_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_actionAbout_triggered();

signals:
    void orcaLauncherSignal();

    void initializeTableInInfoWindow(QStringList taskNames, QStringList taskThreads);

private:
    Ui::MainWindow *ui;
    InfoDialog *infodialog;
    OrcaLauncher *launcher;
    QThread *writingtofilethread;
};

#endif // MAINWINDOW_H
