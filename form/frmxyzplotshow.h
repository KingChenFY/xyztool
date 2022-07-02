#ifndef FRMXYZPLOTSHOW_H
#define FRMXYZPLOTSHOW_H

#include "hardcmd.h"
#include <QWidget>

namespace Ui {
class frmXyzPlotShow;
}

class frmXyzPlotShow : public QWidget
{
    Q_OBJECT

public:
//    static frmXyzPlotShow & GetInstance()
//    {
//        static frmXyzPlotShow m_instance; // 静态局部对象的引用
//        return m_instance;
//    }
    explicit frmXyzPlotShow(QWidget *parent = nullptr);
    ~frmXyzPlotShow();

    void drawDataPoint(const stXYZPos &m_xyzCurPos);
    void clearData();

private:
    Ui::frmXyzPlotShow *ui;
    void initForm();
};

#endif // FRMXYZPLOTSHOW_H
