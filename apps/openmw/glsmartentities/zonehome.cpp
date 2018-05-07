#include "zonehome.hpp"

SmartZoneHomeTemplate::SmartZoneHomeTemplate()
{
	mIngameID = "cargo_container";
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	return nullptr;
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	return nullptr;
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

std::string SmartZoneHomeInstance::getSaveString()
{
	return std::string();
}

bool SmartZoneHomeInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartZoneHomeInstance::useWorldInstance(MWBase::Life * user)
{
	return nullptr;
}
