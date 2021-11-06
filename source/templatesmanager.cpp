#include "templatesmanager.h"
#include "ui_templatesmanager.h"

TemplatesManager::TemplatesManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemplatesManager)
{
    qDebug()<< "TemplatesManager constructor";
    ui->setupUi(this);

    setWindowTitle("OrcaLaucher [Templates manager]");
    QIcon *programIcon = new QIcon(":/new/prefix1/programIcon");
    setWindowIcon(*programIcon);

    settings = new QSettings("ORG335a", "OrcaLauncher", this);
    templatesFileDir = settings->value("TEMPLATES_DIR", "C:\\").toString();

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->listWidget->setFocus();

    setWindowModality(Qt::WindowModal);

    parseJsonFile();
    fillListWidget();
}

TemplatesManager::~TemplatesManager()
{
    qDebug()<< "TemplatesManager destructor";
    delete ui;
}

void TemplatesManager::closeEvent(QCloseEvent *event)
{
    QJsonObject jsonTemplates;
    QJsonArray textsArray;

    for (int i = 0; i < key.length(); i++)
    {
        QJsonObject textObject;
        textObject["title"] = key.at(i);
        textObject["body"] = value.at(i);
        textsArray.append(textObject);
    }

    jsonTemplates["templates"] = textsArray;

    QFile jsonFile(templatesFileDir);
    if (!jsonFile.open(QIODevice::WriteOnly))
        return;

    jsonFile.write(QJsonDocument(jsonTemplates).toJson(QJsonDocument::Indented));
    jsonFile.close();

    emit windowIsClosing();
}

void TemplatesManager::parseJsonFile()
{
    QFile jsonFile(templatesFileDir);
    if(!jsonFile.open(QIODevice::ReadOnly))
        return;

    QByteArray saveData = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    QJsonObject jsonTemplatesObject = jsonDocument.object();
    QJsonArray jsonTemplatesArray = jsonTemplatesObject.value("templates").toArray();

    foreach (QJsonValue jVal, jsonTemplatesArray) {
        QJsonObject jTempl = jVal.toObject();
        key.append(jTempl.value("title").toString());
        value.append(jTempl.value("body").toString());
    }
}

void TemplatesManager::fillListWidget()
{
    for (int i = 0; i < key.length(); i++)
    {
        ui->listWidget->addItem(key.at(i));
    }
}

int TemplatesManager::indexOfKey(QString requestedKey)
{
    for (int i = 0; i < key.length(); i++)
    {
        if (key.at(i) == requestedKey)
            return i;
    }

    return 0;
}

void TemplatesManager::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->plainTextEdit->setPlainText(value.at(indexOfKey(item->text())));
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

void TemplatesManager::on_pushButton_clicked()
{
    key.removeAt(ui->listWidget->currentRow());
    value.removeAt(ui->listWidget->currentRow());
    ui->listWidget->clear();
    ui->plainTextEdit->clear();
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    fillListWidget();
}

void TemplatesManager::on_plainTextEdit_textChanged()
{
    ui->pushButton_2->setEnabled(true);
}

void TemplatesManager::on_pushButton_2_clicked()
{
    value.replace(ui->listWidget->currentRow(), ui->plainTextEdit->toPlainText());
    ui->pushButton_2->setEnabled(false);
    ui->listWidget->setFocus();
}

void TemplatesManager::on_pushButton_3_clicked()
{
    if (ui->lineEdit->text().isEmpty() || ui->lineEdit->text().isNull())
    {
        QMessageBox::warning(this, "Templates Manager", "The title is empty");
        return;
    }

    if (ui->plainTextEdit_2->toPlainText().isEmpty() || ui->plainTextEdit_2->toPlainText().isNull())
    {
        QMessageBox::warning(this, "Templates Manager", "The template body is empty");
        return;
    }

    key.append(ui->lineEdit->text());
    value.append(ui->plainTextEdit_2->toPlainText());
    ui->lineEdit->clear();
    ui->plainTextEdit_2->clear();
    ui->listWidget->clear();
    ui->pushButton_2->setEnabled(false);
    fillListWidget();
}
