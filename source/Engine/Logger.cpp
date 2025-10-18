/**
 * \file Logger.cpp
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Logger.hpp"
#include <iomanip>
#include <iostream>

#define ERROR   CS230::Logger::Severity::Error
#define EVENT   CS230::Logger::Severity::Event
#define DEBUG   CS230::Logger::Severity::Debug
#define VERBOSE CS230::Logger::Severity::Verbose

namespace CS230
{
    Logger::Logger(Logger::Severity severity, bool use_console, std::chrono::system_clock::time_point start_time_) : start_time(start_time_), min_level(severity), out_stream("Trace.log")
    {
        if (use_console == true)
        {
            out_stream.basic_ios<char>::rdbuf(std::cout.rdbuf());
        }
    }

    void Logger::log(Severity severity, std::string message)
    {
        std::map<CS230::Logger::Severity, std::string> get_error = {
            { CS230::Logger::Severity::Verbose, "Verbose" },
            {   CS230::Logger::Severity::Debug,   "Debug" },
            {   CS230::Logger::Severity::Event,   "Event" },
            {   CS230::Logger::Severity::Error,   "Error" }
        };
        std::string answer = get_error[severity] + "\t" + message;

        if (int(CS230::Logger::min_level) <= int(severity))
        {
            out_stream.precision(4);
            out_stream << '[' << std::fixed << seconds_since_start() << "]\t";
            out_stream << answer << "\n";
        }
        return;
    }

}
