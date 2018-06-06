#include "impulseshroom.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwclass/classes.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"




SmartEntityImpulseShroomTemplate::SmartEntityImpulseShroomTemplate()
{
	mIngameIDs.push_back("impulseshroom");
}

MWBase::SmartEntityInstance * SmartEntityImpulseShroomTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	SmartEntityImpulseShroomInstance * instance = new SmartEntityImpulseShroomInstance(ptr);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityImpulseShroomTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityImpulseShroomInstance * instance = new SmartEntityImpulseShroomInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityImpulseShroomTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

SmartEntityImpulseShroomInstance::SmartEntityImpulseShroomInstance(const MWWorld::Ptr & ptr)
{

	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartImpulseShroom" << std::endl;
	mCurrentUserCount = 0;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
}

SmartEntityImpulseShroomInstance::SmartEntityImpulseShroomInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new smartshroom" << std::endl;
	mPingCount = pings;
	mRefId = refid;
}

std::string SmartEntityImpulseShroomInstance::getSaveString()
{
	return std::string();
}

bool SmartEntityImpulseShroomInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartEntityImpulseShroomInstance::useWorldInstance(MWBase::Life * user)
{
	return nullptr;
}

void SmartEntityImpulseShroomInstance::onImpact(MWWorld::Ptr impactwith)
{
	

}

void SmartEntityImpulseShroomInstance::startCharge(MWBase::Life * user)
{
	std::cout << "impulse shroom start charge" << std::endl;
	auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(user->mPtr);
	sei->getStatusManager()->giveStatus(MWBase::ChargingImpulseShroom);
}

void SmartEntityImpulseShroomInstance::releaseCharge(MWBase::Life * user)
{
	std::cout << "impulse shroom release charge" << std::endl;
	auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(user->mPtr);
	sei->getStatusManager()->removeStatus(MWBase::ChargingImpulseShroom);
	sei->getStatusManager()->giveStatus(MWBase::ReleasedImpulseShroom);
}

	//MWBase::Environment::get().getWorld()->hurtCollidingActors
	//MWBase::Environment::get().getWorld()->createImpulseExplosion(mPtr.getCellRef().getPosition().asVec3(), 30);

	/*auto playerptr = MWBase::Environment::get().getWorld()->getPlayerPtr();
	std::cout << "SMART SHROOM IMPACT" << std::endl;
	bool standingon = MWBase::Environment::get().getWorld()->getPlayerStandingOn(mPtr);
	if (standingon)
		std::cout << "LAUNCH PLAYER" << std::endl;

	playerptr.getClass().getMovementSettings(playerptr).mAttemptJump = true;
	playerptr.getClass().getMovementSettings(playerptr).mPosition[2] = 1.0;
	playerptr.getClass().getCreatureStats(playerptr).setMovementFlag(MWMechanics::CreatureStats::Flag_ForceJump, true);*/
	
	//MWBase::Environment::get().getStatusManager()->giveStatus(MWBase::Environment::get().getWorld()->getPlayerPtr(), MWBase::ImpulseShroomLaunch);

