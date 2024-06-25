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

#include <qelapsedtimer.h>
#include <qcolor.h>
#include "libs/qcustomplot/qcustomplot.h"

class ChartWidget : public QCustomPlot
{
    Q_OBJECT

    const size_t GRAPH_COUNT = 5;
    const QColor GRAPH_COLORS[5] =
    {
        QColorConstants::Red,
        QColorConstants::Blue,
        QColorConstants::Green,
        QColorConstants::Yellow,
        QColorConstants::Cyan
    };

    const char* GRAPH_NAMES[5] =
    {
        "Temperature",
        "Voltage",
        "Current",
        "Consumption",
        "RPM"
    };

    enum GraphNames
    {
        TEMPERATURE = 0,
        VOLTAGE = 1,
        CURRENT = 2,
        CONSUMPTION = 3,
        RPM = 4
    };

public:
    ChartWidget(QCustomPlot *parent = 0);
    ~ChartWidget();

    void addVectorData(qint64 time, int data0, int data1, int data2, int data3);
    void addVectorDataRelative(int data0, int data1, int data2, int data3);

    void addEscData(qint64 time, int temp, int voltage, int current, int consumption, int rpm);
    void addEscDataRelative(int temp, int voltage, int current, int consumption, int rpm);

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
