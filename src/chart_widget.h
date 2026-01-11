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
    const size_t GRAPH_COUNT = 6;
    const QColor GRAPH_COLORS[6] =
    {
        QColor(255, 0, 0),       // Red
        QColor(0, 128, 0),       // Dark Green
        QColor(0, 0, 255),       // Blue
        QColor(255, 165, 0),     // Orange
        QColor(128, 0, 128),     // Purple
        QColor(0, 206, 209)
    };
public:
    ChartWidget(QCustomPlot *parent = 0);
    ~ChartWidget();

    void addVectorData(qint64 time, int data0, int data1, int data2, int data3);
    void addVectorDataRelative(int data0, int data1, int data2, int data3);

    void addData(int v1, int v2, int v3, int v4, int v5, int v6);

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
