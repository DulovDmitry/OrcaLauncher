#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug()<< "Main constructor";
    ui->setupUi(this);

    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    loadSettings();

    qRegisterMetaType<Queue>("Queue");

    avaliableThreads = std::thread::hardware_concurrency();

    // Infodialog configuration
    infodialog = new InfoDialog();
    connect(infodialog, SIGNAL(orcaIsFinishedSignal()),
            this, SLOT(orcaIsFinished()));
    connect(infodialog, SIGNAL(infoDialogIsClosing()),
            this, SLOT(showNormal()));

    // Main window configuration
    setWindowTitle("OrcaLauncher");
    QIcon *programIcon = new QIcon(":/new/prefix1/programIcon");
    setWindowIcon(*programIcon);

    // Table widget configuration
    ui->tableWidget->setColumnWidth(0, 229);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setStyleSheet("QTableView::item:selected { color: black; background: #e0f6ff; outline: none; }"
                                   "QTableView::item:focus { outline: none; border: none; }"
                                   "QTableView:focus {outline: none; }");
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(tableCustomMenuRequested(QPoint)));

    // Open file from explorer
    if (QCoreApplication::arguments().size() > 1)
    {
        QStringList arg = QCoreApplication::arguments();
        arg.removeAt(0);
        processFileList(arg);
    }

    // Configuration of context menu for table widget
    contextMenu = new QMenu(this);
    chemcraftIcon = new QIcon(":/new/prefix1/chemcraftIcon");
    explorerIcon = new QIcon(":/new/prefix1/explorerIcon");
    runSingleTask = new QAction("Run this task", this);
    addSingleTask = new QAction("Add this task to queue", this);
    deleteTask = new QAction("Delete this task", this);
    openInpInChemcraft = new QAction(*chemcraftIcon, "Open file in Chemcraft", this);
    openInpInExplorer = new QAction(*explorerIcon, "Show file in explorer", this);

    connect(runSingleTask, SIGNAL(triggered()),
            this, SLOT(runSelectedTask()));
    connect(addSingleTask, SIGNAL(triggered()),
            this, SLOT(addSelectedTask()));
    connect(deleteTask, SIGNAL(triggered()),
            this, SLOT(deleteSelectedTask()));
    connect(openInpInChemcraft, SIGNAL(triggered()),
            this, SLOT(openSelectedFileInChemcraft()));
    connect(openInpInExplorer, SIGNAL(triggered()),
            this, SLOT(openSelectedFileInExplorer()));

    // Hot keys
    keyDelete = new QShortcut(this);
    keyDelete->setKey(Qt::Key_Delete);
    connect(keyDelete, SIGNAL(activated()), this, SLOT(slotShortcutDelete()));

    keyCtrlS = new QShortcut(this);
    keyCtrlS->setKey(Qt::CTRL + Qt::Key_S);
    connect(keyCtrlS, SIGNAL(activated()), this, SLOT(slotShortcutCtrlS()));

    keyCtrlAltS = new QShortcut(this);
    keyCtrlAltS->setKey(Qt::CTRL + Qt::ALT + Qt::Key_S);
    connect(keyCtrlAltS, SIGNAL(activated()), this, SLOT(slotShortcutCtrlAltS()));

    keyCtrlR = new QShortcut(this);
    keyCtrlR->setKey(Qt::CTRL + Qt::Key_R);
    connect(keyCtrlR, SIGNAL(activated()), this, SLOT(slotShortcutCtrlR()));

    keyCtrlO = new QShortcut(this);
    keyCtrlO->setKey(Qt::CTRL + Qt::Key_O);
    connect(keyCtrlO, SIGNAL(activated()), this, SLOT(slotShortcutCtrlO()));

    //
    updateButtonsAvailability();
    parseFileWithTemplates();
    configureContextMenu();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    if (!OrcaIsInProgress)
        event->accept();
    else
        QMessageBox::warning(this, "Warning", "All tasks must be completed");
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete chemcraftIcon;
    delete explorerIcon;
    delete infodialog;
    delete ui;
    qDebug()<< "Main destructor";
}

void MainWindow::saveSettings()
{
    settings->setValue("ORCA_PATH", orcaDir);
    settings->setValue("SUBL_PATH", sublDir);
    settings->setValue("CHEMCRAFT_PATH", chemcraftDir);
    settings->setValue("AVOGADRO_PATH", avogadroDir);
    settings->setValue("LAST_DIR", lastDir);
    settings->setValue("TEMPLATES_DIR", templatesFileDir);
    settings->setValue("showThreadNumberWarning", showThreadNumberWarning);
}

