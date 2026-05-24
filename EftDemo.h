#ifndef EFTDEMO_H
#define EFTDEMO_H

#include <QtWidgets/QMainWindow>
#include "ui_EftDemo.h"

class EftDemo : public QMainWindow
{
    Q_OBJECT

public:
    EftDemo(QWidget *parent = Q_NULLPTR);

private:
    Ui::EftDemoClass ui;

    QAction *actAdd;
    QAction *actDelete;
    unsigned int lastTabIndex;
private slots:
    void addSlot();
    void deleteSlot();
};

#endif // EFTDEMO_H
