#include "book.h"

Book::Book(QWidget *parent) : QWidget(parent)
{
    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("Return");
    m_pCreateDirPB = new QPushButton("CreateDir");
    m_pDelDirPB = new QPushButton("DelDir");
    m_pRenamePB = new QPushButton("Rename");
    m_pFlushPB = new QPushButton("Flush");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenamePB);
    pDirVBL->addWidget(m_pFlushPB);



    m_pUploadPB = new QPushButton("Upload");
    m_pDownLoadPB = new QPushButton("DownLoad");
    m_pDelFilePB = new QPushButton("DelFile");
    m_pShareFielPB = new QPushButton("Share");

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFielPB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);
}
