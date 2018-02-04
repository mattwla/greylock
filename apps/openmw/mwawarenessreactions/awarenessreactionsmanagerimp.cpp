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

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"


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

	void AwarenessReactionsManager::updateActiveAffordances()
	{
		mLiveCellAffordances.clear();
		mLiveCellGuardZones.clear();
		mLiveCellAffordances.push_back(MWBase::Environment::get().getWorld()->getPlayerPtr()); //player always active
		MWWorld::Ptr npc;
		std::vector<MWWorld::Ptr> out;
		std::string lowerCaseName = "affordance";
		auto mActiveCells = MWBase::Environment::get().getWorld()->getActiveCells();
		//auto mCells = MWBase::Environment::get().getWorld()->getCells();
		//for (MWWorld::Scene::CellStoreCollection::const_iterator iter(mActiveCells.begin());
		//	iter != mActiveCells.end(); ++iter)
		//{
		//	// TODO: caching still doesn't work efficiently here (only works for the one CellStore that the reference is in)
		//	MWWorld::CellStore* cellstore = *iter;
		//	MWWorld::Ptr ptr = mCells.getPtr(lowerCaseName, *cellstore, false);

		//	if (!ptr.isEmpty())
		//		std::cout << "hi" << std::endl;
		//	//return ptr;
		//}

		const MWWorld::Scene::CellStoreCollection& collection = mActiveCells;
		for (MWWorld::Scene::CellStoreCollection::const_iterator cellIt = collection.begin(); cellIt != collection.end(); ++cellIt)
		{
			MWWorld::ListObjectsVisitor visitor;
			(*cellIt)->forEach(visitor);

			for (std::vector<MWWorld::Ptr>::iterator it = visitor.mObjects.begin(); it != visitor.mObjects.end(); ++it)
				//if (Misc::StringUtils::ciEqual(it->getCellRef().getOwner(), npc.getCellRef().getRefId()))
			{
				std::string type = it->getClass().getTypeName();
				if (type == typeid(ESM::NPC).name())
				{
					mLiveCellAffordances.push_back(*it);
					//std::cout << "found npc" << it->getCellRef().getRefId() << std::endl;
				}
				else if (type == typeid(ESM::Activator).name() && it->getCellRef().getRefId().substr(0, 3) == "gz_") //if guarded zone
				{
					mLiveCellGuardZones[*it] = MWBase::Environment::get().getWorld()->getGlobalInt(it->getCellRef().getRefId() + "radius");
					std::cout << "stored guard zone" << std::endl;
					//it->getCellRef().
				}
			}
		}
	}
	std::vector<MWWorld::Ptr> AwarenessReactionsManager::calculateAwareness(MWWorld::Ptr ptr)
	{
		std::vector<MWWorld::Ptr> vec;
		unsigned int idx = 0;
		while (idx < mLiveCellAffordances.size())
		{

			if(MWBase::Environment::get().getWorld()->getLOS(ptr, mLiveCellAffordances[idx]) && awarenessCheck(mLiveCellAffordances[idx], ptr) && mLiveCellAffordances[idx] != ptr)
			{
				if (ptr.getCellRef().getRefId() == "slade")
					std::cout << "npc aware of" + mLiveCellAffordances[idx].getCellRef().getRefId() << std::endl;
				vec.push_back(mLiveCellAffordances[idx]);
			}
			idx += 1;
		}

		mNpcAwareOf[ptr] = vec;
		return vec;
	}

	bool AwarenessReactionsManager::awarenessCheck(const MWWorld::Ptr &ptr, const MWWorld::Ptr &observer)
	{
		if (observer.getClass().getCreatureStats(observer).isDead() || !observer.getRefData().isEnabled())
			return false;

		
		osg::Vec3f pos1(ptr.getRefData().getPosition().asVec3());
		osg::Vec3f pos2(observer.getRefData().getPosition().asVec3());
		
		// is ptr behind the observer?
		
//		float y = 0;
		osg::Vec3f vec = pos1 - pos2;
		if (observer.getRefData().getBaseNode())
		{
			osg::Vec3f observerDir = (observer.getRefData().getBaseNode()->getAttitude() * osg::Vec3f(0, 1, 0));

			float angleRadians = std::acos(observerDir * vec / (observerDir.length() * vec.length()));
			if (angleRadians > osg::DegreesToRadians(90.f))
				return false;
			else
				return true;
		}

		std::cout << "couldnt get basenode for awareness check" << std::endl;
		return false;

	}

	void AwarenessReactionsManager::calculateReaction(MWWorld::Ptr npc) //right now, only forced combat if npc sees player near guarded zone
	{
		auto awareof = mNpcAwareOf[npc];
		unsigned int idx = 0;
		bool trespassing = false;
		while (idx < awareof.size())
		{
			if (awareof[idx] == MWBase::Environment::get().getWorld()->getPlayerPtr())
			{
				for (auto& kv : mLiveCellGuardZones)
				{
					int radius = kv.second;
					MWWorld::Ptr ptr = kv.first;
					std::cout << "checking if player is tresspassing..." << std::endl;
					std::cout << (awareof[idx].getRefData().getPosition().asVec3() - ptr.getRefData().getPosition().asVec3()).length2() << std::endl;
					std::cout << radius << std::endl;
					trespassing = (awareof[idx].getRefData().getPosition().asVec3() - ptr.getRefData().getPosition().asVec3()).length2() <= (radius * radius) ;
				}
				
				if (trespassing)
				{
					MWBase::Environment::get().getMechanicsManager()->startCombat(npc, awareof[idx]);
					return;
				}
				
			}
			idx += 1;
		}
		
		//MWBase::Environment::get().getTasksManager()->mNpcMap
	}

	



}