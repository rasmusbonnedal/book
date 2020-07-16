#pragma once

#include <chrono>

class Stopwatch {
    using hrc = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<float>;
public:
    void start() {
        m_start = hrc::now();
        m_lap = m_start;
    }
    float lap() {
        time_type t = hrc::now();
        duration d = std::chrono::duration_cast<duration>(t - m_lap);
        m_lap = t;
        return d.count();
    }
    float stop() {
        time_type t = hrc::now();
        duration d = std::chrono::duration_cast<duration>(t - m_start);
        return d.count();
    }

private:
    using time_type = hrc::time_point;
    time_type m_start;
    time_type m_lap;
};