void MainWindow::loadSettings()
{
    orcaDir = settings->value("ORCA_PATH", "C:\\").toString();
    sublDir = settings->value("SUBL_PATH", "C:\\").toString();
    chemcraftDir = settings->value("CHEMCRAFT_PATH", "C:\\").toString();
    avogadroDir = settings->value("AVOGADRO_PATH", "C:\\").toString();
    lastDir = settings->value("LAST_DIR", "C:\\").toString();
    templatesFileDir = settings->value("TEMPLATES_DIR", "C:\\").toString();
    showThreadNumberWarning = settings->value("showThreadNumberWarning", true).toBool();
}

void MainWindow::processFileList(QStringList loadedFileNames)
{
    queue.fileList.append(loadedFileNames);
    ui->tableWidget->setRowCount(queue.fileList.size());

    for (; fileCounter < queue.fileList.size(); ++fileCounter)
    {
        QFileInfo fileInfo(queue.fileList.at(fileCounter));
        queue.fileNames.append(fileInfo.completeBaseName());
        queue.filePaths.append(fileInfo.absolutePath());
        queue.status.append(Queue::IN_QUEUE);
        queue.fileIsUnsaved.append(false);

        QFile file(queue.fileList.at(fileCounter));
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream in(&file);
        QString text = in.readAll();
        queue.fileBodies.append(text);
        file.close();

        queue.fileThread.append(QString::number(getThreadsNumberFromText(text)));

        QTableWidgetItem *nametableitem = new QTableWidgetItem(queue.fileNames.at(fileCounter));
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(queue.fileThread.at(fileCounter));
        threadstableitem->setTextAlignment(Qt::AlignCenter);
        if (showThreadNumberWarning)
            setIconForThreadsNumber(threadstableitem, nametableitem);
        ui->tableWidget->setItem(fileCounter, 0, nametableitem);
        ui->tableWidget->setItem(fileCounter, 1, threadstableitem);

        ui->tableWidget->setRowHeight(fileCounter, 25);
    }

    ui->tableWidget->setColumnWidth(0, (ui->tableWidget->rowCount() > 9) ? 210 : 214);
    ui->tableWidget->setColumnWidth(1, 100);

    lastDir = queue.filePaths.last();

    updateButtonsAvailability();

    if(showThreadNumberWarning && !threadNumberWarningWasShowed && tooManyThreads)
        showThreadNumberWarningMessageBox();
}

void MainWindow::orcaIsFinished()
{
    OrcaIsInProgress = false;
    updateButtonsAvailability();
}

void MainWindow::on_pushButton_clicked()        // клик по кнопке Load Files
{
    QStringList loadedFileNames = QFileDialog::getOpenFileNames(this, "Add files", lastDir, filter);    // записываем имена+пути открытых файлов

    if (loadedFileNames.isEmpty())
        return;

    processFileList(loadedFileNames);
}

void MainWindow::on_pushButton_2_clicked()                                                          // клик по кнопке Delete
{
    if (ui->tableWidget->currentRow() == -1)
        return;

    queue.removeQueueElementAt(ui->tableWidget->currentRow());
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    ui->plainTextEdit->clear();
    fileCounter--;
    updateButtonsAvailability();
    ui->tableWidget->clearSelection();
    emit ui->plainTextEdit->modificationChanged(false);

    ui->tableWidget->setColumnWidth(0, (ui->tableWidget->rowCount() > 9) ? 210 : 214);

    if (ui->tableWidget->rowCount() == 0)
    {
        ui->tableWidget->setColumnWidth(0, 229);
        ui->tableWidget->setColumnWidth(1, 100);
    }
}

void MainWindow::on_pushButton_3_clicked()                                                          // клик по кнопке Clear all
{
    if (ui->tableWidget->rowCount() == 0)
        return;

    ui->plainTextEdit->clear();
    queue.clearQueue();

    while (ui->tableWidget->rowCount())
        ui->tableWidget->removeRow(0);

    ui->tableWidget->setColumnWidth(0, 229);
    ui->tableWidget->setColumnWidth(1, 100);

    fileCounter = 0;
    updateButtonsAvailability();
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)                              // двойной клик по ячейке в таблице
{
    on_tableWidget_cellClicked(row, column);
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)                                    // клик по ячейке в таблице
{
    ui->plainTextEdit->setPlainText(queue.fileBodies.at(row));
    refreshTableStyle();
    ui->tableWidget->item(row, 0)->setFont(QFont("Segoe UI", 9, 75));
}

