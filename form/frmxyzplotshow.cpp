#include "frmxyzplotshow.h"
#include "ui_frmxyzplotshow.h"

#include "quihelper.h"
#include "quihelperdata.h"

frmXyzPlotShow::frmXyzPlotShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmXyzPlotShow)
{
    ui->setupUi(this);
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

frmXyzPlotShow::~frmXyzPlotShow()
{
    delete ui;
}

void frmXyzPlotShow::initForm()
{
    QList<QColor> color;
    color << Qt::red << Qt::green << Qt::blue << Qt::yellow << Qt::black << Qt::gray;

    QVector<QCPScatterStyle::ScatterShape> shapes;
    shapes << QCPScatterStyle::ssCross;
    shapes << QCPScatterStyle::ssPlus;
    shapes << QCPScatterStyle::ssCircle;

    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(QFont("Helvetica", 9));
    ui->customPlot->legend->setRowSpacing(-3);

//    ui->customPlot->yAxis->setTickLabels(false);
//    connect(ui->customPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
//    ui->customPlot->yAxis2->setVisible(true);
//    ui->customPlot->axisRect()->addAxis(QCPAxis::atRight);
//    ui->customPlot->axisRect()->addAxis(QCPAxis::atRight);
//    ui->customPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(15); // add some padding to have space for tags
//    ui->customPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(15);
//    ui->customPlot->axisRect()->axis(QCPAxis::atRight, 2)->setPadding(15);

    for(quint8 i = 0; i < 3; i++)
    {
//        ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->axisRect()->axis(QCPAxis::atRight, i));
        ui->customPlot->addGraph();
        ui->customPlot->graph(i)->setPen(QPen(color.at(i)));
        ui->customPlot->graph(i)->setScatterStyle(QCPScatterStyle((shapes.at(i)), 10));
    }
    ui->customPlot->graph(0)->setName(QString("X"));
    ui->customPlot->graph(1)->setName(QString("Y"));
    ui->customPlot->graph(2)->setName(QString("Z"));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->xAxis->setVisible(true);
    ui->customPlot->xAxis->setLabel(QString("时间"));
//    ui->customPlot->yAxis->setLabel(QString("坐标"));
//     ui->customPlot->axisRect()->setupFullAxesBox();  //设置四轴可见
}

void frmXyzPlotShow::drawDataPoint(const stXYZPos &m_xyzCurPos)
{
//    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0; // time elapsed since start of demo, in seconds
    double key = QTime::currentTime().msecsSinceStartOfDay()*0.001;

    ui->customPlot->graph(0)->addData(key, m_xyzCurPos.m_iX);
    ui->customPlot->graph(1)->addData(key, m_xyzCurPos.m_iY);
    ui->customPlot->graph(2)->addData(key, m_xyzCurPos.m_iZ);
    // make key axis range scroll with the data (at a constant range size of 8):
//    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);

    ui->customPlot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    for(quint8 i = 1; i < 3; i++) {
        ui->customPlot->graph(i)->rescaleAxes(true);
    }
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

//    ui->customPlot->graph(0)->rescaleValueAxis(false, true);
//    ui->customPlot->graph(0)->rescaleKeyAxis();
//    for(quint8 i = 1; i < sensorNum; i++)
//    {
//        ui->customPlot->graph(i)->rescaleValueAxis(true, true);
//        ui->customPlot->graph(i)->rescaleKeyAxis();
//    }
    ui->customPlot->replot();
}

void frmXyzPlotShow::clearData()
{
    for(quint8 i = 0; i < 6; i++)
    {
        ui->customPlot->graph(i)->data().data()->clear();
    }
    ui->customPlot->replot();
}
