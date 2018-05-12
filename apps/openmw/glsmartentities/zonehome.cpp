#include "zonehome.hpp"
#include <osg/Group>
#include <osg/ComputeBoundsVisitor>
#include <components/sceneutil/positionattitudetransform.hpp>
SmartZoneHomeTemplate::SmartZoneHomeTemplate()
{
	mIngameID = "cargo_container";
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::getInstance(const MWWorld::Ptr & ptr)
{

	return new SmartZoneHomeInstance(ptr);
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	return new SmartZoneHomeInstance(id, refNum);
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return new SmartZoneHomeInstance(refid, refnum);
}

SmartZoneHomeInstance::SmartZoneHomeInstance(const MWWorld::Ptr & ptr)
{
	std::cout << "new home ptr method" << std::endl;
	mIsSmartZone = true;
	osg::ComputeBoundsVisitor computeBounds;

	ptr.getRefData().getBaseNode()->accept(computeBounds);
	mBoundingBox = computeBounds.getBoundingBox();
	

}

SmartZoneHomeInstance::SmartZoneHomeInstance(std::string id, ESM::RefNum)
{
	std::cout << "new home id refnum method" << std::endl;
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
