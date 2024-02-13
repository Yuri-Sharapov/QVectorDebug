#include <QDebug>

#include "common/maths.h"
#include "wave_generator.h"


WaveGenerator::WaveGenerator(waveType_e type)
    : m_currentType(type)
    , m_pConfig(new QSettings("wave_generator.ini", QSettings::IniFormat))
{
    configRestore();
}

WaveGenerator::~WaveGenerator()
{
    configSave();
    delete m_pConfig;
}

void WaveGenerator::start()
{
    m_timer.start();
    m_timeNs = m_timer.nsecsElapsed();
    m_timeNsPrevious = m_timeNs;
    m_counter = 0;
    m_isOn = true;
}

void WaveGenerator::stop()
{
    m_isOn = false;
}


void WaveGenerator::update()
{
    if (!m_isOn)
        return;

    m_timeNs = m_timer.nsecsElapsed();
    //qDebug() << "time(wg): " << m_timeNs;
    uint64_t timeDelta = m_timeNs - m_timeNsPrevious;

    float period = 1.0f / (m_frequency);

    if (timeDelta >= period * 1000000000.f)
        m_timeNsPrevious = m_timeNs;

    //float part = ((float)timeDelta / 1000000000.f)/period;
    float part = ((float)m_counter / 1000.0f) / period;

    if (++m_counter > period * 1000 - 1)
        m_counter = 0;

    //qDebug() << "period: " << period;
    //qDebug() << "part: " << part;

    m_output = m_amplitude * m_amplitudeRadio * math::sin_approx(part * 2 * M_PIf );

}

uint64_t WaveGenerator::getTime()
{
    return m_timeNs;
}

void WaveGenerator::configCreate(const QString name, float value)
{
    if (!m_pConfig->contains(name))
        m_pConfig->setValue(name, value);
}

void WaveGenerator::configRestore(void)
{
    configCreate("frequency",       m_frequency);
    configCreate("amplitude",       m_amplitude);
    configCreate("amplitudeRadio",  m_amplitudeRadio);
    configCreate("offset",          m_offset);
    configCreate("type",            m_currentType);
    m_pConfig->sync();

    m_frequency         = m_pConfig->value("frequency").toFloat();
    m_amplitude         = m_pConfig->value("amplitude").toFloat();
    m_amplitudeRadio    = m_pConfig->value("amplitudeRadio").toFloat();
    m_offset            = m_pConfig->value("offset").toFloat();
    m_currentType       = static_cast<waveType_e>(m_pConfig->value("type").toInt());
}

void WaveGenerator::configSave(void)
{
    m_pConfig->setValue("frequency",        m_frequency);
    m_pConfig->setValue("amplitude",        m_amplitude);
    m_pConfig->setValue("amplitudeRadio",   m_amplitudeRadio);
    m_pConfig->setValue("offset",           m_offset);
    m_pConfig->setValue("type",             m_currentType);
    m_pConfig->sync();
}
