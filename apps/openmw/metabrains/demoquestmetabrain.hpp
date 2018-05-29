#include "metabrain.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../subbrains/subbrain.hpp"


namespace MWBase {
		

	class DemoQuestMetaBrain : public MWBase::MetaBrain
	{
		MWBase::Life * mArxLife = 0;
		MWBase::Life * mNadiaLife = 0;



	public:
		DemoQuestMetaBrain();

		virtual void update(float duration);
	};

	class SubBrainDemoQuest : public MWBase::SubBrain
	{

	public:
		SubBrainDemoQuest::SubBrainDemoQuest(MWBase::Life * life);

		virtual ~SubBrainDemoQuest() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus status);

	};
}
