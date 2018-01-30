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

	Journey::Journey(std::string destId, MWTasks::Task * lifetask, float range):
		mDestId(destId)
		, mRange(range)
		, mHeadedToDoor(false)
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask->mNpcId;
		mNpcPtr = mLifeTask->mNpcPtr;
		mWasActiveLastUpdate = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcPtr); //when task was created, was npc in active range of player?
		
		//MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		mStep = 0;
		//mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		mDone = false;
		mTickCount = 0;
		init();
		mDeliveredByInactive = false;
		mHeadedToDoor = false;
	}

	MWWorld::Ptr Journey::update()
	{
		
		if (mStep > mTravelNodeItinerary.size()) 
			{
				mDone = true;
				return mNpcPtr;
			}
		
		//mNpcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false); //mwx fix me, do I really need to find pointer every update? Can I cache a permanant one in life?
		MWMechanics::AiSequence& seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence(); //Do I really need to find the seq ref every update? Can I cache a permanant one in life?
		bool currentlyActive = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcPtr);
		

		if (hasArrived(mDestId))
		{
			std::cout << mNpcId + " has arrived within radius of dest" << std::endl;
			seq.clear();
			mDone = true;
			return mNpcPtr;
		}
		if (mWasActiveLastUpdate && !currentlyActive) //npc has left active range since last update
		{
			//seq.clear(); //wherever npc was walking to, stop walking there.
			mWasActiveLastUpdate = false; //flag that npc was no active this update
			//if(mStep > 0 && mTravelNodeItinerary.size() == 2)
			//	mStep -= 1; //go back one step, because now we want to teleport to the step we just tried to do
			//leftActiveCells();
			//return mNpcPtr;
		}
		else if (!currentlyActive) //not active, so just run inactive logic.
		{
			//seq.clear(); //mwx fix me so darn messy this use of clear()
			inactiveUpdate();
			return mNpcPtr;
		}
		else if (currentlyActive && !mWasActiveLastUpdate) //suddenly we find ourselves in players sight (or at least in their active range)!
		{
			if (!seq.isPackageDone()) //we were in middle of something before, lets just complete it for now.
			{
				mWasActiveLastUpdate = true;
				return mNpcPtr;
			}
			//else //Package is done, maybe it is because a travel or something finished?
			//{


			//}

		}
		
		//all this below and heck probably above likely needs to be refactored mwx fix me

		if (seq.getTypeId() == -1 || seq.getTypeId() == 0) //nothing in seq means AI is no longer walking, waiting for next dest.
		{
			/*if (mStep == mTravelNodeItinerary.size())
			{
				mDone = true;
				
			}
			else {*/
				if (mHeadedToDoor)
				{
					mHeadedToDoor = false;
					auto tnodeId = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker);
				
					MWWorld::Ptr tnode = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false);
					MWBase::Environment::get().getWorld()->activate(tnode, mNpcPtr);
					std::cout << "attempted to open door" << std::endl;
					return  MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
				}
				std::string tnodeId;
				
				//if (hasArrived(getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker)))
				if (mDeliveredByInactive) //if we are making a new package and just arrived in active area, probably sent here by an inactive update which iterates mstepn on its own mwx fix me. Or this whole journey package is new
				{
					mDeliveredByInactive = false;
					
				}
				else
				{
					mStep += 1;
				}
					
				if (mStep >= mTravelNodeItinerary.size())
					tnodeId = mDestId;
				else
					tnodeId = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker);
				if (MWBase::Environment::get().getWorld()->searchPtr(tnodeId, true))
					std::cout << "dest in cell" << std::endl;
				else
					std::cout << "dest outside of cell" << std::endl;
				std::cout << "tnode is: " << tnodeId << std::endl;
				MWWorld::Ptr tnode = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false); //find transition node.
				ESM::Position tnodePos = tnode.getRefData().getPosition();
				if (tnode.getClass().isDoor())
					mHeadedToDoor = true;
				if (mHeadedToDoor) //look up opposite door, and look where it sends us. Breaks one way doors though.
				{
					std::string arrivaldoor = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker);
					auto arrivaldoorptr = MWBase::Environment::get().getWorld()->searchPtr(arrivaldoor, false);
					tnodePos = arrivaldoorptr.getCellRef().getDoorDest();

				}
				seq.stack(MWMechanics::AiTravel(tnodePos.pos[0], tnodePos.pos[1], tnodePos.pos[2]), mNpcPtr);
				mWasActiveLastUpdate = true;
			}
		

		mNpcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false); //this is to catch a situation where ptr gets outdated and I'm not sure where.
		return mNpcPtr;
	}

	void Journey::leftActiveCells()
	{
		mWasActiveLastUpdate = false;
	}

	bool Journey::hasArrived(std::string destid)
	{
		//MWWorld::Ptr npc = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWWorld::Ptr dest = MWBase::Environment::get().getWorld()->searchPtr(destid, false);
		bool inRange = (mNpcPtr.getRefData().getPosition().asVec3() - dest.getRefData().getPosition().asVec3()).length2() <= mRange;
		return inRange;
	}

	void Journey::inactiveUpdate()
	{
		//if (mStep == mTravelNodeItinerary.size()) //we have just teleported to the final destination
		//{
		//	mDone = true;
		//	return;
		//}
		mTickCount += 1; //add tick, so we don't teleport every update. This can be used to determine virtual travel time. 
		if (mTickCount >= 5)
		{
			if (mStep == 0)
			{
				mStep = 1;
			}
			MWMechanics::AiSequence& seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence(); //Do I really need to find the seq ref every update? Can I cache a permanant one in life?
			std::string tnodeId;
			mTickCount = 0;
			//mStep += 1;
			if (mStep == mTravelNodeItinerary.size()) //if we are on last step, teleport to the destination
			{
				tnodeId = mDestId;
				mDone = true;
			}
			else //otherwise navigate along bordernodes
			{
				//this might be better if I teleport them to the actual t node, as opposed to the transition node. Also would border node be better? Yes.
				tnodeId = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker);
			}
			//if (mHeadedToDoor) //if we are headed to door, actually teleport to opposite side of door.
			//{
			//	tnodeId = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker);
			//}
			
			std::cout << "teleporting to... " + tnodeId << std::endl;
			//This double string movement method really should be a method. mwx fix me
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false);
			//if (marker.getClass().isDoor())
			mHeadedToDoor = marker.getClass().isDoor(); //if our destination is door, mark it as such so npc can open it if they end up in active cell
			//if (mHeadedToDoor) //if we are headed to door, actually teleport to opposite side of door.
			//{
			//	tnodeId = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker);
			//	
			//	
			//	marker = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false);
			//
			//}
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			
			MWWorld::CellStore* store = marker.getCell();
			if (mHeadedToDoor)
			{
				auto opendoor = marker;
				std::string arrivaldoorid = getBorderNodeId(mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep]]->marker, mTravelNodesManager->mtravelNodeMap[mTravelNodeItinerary[mStep - 1]]->marker);
				auto arrivaldoor = MWBase::Environment::get().getWorld()->searchPtr(arrivaldoorid, false);
				if (arrivaldoor.getCell()->isExterior())
				{

					markerPos = opendoor.getCellRef().getDoorDest();
					int cellX, cellY;
					MWBase::Environment::get().getWorld()->positionToIndex(markerPos.pos[0], markerPos.pos[1], cellX, cellY);
					store = MWBase::Environment::get().getWorld()->getExterior(cellX, cellY);
					//store = MWBase::Environment::get().getWorld()->getInterior(marker.getCellRef().getDestCell());//but what if not interior?
				}
				else 
				{
					markerPos = opendoor.getCellRef().getDoorDest();
					// door leads to an interior, use interior name as tooltip
					store = MWBase::Environment::get().getWorld()->getInterior(opendoor.getCellRef().getDestCell());
					//std::string dest = door.mRef.getDestCell();
				}
				//store = MWBase::Environment::get().getWorld()->getExterior(markerPos.pos[0], markerPos.pos[1]);
					
			}
			seq.clear(); //if there was an ai package we were holding on to, gone now, player has reached node
			mNpcPtr = MWBase::Environment::get().getWorld()->moveObject(mNpcPtr, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
			mHeadedToDoor = false; 
			mStep += 1;
			mDeliveredByInactive = true;
		}
		
		return;
	}

	

	int Journey::getTypeId() const
	{
		return TypeIDJourney;
	}

	std::string Journey::getBorderNodeId(std::string tnode1, std::string tnode2)
	{
		//example, input is v1 and vhome1
		//output is bn_v1tovhome1
		tnode1.erase(0, 3);
		tnode2.erase(0, 3);	
		return "bn_"+tnode1+"to"+tnode2;
	}

	bool Journey::init()
	{
		
		std::cout << mNpcId + "is searching for dest id: " + mDestId << std::endl;
		//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWWorld::Ptr destPtr = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
		int currentNode = mTravelNodesManager->mCellToNodeMap[mNpcPtr.getCell()]->id;
		int destNode = mTravelNodesManager->mCellToNodeMap[destPtr.getCell()]->id;
		auto path = mTravelNodesManager->mtravelPathGridGraph.aStarSearch(currentNode, destNode); //WANT CURRENT NODE END NODE
		for (std::list<ESM::Pathgrid::Point>::iterator it = path.begin(); it != path.end(); it++)
				{
					mTravelNodeItinerary.push_back(it->mUnknown); //a list of all cells npc needs to travel to, 0 is current, final is where the cell that the destination object is in
				}
		return true; //fail conditionw ill be added at some time 
	}
}

