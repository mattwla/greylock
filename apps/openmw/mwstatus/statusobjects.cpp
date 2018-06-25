#include "statusobjects.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/esmstore.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/class.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwclass/npc.hpp"
#include "../mwrender/animation.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/containerstore.hpp"

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

	if (MWBase::Environment::get().getWorld()->isInitializingWorld())
		return;
	
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
			return;
		}
	}

	
	
	//MWBase::Environment::get().getWorld()->obje
	

	
	if (mSEI->getPtr().isInCell())
	{
		MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(mSEI->getPtr());

		const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
		int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
		const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);


		animation->addSpellCastGlow(effect, 5); // 1 second glow to match the time taken for a door opening or closing




		auto pos = mSEI->getPtr().getRefData().getPosition().pos;
		auto actor = mSEI->getPtr();

		if (!fireptr || fireptr.getRefData().isDeleted())
		{
			fireptr = MWBase::Environment::get().getWorld()->safePlaceObject(mFireRef->getPtr(), actor, actor.getCell(), 0, 0);
		}
		else
			MWBase::Environment::get().getWorld()->moveObject(fireptr, pos[0], pos[1], pos[2]);



		//MWBase::Environment::get().getWorld()->deleteObject(fireptr);
	
	
		auto seilist = MWBase::Environment::get().getSmartEntitiesManager()->getLiveSmartInstances();

		for (MWBase::SmartInstanceMap::iterator it = seilist.begin(); it != seilist.end(); it++)
		{
			if (it->second->getStatusManager()->hasStatus(MWBase::IsFlammable))
			{
				if (mSEI->containsPtr(it->second->getPtr()))
					it->second->getStatusManager()->giveStatus(MWBase::OnFire);
			}
		}

	
	
	}
	else
	{
		if (!fireptr.getRefData().isDeleted())
		{
			MWBase::Environment::get().getWorld()->deleteObject(fireptr);
		}

		if (mSEI->getInInventorySEI())
		{
			auto invptr = mSEI->getInInventorySEI()->getPtr();
			MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(invptr);

			const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
			int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
			const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);


			animation->addSpellCastGlow(effect, 5); // 1 second glow to match the time taken for a door opening or closing


		}
		//mSEI->getPtr().mContainerStore->
		//MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(mUserLife->mPtr);

		//const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
		//int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
		//const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);

		//animation->addSpellCastGlow(effect, duration);
		////isReady = true;
	}
	

	//MWWorld::ConstPtr constactor = mSEI->getPtr();
	//fireptr = MWBase::Environment::get().getWorld()->safePlaceObject(ref.getPtr(), actor, actor.getCell(), 0, 0);

	//std::vector<MWWorld::Ptr> out;
	//MWBase::Environment::get().getWorld()->getCollidingObjects(constactor, out);

	

	//if (out.size() > 0)
	//	std::cout << "spread fire" << std::endl;
	


	//get fire bounding box
	//see if anything in it
	//if in it, give it fire status
}

void MWBase::OnFireStatusObject::init()
{
	
	bool burnglow = true;

	if (MWBase::Environment::get().getWorld()->isInitializingWorld())
		burnglow = false;

	if (burnglow)
	{
		
			MWRender::Animation* animation = MWBase::Environment::get().getWorld()->getAnimation(mSEI->getPtr());

		const MWWorld::ESMStore& store = MWBase::Environment::get().getWorld()->getStore();
		int index = ESM::MagicEffect::effectStringToId("sEffectTelekinesis");
		const ESM::MagicEffect *effect = store.get<ESM::MagicEffect>().find(index);
		

		animation->addSpellCastGlow(effect, 5); // 1 second glow to match the time taken for a door opening or closing

	}

	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::OnFire);
	mFireRef = new MWWorld::ManualRef(MWBase::Environment::get().getWorld()->getStore(), "light_fire_300", 1);
	//mFireRef = ref;
	MWWorld::Ptr actor = mSEI->getPtr();
	//MWWorld::ConstPtr constactor = mSEI->getPtr();


	//fireptr = MWBase::Environment::get().getWorld()->safePlaceObject(mFireRef->getPtr(), actor, actor.getCell(), 0, 0);
	
	//std::vector<MWWorld::Ptr> out;
	//MWBase::Environment::get().getWorld()->getCollidingObjects(constactor, out);

//	mSEI->buildBoundingBox();


	//MWBase::Environment::get().getWorld()->enableActorCollision(fireptr, false);
	
}

void MWBase::OnFireStatusObject::end()
{
	MWBase::Environment::get().getWorld()->disable(fireptr);
	mSEI->getStatusManager()->removeStatus(MWBase::OnFire);
	delete mFireRef;
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
