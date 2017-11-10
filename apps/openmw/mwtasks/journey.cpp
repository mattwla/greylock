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

	/**Journey::Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp starttime) :
		mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mStartTime(starttime)
	{
	}

	Journey::Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, std::string task) :
		mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mOnCompleteTask(task)
	{
	}
	
	*/

	void Journey::update() //journey should become a task
	{
		mStep = mStep + 1;
		if (mStep < mTravelNodeItinerary.size()) {
			auto m = MWBase::Environment::get().getTravelNodesManager()->mtravelNodeMap[mTravelNodeItinerary[mStep]];

			MWWorld::Ptr markerPtr = MWBase::Environment::get().getWorld()->searchPtr(m->marker, false);
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);


			ESM::Position markerPos = markerPtr.getRefData().getPosition();
			MWWorld::CellStore* store = markerPtr.getCell();

			if (!MWBase::Environment::get().getWorld()->searchPtr(npcPtr.getCellRef().getRefId(), true)) //if npc is not in active cell
			{
				MWBase::Environment::get().getWorld()->moveObject(npcPtr, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
				std::cout << mNpcId << " at " << m->marker << std::endl;;
			}
			else //npc is in active cell
			{
				std::string tnodeId = "tn_" + std::to_string((mTravelNodeItinerary[mStep - 1])) + "to" + std::to_string(mTravelNodeItinerary[mStep]);
				std::cout << "tnode is: " << tnodeId << std::endl;
				MWWorld::Ptr tnode = MWBase::Environment::get().getWorld()->searchPtr(tnodeId, false); //find transition node.

				ESM::Position tnodePos = tnode.getRefData().getPosition();
				MWMechanics::AiSequence& seq = npcPtr.getClass().getCreatureStats(npcPtr).getAiSequence();
				seq.stack(MWMechanics::AiTravel(tnodePos.pos[0], tnodePos.pos[1], tnodePos.pos[2]), npcPtr);

				//SEQ TRAVEL HERE.....
			}
		}

	}

	//bool Journey::readyForUpdate()
	//{
	//	if (mStep == 0)
	//	{
	//		return true;
	//	}

	//	MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
	//	MWMechanics::AiSequence& seq = npcPtr.getClass().getCreatureStats(npcPtr).getAiSequence();
	//	if (seq.getTypeId() != 1) //Are we not currently travelling?
	//	{
	//		if (((MWBase::Environment::get().getWorld()->getTimeStamp() - mStartTime) / mStep) <= 1) //Have we reached right time? But mEndTime is for whole journey.... not each piece.
	//		{
	//			return true;
	//		}

	//	}


	//	return false;
	//}

	/*bool Journey::travel(MWWorld::Ptr npc, MWWorld::Ptr dest)
	{
		bool actorInLive;
		bool destInLive;

		actorInLive = MWBase::Environment::get().getWorld()->searchPtr(npc.getCellRef().getRefId(), true);
		destInLive = MWBase::Environment::get().getWorld()->searchPtr(dest.getCellRef().getRefId(), true);

		if (actorInLive && destInLive) //NPC can just walk there, so do that..... for now
		{
			ESM::Position destPos = dest.getRefData().getPosition();
			MWMechanics::AiSequence& seq = npc.getClass().getCreatureStats(npc).getAiSequence();
			seq.stack(MWMechanics::AiTravel(destPos.pos[0], destPos.pos[1], destPos.pos[2]), npc);
			return true;
		}

		MWWorld::TimeStamp tstamp = MWBase::Environment::get().getWorld()->getTimeStamp();

		//std::cout << npc.getCellRef().getRefId() << " in live: " << actorInLive << std::endl;
		//std::cout << dest.getCellRef().getRefId() << " in live: " << destInLive << std::endl; //GAHH, WE ARE SEARCHING BY NAME HERE NOT ID :/ EDIT: fixed, do same for actor?


		//We are here because NPC needs to traverse while outside of cell, so we will use the travelNode system
		//Build a path through the nodes

		//lookup what node is associated with NPCs current cell.
		int currentNode = mCellToNodeMap[npc.getCell()]->id;
		int destNode = mCellToNodeMap[dest.getCell()]->id;


		auto path = mtravelPathGridGraph.aStarSearch(currentNode, destNode); //WANT CURRENT NODE END NODE.

																			 //collect the ids of which nodes we will use
		std::vector<int> travelNodeList;
		for (std::list<ESM::Pathgrid::Point>::iterator it = path.begin(); it != path.end(); it++)
		{
			travelNodeList.push_back(it->mUnknown);
		}

		//Make a journey.
		MWBase::AIScheduleManager::Journey *j = new MWBase::AIScheduleManager::Journey(npc.getCellRef().getRefId(), travelNodeList, dest, tstamp);

		mActiveJourneys.push_back(j); //Do I want to do this through a method?


		return true;
	}*/

}

