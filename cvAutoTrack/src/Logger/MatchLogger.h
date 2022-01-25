#pragma once
#include "StandardOutputLogger.h"
class MatchLogger:public StandardOutputLogger
{
public:
    MatchLogger() {
        this->_objectMessage = "[Match]";
    }

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
};