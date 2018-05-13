#include "subbrainrelax.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include <memory>



MWBase::SubBrainRelax::SubBrainRelax(MWBase::Life * life)
{
	mOwnerLife = life;
	GOAPStatus desirestatus(MWBase::GOAPStatus::VITALS, "stress", -1);
	std::shared_ptr<GOAPDesire> pDesire = std::make_shared<GOAPDesire>(desirestatus, 100);
	pDesire->debugInfo = "relax desire";
	mOwnerLife->submitDesirePtr(pDesire);


}

void MWBase::SubBrainRelax::calculate(MWBase::Awareness * awareness)
{

	mWorldState.clear();
}

std::string MWBase::SubBrainRelax::getID()
{
	return "sbRelax";
}

void MWBase::SubBrainRelax::getDebugInfo()
{

}