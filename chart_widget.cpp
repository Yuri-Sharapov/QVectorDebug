/*
 * This file is part of the QVectorDebug (https://github.com/Yuri-Sharapov/QVectorDebug).
 * Copyright (c) 2021 Yuris Sharapov.
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

ChartWidget::ChartWidget(QCustomPlot *parent) :
    QCustomPlot(parent)
{
    setupChart(this);

    /*for (int i = 0; i < 100; i++)
    {
        this->graph(0)->addData(i, sin(M_PI * i/10));
        this->graph(1)->addData(i, sin(M_PI * i/10 + M_PI/3));
        this->graph(2)->addData(i, sin(M_PI * i/10 + M_PI*2/3));
    }*/
    this->rescaleAxes();
    this->replot();
}

ChartWidget::~ChartWidget()
{
}

void ChartWidget::appendData(qint64 time, int16_t data0, int16_t data1, int16_t data2, int16_t data3)
{

    this->graph(0)->addData(time/1000000U, data0);
    this->graph(1)->addData(time/1000000U, data1);
    this->graph(2)->addData(time/1000000U, data2);
    this->graph(3)->addData(time/1000000U, data3);
}

void ChartWidget::appendData(qint64 time, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3)
{

    this->graph(0)->addData(time/1000000U, data0);
    this->graph(1)->addData(time/1000000U, data1);
    this->graph(2)->addData(time/1000000U, data2);
    this->graph(3)->addData(time/1000000U, data3);
}

void ChartWidget::startChart()
{
    this->graph(0)->data()->clear();
    this->graph(1)->data()->clear();
    this->graph(2)->data()->clear();
    this->graph(3)->data()->clear();
}

void ChartWidget::updateChart()
{
    this->rescaleAxes();
    this->replot();
}

void ChartWidget::changeVisablilty(int graph, bool state)
{
    this->graph(graph)->setVisible(state);
    this->replot();
}

void ChartWidget::setupChart(QCustomPlot *pChart)
{
    pChart->setBackground(QBrush(QColor(254,254,254)));

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

    pChart->addGraph();                                                             // add Graph 0
    pChart->graph(0)->setPen(QPen(Qt::red));
    pChart->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4)); // adding dots to graph
    pChart->addGraph();                                                             // add Graph 1
    pChart->graph(1)->setPen(QPen(Qt::green));
    pChart->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4)); // adding dots to graph
    pChart->addGraph();                                                             // add Graph 1
    pChart->graph(2)->setPen(QPen(Qt::blue));
    pChart->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4)); // adding dots to graph
    pChart->addGraph();                                                             // add Graph 1
    pChart->graph(3)->setPen(QPen(Qt::yellow));
    pChart->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4)); // adding dots to graph

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
