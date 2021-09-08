#pragma once

#include "libs/qcustomplot/qcustomplot.h"

#include <QElapsedTimer>

class ChartWidget : public QCustomPlot
{
    Q_OBJECT
public:
    ChartWidget(QCustomPlot *parent = 0);
    ~ChartWidget();

    void appendData(qint64 time, int16_t data0, int16_t data1, int16_t data2, int16_t data3);
    void appendData(qint64 time, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3);

    void startChart(void);
    void updateChart(void);
public slots:
    void changeVisablilty(int graph, bool state);
private:
    void setupChart(QCustomPlot* pChart);
    void addChart(void);
private slots:
    void plotMousePress(QMouseEvent *event);
    void plotMouseWheel(QWheelEvent *event);
    void plotSelectionChanged();
    void plotDoubleClicked(QMouseEvent *event);
    void plotChangeRangeAll(const QCPRange &range);
private:
    QDateTime   m_dateTimeStart;

    QElapsedTimer m_timerNs;
};
