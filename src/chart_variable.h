#ifndef CHART_VARIABLE_H
#define CHART_VARIABLE_H

#include <QWidget>

namespace Ui {
class ChartVariable;
}

class ChartVariable : public QWidget
{
    Q_OBJECT

public:
    explicit ChartVariable(QWidget *parent = nullptr);
    ~ChartVariable();
    void setColor(const QColor& color);
    void setName(const QString& name);
signals:
    void stateChanged(int arg1);
    void colorChanged(const QColor& newColor);
private:
    Ui::ChartVariable *ui;
};

#endif // CHART_VARIABLE_H
