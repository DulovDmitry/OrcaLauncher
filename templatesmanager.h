#ifndef TEMPLATESMANAGER_H
#define TEMPLATESMANAGER_H

#include <QDialog>
#include <QString>
#include <QSettings>
#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
class TemplatesManager;
}

class TemplatesManager : public QDialog
{
    Q_OBJECT

public:
    explicit TemplatesManager(QWidget *parent = 0);
    ~TemplatesManager();

public slots:
    void parseJsonFile();

    void fillListWidget();

    int indexOfKey(QString requestedKey);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void on_plainTextEdit_textChanged();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void windowIsClosing();

protected slots:
    void closeEvent(QCloseEvent *event);

private:
    Ui::TemplatesManager *ui;
    QSettings *settings;
    QString templatesFileDir;
    QStringList key;
    QStringList value;
};

#endif // TEMPLATESMANAGER_H
