#include "subbrainrelax.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"



MWBase::SubBrainRelax::SubBrainRelax(MWBase::Life * life)
{
	mOwnerLife = life;
}

void MWBase::SubBrainRelax::calculate(MWBase::Awareness * awareness)
{
//	mGOAPDesires.clear();
	mWorldState.clear();

	GOAPStatus desirestatus(MWBase::GOAPStatus::VITALS, "stress", -1);
	GOAPDesire desire(desirestatus, 1);
	desire.debugInfo = "relax desire";
	//mGOAPDesires.push_back(desire);
	
}

std::string MWBase::SubBrainRelax::getID()
{
	return "sbRelax";
}

void MWBase::SubBrainRelax::getDebugInfo()
{

}