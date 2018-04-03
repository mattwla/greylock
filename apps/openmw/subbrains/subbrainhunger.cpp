#include "subbrainhunger.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/lifemanager.hpp"



MWBase::SubBrainHunger::SubBrainHunger(MWBase::Life * life)
{
	mOwnerLife = life;
}

void MWBase::SubBrainHunger::calculate(MWBase::Awareness * awareness)
{
	std::cout << "calculating hunger" << std::endl;
}
