#include "fireshroom.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwclass/classes.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"




SmartEntityFireShroomTemplate::SmartEntityFireShroomTemplate()
{
	mIngameIDs.push_back("gl_fire_shroom");
}

MWBase::SmartEntityInstance * SmartEntityFireShroomTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	SmartEntityFireShroomInstance * instance = new SmartEntityFireShroomInstance(ptr);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityFireShroomTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityFireShroomInstance * instance = new SmartEntityFireShroomInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityFireShroomTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

SmartEntityFireShroomInstance::SmartEntityFireShroomInstance(const MWWorld::Ptr & ptr)
{

	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartFireShroom" << std::endl;
	mCurrentUserCount = 0;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
}

SmartEntityFireShroomInstance::SmartEntityFireShroomInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new smartshroom" << std::endl;
	mPingCount = pings;
	mRefId = refid;
}

std::string SmartEntityFireShroomInstance::getSaveString()
{
	return std::string();
}

bool SmartEntityFireShroomInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartEntityFireShroomInstance::useWorldInstance(MWBase::Life * user)
{
	return nullptr;
}

void SmartEntityFireShroomInstance::onImpact(MWWorld::Ptr impactwith)
{

	MWBase::Environment::get().getWorld()->createFireExplosion(mPtr.getCellRef().getPosition().asVec3(), 30);

	/*auto playerptr = MWBase::Environment::get().getWorld()->getPlayerPtr();
	std::cout << "SMART SHROOM IMPACT" << std::endl;
	bool standingon = MWBase::Environment::get().getWorld()->getPlayerStandingOn(mPtr);
	if (standingon)
		std::cout << "LAUNCH PLAYER" << std::endl;

	playerptr.getClass().getMovementSettings(playerptr).mAttemptJump = true;
	playerptr.getClass().getMovementSettings(playerptr).mPosition[2] = 1.0;
	playerptr.getClass().getCreatureStats(playerptr).setMovementFlag(MWMechanics::CreatureStats::Flag_ForceJump, true);*/
	
	//MWBase::Environment::get().getStatusManager()->giveStatus(MWBase::Environment::get().getWorld()->getPlayerPtr(), MWBase::FireShroomLaunch);
}
