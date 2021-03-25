#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QSettings>
#include <QProcess>
#include "qtablewidget.h"
#include <QKeyEvent>
#include <QShortcut>

#include "queue.h"


namespace Ui {
class InfoDialog;
}

class OrcaLauncher : public QObject
{
    Q_OBJECT

public:
    explicit OrcaLauncher(QObject *parent = 0);
    ~OrcaLauncher();

public slots:
    void launchProgram(Queue *queue);

private slots:
    QByteArray getHeaderText(QString inputFilePath);

signals:
    void updateTable();

    void programIsFinished();

private:
    QProcess *process;
    QSettings *settings;

};

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = 0);
    ~InfoDialog();

public slots:
    void updateTable();

    void moveWindowIntoCorner();

    void updateWindowHeight();

    QString getTaskStatus(int status);

    QColor getStatusColor(int status);

    void orcaIsFinished();

    void launchProgram(Queue _queue);

    void addTasksToQueue(Queue _queue);

    void launchSublimeFromContextMenu(bool b);

    void launchOrca2aim(bool b);

    void launchChemcraft(bool b);

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void killCurrentProcess();

    void slotCustomMenuRequested(QPoint pos);

    bool windowIsTooTall();

    void setWidgetStyle();

    void on_moveDownButton_clicked();

    void on_moveUpButton_clicked();

    void interchangeTableRows(int row1, int row2);

    void slotShortcutDelete();

    void on_tableWidget_cellActivated(int row, int column);

    void on_tableWidget_cellEntered(int row, int column);

    void on_tableWidget_cellPressed(int row, int column);

signals:
    void launchOrca (Queue *q);

    void sublLaunchSignal(int selectedRow);

    void infoDialogIsClosing();

    void orcaIsFinishedSignal();

protected slots:
    void closeEvent(QCloseEvent *event);

private:
    Ui::InfoDialog *ui;
    QSettings *settings;
    OrcaLauncher *launcher;
    Queue *queue;
    QThread *writingtofilethread;
    QStringList pathsOfFiles;
    QShortcut *keyDelete;
};

#endif // INFODIALOG_H
