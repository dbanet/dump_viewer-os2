/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun Dec 1 06:24:52 2013
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *unpackAction;
    QAction *exitAction;
    QAction *selectAction;
    QAction *base32Action;
    QAction *cp1251Action;
    QAction *selectDescriptionAction;
    QAction *action_copy_rutracker_link;
    QAction *action_open_rutracker;
    QAction *action_copy_magnet;
    QAction *action_open_magnet;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *patternLineEdit;
    QLabel *label_2;
    QSpinBox *limitSpinBox;
    QPushButton *searchButton;
    QPushButton *stopButton;
    QVBoxLayout *resultsVerticalLayout;
    QTableWidget *resultsTableWidget;
    QWebView *descriptionWebView;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(646, 455);
        unpackAction = new QAction(MainWindow);
        unpackAction->setObjectName(QString::fromUtf8("unpackAction"));
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));
        selectAction = new QAction(MainWindow);
        selectAction->setObjectName(QString::fromUtf8("selectAction"));
        base32Action = new QAction(MainWindow);
        base32Action->setObjectName(QString::fromUtf8("base32Action"));
        base32Action->setCheckable(true);
        cp1251Action = new QAction(MainWindow);
        cp1251Action->setObjectName(QString::fromUtf8("cp1251Action"));
        cp1251Action->setCheckable(true);
        selectDescriptionAction = new QAction(MainWindow);
        selectDescriptionAction->setObjectName(QString::fromUtf8("selectDescriptionAction"));
        action_copy_rutracker_link = new QAction(MainWindow);
        action_copy_rutracker_link->setObjectName(QString::fromUtf8("action_copy_rutracker_link"));
        action_open_rutracker = new QAction(MainWindow);
        action_open_rutracker->setObjectName(QString::fromUtf8("action_open_rutracker"));
        action_copy_magnet = new QAction(MainWindow);
        action_copy_magnet->setObjectName(QString::fromUtf8("action_copy_magnet"));
        action_open_magnet = new QAction(MainWindow);
        action_open_magnet->setObjectName(QString::fromUtf8("action_open_magnet"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        patternLineEdit = new QLineEdit(centralWidget);
        patternLineEdit->setObjectName(QString::fromUtf8("patternLineEdit"));

        horizontalLayout->addWidget(patternLineEdit);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        limitSpinBox = new QSpinBox(centralWidget);
        limitSpinBox->setObjectName(QString::fromUtf8("limitSpinBox"));
        limitSpinBox->setMaximum(99999999);
        limitSpinBox->setValue(500);

        horizontalLayout->addWidget(limitSpinBox);

        searchButton = new QPushButton(centralWidget);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));

        horizontalLayout->addWidget(searchButton);

        stopButton = new QPushButton(centralWidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));

        horizontalLayout->addWidget(stopButton);

        horizontalLayout->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout);

        resultsVerticalLayout = new QVBoxLayout();
        resultsVerticalLayout->setSpacing(6);
        resultsVerticalLayout->setObjectName(QString::fromUtf8("resultsVerticalLayout"));
        resultsTableWidget = new QTableWidget(centralWidget);
        if (resultsTableWidget->columnCount() < 8)
            resultsTableWidget->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        resultsTableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        resultsTableWidget->setObjectName(QString::fromUtf8("resultsTableWidget"));
        resultsTableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

        resultsVerticalLayout->addWidget(resultsTableWidget);

        descriptionWebView = new QWebView(centralWidget);
        descriptionWebView->setObjectName(QString::fromUtf8("descriptionWebView"));
        descriptionWebView->setProperty("url", QVariant(QUrl("about:blank")));

        resultsVerticalLayout->addWidget(descriptionWebView);

        resultsVerticalLayout->setStretch(0, 1);
        resultsVerticalLayout->setStretch(1, 1);

        verticalLayout->addLayout(resultsVerticalLayout);

        verticalLayout->setStretch(1, 1);
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 646, 24));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QString::fromUtf8("menu_3"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(selectAction);
        menu->addAction(unpackAction);
        menu->addAction(selectDescriptionAction);
        menu->addAction(exitAction);
        menu_2->addAction(base32Action);
        menu_2->addAction(cp1251Action);
        menu_3->addAction(action_copy_rutracker_link);
        menu_3->addAction(action_open_rutracker);
        menu_3->addAction(action_copy_magnet);
        menu_3->addAction(action_open_magnet);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272 \321\202\320\276\321\200\321\200\320\265\320\275\321\202\320\276\320\262", 0, QApplication::UnicodeUTF8));
        unpackAction->setText(QApplication::translate("MainWindow", "\320\240\320\260\321\201\320\277\320\260\320\272\320\276\320\262\320\260\321\202\321\214 \320\261\320\260\320\267\321\203", 0, QApplication::UnicodeUTF8));
        exitAction->setText(QApplication::translate("MainWindow", "\320\222\321\213\321\205\320\276\320\264", 0, QApplication::UnicodeUTF8));
        selectAction->setText(QApplication::translate("MainWindow", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214 \320\261\320\260\320\267\321\203", 0, QApplication::UnicodeUTF8));
        base32Action->setText(QApplication::translate("MainWindow", "base32-\321\205\320\265\321\210\320\270", 0, QApplication::UnicodeUTF8));
        cp1251Action->setText(QApplication::translate("MainWindow", "\320\272\320\276\320\264\320\270\321\200\320\276\320\262\320\272\320\260 cp1251", 0, QApplication::UnicodeUTF8));
        selectDescriptionAction->setText(QApplication::translate("MainWindow", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214 \320\261\320\260\320\267\321\203 \320\276\320\277\320\270\321\201\320\260\320\275\320\270\320\271", 0, QApplication::UnicodeUTF8));
        action_copy_rutracker_link->setText(QApplication::translate("MainWindow", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214 \321\201\321\201\321\213\320\273\320\272\321\203 \320\275\320\260 Rutracker", 0, QApplication::UnicodeUTF8));
        action_open_rutracker->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214 \321\201\321\202\321\200\320\260\320\275\320\270\321\206\321\203 \320\275\320\260 Rutracker", 0, QApplication::UnicodeUTF8));
        action_copy_magnet->setText(QApplication::translate("MainWindow", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214 magnet", 0, QApplication::UnicodeUTF8));
        action_open_magnet->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214 magnet", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265 \320\262\320\272\320\273\321\216\321\207\320\260\320\265\321\202", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "\320\274\320\260\320\272\321\201.\320\275\320\260\321\205\320\276\320\264\320\276\320\272", 0, QApplication::UnicodeUTF8));
        searchButton->setText(QApplication::translate("MainWindow", "\320\235\320\260\320\271\321\202\320\270", 0, QApplication::UnicodeUTF8));
        stopButton->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = resultsTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "\320\275\320\276\320\274\320\265\321\200", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = resultsTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "\320\275\320\260\320\267\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = resultsTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "\321\200\320\260\320\267\320\274\320\265\321\200", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = resultsTableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "\321\201\320\270\320\264\321\213", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = resultsTableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "\320\277\320\270\321\200\321\213", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = resultsTableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "\321\205\320\265\321\210", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = resultsTableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "\320\267\320\260\320\263\321\200\321\203\320\267\320\276\320\272", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = resultsTableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "\320\276\320\261\320\275\320\276\320\262\320\273\320\265\320\275\320\276", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", 0, QApplication::UnicodeUTF8));
        menu_2->setTitle(QApplication::translate("MainWindow", "\320\222\320\270\320\264", 0, QApplication::UnicodeUTF8));
        menu_3->setTitle(QApplication::translate("MainWindow", "\320\242\320\276\321\200\321\200\320\265\320\275\321\202", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