void MainWindow::on_pushButton_4_clicked()                                                          // клик по кнопке Move UP
{
    int current_row = ui->tableWidget->currentRow();

    if (current_row == -1) return;

    if (current_row > 0)
    {
        queue.interchangeElementsAt(current_row, current_row - 1);
        interchangeTableRows(current_row, current_row-1);
        refreshTableStyle();
        ui->tableWidget->item(current_row - 1, 0)->setFont(QFont("Segoe UI", 9, 75, false));
        ui->tableWidget->selectRow(current_row - 1);
    }
}

void MainWindow::on_pushButton_5_clicked()                                                          // клик по кнопке Move DOWN
{
    int current_row = ui->tableWidget->currentRow();

    if (current_row == -1) return;

    if (current_row < fileCounter-1)
    {
        queue.interchangeElementsAt(current_row, current_row + 1);
        interchangeTableRows(current_row, current_row + 1);
        refreshTableStyle();
        ui->tableWidget->item(current_row + 1, 0)->setFont(QFont("Segoe UI", 9, 75, false));
        ui->tableWidget->selectRow(current_row + 1);
    }
}

void MainWindow::on_actionSet_Orca_directory_triggered()
{
    QString selectedOrcaDir = QFileDialog::getOpenFileName(this, "Set path to orca.exe", orcaDir, "Executable files *.exe");

    if (selectedOrcaDir.isEmpty() || selectedOrcaDir.isNull())
        return;

    orcaDir = selectedOrcaDir;
    saveSettings();
}

void MainWindow::on_pushButton_9_clicked()                                                          // клик по книпке RUN
{
    if (!queue.fileList.length())                                                         // проверка на наличие в очереди файлов
    {
        QMessageBox::warning(this, "", "Queue is empty");
        return;
    }
    if (!orcaDir.contains("orca.exe"))                                           // проверка на наличие пути именно к orca.exe
    {
        QMessageBox::warning(this, "", "Please specify the path to orca.exe in the settings");
        return;
    }

    launchOrcaWithQueue(&queue);
}

void MainWindow::launchOrcaWithQueue(Queue *_queue)
{
    infodialog->launchProgram(*_queue);
    OrcaIsInProgress = true;
    updateButtonsAvailability();
    infodialog->show();                                                     // отображение информационного окна
    this->showMinimized();                                                  // свертывание главного окна
}

void MainWindow::on_pushButton_6_clicked()                                                          // клик по кнопке Save
{
    int current_row = ui->tableWidget->currentRow(); 

    if (current_row == -1)
    {
        QMessageBox::warning(this, "", "There is no file to save");
        return;
    }

    if (ui->plainTextEdit->toPlainText() == queue.fileBodies.at(current_row))
        return;

    QMessageBox::StandardButton confirmation;
    confirmation = QMessageBox::question(this,
                                         QString("Save %1.inp").arg(queue.fileNames.at(current_row)),
                                         QString("Are you sure you want to save changes in %1.inp?").arg(queue.fileNames.at(current_row)),
                                         QMessageBox::Yes|QMessageBox::No);
    if (confirmation == QMessageBox::Yes)
    {
        QFile file(queue.fileList.at(current_row));
        file.open(QFile::WriteOnly);
        QTextStream out(&file);
        QString actualText = ui->plainTextEdit->toPlainText();
        queue.fileBodies.replace(current_row, actualText);
        out << actualText;
        file.flush();
        file.close();

        queue.fileThread.replace(current_row, QString::number(getThreadsNumberFromText(actualText)));
        QTableWidgetItem *threadstableitem = new QTableWidgetItem(queue.fileThread.at(current_row));
        QTableWidgetItem *nametableitem = new QTableWidgetItem(queue.fileNames.at(current_row));
        threadstableitem->setTextAlignment(Qt::AlignCenter);
        if (showThreadNumberWarning)
            setIconForThreadsNumber(threadstableitem, nametableitem);
        ui->tableWidget->setItem(current_row, 0, nametableitem);
        ui->tableWidget->setItem(current_row, 1, threadstableitem);

        refreshTableStyle();
        ui->tableWidget->item(current_row, 0)->setFont(QFont("Segoe UI", 9, 75, false));

        if(showThreadNumberWarning && !threadNumberWarningWasShowed && tooManyThreads)
            showThreadNumberWarningMessageBox();
    }
}

