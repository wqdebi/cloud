/********************************************************************************
** Form generated from reading UI file 'online.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINE_H
#define UI_ONLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Online
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QListWidget *online_lw;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *addFriend_pb;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *Online)
    {
        if (Online->objectName().isEmpty())
            Online->setObjectName(QString::fromUtf8("Online"));
        Online->resize(357, 300);
        verticalLayout_2 = new QVBoxLayout(Online);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        online_lw = new QListWidget(Online);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        online_lw->setObjectName(QString::fromUtf8("online_lw"));

        horizontalLayout->addWidget(online_lw);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        addFriend_pb = new QPushButton(Online);
        addFriend_pb->setObjectName(QString::fromUtf8("addFriend_pb"));

        verticalLayout->addWidget(addFriend_pb);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(Online);

        QMetaObject::connectSlotsByName(Online);
    } // setupUi

    void retranslateUi(QWidget *Online)
    {
        Online->setWindowTitle(QCoreApplication::translate("Online", "Form", nullptr));

        const bool __sortingEnabled = online_lw->isSortingEnabled();
        online_lw->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = online_lw->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("Online", "1", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = online_lw->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("Online", "2", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = online_lw->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("Online", "3", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = online_lw->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("Online", "4", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = online_lw->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("Online", "5", nullptr));
        online_lw->setSortingEnabled(__sortingEnabled);

        addFriend_pb->setText(QCoreApplication::translate("Online", "\345\212\240\345\245\275\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Online: public Ui_Online {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINE_H
