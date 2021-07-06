#ifndef QUEUE_H
#define QUEUE_H

#include <QStringList>
#include <QVector>
#include <QtDebug>

class Queue
{
public:
    Queue(QWidget *parent = 0);
    ~Queue();

    void addElementsInQueue(const Queue &q);

    void removeQueueElementAt(int numberOfElement);

    void interchangeElementsAt(int numberOfFirstElement, int numberOfSecondElement);

    void clearQueue();

    QString getListOfFileNames();

    QStringList fileList;
    QStringList fileNames;
    QStringList filePaths;
    QStringList fileBodies;
    QStringList fileBodiesChanged;
    QStringList fileThread;

    QVector<int> status;    // 0 - in queue
                            // 1 - in progress
                            // 2 - completed
                            // 3 - aborted

    int currentProcessID;
};

#endif // QUEUE_H
