#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Timestep class definition
 */

namespace Engine
{
    class Timestep
    {
    public:
        Timestep() = default;

        Timestep(float time) : m_Time(time) {}

    public:
        float ToMillis() const { return m_Time * (float) 1000; }

        float ToMicros() const { return m_Time * (float) 1000000; }

        float Time() const { return m_Time; }

        void SetTime(float time) { m_Time = time; }

    public:
        Timestep operator-(Timestep& other) { return Timestep(this->Time() - other.Time()); }

        Timestep operator+(Timestep& other) { return Timestep(this->Time() + other.Time()); }

        operator float() const { return m_Time; }

    private:
        float m_Time{};
    };
}// namespace Engine