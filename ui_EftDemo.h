/********************************************************************************
** Form generated from reading UI file 'EftDemo.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EFTDEMO_H
#define UI_EFTDEMO_H

#include <QtCore/QVariant>
#include <QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "roboxcomwidget.h"

QT_BEGIN_NAMESPACE

class Ui_EftDemoClass
{
public:
    QAction *action_About;
    QAction *action_Quit;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    RoboxComWidget *tabController1;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_H;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *EftDemoClass)
    {
        if (EftDemoClass->objectName().isEmpty())
            EftDemoClass->setObjectName(QStringLiteral("EftDemoClass"));
        EftDemoClass->resize(1024, 768);
        EftDemoClass->setMinimumSize(QSize(1024, 768));
        EftDemoClass->setStyleSheet(QStringLiteral("font: 12px \"Arial\";"));
        action_About = new QAction(EftDemoClass);
        action_About->setObjectName(QStringLiteral("action_About"));
        action_Quit = new QAction(EftDemoClass);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        centralWidget = new QWidget(EftDemoClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(5);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabController1 = new RoboxComWidget();
        tabController1->setObjectName(QStringLiteral("tabController1"));
        tabWidget->addTab(tabController1, QString());

        verticalLayout->addWidget(tabWidget);

        EftDemoClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(EftDemoClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_H = new QMenu(menuBar);
        menu_H->setObjectName(QStringLiteral("menu_H"));
        EftDemoClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(EftDemoClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        EftDemoClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(EftDemoClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        EftDemoClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_H->menuAction());
        menu->addAction(action_Quit);
        menu_H->addAction(action_About);

        retranslateUi(EftDemoClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(EftDemoClass);
    } // setupUi

    void retranslateUi(QMainWindow *EftDemoClass)
    {
        EftDemoClass->setWindowTitle(QApplication::translate("EftDemoClass", "EftDemo", Q_NULLPTR));
        action_About->setText(QApplication::translate("EftDemoClass", "\345\205\263\344\272\216(&A)", Q_NULLPTR));
        action_Quit->setText(QApplication::translate("EftDemoClass", "\351\200\200\345\207\272(&Q)", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabController1), QApplication::translate("EftDemoClass", "controller1", Q_NULLPTR));
        menu->setTitle(QApplication::translate("EftDemoClass", "\346\226\207\344\273\266(&F)", Q_NULLPTR));
        menu_H->setTitle(QApplication::translate("EftDemoClass", "\345\270\256\345\212\251(&H)", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EftDemoClass: public Ui_EftDemoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EFTDEMO_H
