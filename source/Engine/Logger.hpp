/**
 * \file Logger.hpp
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <chrono>
#include <fstream>
#include <map>
#include <memory>
#include <string>

namespace CS230
{
    class Logger
    {
    public:
        enum class Severity
        {
            Verbose, // Minor messages
            Debug,   // Only used while actively debugging
            Event,   // General event, like key press or state change
            Error    // Errors, such as file load errors
        };

        Logger(Severity severity, bool use_console, std::chrono::system_clock::time_point start_time);

        void LogError(std::string text)
        {
            log(Severity::Error, text);
        }

        void LogEvent(std::string text)
        {
            log(Severity::Event, text);
        }

        void LogDebug(std::string text)
        {
            log(Severity::Debug, text);
        }

        void LogVerbose(std::string text)
        {
            log(Severity::Verbose, text);
        }

    private:
        void log(Severity severity, std::string message);

        std::chrono::system_clock::time_point start_time;
        Severity                              min_level;
        std::ofstream                         out_stream;

        double seconds_since_start()
        {
            auto                          now         = std::chrono::system_clock::now();
            std::chrono::duration<double> check_start = now - start_time;
            return check_start.count();
        }
    };
}
