#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>

#include "friend.h"
#include "book.h"
#include <QStackedWidget>

class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget &getInstance();
    Friend *getFriend();
    Book *getBook();

signals:

public slots:

private:
    QListWidget *m_pListWidget;

    Friend *m_pFriend;
    Book *m_pBook;

    QStackedWidget *m_pSW;

    QLabel *m_pQL;
};

#endif // OPEWIDGET_H
