#include "statusobjects.hpp"'
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwclass/npc.hpp"


void MWBase::FloatStatusObject::update(float duration)
{

	mTotalTime += duration;

	if (mTotalTime > 7.0)
	{
		end();
		return;
	}

	//MWBase::Environment::get().getWorld()->queueMovement(mSEI->getPtr(), osg::Vec3f(100.f, 0.f, 600.f));
	auto pos = mSEI->getPtr().getRefData().getPosition().pos;
	MWBase::Environment::get().getWorld()->moveObject(mSEI->getPtr(), pos[0], pos[1], pos[2]+1.0f );
	std::cout << "updating float" << std::endl;

}

void MWBase::FloatStatusObject::init()
{
	//MWBase::Environment::get().getWorld()->addPhysicsActor(mSEI->getPtr());
	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::FloatShroomPowdered);
}

void MWBase::FloatStatusObject::end()
{
	mSEI->getStatusManager()->removeStatus(MWBase::FloatShroomPowdered);
	mDone = true;
}

void MWBase::OnFireStatusObject::update(float duration)
{
	mTotalTime += duration;
	if (mTotalTime > 5.0)
	{
		if (mSEI->isHumanLife())
		{
			auto mPtr = mSEI->getPtr();
			MWMechanics::CreatureStats& stats = mPtr.getClass().getCreatureStats(mPtr);
			MWMechanics::DynamicStat<float> health(mPtr.getClass().getCreatureStats(mPtr).getHealth());
			health.setCurrent(health.getCurrent() - 100.0f);
			stats.setHealth(health);
		}
		else
		{

			MWBase::Environment::get().getWorld()->disable(mSEI->getPtr());
		}
	}
	//MWBase::Environment::get().getWorld()->obje
	auto pos = mSEI->getPtr().getRefData().getPosition().pos;
	MWBase::Environment::get().getWorld()->moveObject(fireptr, pos[0], pos[1], pos[2]+150);
}

void MWBase::OnFireStatusObject::init()
{
	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::OnFire);
	MWWorld::ManualRef ref(MWBase::Environment::get().getWorld()->getStore(), "light_fire_300", 1);
	auto actor = mSEI->getPtr();
	fireptr = MWBase::Environment::get().getWorld()->safePlaceObject(ref.getPtr(), actor, actor.getCell(), 0, 0);
	
}

void MWBase::OnFireStatusObject::end()
{
	mSEI->getStatusManager()->removeStatus(MWBase::OnFire);
	mDone = true;
}
