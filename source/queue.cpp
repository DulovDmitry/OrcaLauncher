#include "queue.h"

Queue::Queue(QWidget *parent)
{
    qDebug()<< "Queue constructor";
}

Queue::~Queue()
{
    qDebug()<< "Queue destructor";
}

void Queue::addElementsInQueue(const Queue &q)
{
    for (int i = 0; i < q.fileList.size(); i++)
    {
        this->fileList.append(q.fileList.at(i));
        this->fileNames.append(q.fileNames.at(i));
        this->filePaths.append(q.filePaths.at(i));
        this->fileBodies.append(q.fileBodies.at(i));
        this->fileThread.append(q.fileThread.at(i));
        this->status.append(q.status.at(i));
        this->fileIsUnsaved.append(false);
    }
}

void Queue::removeQueueElementAt(int numberOfElement)
{
    this->fileList.removeAt(numberOfElement);
    this->fileNames.removeAt(numberOfElement);
    this->filePaths.removeAt(numberOfElement);
    this->fileBodies.removeAt(numberOfElement);
    this->fileThread.removeAt(numberOfElement);
    this->status.removeAt(numberOfElement);
    this->fileIsUnsaved.removeAt(numberOfElement);
}

void Queue::interchangeElementsAt(int numberOfFirstElement, int numberOfSecondElement)
{
    this->fileList.move(numberOfFirstElement, numberOfSecondElement);
    this->fileNames.move(numberOfFirstElement, numberOfSecondElement);
    this->filePaths.move(numberOfFirstElement, numberOfSecondElement);
    this->fileBodies.move(numberOfFirstElement, numberOfSecondElement);
    this->fileThread.move(numberOfFirstElement, numberOfSecondElement);
    this->status.move(numberOfFirstElement, numberOfSecondElement);
    this->fileIsUnsaved.move(numberOfFirstElement, numberOfSecondElement);
}

void Queue::clearQueue()
{
    this->fileList.clear();
    this->fileNames.clear();
    this->filePaths.clear();
    this->fileBodies.clear();
    this->fileThread.clear();
    this->status.clear();
    this->fileIsUnsaved.clear();
}

Queue Queue::getOneElementByName(QString filename)
{
    Queue element;
    int index = 0;

    for (; index < fileNames.size(); index++)
        if (this->fileNames.at(index) == filename)
            break;

    element.fileList.append(this->fileList.at(index));
    element.fileNames.append(this->fileNames.at(index));
    element.filePaths.append(this->filePaths.at(index));
    element.fileBodies.append(this->fileBodies.at(index));
    element.fileThread.append(this->fileThread.at(index));
    element.status.append(this->status.at(index));
    element.fileIsUnsaved.append(this->fileIsUnsaved.at(index));

    return element;
}

QString Queue::getListOfFileNames()
{
    QString string;
    for(int i = 0; i < this->fileNames.size(); i++)
    {
        string += QString("%1").arg(i+1) + ". " + this->fileNames.at(i) + ".inp\n";
    }
    return string;
}