void MainWindow::on_pushButton_8_clicked()                                                          // клик по кнопке Save as new
{
    QString actualText = ui->plainTextEdit->toPlainText();
    QString savingFileName;

    if (ui->tableWidget->rowCount() && (ui->tableWidget->currentRow() != -1))
    {
        QString fileName = queue.fileNames.at(ui->tableWidget->currentRow());
        savingFileName = QFileDialog::getSaveFileName(this, "Save file as", lastDir + "\\" + fileName, filter);
    }
    else
        savingFileName = QFileDialog::getSaveFileName(this, "Save file as", lastDir, filter);

    if(savingFileName.isEmpty() || savingFileName.isNull())
        return;

    QFile file(savingFileName);
    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    out << actualText;
    file.flush();
    file.close();

    queue.fileList.append(savingFileName);
    queue.fileBodies.append(actualText);
    queue.status.append(Queue::IN_QUEUE);
    queue.fileIsUnsaved.append(false);

    QFileInfo fileInfo(queue.fileList.at(fileCounter));
    queue.fileNames.append(fileInfo.completeBaseName());
    queue.filePaths.append(fileInfo.absolutePath());

    queue.fileThread.append(QString::number(getThreadsNumberFromText(actualText)));

    ui->tableWidget->setRowCount(queue.fileList.size());

    QTableWidgetItem *nametableitem = new QTableWidgetItem(queue.fileNames.at(fileCounter));
    QTableWidgetItem *threadstableitem = new QTableWidgetItem(queue.fileThread.at(fileCounter));

    threadstableitem->setTextAlignment(Qt::AlignCenter);
    if (showThreadNumberWarning)
        setIconForThreadsNumber(threadstableitem, nametableitem);

    ui->tableWidget->setItem(fileCounter, 0, nametableitem);
    ui->tableWidget->setItem(fileCounter, 1, threadstableitem);

    ui->tableWidget->setRowHeight(fileCounter, 25);
    ui->tableWidget->selectRow(fileCounter);

    ui->tableWidget->setColumnWidth(0, (ui->tableWidget->rowCount() > 9) ? 210 : 214);

    refreshTableStyle();
    ui->tableWidget->item(queue.fileList.length() - 1, 0)->setFont(QFont("Segoe UI", 9, 75, false));

    lastDir = queue.filePaths.last();

    fileCounter++;

    updateButtonsAvailability();

    if(showThreadNumberWarning && !threadNumberWarningWasShowed && tooManyThreads)
        showThreadNumberWarningMessageBox();
}

void MainWindow::on_actionAbout_triggered()                                                         // клик по кнопке About
{
    aboutdialog = new AboutDialog(this);
    aboutdialog->show();
}

void MainWindow::on_actionSet_path_to_Sublime_Text_triggered()
{
    QString sublimeTextDir;
    sublimeTextDir = QFileDialog::getOpenFileName(this, "Set path to Sublime Text 3", sublDir, "Executable files *.exe");

    if (sublimeTextDir.isEmpty())
        return;

    if (sublimeTextDir.contains("sublime_text.exe"))
    {
        QFileInfo sublDirInfo(sublimeTextDir);
        sublDir = sublDirInfo.absolutePath() + "/subl.exe";
        saveSettings();
    }
    else
        QMessageBox::warning(this, "", "This is not the Sublime Text");
}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    if (ui->comboBox->currentIndex())
    {
        ui->plainTextEdit->setPlainText(templateValue.at(ui->comboBox->currentIndex() - 1));
        ui->tableWidget->clearSelection();
        refreshTableStyle();
    }
}

void MainWindow::comboBoxFilling()
{
    ui->comboBox->clear();
    ui->comboBox->addItem("Select the template");

    for (int i = 0; i < templateKey.length(); i++)
    {
        ui->comboBox->addItem(templateKey.at(i));
    }
}

void MainWindow::on_actionSet_path_to_templates_dat_triggered()
{
    QString selectedDir = QFileDialog::getOpenFileName(this, "Set path to the file with templates", templatesFileDir, "Data files *.dat");

    if (selectedDir.isEmpty() || selectedDir.isNull())
        return;

    templatesFileDir = selectedDir;

    if (templatesFileDir.contains("templates.dat"))
    {
        parseFileWithTemplates();
        saveSettings();
    }
    else
    {
        QMessageBox::warning(this, "", "This is not the templates.dat");
        templatesFileDir = "C:\\";
    }
}

