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

    Queue getOneElementByName(QString filename);

    QString getListOfFileNames();

    QStringList fileList;
    QStringList fileNames;
    QStringList filePaths;
    QStringList fileBodies;
    QStringList fileBodiesChanged;
    QStringList fileThread;

    enum Status
    {
        IN_QUEUE,    // == 0
        IN_PROGRESS, // == 1
        COMPLETED,   // == 2
        ABORTED      // == 3
    };

    QVector<Status> status;

    int currentProcessID;

    QVector<bool> fileIsUnsaved;
};

#endif // QUEUE_H
