#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <QElapsedTimer>
#include <QSettings>
/*
 * This file is part of the QVectorDebug (https://github.com/Yuri-Sharapov/QVectorDebug).
 * Copyright (c) 2022 Yuri Sharapov.
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

class WaveGenerator
{
    enum waveType_e
    {
        TYPE_SQUARE,
        TYPE_SINUS
    };

public:
    WaveGenerator(waveType_e type = TYPE_SQUARE);
    ~WaveGenerator();

    void        start(void);
    void        stop(void);
    void        update(void);

    waveType_e  getType(void)                   {return m_currentType;}
    float       getFrequency(void)              {return m_frequency;}
    float       getAmplitude(void)              {return m_amplitude;}
    float       getAmplitudeRatio(void)         {return m_amplitudeRadio;}
    float       getOffset(void)                 {return m_offset;}

    void        setType(waveType_e type)        {m_currentType = type;}
    void        setFrequency(float val)         {m_frequency = val;}
    void        setAmplitude(float val)         {m_amplitude = val;}
    void        setAmplitudeRatio(float val)    {m_amplitudeRadio = val;}
    void        setOffset(float val)            {m_offset = val;}

private:
    void        configCreate(QString name, float value);
    void        configRestore(void);
    void        configSave(void);
private:
    float       m_frequency         = 1;
    float       m_amplitude         = 1;
    float       m_amplitudeRadio    = 1;
    float       m_offset            = 0;
    waveType_e  m_currentType;

    QElapsedTimer   m_timer;
    uint64_t        m_timeNsPrevious;
    bool            m_isOn          = false;

    QSettings*      m_pConfig;
};
