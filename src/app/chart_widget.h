/*
 * This file is part of the QVectorDebug (https://github.com/Yuri-Sharapov/QVectorDebug).
 * Copyright (c) 2021 Yuri Sharapov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QElapsedTimer>
#include "libs/qcustomplot/qcustomplot.h"

class ChartWidget : public QCustomPlot
{
    Q_OBJECT
public:
    ChartWidget(QCustomPlot *parent = 0);
    ~ChartWidget();

    void appendData(qint64 time, int16_t data0, int16_t data1, int16_t data2, int16_t data3);
    void appendData(int16_t data0, int16_t data1, int16_t data2, int16_t data3);
    void appendData(qint64 time, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3);

    void appendData(int voltage, int current, int ppm, int rpm, int position, int currentA, int currentB);


    void startChart(void);
    void updateChart(void);

    void setThemeBackground(const QColor &brush);

    void enableCursor(bool enable);
    void setVCursor(int position);
    void setHCursor(int position);
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
    bool            m_cursorEnable = false;
    QCPItemLine*    m_hCursor;
    QCPItemLine*    m_vCursor;
    QPen            m_cursorColor;

    int             m_hPos = 0;
    int             m_vPos = 0;
    int             m_timeStep = 0;
};
