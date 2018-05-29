#include "metabrain.hpp"
#include "demoquestmetabrain.hpp"

void MWBase::MetaBrainManager::update(float duration)
{
}

void MWBase::MetaBrainManager::clear()
{
}

void MWBase::MetaBrainManager::newGame()
{
	std::cout << "======Init MetaBrain Manager=======" << std::endl;
	new DemoQuestMetaBrain;
}
