#include "impulseshroom.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwclass/classes.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"
#include "../mwmechanics/drawstate.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwrender/animation.hpp"






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
	//user->mPtr.getClass().getCreatureStats(user->mPtr).land();
	std::cout << "impulse shroom start charge" << std::endl;
	auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(user->mPtr);
	sei->getStatusManager()->giveStatus(MWBase::ChargingImpulseShroom);


	//MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(user->mPtr);

	//const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
	//int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
	//const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);


	//animation->addSpellCastGlow(effect, 5);


}

void SmartEntityImpulseShroomInstance::releaseCharge(MWBase::Life * user)
{
	mAirTimer = 0.f;
	user->mPtr.getClass().getCreatureStats(user->mPtr).land();
	std::cout << "impulse shroom release charge" << std::endl;
	auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(user->mPtr);
	sei->getStatusManager()->removeStatus(MWBase::ChargingImpulseShroom);
	sei->getStatusManager()->giveStatus(MWBase::ReleasedImpulseShroom);
}

void SmartEntityImpulseShroomInstance::unequip(MWBase::Life * user)
{
	user->mPtr.getClass().getCreatureStats(user->mPtr).land();
	std::cout << "impulse shroom release charge" << std::endl;
	auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(user->mPtr);
	sei->getStatusManager()->removeStatus(MWBase::ChargingImpulseShroom);
	sei->getStatusManager()->setStoredImpulse(0.0f);
	MWBase::Environment::get().getSmartEntitiesManager()->removeSmartInstanceFromScene(mPtr);
	this->getStatusManager()->removeStatus(MWBase::RequiresUpdate);
}

void SmartEntityImpulseShroomInstance::onEquip(MWBase::Life * user)
{
	mUserLife = user;
	std::cout << "impulse shroom requested update" << std::endl;
	MWBase::Environment::get().getSmartEntitiesManager()->addSmartInstanceToScene(mPtr);
	this->getStatusManager()->giveStatus(MWBase::RequiresUpdate);
}

void SmartEntityImpulseShroomInstance::activateDuringCharge(MWBase::Life * user)
{
	user->mPtr.getClass().getCreatureStats(user->mPtr).land();
	std::cout << "impulse shroom nullify charge" << std::endl;
	auto sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(user->mPtr);
	sei->getStatusManager()->removeStatus(MWBase::ChargingImpulseShroom);
	sei->getStatusManager()->setStoredImpulse(0.0f);
	MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState_Nothing);
}

void SmartEntityImpulseShroomInstance::update(float duration)
{
	if (!MWBase::Environment::get().getWorld()->isOnGround(mUserLife->mPtr))
	{
		mAirTimer += duration;
	}
	else
		mAirTimer = 0.f;

	float distance = mUserLife->mPtr.getClass().getCreatureStats(mUserLife->mPtr).getFallHeight();
	
	if (mAirTimer > 2.0)
	{
		MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(mUserLife->mPtr);

		const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
		int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
		const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);

		animation->addSpellCastGlow(effect, duration);
		isReady = true;
	}
	else
	{
		isReady = false;
		MWBase::Environment::get().getWorld()->getPlayer().setAttackingOrSpell(false);
		
	}

	
	std::cout << distance << std::endl;
}

bool SmartEntityImpulseShroomInstance::getCanSwing()
{
	if(MWBase::Environment::get().getWorld()->isOnGround(mUserLife->mPtr))
		return true;

	return isReady;
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

