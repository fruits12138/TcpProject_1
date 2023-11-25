#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h"
#include "friend.h"
#include "privatechat.h"


TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(500,300);
    loadConfig();
    connect(&m_tcpSocket,SIGNAL(connected())
            ,this,SLOT(showConnect()));//对当前对象（connected信号）实施对应的槽函数

    connect(&m_tcpSocket,SIGNAL(readyRead())
            ,this,SLOT(recvMsg()));


    //链接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();

        strData.replace("\r\n"," ");
        file.close();


        QStringList stringList = strData.split(" ");

        m_strIP = stringList.at(0);
        m_usPort = stringList.at(1).toUShort();
        qDebug()<<"IP:"<<m_strIP<<"Port:"<<m_usPort;
    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"connected server","connected success");
}

/**
* @brief TcpClient类的recvMsg函数，用于接收消息
*/
void TcpClient::recvMsg()
{
   qDebug()<<m_tcpSocket.bytesAvailable(); // 输出可用字节数

   uint uiPDULen = 0; // 消息长度
   m_tcpSocket.read((char*)&uiPDULen,sizeof(uint)); // 读取消息长度

   uint uiMsgLen = uiPDULen - sizeof(PDU); // 消息总长度
   PDU* pdu = mkPDU(uiMsgLen); // 创建PDU结构体指针
   m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen); // 读取消息内容

   switch (pdu->uiMsgType) // 根据消息类型进行处理
   {
   case ENUM_MSG_TYPE_REGISTER_RESPONSE:
   {
       if(0 == strcmp(pdu->caData,REGIST_OK)){ // 如果返回注册成功
           QMessageBox::information(this,"regist_ok",REGIST_OK); // 弹出注册成功信息提示框
       }
       else if (0 == strcmp(pdu->caData,REGIST_FAILED)){ // 如果返回注册失败
           QMessageBox::information(this,"regist_failed",REGIST_FAILED); // 弹出注册失败信息提示框
       }
//        qDebug()<<"Name:"<<(char*)caName;
//        qDebug()<<"Pwd:"<<(char*)caPwd;
//        qDebug()<<"Msg_Type:"<<pdu->uiMsgType;
       break;
   }
   case ENUM_MSG_TYPE_LOGIN_RESPONSE:
   {
       if(0 == strcmp(pdu->caData,LOGIN_OK)){ // 如果返回登录成功
           QMessageBox::information(this,"login",LOGIN_OK); // 弹出登录成功信息提示框
           OpeWidget::getInstance().show(); // 显示操作控件
           hide(); // 隐藏当前窗口
       }
       else if (0 == strcmp(pdu->caData,LOGIN_FAILED)){ // 如果返回登录失败
           QMessageBox::warning(this,"login",LOGIN_FAILED); // 弹出登录失败信息提示框
       }
//        qDebug()<<"Name:"<<(char*)caName;
//        qDebug()<<"Pwd:"<<(char*)caPwd;
//        qDebug()<<"Msg_Type:"<<pdu->uiMsgType;
       break;
   }
   case ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE:
   {
       OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);

       break;
   }
   case ENUM_MSG_TYPE_ALL_USR_RESPONSE:
   {
       OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);

       break;
   }
   case ENUM_MSG_TYPE_SEARCH_USR_RESPONSE:
   {


//       qDebug()<<(char*)pdu->caData;

//       qDebug()<<(0==strcmp(SEARCH_USR_OFFLINE,pdu->caData));



       if(0==strcmp(SEARCH_USR_NO,pdu->caData)){
           QMessageBox::information(this,"search",QString("%1:not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));

       }else if(0==strcmp(SEARCH_USR_ONLINE,pdu->caData)){
           QMessageBox::information(this,"search",QString("%1:online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));


       }else if(0==strcmp(SEARCH_USR_OFFLINE,pdu->caData)){
           QMessageBox::information(this,"search",QString("%1:offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));

       }
       break;
   }
   case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
   {
       char caName[32] ={"/0"};
       strncpy(caName,pdu->caData+32,32);

       int ret = QMessageBox::information(this,"Add Friend Request",QString("%1 want to add you as a friend?").arg(caName)
                                ,QMessageBox::Ok,QMessageBox::No);

       PDU* respdu = mkPDU(0);

       memcpy(respdu->caData,pdu->caData,64);
       if(ret == QMessageBox::Ok){
           respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
       }else if (ret == QMessageBox::No) {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
       }

       m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
       free(respdu);
       respdu = NULL;
       break;
   }
   case ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE:
   {

       if(0==strcmp(ADD_FRIEND_AGREE,pdu->caData)){
           QMessageBox::information(this,"Agree",QString("he/she agree your request"));
       }else if (0==strcmp(ADD_FRIEND_REFUSE,pdu->caData)) {
           QMessageBox::information(this,"Refuse",QString("he/she reject your request"));
        }

       break;
   }
   case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE:
   {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);

       break;
   }
   case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
   {
       char caName[32] = {'\0'};
       memcpy(caName,pdu->caData,32);
       QMessageBox::information(this,"Delete Friend",QString("%1 delete you as his friend").arg(caName));
       break;
   }
   case ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE:
   {
       QMessageBox::information(this,"Delete Friend","DELETE FRIEND OK");
       break;
   }
   case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
   {
       if(PrivateChat::getInstance().isHidden()){
           char caSendName[32] = {'\0'};
           memcpy(caSendName,pdu->caData,32);
           QString strSendName = caSendName;
           PrivateChat::getInstance().setChatName(strSendName);
           PrivateChat::getInstance().show();

       }
       PrivateChat::getInstance().updateMsg(pdu);
       break;
   }
   case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
   {
       OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);


       break;
   }
   default:
   {
       break;
   }
   free(pdu); // 释放内存
   pdu = NULL; // 指针置空
    }
}



#if 0
void TcpClient::on_send_pd_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if(strMsg!=NULL){
        PDU* pdu = mkPDU(strMsg.size());
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}
#endif

void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty()){
        m_strLoginName = strName;

        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::critical(this,"login failed","login failed,name or pwd is null");
    }

}


void TcpClient::on_regis_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty()){
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::critical(this,"regist failed","regist failed,name or pwd is null");
    }

}


void TcpClient::on_cancel_pb_clicked()
{

}




void TcpClient::on_pwd_le_returnPressed()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty()){
        m_strLoginName = strName;

        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::critical(this,"login failed","login failed,name or pwd is null");
    }

}
