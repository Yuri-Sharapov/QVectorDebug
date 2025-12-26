#include "app_palette.h"

AppPalette::AppPalette()
{
    m_paletteBlack.setColor(QPalette::Window, QColor(53, 53, 53));
    m_paletteBlack.setColor(QPalette::WindowText, Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));

    m_paletteBlack.setColor(QPalette::Base, QColor(42, 42, 42));
    m_paletteBlack.setColor(QPalette::AlternateBase, QColor(66, 66, 66));

    m_paletteBlack.setColor(QPalette::ToolTipBase, QColor(66, 66, 66));
    m_paletteBlack.setColor(QPalette::ToolTipText, Qt::white);

    m_paletteBlack.setColor(QPalette::Text, Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));

    m_paletteBlack.setColor(QPalette::Dark, QColor(35, 35, 35));
    m_paletteBlack.setColor(QPalette::Shadow, QColor(20, 20, 20));

    m_paletteBlack.setColor(QPalette::Button, QColor(53, 53, 53));
    m_paletteBlack.setColor(QPalette::ButtonText, Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));

    m_paletteBlack.setColor(QPalette::BrightText, Qt::red);

    m_paletteBlack.setColor(QPalette::Link, QColor(42, 130, 218));

    m_paletteBlack.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_paletteBlack.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));

    m_paletteBlack.setColor(QPalette::HighlightedText, Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

    m_paletteWhite.setColor(QPalette::Window, QColor(245, 245, 245));
    m_paletteWhite.setColor(QPalette::WindowText, QColor(0, 0, 0));
    m_paletteWhite.setColor(QPalette::Disabled, QPalette::WindowText, QColor(160, 160, 160));

    m_paletteWhite.setColor(QPalette::Base, Qt::white);
    m_paletteWhite.setColor(QPalette::AlternateBase, QColor(233, 233, 233));

    m_paletteWhite.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    m_paletteWhite.setColor(QPalette::ToolTipText, QColor(0, 0, 0));

    m_paletteWhite.setColor(QPalette::Text, QColor(0, 0, 0));
    m_paletteWhite.setColor(QPalette::Disabled, QPalette::Text, QColor(160, 160, 160));

    m_paletteWhite.setColor(QPalette::Dark, QColor(200, 200, 200));
    m_paletteWhite.setColor(QPalette::Shadow, QColor(150, 150, 150));

    m_paletteWhite.setColor(QPalette::Button, QColor(245, 245, 245));
    m_paletteWhite.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    m_paletteWhite.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(160, 160, 160));

    m_paletteWhite.setColor(QPalette::BrightText, Qt::red);

    m_paletteWhite.setColor(QPalette::Link, QColor(42, 130, 218));

    m_paletteWhite.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_paletteWhite.setColor(QPalette::Disabled, QPalette::Highlight, QColor(200, 200, 200));

    m_paletteWhite.setColor(QPalette::HighlightedText, Qt::white);
    m_paletteWhite.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(160, 160, 160));
}
