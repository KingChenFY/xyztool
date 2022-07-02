#include "frmmain.h"
#include "ui_frmmain.h"
#include "quihelperdata.h"
#include "quihelper.h"
#include <QMessageBox>
#include <QDebug>

frmMain::frmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initForm()
{
    //设置是否开启日志上下文打印比如行号、函数等
    SaveLog::Instance()->setUseContext(true);
    //设置文件存储目录
    SaveLog::Instance()->setPath(qApp->applicationDirPath() + "/" + QUIHelper::appName() + "log");
    //安装日志钩子
    SaveLog::Instance()->start();

    ui->tabWidget->setTabPosition(QTabWidget::West);

    XyzCtlFrm = new frmXyzControl;
    XyzPlotFrm = new frmXyzPlotShow;
    ui->tabWidget->addTab(XyzCtlFrm, "XYZ控制窗口");
    ui->tabWidget->addTab(XyzPlotFrm, "XYZ轨迹显示");

    gIsConnected = false;
    //实例化Socket对象并绑定网络信号槽
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
#endif

    m_xyzCurPos = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit, 0};
    m_xyzBound = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    textXyzCurPosAppend();
    textXyzCurBoundaryAppend();

    connect(&timerPosUpdate, SIGNAL(timeout()), this, SLOT(xyzReadCurPos()));
}

void frmMain::initConfig()
{

}

void frmMain::textXyzCurPosAppend()
{
    ui->textEdit_xyzCurPos->setPlainText(QString("X: %1\r\nY: %2\r\nZ: %3\r\nT: %4")\
                                         .arg(m_xyzCurPos.m_iX).arg(m_xyzCurPos.m_iY)\
                                         .arg(m_xyzCurPos.m_iZ).arg(m_xyzCurPos.m_postime));
}

void frmMain::textXyzCurBoundaryAppend()
{
    ui->textEdit_xyzBoundary->setPlainText(QString("X: %1 ~ %2\r\nY: %3 ~ %4\r\nZ: %5 ~ %6\r\n")\
                                   .arg(m_xyzBound.m_iXLeft).arg(m_xyzBound.m_iXRight).arg(m_xyzBound.m_iYIn)\
                                   .arg(m_xyzBound.m_iYOut).arg(m_xyzBound.m_iZDown).arg(m_xyzBound.m_iZUp));
}

void frmMain::connected()
{
    gIsConnected = true;
    ui->pushButton_connect->setText("断开主板");
    timerPosUpdate.setInterval(1000);
    timerPosUpdate.start();
//    append(0, "服务器连接");
//    append(0, QString("本地地址: %1  本地端口: %2").arg(socket->localAddress().toString()).arg(socket->localPort()));
//    append(0, QString("远程地址: %1  远程端口: %2").arg(socket->peerAddress().toString()).arg(socket->peerPort()));
}

void frmMain::disconnected()
{
    gIsConnected = false;
    ui->pushButton_connect->setText("连接主板");
//    append(1, "服务器断开");
}

void frmMain::error()
{
    qDebug() << socket->errorString();
}

void frmMain::readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

    quint8 cmdid;
    QByteArray cmdcontent;
    if(HardCmd::parseBoardResponse(data, cmdid, cmdcontent))
    {
        if(EnumBoardId_getXYZTaskInfo == cmdid)
        {
            getXYZTaskInfoParse(cmdcontent);
            textXyzCurPosAppend();
            textXyzCurBoundaryAppend();
            XyzPlotFrm->drawDataPoint(m_xyzCurPos);
            if(gIsXyzPosSave)
            {
                saveData();
            }
        }
    }
    else
    {
        qDebug() << "board response parse Err";
    }
}

