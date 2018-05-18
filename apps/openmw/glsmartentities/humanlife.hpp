#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityHumanTemplate : public MWBase::SmartEntityTemplate {


public:
	SmartEntityHumanTemplate::SmartEntityHumanTemplate();

	virtual ~SmartEntityHumanTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);


};

class SmartEntityHumanInstance : public MWBase::SmartEntityInstance {


public:

	virtual float getActivationDistance()
	{
		return 1600.0f;
	}

	SmartEntityHumanInstance::SmartEntityHumanInstance(const MWWorld::Ptr &ptr);

	SmartEntityHumanInstance::SmartEntityHumanInstance(std::string refid, ESM::RefNum refnum, int pings);

	virtual ~SmartEntityHumanInstance() {

	};

	virtual std::string getSaveString();

};
