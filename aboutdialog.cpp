#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::WindowModal);
    setWindowTitle("About OrcaLauncher");

    QPixmap icon(":/new/prefix1/programIcon");
    ui->programIconLabel->setPixmap(icon.scaled(ui->programIconLabel->width() ,ui->programIconLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui->label_2->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    ui->label_2->setOpenExternalLinks(true);
    ui->label_2->setTextFormat(Qt::MarkdownText);
    ui->label_2->setText("This is an open source project designed to simplify communication with "
                         "<a href=\"https://orcaforum.kofo.mpg.de/app.php/portal\">ORCA quantum chemistry package</a>\n\n"
                         "OrcaLauncher doesn't support automatic updates. Latest version of the program is available on <a href=\"https://github.com/DulovDmitry/OrcaLauncher\">project's GitHub page</a>\n\n"
                         "The program is distributed under the terms of the GNU General Public License \n\n"
                         "Author: Dmitry Dulov, dulov.dmitry@gmail.com");

}

AboutDialog::~AboutDialog()
{
    delete ui;
    qDebug() << "AboutDialog destructor";
}