void frmMain::saveData()
{
//    QDateTime now = QDateTime::currentDateTime();
//    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
//    QString fileName = QString("%1/%2.txt").arg(ui->lineEdit_SaveDir->text()).arg(name);

    QString fileName = ui->lineEdit_XyzSaveName->text();
    QStringList list = fileName.split(".");
    QString filetype = list.at(list.count() - 1);
    QString strData = QString("%1").arg(TIMEMS);

    if(filetype == "xlsx")
    {
        QXlsx::Document xlsx(fileName);

        xlsx.write(xlsx_row, xlsx_col++, m_xyzCurPos.m_iX);
        xlsx.write(xlsx_row, xlsx_col++, m_xyzCurPos.m_iY);
        xlsx.write(xlsx_row, xlsx_col++, m_xyzCurPos.m_iZ);
        xlsx.write(xlsx_row, xlsx_col++, m_xyzCurPos.m_postime);
        xlsx.write(xlsx_row, xlsx_col++, strData);
        xlsx_row++;
        xlsx_col = 1;
        xlsx.save();
    }
    else
    {
//        QFile file(fileName);
//        file.open(QFile::WriteOnly | QIODevice::Text | QIODevice::Append);
//        QTextStream out(&file);
//        strData = strData + QString(",%1").arg(data);
//        out << strData << Qt::endl;
//        file.close();
    }
}

void frmMain::on_pushButton_connect_clicked()
{
    if ("连接主板" == ui->pushButton_connect->text()) {
        if((ui->LineEdit_iP->text().isEmpty()) || (ui->LineEdit_port->text().isEmpty()))
        {
            QMessageBox::warning(this, tr("Warning"), tr("[Err] Check The NetParama!"), QMessageBox::Abort);
            return;
        }

        socket->connectToHost(ui->LineEdit_iP->text(),  ui->LineEdit_port->text().toUInt());
    } else {
        socket->abort();
    }
}

void frmMain::xyzReadCurPos()
{
    QByteArray boardCmdArray;
    boardCmdArray = HardCmd::formatBoardCmd(EnumBoardId_getXYZTaskInfo, boardCmdArray);
    socket->write(boardCmdArray);
}

void frmMain::getXYZTaskInfoParse(QByteArray &boardcontent)
{/* 返回：eTskType(U8 emTskDXYZType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDMoveXYZ)
        + PosX(I32) + PosY(I32) + PosZ(I32) + Time(U32 ms) + curPosX(I32) + curPosY(I32) + curPosZ(I32) + rdTmStamp(U64)
        + 左(I32) + 右(I32) + 里(I32) + 外(I32) + 底(I32) + 顶(I32)+ X正在移动(bool)+ Y正在移动(bool)+ Z正在移动(bool)
        + id(U16)+IsOut(U8)+IsDown(U8)+ PosXStart(I32) + PosYStart(I32) + PosZStart(I32)*/
    quint8 i = 19;
    quint64 postime;
    m_xyzCurPos.m_iX = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzCurPos.m_iY = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzCurPos.m_iZ = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzCurPos.m_postime = QUIHelperData::byteToInt(boardcontent.mid(i, 8));
    i += 8;
    m_xyzBound.m_iXLeft = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzBound.m_iXRight = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzBound.m_iYIn = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzBound. m_iYOut = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzBound.m_iZDown = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
    i += 4;
    m_xyzBound.m_iZUp = QUIHelperData::byteToInt(boardcontent.mid(i, 4));
}



void frmMain::on_checkBox_XyzSaveEnable_stateChanged(int arg1)
{
    if(0 == arg1)
    {
        gIsXyzPosSave = false;
    }
    else
    {
        QString filename = QFileDialog::getSaveFileName(this, "Save File", "" ,tr("Excel files (*.xlsx)"));
        if (!filename.isEmpty()) {
            ui->lineEdit_XyzSaveName->setText(filename);
            QStringList list = filename.split(".");
            QString filetype = list.at(list.count() - 1);

            if("xlsx" == filetype) {
                xlsx_row = xlsx_col = 1;

                QXlsx::Document xlsx;
                xlsx.write(xlsx_row, xlsx_col++, QString("X(phy)"));
                xlsx.write(xlsx_row, xlsx_col++, QString("Y(phy)"));
                xlsx.write(xlsx_row, xlsx_col++, QString("Z(phy)"));
                xlsx.write(xlsx_row, xlsx_col++, QString("HardT(us)"));
                xlsx.write(xlsx_row, xlsx_col++, QString("LogT"));
                xlsx_row++;
                xlsx_col = 1;
                xlsx.saveAs(filename);
            }
            gIsXyzPosSave = true;
        }
    }
}

