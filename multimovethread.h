#ifndef MULTIMOVETHREAD_H
#define MULTIMOVETHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>

class MultiMoveThread : public QObject
{
    Q_OBJECT
public:
    MultiMoveThread();

    void start();

protected slots:
    void process();

signals:
    void bstart();
    void isDone();
};

#endif // MULTIMOVETHREAD_H
