#include "statusobjects.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwclass/npc.hpp"
#include "../mwrender/animation.hpp"


void MWBase::FloatStatusObject::update(float duration)
{

	mTotalTime += duration;

	if (mTotalTime > 30.0)
	{
		end();
		return;
	}

	//MWBase::Environment::get().getWorld()->queueMovement(mSEI->getPtr(), osg::Vec3f(100.f, 0.f, 600.f));
	auto pos = mSEI->getPtr().getRefData().getPosition().pos;
	MWBase::Environment::get().getWorld()->moveObject(mSEI->getPtr(), pos[0], pos[1], pos[2]+200.0f*(duration/1) );
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
			//end();
		}
		else if (mSEI->getPtr().getClass().isDoor())
		{

			//MWBase::Environment::get().getWorld()->disable(mSEI->getPtr());
			mSEI->disable();
			end();
		}
	}
	//MWBase::Environment::get().getWorld()->obje
	auto pos = mSEI->getPtr().getRefData().getPosition().pos;
	MWBase::Environment::get().getWorld()->moveObject(fireptr, pos[0], pos[1], pos[2]);

	//get fire bounding box
	//see if anything in it
	//if in it, give it fire status
}

void MWBase::OnFireStatusObject::init()
{
	bool burnglow = true;

	if (burnglow)
	{
		
			MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(mSEI->getPtr());

		const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
		int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
		const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);
		

		animation->addSpellCastGlow(effect, 5); // 1 second glow to match the time taken for a door opening or closing

	}

	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::OnFire);
	MWWorld::ManualRef ref(MWBase::Environment::get().getWorld()->getStore(), "light_fire_300", 1);
	auto actor = mSEI->getPtr();
	fireptr = MWBase::Environment::get().getWorld()->safePlaceObject(ref.getPtr(), actor, actor.getCell(), 0, 0);
	
	//MWBase::Environment::get().getWorld()->enableActorCollision(fireptr, false);
	
}

void MWBase::OnFireStatusObject::end()
{
	MWBase::Environment::get().getWorld()->disable(fireptr);
	mSEI->getStatusManager()->removeStatus(MWBase::OnFire);
	mDone = true;
}

void MWBase::ThiefStatusObject::update(float duration)
{

	mTotalTime += duration;
	if (mTotalTime > 1.f)
		end();

}

void MWBase::ThiefStatusObject::init()
{
	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::Thief);
}

void MWBase::ThiefStatusObject::end()
{
	std::cout << "thief over" << std::endl;
	mDone = true;
	mSEI->getStatusManager()->removeStatus(MWBase::Thief);
}

void MWBase::AssaultStatusObject::update(float duration)
{

	mTotalTime += duration;
	if (mTotalTime > 1.f)
		end();
}

void MWBase::AssaultStatusObject::init()
{
	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::Assaulter);
}

void MWBase::AssaultStatusObject::end()
{
	std::cout << "assaulter over" << std::endl;
	mDone = true;
	mSEI->getStatusManager()->removeStatus(MWBase::Assaulter);
}
