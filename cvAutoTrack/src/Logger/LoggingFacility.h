#pragma once

#include <string_view>

class LoggingFacility {
public:
    virtual ~LoggingFacility() = default;
    virtual void Log(std::string_view message) = 0;
    virtual void Info(std::string_view message) = 0;
    virtual void Warning(std::string_view message) = 0;
    virtual void Error(std::string_view message) = 0;
};

using Logger = std::shared_ptr<LoggingFacility>;