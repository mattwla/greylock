#include "tasksmanager.hpp"
#include "lifemanager.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwworld/class.hpp"

//bool MWBase::JourneyManager::requestNewJourney()
//{
//	return false;
//}

bool MWBase::JourneyManager::requestNewJourney(ESM::Position pos)
{
	MWWorld::Ptr ownerPtr = mOwnerLife->mPtr;

	
	MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
	
		seq.stack(MWMechanics::AiTravel(pos.pos[0], pos.pos[1], pos.pos[2]), ownerPtr);
		std::cout << "attempting travel" << std::endl;
	
	
	return true;
}

void MWBase::JourneyManager::cancelJourney(int ticket)
{
	MWWorld::Ptr ownerPtr = mOwnerLife->mPtr;
	MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
	seq.clear();
}

MWBase::JourneyStatus MWBase::JourneyManager::updateJourney(int ticket)
{
	MWWorld::Ptr ownerPtr = mOwnerLife->mPtr;
	MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();

	if (seq.getTypeId() == -1 || seq.getTypeId() == 0)
	{
		return JOURNEY_COMPLETED;
	}
	else
		return JOURNEY_IN_PROGRESS;

	//return JourneyStatus();
}
