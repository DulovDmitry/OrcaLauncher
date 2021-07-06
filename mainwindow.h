#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QProcess>
#include <QMap>
#include <QTableWidgetItem>
#include <QShortcut>
#include <QLinkedList>
#include "infodialog.h"
#include "aboutdialog.h"
#include "templatesmanager.h"
#include "queue.h"
#include "choosequeuedialog.h"

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void saveSettings();

    void loadSettings();

    void processFileList(QStringList loadedFileNames);

private slots:
    void closeEvent(QCloseEvent *event);

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

    void on_actionSet_path_to_Sublime_Text_triggered();

    void on_comboBox_activated(const QString &arg1);

    void comboBoxFilling();

    void parseFileWithTemplates();

    void checkTemplatesManagerButton();

    void on_actionSet_path_to_templates_dat_triggered();

    void on_toolButton_clicked();

    void on_actionSet_path_to_Chemcraft_triggered();

    void on_pushButton_7_clicked();

    void orcaIsFinished();

    void updateButtonsAvailability();

    void setIconForThreadsNumber(QTableWidgetItem *threadstableitem, QTableWidgetItem *nametableitem);

    void showThreadNumberWarningMessageBox();

    void interchangeTableRows(int row1, int row2);

    void refreshTableStyle();

    short getThreadsNumberFromText(QString text);

    void on_tableWidget_cellActivated(int row, int column);

    void on_tableWidget_cellEntered(int row, int column);

    void on_tableWidget_cellPressed(int row, int column);

    void slotShortcutDelete();

    void slotShortcutCtrlS();

    void slotShortcutCtrlAltS();

    void slotShortcutCtrlR();

    void slotShortcutCtrlO();

signals:
    void orcaLauncherSignal(Queue _queue);

    void addTasksToQueueSignal(Queue _queue);

    void initializeTableInInfoWindow(QStringList taskNames, QStringList subtaskNumber, QStringList taskThreads);


private:
    Ui::MainWindow *ui;
    QSettings *settings;
    InfoDialog *infodialog;
    AboutDialog *aboutdialog;
    TemplatesManager *templatesmanager;

    Queue queue = Queue(this);

    QStringList templateKey;
    QStringList templateValue;

    QShortcut *keyDelete;
    QShortcut *keyCtrlS;
    QShortcut *keyCtrlAltS;
    QShortcut *keyCtrlR;
    QShortcut *keyCtrlO;

    QString lastDir;
    QString orcaDir;
    QString sublDir;
    QString chemcraftDir;
    QString templatesFileDir;

    bool OrcaIsInProgress = false;
    bool showThreadNumberWarning;
    bool threadNumberWarningWasShowed = false;
    bool tooManyThreads = false;

    int fileCounter = 0;
    int avaliableThreads = 0;

    const QString filter = "Orca input files (*.inp) ;; All files (*.*)";
};

#endif // MAINWINDOW_H
