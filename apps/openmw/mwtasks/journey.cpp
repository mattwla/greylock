#include "journey.hpp"

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
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{

	Journey::Journey()
	{
	}

	Journey::Journey(MWWorld::Ptr dest):
		mDestination(dest)
	{
		
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		mDone = false;
	}

	Journey::Journey(std::string destId, std::string npcId, float range):
		mDestId(destId)
		, mRange(range)
		, mHeadedToDoor(false)
	{
		mWasActiveLastUpdate = MWBase::Environment::get().getTasksManager()->isInActiveRange(npcId);
		mNpcId = npcId;
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		init();
		mReadyForUpdate = true;
		mDone = false;
		mTickCount = 0;
	}

	void Journey::update()
	{


		MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWMechanics::AiSequence& seq = npcPtr.getClass().getCreatureStats(npcPtr).getAiSequence();
		bool currentlyActive = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId);

		if (hasArrived())
		{
			std::cout << "has arrived shortcut" << std::endl;
			mDone = true;
			seq.clear();
			return;
		}
		
	
		


		if (currentlyActive != mWasActiveLastUpdate)
		{
			//std::cout << "swapping active status..." << std::endl;
			leftActiveCells();
			return;
		}
		else if (!currentlyActive)
		{
			inactiveUpdate();
			return;
		}
		
		//all this below and heck probably above likely needs to be refactored mwx fix me

		if (mReadyForUpdate == true || seq.getTypeId() == -1)
		{
			if (mStep == mTravelNodeItinerary.size())
			{
				mDone = true;
				
			}
			else if (mHeadedToDoor)
			{
				mHeadedToDoor = false;
				auto tnodeId = "tn_" + std::to_string((mTravelNodeItinerary[mStep - 1])) + "to" + std::to_string(mTravelNodeItinerary[mStep]);
				MWWorld::Ptr tnode = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false);
				MWBase::Environment::get().getWorld()->activate(tnode, npcPtr);
				std::cout << "attempted to open door" << std::endl;

			}
				
			else {
				std::string tnodeId;
				mStep += 1;
				if (mStep == mTravelNodeItinerary.size())
				{
					tnodeId = mDestId;
				}
				else
				{
					tnodeId = "tn_" + std::to_string((mTravelNodeItinerary[mStep - 1])) + "to" + std::to_string(mTravelNodeItinerary[mStep]); //this back to strings
				}

				if (MWBase::Environment::get().getWorld()->searchPtr(tnodeId, true))
					std::cout << "dest in cell" << std::endl;
				else
					std::cout << "dest outside of cell" << std::endl;

				std::cout << "tnode is: " << tnodeId << std::endl;
				MWWorld::Ptr tnode = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false); //find transition node.
				//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
				ESM::Position tnodePos = tnode.getRefData().getPosition();
				if (tnode.getClass().isDoor())
				{
					mHeadedToDoor = true;
				}
				//MWMechanics::AiSequence& seq = npcPtr.getClass().getCreatureStats(npcPtr).getAiSequence();
				seq.stack(MWMechanics::AiTravel(tnodePos.pos[0], tnodePos.pos[1], tnodePos.pos[2]), npcPtr);
				mReadyForUpdate = false;
				if (mNpcId == "barnabas")
				{
					std::cout << "barnabas journey" << std::endl;
				}
			}
		}

	}

	void Journey::leftActiveCells()
	{
		mWasActiveLastUpdate = false;

	}

	bool Journey::hasArrived()
	{


		MWWorld::Ptr npc = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWWorld::Ptr dest = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);

		bool inRange = (npc.getRefData().getPosition().asVec3() - dest.getRefData().getPosition().asVec3()).length2() <= mRange;


		return inRange;
	}

	void Journey::inactiveUpdate()
	{
		if (mStep == mTravelNodeItinerary.size())
		{
			mDone = true;
			return;

		}
		mTickCount += 1;
		if (mTickCount >= 5)
		{
			std::string tnodeId;
			mTickCount = 0;
			mStep += 1;
			if (mStep == mTravelNodeItinerary.size())
			{
				tnodeId = mDestId;
			}
			else
			{
				tnodeId = "tn_" + std::to_string((mTravelNodeItinerary[mStep - 1])) + "to" + std::to_string(mTravelNodeItinerary[mStep]);
			}
			std::cout << "teleporting to... " + tnodeId << std::endl;
			//This double string movement method really should be a method.
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false);
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			MWWorld::CellStore* store = marker.getCell();
			MWBase::Environment::get().getWorld()->moveObject(npcPtr, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
		}
		
		return;
	}

	

	int Journey::getTypeId() const
	{
		return TypeIDJourney;
	}


	bool Journey::init()
	{
		MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWWorld::Ptr destPtr = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);


		int currentNode = mTravelNodesManager->mCellToNodeMap[npcPtr.getCell()]->id;
		int destNode = mTravelNodesManager->mCellToNodeMap[destPtr.getCell()]->id;

		auto path = mTravelNodesManager->mtravelPathGridGraph.aStarSearch(currentNode, destNode); //WANT CURRENT NODE END NODE

		for (std::list<ESM::Pathgrid::Point>::iterator it = path.begin(); it != path.end(); it++)
				{
					mTravelNodeItinerary.push_back(it->mUnknown);
				}
		
		
		
		return true;


	}
}

