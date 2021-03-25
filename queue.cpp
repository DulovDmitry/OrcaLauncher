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
}

void Queue::interchangeElementsAt(int numberOfFirstElement, int numberOfSecondElement)
{
    this->fileList.move(numberOfFirstElement, numberOfSecondElement);
    this->fileNames.move(numberOfFirstElement, numberOfSecondElement);
    this->filePaths.move(numberOfFirstElement, numberOfSecondElement);
    this->fileBodies.move(numberOfFirstElement, numberOfSecondElement);
    this->fileThread.move(numberOfFirstElement, numberOfSecondElement);
    this->status.move(numberOfFirstElement, numberOfSecondElement);
}

void Queue::clearQueue()
{
    this->fileList.clear();
    this->fileNames.clear();
    this->filePaths.clear();
    this->fileBodies.clear();
    this->fileThread.clear();
    this->status.clear();
}
