#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QElapsedTimer>

#include "chart_widget.h"

#define PROTOCOL_START_BYTE     0x55

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef struct __attribute__ ((packed)) ProtocolData_struct
    {
        int16_t    data[4];
        uint8_t     checkSum;
    }ProtocolData_t;

    struct ChartVar
    {
        qint64  timeNs;
        int16_t data[4];
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();

    void on_cbEnabled_1_stateChanged(int arg1);
    void on_cbEnabled_2_stateChanged(int arg1);
    void on_cbEnabled_3_stateChanged(int arg1);
    void on_cbEnabled_4_stateChanged(int arg1);

    void on_btnSend_clicked();

    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();

private:
    void showStatusMessage(const QString &message);

    void serialSetup(void);
    void serialConnect(void);
    void serialDisconnect(void);
    void serialReadyRead(void);

    void protocolParseData(const QByteArray &data);

    Ui::MainWindow  *m_pUi;

    ChartWidget     *m_pChart = nullptr;
    QSerialPort     *m_pSerial = nullptr;
    QLabel          *m_pStatus = nullptr;
    QByteArray      m_rxData;
    QVector<ChartVar> m_rxRawData;
    QElapsedTimer   m_timerNs;
    qint64          m_timerNs_1;

    int             m_errorsCnt = 0;
};
#endif // MAINWINDOW_H
