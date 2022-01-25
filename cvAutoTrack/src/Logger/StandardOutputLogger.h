#pragma once
#include "LoggingFacility.h"
#include "../Timer/SystemTime.h"

#include <iostream>

class StandardOutputLogger:public LoggingFacility 
{
    std::string _CoutNowTimeMessage;
    std::string_view _CoutLogMessage;
    std::string_view _CoutInfoMessage;
    std::string_view _CoutWarningMessage;
    std::string_view _CoutErrorMessage;
public:
    std::string _coutNowTimeInfo;
    std::string _objectMessage;
    SystemTimer _systemTimer;

    std::ostream& ioOutput = std::cout;

	StandardOutputLogger(){}

    virtual void Log(std::string_view message)
    {
        cout() << "[   log   ]:" << message << std::endl;
    }
    virtual void Info(std::string_view message)
    {
        cout() << "[  info   ]:" << message << std::endl;
    }
    virtual void Warning(std::string_view message)
    {
        cout() << "[ warning ]:" << message << std::endl;
    }
    virtual void Error(std::string_view message)
    {
        cout() << "[  error  ]:" << message << std::endl;
    }

    std::string getNowTime()
    {
        _coutNowTimeInfo = _systemTimer.now();
        return "[" + _coutNowTimeInfo + "]";
    }

    std::ostream& cout()
    {
        return ioOutput << getNowTime() << _objectMessage;
    }
};