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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QStyleFactory>
#include <QElapsedTimer>

#include "platform.h"
#include "app_palette.h"
#include "chart_variable.h"
#include "chart_widget.h"
#include "libs/cli_wgt/cli_wgt.h"
#include "port.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

static const QVector<QColor> g_defaultColors =
    {
        QColor(255, 0, 0),       // Red
        QColor(0, 128, 0),       // Dark Green
        QColor(0, 0, 255),       // Blue
        QColor(255, 165, 0),     // Orange
        QColor(128, 0, 128),     // Purple
        QColor(0, 206, 209),     // Dark Turquoise
        QColor(255, 105, 180),   // Hot Pink
        QColor(160, 82, 45),     // Sienna
        QColor(0, 255, 127),     // Spring Green
        QColor(255, 255, 0),     // Yellow
        QColor(70, 130, 180),    // Steel Blue
        QColor(199, 21, 133),    // Medium Violet Red
        QColor(255, 20, 147),    // Deep Pink
        QColor(105, 105, 105),   // Dim Gray
        QColor(255, 140, 0),     // Dark Orange
        QColor(72, 209, 204),    // Medium Turquoise
        QColor(138, 43, 226),    // Blue Violet
        QColor(60, 179, 113),    // Medium Sea Green
        QColor(255, 99, 71),     // Tomato
        QColor(47, 79, 79),      // Dark Slate Gray
        QColor(0, 191, 255),     // Deep Sky Blue
        QColor(127, 255, 0),     // Chartreuse
        QColor(186, 85, 211),    // Medium Orchid
        QColor(244, 164, 96),    // Sandy Brown
        QColor(95, 158, 160),    // Cadet Blue
        QColor(255, 228, 181),   // Moccasin
        QColor(100, 149, 237),   // Cornflower Blue
        QColor(255, 215, 0),     // Gold
        QColor(189, 183, 107),   // Dark Khaki
        QColor(152, 251, 152),   // Pale Green
        QColor(123, 104, 238),   // Medium Slate Blue
        QColor(219, 112, 147)    // Pale Violet Red
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum ThemeSelector
    {
        THEME_WHITE,
        THEME_BLACK
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_PortUpdatePlot(int var1, int var2, int var3, int var4, int var5, int var6);

    void on_btnConnect_clicked();
    void on_btnSend_clicked();

    void onChartStateChanged(int arg);

    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();

    void on_actionBlack_toggled(bool arg1);
    void on_actionWhite_toggled(bool arg1);

    void on_actionV1_toggled(bool arg1);
    void on_actionV2_toggled(bool arg1);
private:
    void showEvent(QShowEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void showStatusMessage(const QString &message);

    void serialSetup(void);
    void serialConnect(void);
    void serialDisconnect(void);

    void openChart(QVector<Port::ChartVar>* pVars);

    void restoreSettings(void);
    void saveSettings(void);
    void updateTheme(void);

    Ui::MainWindow* m_pUi;
    Port*           m_pPort;
    ChartWidget*    m_pChart = nullptr;
    QLabel*         m_pStatus = nullptr;
    cli_wgt*        m_pCliWgt = nullptr;
    QVector<ChartVariable*> m_chartVals;

    QSettings*      m_pSettings;
    ThemeSelector   m_uiTheme = THEME_WHITE;
    Port::ProcotolType  m_protocol = Port::TYPE_V1;
    AppPalette*     m_pAppPalette = nullptr;

    uint64_t        m_lastTimeUpdate = 0;
     QElapsedTimer  m_updateTimer;
};
#endif // MAINWINDOW_H
