#include "demoquestmetabrain.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"


MWBase::DemoQuestMetaBrain::DemoQuestMetaBrain()
{
	
	
	
	MWWorld::Ptr arxptr = MWBase::Environment::get().getWorld()->searchPtr("arx", false);
	MWWorld::Ptr nadiaptr = MWBase::Environment::get().getWorld()->searchPtr("nadia", false);
	mArxLife = MWBase::Environment::get().getLifeManager()->getLifeFromID("arx");
	mNadiaLife = MWBase::Environment::get().getLifeManager()->getLifeFromID("nadia");

	if (mArxLife && mNadiaLife)
	{
		std::cout << "=====Found Arx and Nadia lifes======" << std::endl;
	}
	else
	{
		std::cout << "==============Failed to find arx and nadia lifes===============" << std::endl;
	}

	SubBrainDemoQuest * arxsb = new SubBrainDemoQuest(mArxLife);
	arxsb->setMetaBrain(this);
	SubBrainDemoQuest * nadiasb = new SubBrainDemoQuest(mNadiaLife);
	nadiasb->setMetaBrain(this);
	

	//Give Arx and Nadia subbrains which ping me for stuff.
	
}

void MWBase::DemoQuestMetaBrain::update(float duration)
{
}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::DemoQuestMetaBrain::requestDesire(MWBase::Life * life)
{
	return std::vector<std::shared_ptr<MWBase::GOAPNodeData>>();

}

MWBase::SubBrainDemoQuest::SubBrainDemoQuest(MWBase::Life * life)
{
}

void MWBase::SubBrainDemoQuest::calculate(MWBase::Awareness * awareness)
{
	mMetaBrain->requestDesire(mOwnerLife);
}

std::string MWBase::SubBrainDemoQuest::getID()
{
	return std::string();
}

void MWBase::SubBrainDemoQuest::getDebugInfo()
{
}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::SubBrainDemoQuest::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	return std::vector<std::shared_ptr<GOAPNodeData>>();
}
