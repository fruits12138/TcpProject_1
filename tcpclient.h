#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpServer>
#include<QTcpSocket>
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();
    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();//接口获得

public slots:
    void showConnect();
    void recvMsg();
private slots:
//    void on_send_pd_clicked();

    void on_login_pb_clicked();

    void on_regis_pb_clicked();

    void on_cancel_pb_clicked();

    void on_pwd_le_returnPressed();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    //链接服务器，与服务器进行数据交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
};
#endif // TCPCLIENT_H
