#pragma once

#include <QLabel>
#include <QColor>

class ColorCircleLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ColorCircleLabel(QWidget* parent = nullptr);

    void setColor(const QColor& color);
    QColor getColor() const;
signals:
    void colorChanged(const QColor& newColor);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
private:
    QColor  m_color;
};
