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
    m_timeNsPrevious = 0;
    m_timer.start();
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

    uint64_t timeNs = m_timer.nsecsElapsed();
    uint64_t timeDelta = m_timer.nsecsElapsed() - m_timeNsPrevious;
    m_timeNsPrevious = timeNs;


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