void MainWindow::parseFileWithTemplates()
{
    checkTemplatesManagerButton();

    QFile jsonFile(templatesFileDir);
    if(!jsonFile.open(QIODevice::ReadOnly))
        return;

    templateKey.clear();
    templateValue.clear();

    QByteArray saveData = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    QJsonObject jsonTemplatesObject = jsonDocument.object();
    QJsonArray jsonTemplatesArray = jsonTemplatesObject.value("templates").toArray();

    foreach (QJsonValue jVal, jsonTemplatesArray) {
        QJsonObject jTempl = jVal.toObject();
        templateKey.append(jTempl.value("title").toString());
        templateValue.append(jTempl.value("body").toString());
    }

    comboBoxFilling();
}

void MainWindow::checkTemplatesManagerButton()
{
    ui->toolButton->setDisabled(false);

    if (templatesFileDir == "C:\\")
        ui->toolButton->setDisabled(true);
}

void MainWindow::on_toolButton_clicked()            // Клик по кнопке Templates Manager
{
    templatesmanager = new TemplatesManager(this);

    connect(templatesmanager, SIGNAL(windowIsClosing()),
            this, SLOT(parseFileWithTemplates()));

    templatesmanager->show();
}

void MainWindow::on_actionSet_path_to_Chemcraft_triggered()
{
    QString selectedDir = QFileDialog::getOpenFileName(this, "Set path to the Chemcraft.exe", chemcraftDir, "Executable files *.exe");

    if (selectedDir.isEmpty() || selectedDir.isNull())
        return;

    if (selectedDir.toLower().contains("chemcraft.exe"))
    {
        chemcraftDir = selectedDir;
        saveSettings();
    }
    else
    {
        QMessageBox::warning(this, "", "This is not the Chemcraft.exe");
        chemcraftDir = "C:\\";
    }
}

void MainWindow::on_pushButton_7_clicked()          // Клик по кнопке Add to queue
{
    addTasksToQueue(&queue);
}

void MainWindow::addTasksToQueue(Queue *_queue)
{
    infodialog->addTasksToQueue(*_queue);
    this->showMinimized();
}

void MainWindow::updateButtonsAvailability()
{
    ui->pushButton_7->setDisabled(ui->tableWidget->rowCount() ? (OrcaIsInProgress ? false : true) : true);
    ui->pushButton_9->setDisabled(ui->tableWidget->rowCount() ? (OrcaIsInProgress ? true : false) : true);
}

void MainWindow::setIconForThreadsNumber(QTableWidgetItem *threadstableitem, QTableWidgetItem *nametableitem)
{
    if (threadstableitem->text().toInt() > avaliableThreads)
    {
        nametableitem->setIcon(QIcon(":/new/prefix1/warningIcon"));
        threadstableitem->setBackgroundColor(QColor(255, 255, 0, 127));
        threadstableitem->setToolTip("Too many threads");
        nametableitem->setToolTip("Too many threads");
        tooManyThreads = true;
    }
}

void MainWindow::showThreadNumberWarningMessageBox()
{
    QMessageBox *msgbox = new QMessageBox(this);
    QCheckBox *cb = new QCheckBox("Do not show this warning again");
    msgbox->setText("One of your .inp files seems to require more threads than this computer has.\nThis may lead to incorrect work.");
    msgbox->setWindowTitle("Warning");
    msgbox->setIcon(QMessageBox::Icon::Warning);
    msgbox->addButton(QMessageBox::Ok);
    msgbox->setCheckBox(cb);
    msgbox->exec();

    if (cb->isChecked())
        showThreadNumberWarning = false;

    threadNumberWarningWasShowed = true;
}

void MainWindow::interchangeTableRows(int row1, int row2)
{
    QTableWidgetItem *name1 = ui->tableWidget->takeItem(row1, 0);
    QTableWidgetItem *threads1 = ui->tableWidget->takeItem(row1, 1);

    QTableWidgetItem *name2 = ui->tableWidget->takeItem(row2, 0);
    QTableWidgetItem *threads2 = ui->tableWidget->takeItem(row2, 1);

    ui->tableWidget->setItem(row1, 0, name2);
    ui->tableWidget->setItem(row1, 1, threads2);

    ui->tableWidget->setItem(row2, 0, name1);
    ui->tableWidget->setItem(row2, 1, threads1);
}

