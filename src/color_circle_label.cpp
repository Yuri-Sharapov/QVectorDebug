#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

#include "color_circle_label.h"

ColorCircleLabel::ColorCircleLabel(QWidget *parent)
    : QLabel(parent)
    , m_color(Qt::yellow)
{
    setFixedSize(30, 30);           // small circle
    setCursor(Qt::PointingHandCursor);
}

void ColorCircleLabel::setColor(const QColor &color)
{
    if (color != m_color)
    {
        m_color = color;
        update(); // repaint
        emit colorChanged(color);
    }
}


QColor ColorCircleLabel::getColor() const
{
    return m_color;
}

void ColorCircleLabel::mousePressEvent(QMouseEvent *event)
{
    QColor chosen = QColorDialog::getColor(m_color, this, "Select Color");
    if (chosen.isValid())
    {
        setColor(chosen);
    }
    QLabel::mousePressEvent(event);
}

void ColorCircleLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(m_color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect().adjusted(2, 2, -2, -2));
}
