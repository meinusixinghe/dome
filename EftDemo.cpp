#include "EftDemo.h"
#include "stdafx.h"

EftDemo::EftDemo(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // ui.mainToolBar->hide();
    actAdd = new QAction(QIcon(":/EftDemo/image/add.png"), tr("添加(&A)"), this);
    actDelete = new QAction(QIcon(":/EftDemo/image/delete.png"), tr("删除(&D)"), this);
    ui.mainToolBar->addAction(actAdd);
    ui.mainToolBar->addAction(actDelete);
    ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // 设置文字图标下
    actDelete->setEnabled(false);
    lastTabIndex = 1;
    connect(actAdd, SIGNAL(triggered()), this, SLOT(addSlot()));
    connect(actDelete, SIGNAL(triggered()), this, SLOT(deleteSlot()));

    connect(ui.action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui.action_About, &QAction::triggered, this, [=]() {

        QMessageBox::about(this, "关于 EftDemo", "         EftSDK Demo, V1.0\nEfort 2022, All Rights Reserved.");

        });

}

void EftDemo::addSlot()
{
    QString tabName = QString("controller%1").arg(++lastTabIndex);
    ui.tabWidget->addTab(new RoboxComWidget(), tabName);
    if (ui.tabWidget->count() > 1)
    {
        actDelete->setEnabled(true);
    }
}

void EftDemo::deleteSlot()
{
    if (ui.tabWidget->count() <= 1)
    {
        actDelete->setDisabled(true);
        return;
    }
    QWidget *curQWidget = ui.tabWidget->currentWidget();
    delete curQWidget;
    curQWidget = nullptr;
}