void MainWindow::refreshTableStyle()
{
    for (int i = 0; i < queue.fileList.length(); i++)
    {
        ui->tableWidget->item(i, 0)->setFont(QFont("Segoe UI", 9, -1));
    }
}

short MainWindow::getThreadsNumberFromText(QString text)
{
    text = text.toLower();
    short i = 257;
    while(!(text.contains(QString("pal%1").arg(i)) || (text.contains(QString("nprocs %1\n").arg(i)) || text.contains(QString("nprocs %1 ").arg(i)))))
    {
        if (i == 1) break;
        i--;
    }

    //if (i == 257) return 1;

    return i;
}

void MainWindow::on_tableWidget_cellActivated(int row, int column)
{
    on_tableWidget_cellClicked(row, column);
}

void MainWindow::on_tableWidget_cellEntered(int row, int column)
{
    //on_tableWidget_cellClicked(row, column);
}

void MainWindow::on_tableWidget_cellPressed(int row, int column)
{
    on_tableWidget_cellClicked(row, column);
}

void MainWindow::slotShortcutDelete()
{
    if(ui->tableWidget->hasFocus())
        on_pushButton_2_clicked();
}

void MainWindow::slotShortcutCtrlS()
{
    if (ui->plainTextEdit->hasFocus())
        on_pushButton_6_clicked();
}

void MainWindow::slotShortcutCtrlAltS()
{
    if (ui->plainTextEdit->hasFocus())
        on_pushButton_8_clicked();
}

void MainWindow::slotShortcutCtrlR()
{
    if(ui->pushButton_9->isEnabled())
        on_pushButton_9_clicked();
}

void MainWindow::slotShortcutCtrlO()
{
    on_pushButton_clicked();
}

void MainWindow::tableCustomMenuRequested(QPoint pos)
{
    if (!ui->tableWidget->rowCount()) return;

    runSingleTask->setEnabled(ui->pushButton_9->isEnabled());
    addSingleTask->setEnabled(ui->pushButton_7->isEnabled());

    if (ui->tableWidget->itemAt(pos))
        contextMenu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::configureContextMenu()
{
    contextMenu->addAction(runSingleTask);
    contextMenu->addAction(addSingleTask);
    contextMenu->addAction(deleteTask);
    contextMenu->addSeparator();
    contextMenu->addAction(openInpInChemcraft);
    contextMenu->addAction(openInpInExplorer);
}

void MainWindow::runSelectedTask()
{
    Queue _queue = queue.getOneElementByName(ui->tableWidget->currentItem()->text());
    launchOrcaWithQueue(&_queue);
}

void MainWindow::addSelectedTask()
{
    Queue _queue = queue.getOneElementByName(ui->tableWidget->currentItem()->text());
    addTasksToQueue(&_queue);
}

void MainWindow::deleteSelectedTask()
{
    on_pushButton_2_clicked();
}

void MainWindow::openSelectedFileInChemcraft()
{
    if (!chemcraftDir.toLower().contains("chemcraft.exe"))
    {
        QMessageBox::warning(this, "", "Please specify the path to the Chemcraft in the settings");
        return;
    }

    short current_row = ui->tableWidget->currentRow();

    QStringList arguments;
    arguments.append(queue.fileList.at(current_row));

    QProcess *process = new QProcess(this);
    process->setArguments(arguments);
    process->setProgram(chemcraftDir);
    process->start();
}

void MainWindow::openSelectedFileInExplorer()
{
    short current_row = ui->tableWidget->currentRow();

    QStringList args;
    args << "/select," << QDir::toNativeSeparators(queue.fileList.at(current_row));

    QProcess *process = new QProcess(this);
    process->start("explorer.exe", args);
}

void MainWindow::on_actionSet_path_to_Avogadro_triggered()
{
    QString selectedDir = QFileDialog::getOpenFileName(this, "Set path to the Avogadro.exe", avogadroDir, "Executable files *.exe");

    if (selectedDir.isEmpty() || selectedDir.isNull())
        return;

    if (selectedDir.toLower().contains("avogadro.exe"))
    {
        avogadroDir = selectedDir;
        saveSettings();
    }
    else
    {
        QMessageBox::warning(this, "", "This is not the Avogadro.exe");
        avogadroDir = "C:\\";
    }
}
