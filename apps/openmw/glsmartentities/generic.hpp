#include "../mwbase/smartentitiesmanager.hpp"

//A generic SEI for things that might not do anything, but need gravity acted upon them or need basic properties like flammable

class SmartEntityGenericTemplate : public MWBase::SmartEntityTemplate {


public:
	SmartEntityGenericTemplate::SmartEntityGenericTemplate();

	virtual ~SmartEntityGenericTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

	//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, int pings);

	//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, int refnum, int pings);

};



class SmartEntityGenericInstance : public MWBase::SmartEntityInstance {


public:


	SmartEntityGenericInstance::SmartEntityGenericInstance(const MWWorld::Ptr &ptr);

	SmartEntityGenericInstance::SmartEntityGenericInstance(std::string refid, ESM::RefNum refnum, int pings);

	virtual ~SmartEntityGenericInstance() {

	};

	virtual std::string getSaveString();

	//virtual bool isAvailableForUse();

	//virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);


};

