#pragma once
#include <chrono>
#include <string_view>
#include <string>

class Timer
{
public:
    Timer() { Reset(); }

    void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

    double Elapsed() const
    {
        return (double) std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now() - m_Start)
                       .count() *
               0.001 * 0.001;
    }

    double ElapsedMillis() const
    {
        return (double) std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now() - m_Start)
                       .count() *
               0.001;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

#include <stdio.h>

class ScopedTimer
{
public:
    ScopedTimer() = default;

    explicit ScopedTimer(const char* name) : m_Name(name) {}

    explicit ScopedTimer(std::string_view name) : m_Name(name) {}

    ~ScopedTimer()
    {
        double time = m_Timer.ElapsedMillis();
        printf("Timer: %s - %lfms\n", m_Name.c_str(), time);
    }

private:
    Timer m_Timer{};
    std::string m_Name;
};