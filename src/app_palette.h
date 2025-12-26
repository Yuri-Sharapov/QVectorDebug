#ifndef APPPALETTE_H
#define APPPALETTE_H

#include <QPalette>

class AppPalette
{
public:
    AppPalette();
    QPalette& getWhitePalette() {return m_paletteWhite;}
    QPalette& getBlackPalette() {return m_paletteBlack;}
private:
    QPalette        m_paletteWhite;
    QPalette        m_paletteBlack;
};

#endif // APPPALETTE_H
