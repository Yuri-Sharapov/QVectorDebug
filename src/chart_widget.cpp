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

#include <math.h>
#include "chart_widget.h"

ChartWidget::ChartWidget(QCustomPlot *parent) 
    : QCustomPlot(parent)
{
    setupChart(this);
    rescaleAxes();
    replot();
}

ChartWidget::~ChartWidget()
{
}

void ChartWidget::addVectorData(qint64 time, int data0, int data1, int data2, int data3)
{
    graph(0)->addData(static_cast<double>(time)/1000000.0, static_cast<double>(data0));
    graph(1)->addData(static_cast<double>(time)/1000000.0, static_cast<double>(data1));
    graph(2)->addData(static_cast<double>(time)/1000000.0, static_cast<double>(data2));
    graph(3)->addData(static_cast<double>(time)/1000000.0, static_cast<double>(data3));
}

void ChartWidget::addVectorDataRelative(int data0, int data1, int data2, int data3)
{
    graph(0)->addData(m_timeStep, static_cast<double>(data0));
    graph(1)->addData(m_timeStep, static_cast<double>(data1));
    graph(2)->addData(m_timeStep, static_cast<double>(data2));
    graph(3)->addData(m_timeStep, static_cast<double>(data3));
    m_timeStep++;
}

void ChartWidget::addData(int v1, int v2, int v3, int v4, int v5, int v6)
{
    graph(0)->addData(m_timeStep, static_cast<double>(v1));
    graph(1)->addData(m_timeStep, static_cast<double>(v2));
    graph(2)->addData(m_timeStep, static_cast<double>(v3));
    graph(3)->addData(m_timeStep, static_cast<double>(v4));
    graph(4)->addData(m_timeStep, static_cast<double>(v5));
    graph(5)->addData(m_timeStep, static_cast<double>(v6));
    m_timeStep++;
}

void ChartWidget::startChart()
{
    for (int i = 0; i < GRAPH_COUNT; i++)
        graph(i)->data()->clear();
 
    m_timeStep = 0;
}

void ChartWidget::updateChart()
{
    rescaleAxes();
    replot();
}

void ChartWidget::setThemeBackground(const QColor &brush)
{
    if (brush == Qt::white)
    {
        setBackground(QBrush(QColor(242,242,242)));

        xAxis->setTickLabelColor(Qt::black);
        xAxis->setBasePen(QPen(Qt::black));
        xAxis->setLabelColor(Qt::black);
        xAxis->setTickPen(QPen(Qt::black));
        xAxis->setSubTickPen(QPen(Qt::black));

        yAxis->setTickLabelColor(Qt::black);
        yAxis->setBasePen(QPen(Qt::black));
        yAxis->setLabelColor(Qt::black);
        yAxis->setTickPen(QPen(Qt::black));
        yAxis->setSubTickPen(QPen(Qt::black));

        m_cursorColor = QPen(Qt::black);
    }
    else
    {
        setBackground(QBrush(QColor(42,42,42)));

        xAxis->setTickLabelColor(Qt::white);
        xAxis->setBasePen(QPen(Qt::white));
        xAxis->setLabelColor(Qt::white);
        xAxis->setTickPen(QPen(Qt::white));
        xAxis->setSubTickPen(QPen(Qt::white));

        yAxis->setTickLabelColor(Qt::white);
        yAxis->setBasePen(QPen(Qt::white));
        yAxis->setLabelColor(Qt::white);
        yAxis->setTickPen(QPen(Qt::white));
        yAxis->setSubTickPen(QPen(Qt::white));

        m_cursorColor = QPen(Qt::yellow);
    }


    this->replot();
}

void ChartWidget::changeVisablilty(int graph, bool state)
{
    this->graph(graph)->setVisible(state);
    this->replot();
}

void ChartWidget::setupChart(QCustomPlot *pChart)
{
    pChart->setBackground(QBrush(QColor(42,42,42)));

    pChart->clearItems();
    //pcChart->yAxis->setTickStep(100);
    pChart->yAxis->setRange(0,100);
    pChart->xAxis->setRange(0,100);

    //pChart->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);

    pChart->legend->setFont(legendFont);
    pChart->legend->setSelectedFont(legendFont);
    pChart->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    for (size_t i = 0; i < GRAPH_COUNT; i++)
    {
        pChart->addGraph();
        pChart->graph(i)->setPen(QPen(GRAPH_COLORS[i]));
        pChart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
        pChart->graph(i)->setVisible(false);
    }

    pChart->setInteraction(QCP::iRangeDrag, true);
    pChart->setInteraction(QCP::iRangeZoom, true);
    pChart->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);

    connect(pChart, &QCustomPlot::mousePress, this, &ChartWidget::plotMousePress);
    connect(pChart, &QCustomPlot::mouseWheel, this, &ChartWidget::plotMouseWheel);
    connect(pChart, &QCustomPlot::mouseDoubleClick, this, &ChartWidget::plotDoubleClicked);
    connect(pChart, &QCustomPlot::selectionChangedByUser, this, &ChartWidget::plotSelectionChanged);
    connect(pChart->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(plotChangeRangeAll(QCPRange)));
}

void ChartWidget::addChart()
{

}

void ChartWidget::plotMousePress(QMouseEvent *event)
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged
    Q_UNUSED(event)

    if(Qt::KeyboardModifier::ControlModifier == QApplication::keyboardModifiers())
    {
        this->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    }
    else
    {
        this->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
            this->axisRect()->setRangeDrag(this->xAxis->orientation());
        else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
            this->axisRect()->setRangeDrag(this->yAxis->orientation());
        else
            this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    }
}

void ChartWidget::plotMouseWheel(QWheelEvent *event)
{
    Q_UNUSED(event)

    if(Qt::KeyboardModifier::ControlModifier == QApplication::keyboardModifiers())
    {
        this->axisRect()->setRangeZoom(Qt::Orientation::Horizontal);
    }
    else if(Qt::KeyboardModifier::ShiftModifier == QApplication::keyboardModifiers())
    {
        this->axisRect()->setRangeZoom(Qt::Orientation::Vertical);
    }
    else
    {
        this->axisRect()->setRangeZoom(Qt::Orientation::Horizontal | Qt::Orientation::Vertical);
    }
}

void ChartWidget::plotSelectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
            this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
            this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
// make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
            this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
            this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
// synchronize selection of graphs with selection of corresponding legend items:
    for (int i = 0; i < this->graphCount(); ++i)
    {
        QCPGraph *graph = this->graph(i);
        QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            QCPDataRange range = graph->data()->dataRange();
            range.setBegin(range.end() - 1);
            graph->setSelection(QCPDataSelection(range));
//            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

void ChartWidget::plotDoubleClicked(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->rescaleAxes();
    this->replot();
}

void ChartWidget::plotChangeRangeAll(const QCPRange &range)
{
        QCPAxis *axis = this->xAxis;
        if(axis != sender())
        {
            axis->setRange(range);
            this->replot();
        }
}
