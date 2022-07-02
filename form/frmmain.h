#ifndef FRMMAIN_H
#define FRMMAIN_H

#include "frmxyzcontrol.h"
#include "frmxyzplotshow.h"
#include "hardcmd.h"
#include "xlsxdocument.h"
#include <QWidget>

#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = nullptr);
    ~frmMain();

private:
    Ui::frmMain *ui;

    frmXyzControl *XyzCtlFrm;
    frmXyzPlotShow *XyzPlotFrm;

    QTcpSocket *socket;
    bool gIsConnected;

    stXYZPos m_xyzCurPos;
    stXYZBoundary m_xyzBound;

    QTimer timerPosUpdate;

    bool gIsXyzPosSave;
    quint64 xlsx_row;
    quint64 xlsx_col;

public slots:
    void initForm();
    void initConfig();

private slots:
    void connected();
    void disconnected();
    void error();
    void readData();
    void on_pushButton_connect_clicked();

private slots:
    void textXyzCurPosAppend();
    void textXyzCurBoundaryAppend();
    void saveData();
    void xyzReadCurPos();
    void getXYZTaskInfoParse(QByteArray &boardcontent);
    void on_checkBox_XyzSaveEnable_stateChanged(int arg1);
};

#endif // FRMMAIN_H
