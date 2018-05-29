#include "metabrain.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../subbrains/subbrain.hpp"


namespace MWBase {
		

	class DemoQuestMetaBrain : public MWBase::MetaBrain
	{
		MWBase::Life * mArxLife = 0;
		MWBase::Life * mNadiaLife = 0;
		MWBase::BehaviorObject * mArxBO = 0;



	public:
		DemoQuestMetaBrain();

		virtual void update(float duration);

		virtual std::vector<std::shared_ptr<GOAPNodeData>> requestDesire(MWBase::Life * life);

		std::vector<std::shared_ptr<MWBase::GOAPNodeData>> getMatchingBO(MWBase::GOAPStatus status, MWBase::Life * life);
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


	class ArxDemoQuestBO : public MWBase::BehaviorObject
	{
	public:

		ArxDemoQuestBO();

		ArxDemoQuestBO* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			ArxDemoQuestBO * newbo = new ArxDemoQuestBO(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			return newbo;
		}


		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual void getDebugInfo();


		virtual BOReturn start();

		virtual bool stop()
		{
			return true;
		};


		virtual ~ArxDemoQuestBO()
		{

		};


	};

}
