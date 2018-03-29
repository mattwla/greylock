#include "awarenessreactionsmanagerimp.hpp"


#include <limits.h>
#include <iostream>
#include <fstream>

#include <components/misc/rng.hpp>

#include <components/esm/esmwriter.hpp>
#include <components/esm/stolenitems.hpp>

#include <components/sceneutil/positionattitudetransform.hpp>

#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/timestamp.hpp"
#include "../mwbase/lifemanager.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aiface.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "../mwmechanics/steering.hpp"


#include "../mwworld/player.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/actionteleport.hpp"
#include "../mwworld/projectilemanager.hpp"
#include "../mwworld/weather.hpp"
#include "../mwworld/scene.hpp"
#include "../mwworld/worldimp.cpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwtasks/task.hpp"
#include "../mwtasks/fight.hpp"
#include "../mwtasks/confronttrespasser.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

#include <osg/Group>
#include <osg/ComputeBoundsVisitor>


namespace MWAwarenessReactions
{
	AwarenessReactionsManager::AwarenessReactionsManager()
	{

	}

	//void AwarenessReactionsManager::updateActiveAffordances()
	//{
	//	mLiveCellAffordances.clear();
	//	mLiveCellGuardZones.clear();
	//	mLiveCellAffordances.push_back(MWBase::Environment::get().getWorld()->getPlayerPtr()); //player always active
	//	MWWorld::Ptr npc;
	//	std::vector<MWWorld::Ptr> out;
	//	std::string lowerCaseName = "affordance";
	//	auto mActiveCells = MWBase::Environment::get().getWorld()->getActiveCells();
	//	const MWWorld::Scene::CellStoreCollection& collection = mActiveCells;
	//	for (MWWorld::Scene::CellStoreCollection::const_iterator cellIt = collection.begin(); cellIt != collection.end(); ++cellIt)
	//	{
	//		MWWorld::ListObjectsVisitor visitor;
	//		(*cellIt)->forEach(visitor);

	//		for (std::vector<MWWorld::Ptr>::iterator it = visitor.mObjects.begin(); it != visitor.mObjects.end(); ++it)
	//		{
	//			std::string type = it->getClass().getTypeName();
	//			if (type == typeid(ESM::NPC).name())
	//			{
	//				mLiveCellAffordances.push_back(*it);
	//			}
	//			else if (type == typeid(ESM::Activator).name() && it->getCellRef().getRefId().substr(0, 3) == "gz_") //if guarded zone
	//			{
	//				mLiveCellGuardZones[*it] = MWBase::Environment::get().getWorld()->getGlobalInt(it->getCellRef().getRefId() + "radius");
	//				std::cout << "stored guard zone" + it->getCellRef().getRefId() << std::endl;
	//				std::cout << MWBase::Environment::get().getWorld()->getGlobalInt(it->getCellRef().getRefId() + "radius") << std::endl;
	//			}
	//		}
	//	}
	//}

	//std::vector<MWWorld::Ptr> AwarenessReactionsManager::calculateAwareness(MWWorld::Ptr ptr)
	//{
	//	std::vector<MWWorld::Ptr> vec;
	//	unsigned int idx = 0;
	//	while (idx < mLiveCellAffordances.size())
	//	{

	//		if(MWBase::Environment::get().getWorld()->getLOS(ptr, mLiveCellAffordances[idx]) && awarenessCheck(mLiveCellAffordances[idx], ptr) && mLiveCellAffordances[idx] != ptr)
	//		{
	//			vec.push_back(mLiveCellAffordances[idx]);
	//		}
	//		idx += 1;
	//	}

	//	mNpcAwareOf[ptr] = vec; //mwx fix me massive memory leak here, updating ptr means massive bloat.
	//	return vec;
	//}

	//bool AwarenessReactionsManager::awarenessCheck(const MWWorld::Ptr &ptr, const MWWorld::Ptr &observer)
	//{
	//	if (observer.getClass().getCreatureStats(observer).isDead() || !observer.getRefData().isEnabled())
	//		return false;
	//	osg::Vec3f pos1(ptr.getRefData().getPosition().asVec3());
	//	osg::Vec3f pos2(observer.getRefData().getPosition().asVec3());
	//	// is ptr behind the observer?
	//	osg::Vec3f vec = pos1 - pos2;
	//	if (observer.getRefData().getBaseNode())
	//	{
	//		osg::Vec3f observerDir = (observer.getRefData().getBaseNode()->getAttitude() * osg::Vec3f(0, 1, 0));

