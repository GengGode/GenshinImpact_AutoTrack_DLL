#pragma once
class TimerFacility
{
public:
	virtual ~TimerFacility() = default;
	virtual std::string now() = 0;
};
using Timer = std::shared_ptr<TimerFacility>;