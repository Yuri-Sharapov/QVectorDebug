#include "chart_variable.h"
#include "ui_chart_variable.h"

ChartVariable::ChartVariable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChartVariable)
{
    ui->setupUi(this);
    connect(ui->cbEnabled, &QCheckBox::stateChanged, this, &ChartVariable::stateChanged);
    connect(ui->lblColor, &ColorCircleLabel::colorChanged, this, &ChartVariable::colorChanged);
}

ChartVariable::~ChartVariable()
{
    delete ui;
}

void ChartVariable::setColor(const QColor &color)
{
    ui->lblColor->setColor(color);
}

void ChartVariable::setName(const QString &name)
{
    ui->leName->setText(name);
}