	//		float angleRadians = std::acos(observerDir * vec / (observerDir.length() * vec.length()));
	//		if (angleRadians > osg::DegreesToRadians(90.f))
	//			return false;
	//		else
	//			return true;
	//	}

	//	std::cout << "couldnt get basenode for awareness check" << std::endl;
	//	return false;
	//}

	//std::map<MWTasks::Task*, int> AwarenessReactionsManager::calculateReactions(MWWorld::Ptr npc, MWBase::Life& life) //run every frame, figures out all the things npcs can do with what they can see.
	//{
	//	std::map<MWTasks::Task*, int> reactions;
	//	auto awareof = mNpcAwareOf[npc];
	//	unsigned int idx = 0;
	//	bool seetrespassing = false;
	//	bool isShaman = MWBase::Environment::get().getStatusManager()->hasStatus(npc, MWBase::Shaman);
	//	while (idx < awareof.size())
	//	{
	//		if (awareof[idx] == MWBase::Environment::get().getWorld()->getPlayerPtr())
	//		{
	//			for (auto& kv : mLiveCellGuardZones)
	//			{
	//				int radius = kv.second;
	//				MWWorld::Ptr ptr = kv.first;
	//				seetrespassing = (awareof[idx].getRefData().getPosition().asVec3() - ptr.getRefData().getPosition().asVec3()).length2() <= (radius * radius);

	//				if (seetrespassing)
	//				{
	//					if (isShaman && !MWBase::Environment::get().getStatusManager()->hasStatus(npc, MWBase::Fighting))
	//					{

	//						//reactions[new MWTasks::Fight(life.mTaskChain, awareof[idx])] = 4; //make a fight task, offer it to lifemanager
	//						life.mCurrentGuardZone = kv.first; //set which guard zone we are concerned about to figure out where to escort player if they surrender
	//						reactions[new MWTasks::ConfrontTrespasser(life.mTaskChain, awareof[idx])] = 4;
	//					}
	//				}
	//				else
	//				{
	//					if (MWBase::Environment::get().getStatusManager()->hasStatus(npc, MWBase::Guarding))
	//					{
	//						turnTo(npc, awareof[idx]);
	//					}
	//				}
	//			}
	//			
	//		}
	//		idx += 1;
	//	}
	//	
	//	return reactions;
	//}

	//bool AwarenessReactionsManager::turnTo(MWWorld::Ptr actor, MWWorld::Ptr target) {
	//
	//	osg::Vec3f targetPos(target.getRefData().getPosition().asVec3());
	//	osg::Vec3f actorPos(actor.getRefData().getPosition().asVec3());

	//	osg::Vec3f dir = targetPos - actorPos;

	//	float faceAngleRadians = std::atan2(dir.x(), dir.y());

	//	if (true)
	//	{
	//		if (MWMechanics::zTurn(actor, faceAngleRadians))
	//			bool rotate = false;
	//		else
	//			return false;
	//	}
	//	
	//}

	//MWWorld::Ptr AwarenessReactionsManager::getGuardZoneOut(MWWorld::Ptr npcPtr) {
	//	auto list = MWBase::Environment::get().getLifeManager()->mLifeList;
	//	int itx = 0;
	//	while (itx < list.size())
	//	{
	//		if (list[itx]->mPtr == npcPtr) //mwx fix me refactor
	//		{
	//			std::string gzstring = list[itx]->mCurrentGuardZone.getCellRef().getRefId();
	//			//std::cout << "going to" + MWBase::Environment::get().getWorld()->searchPtr(gzstring + "_out", true) << std::endl;
	//			delete list[itx]->mSubTask;
	//			list[itx]->mSubTask = 0;
	//			list[itx]->mCurrentTask->resume();
	//			MWBase::Environment::get().getStatusManager()->removeStatus(npcPtr, MWBase::Fighting);
	//			return MWBase::Environment::get().getWorld()->searchPtr(gzstring + "_out", true);
	//		}
	//		itx += 1;
	//	}
	//}
}