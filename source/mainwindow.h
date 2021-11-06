#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QProcess>
#include <QMap>
#include <QTableWidgetItem>
#include <QShortcut>
#include <QLinkedList>
#include <thread>
#include <QFileDialog>
#include <QStringList>
#include <QArrayData>
#include <QTextStream>
#include <QFileInfo>
#include <QFile>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFont>
#include <QIcon>
#include <QDateTime>
#include <QTextEdit>
#include <QThread>
#include <QCloseEvent>

#include "infodialog.h"
#include "aboutdialog.h"
#include "templatesmanager.h"
#include "queue.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    // Buttons control
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_toolButton_clicked();
    void updateButtonsAvailability();
    void checkTemplatesManagerButton();

    // Table control
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_tableWidget_cellClicked(int row, int column);
    void on_tableWidget_cellActivated(int row, int column);
    void on_tableWidget_cellEntered(int row, int column);
    void on_tableWidget_cellPressed(int row, int column);
    void refreshTableStyle();
    void interchangeTableRows(int row1, int row2);

    // Hot keys
    void slotShortcutDelete();
    void slotShortcutCtrlS();
    void slotShortcutCtrlAltS();
    void slotShortcutCtrlR();
    void slotShortcutCtrlO();

    // Settings in window
    void on_actionSet_Orca_directory_triggered();
    void on_actionSet_path_to_Sublime_Text_triggered();
    void on_actionSet_path_to_templates_dat_triggered();
    void on_actionSet_path_to_Chemcraft_triggered();
    void on_actionSet_path_to_Avogadro_triggered();

    // Register settings
    void saveSettings();
    void loadSettings();

    // Other widgets control
    void on_comboBox_activated(const QString &arg1);
    void comboBoxFilling();

    // Slots for context menu
    void configureContextMenu();
    void tableCustomMenuRequested(QPoint pos);
    void runSelectedTask();
    void addSelectedTask();
    void deleteSelectedTask();
    void openSelectedFileInChemcraft();
    void openSelectedFileInExplorer();

    // Files interaction
    void processFileList(QStringList loadedFileNames);
    void parseFileWithTemplates();

    // General
    void closeEvent(QCloseEvent *event);
    void launchOrcaWithQueue(Queue *_queue);
    void addTasksToQueue(Queue *_queue);
    void orcaIsFinished();
    void on_actionAbout_triggered();
    void setIconForThreadsNumber(QTableWidgetItem *threadstableitem, QTableWidgetItem *nametableitem);
    void showThreadNumberWarningMessageBox();
    short getThreadsNumberFromText(QString text);

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    InfoDialog *infodialog;
    AboutDialog *aboutdialog;
    TemplatesManager *templatesmanager;

    Queue queue = Queue(this);

    QStringList templateKey;
    QStringList templateValue;

    // Context menu for table
    QMenu *contextMenu;
    QIcon *chemcraftIcon;
    QIcon *explorerIcon;
    QAction *runSingleTask;
    QAction *addSingleTask;
    QAction *deleteTask;
    QAction *openInpInChemcraft;
    QAction *openInpInExplorer;

    // Hot keys
    QShortcut *keyDelete;
    QShortcut *keyCtrlS;
    QShortcut *keyCtrlAltS;
    QShortcut *keyCtrlR;
    QShortcut *keyCtrlO;

    // Register variables
    QString lastDir;
    QString orcaDir;
    QString sublDir;
    QString chemcraftDir;
    QString avogadroDir;
    QString templatesFileDir;
    bool showThreadNumberWarning;

    // Flags
    bool OrcaIsInProgress = false;
    bool threadNumberWarningWasShowed = false;
    bool tooManyThreads = false;

    int fileCounter = 0;
    int avaliableThreads = 0;

    const QString filter = "Orca input files (*.inp) ;; All files (*.*)";
};

#endif // MAINWINDOW_H
