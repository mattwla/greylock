#include "bounceshroom.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwclass/classes.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"




SmartEntityBounceShroomTemplate::SmartEntityBounceShroomTemplate()
{
	mIngameIDs.push_back("bounceshroom");
}

MWBase::SmartEntityInstance * SmartEntityBounceShroomTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	SmartEntityBounceShroomInstance * instance = new SmartEntityBounceShroomInstance(ptr);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityBounceShroomTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityBounceShroomInstance * instance = new SmartEntityBounceShroomInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityBounceShroomTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

SmartEntityBounceShroomInstance::SmartEntityBounceShroomInstance(const MWWorld::Ptr & ptr)
{

	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartBounceShroom" << std::endl;
	mCurrentUserCount = 0;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
}

SmartEntityBounceShroomInstance::SmartEntityBounceShroomInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new smartshroom" << std::endl;
	mPingCount = pings;
	mRefId = refid;
}

std::string SmartEntityBounceShroomInstance::getSaveString()
{
	return std::string();
}

bool SmartEntityBounceShroomInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartEntityBounceShroomInstance::useWorldInstance(MWBase::Life * user)
{
	return nullptr;
}

void SmartEntityBounceShroomInstance::onImpact(MWWorld::Ptr impactwith)
{

	auto playerptr = MWBase::Environment::get().getWorld()->getPlayerPtr();
	std::cout << "SMART SHROOM IMPACT" << std::endl;
	bool standingon = MWBase::Environment::get().getWorld()->getPlayerStandingOn(mPtr);
	if (standingon)
		std::cout << "LAUNCH PLAYER" << std::endl;

	playerptr.getClass().getMovementSettings(playerptr).mAttemptJump = true;
	playerptr.getClass().getMovementSettings(playerptr).mPosition[2] = 1.0;
	playerptr.getClass().getCreatureStats(playerptr).setMovementFlag(MWMechanics::CreatureStats::Flag_ForceJump, true);
	
	MWBase::Environment::get().getStatusManager()->giveStatus(MWBase::Environment::get().getWorld()->getPlayerPtr(), MWBase::BounceShroomLaunch);
}
