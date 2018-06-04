#include "floatshroom.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwclass/classes.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"




SmartEntityFloatShroomTemplate::SmartEntityFloatShroomTemplate()
{
	mIngameIDs.push_back("floatshroom");
}

MWBase::SmartEntityInstance * SmartEntityFloatShroomTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	SmartEntityFloatShroomInstance * instance = new SmartEntityFloatShroomInstance(ptr);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityFloatShroomTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityFloatShroomInstance * instance = new SmartEntityFloatShroomInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityFloatShroomTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

SmartEntityFloatShroomInstance::SmartEntityFloatShroomInstance(const MWWorld::Ptr & ptr)
{

	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartFloatShroom" << std::endl;
	mCurrentUserCount = 0;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
}

SmartEntityFloatShroomInstance::SmartEntityFloatShroomInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new smartshroom" << std::endl;
	mPingCount = pings;
	mRefId = refid;
}

std::string SmartEntityFloatShroomInstance::getSaveString()
{
	return std::string();
}

bool SmartEntityFloatShroomInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartEntityFloatShroomInstance::useWorldInstance(MWBase::Life * user)
{
	return nullptr;
}

void SmartEntityFloatShroomInstance::onImpact(MWWorld::Ptr impactwith)
{
	if (impactwith)
	{
		//MWBase::Environment::get().getStatusManager()->giveStatus(impactwith, MWBase::FloatShroomPowdered);
		auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(impactwith, true);
		if (sei)
		{
			sei->getStatusManager()->giveStatus(MWBase::FloatShroomPowdered);
		}
		std::cout << "gave float status" << std::endl;
	
		disable();
	}



}

	//MWBase::Environment::get().getWorld()->hurtCollidingActors
	//MWBase::Environment::get().getWorld()->createFloatExplosion(mPtr.getCellRef().getPosition().asVec3(), 30);

	/*auto playerptr = MWBase::Environment::get().getWorld()->getPlayerPtr();
	std::cout << "SMART SHROOM IMPACT" << std::endl;
	bool standingon = MWBase::Environment::get().getWorld()->getPlayerStandingOn(mPtr);
	if (standingon)
		std::cout << "LAUNCH PLAYER" << std::endl;

	playerptr.getClass().getMovementSettings(playerptr).mAttemptJump = true;
	playerptr.getClass().getMovementSettings(playerptr).mPosition[2] = 1.0;
	playerptr.getClass().getCreatureStats(playerptr).setMovementFlag(MWMechanics::CreatureStats::Flag_ForceJump, true);*/
	
	//MWBase::Environment::get().getStatusManager()->giveStatus(MWBase::Environment::get().getWorld()->getPlayerPtr(), MWBase::FloatShroomLaunch);

