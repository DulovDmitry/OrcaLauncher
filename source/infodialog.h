#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QSettings>
#include <QProcess>
#include "qtablewidget.h"
#include <QKeyEvent>
#include <QShortcut>
#include <QDir>

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

    // Public general slots
    void launchProgram(Queue _queue);
    void addTasksToQueue(Queue _queue);

private slots:

    // Table control
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_tableWidget_cellClicked(int row, int column);
    void on_tableWidget_cellActivated(int row, int column);
    void on_tableWidget_cellEntered(int row, int column);
    void on_tableWidget_cellPressed(int row, int column);
    void updateTable();

    // Button control
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_moveDownButton_clicked();
    void on_moveUpButton_clicked();

    // Window control
    void setWidgetStyle();
    bool windowIsTooTall();
    void moveWindowIntoCorner();
    void updateWindowHeight();
    QString getTaskStatus(int status);
    QColor getStatusColor(int status);

    // Hot keys
    void slotShortcutDelete();
    void slotShortcutEsc();

    // Context menu
    void slotCustomMenuRequested(QPoint pos);
    void customMenuInitialization();
    void launchSublimeFromContextMenu();
    void launchOrca2aim();
    void launchChemcraftFromContextMenu();
    void launchAvogadroFromContextMenu();
    void openFileInExplorer();

    // General slots
    void orcaIsFinished();
    void killCurrentProcess();
    void interchangeTableRows(int row1, int row2);
    void resizeEvent(QResizeEvent* event);
    void closeEvent(QCloseEvent *event);

signals:
    void launchOrca (Queue *q);
    void infoDialogIsClosing();
    void orcaIsFinishedSignal();

private:
    Ui::InfoDialog *ui;
    OrcaLauncher *launcher;
    Queue *queue;
    QThread *writingtofilethread;
    QStringList pathsOfFiles;

    // Hot keys
    QShortcut *keyDelete;
    QShortcut *keyEsc;

    // Context menu
    QMenu *menu;
    QIcon *orcaIcon;
    QIcon *sublimeIcon;
    QIcon *chemcraftIcon;
    QIcon *avogadroIcon;
    QIcon *explorerIcon;
    QAction *makeWfnFile;
    QAction *launchSublime;
    QAction *launchChemcraft;
    QAction *launchAvogadro;
    QAction *openExplorer;
};

#endif // INFODIALOG_H
