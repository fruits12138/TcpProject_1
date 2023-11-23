#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>


Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDelFriendPB = new QPushButton("DEL FRIEND");
    m_pFlushFriendPB = new QPushButton("FLUSH FRIEND");
    m_pShowOnlineUsrPB = new QPushButton("SHOW ONLINE FRIEND");
    m_pSearchUsrPB = new QPushButton("SEARCH USR");
    m_pMsgSendPB = new QPushButton("Msg Send");
    m_pPrivateChatPB = new QPushButton("Private Chat");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUsr()));

    connect(m_pFlushFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFriend()));

    connect(m_pDelFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(delFriend()));
}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    uint uiSize = pdu->uiMsgLen/32;
    qDebug()<<"name size::"<<uiSize;
    char caName[32] = {'\0'};
    m_pFriendListWidget->clear();
    for(uint i=0;i<uiSize;i++){
        memcpy(caName,(char*)pdu->caMsg+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else{
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    m_strSearchName = QInputDialog::getText(this,"Search","Usr Name:");
    if(!m_strSearchName.isEmpty()){
        qDebug()<<m_strSearchName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;


        strncpy(pdu->caData,m_strSearchName.toStdString().c_str(),32);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;


    }
    else{

    }
}

void Friend::flushFriend()
{
    QString strName  = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    m_pFriendListWidget->currentItem();
}
