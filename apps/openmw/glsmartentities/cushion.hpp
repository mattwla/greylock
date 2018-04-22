#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityCushionTemplate : public MWBase::SmartEntityTemplate {


	public:
		SmartEntityCushionTemplate::SmartEntityCushionTemplate();

		virtual ~SmartEntityCushionTemplate() {

		};

		virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

		virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

		virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

		//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, int pings);

		//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, int refnum, int pings);

};

class SmartEntityCushionInstance : public MWBase::SmartEntityInstance {


	public:

		SmartEntityCushionInstance::SmartEntityCushionInstance(const MWWorld::Ptr &ptr);

		SmartEntityCushionInstance::SmartEntityCushionInstance(std::string refid, ESM::RefNum refnum, int pings);

		virtual ~SmartEntityCushionInstance() {

		};

		virtual std::string getSaveString();

		virtual bool isAvailableForUse();
	
		virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);



	

		
};

class BOCushionSit : public MWBase::BehaviorObject
{


public:

	//BOCushionSit::BOCushionSit();

	BOCushionSit* Clone()
	{
		return new BOCushionSit(*this);
	};


	BOCushionSit::BOCushionSit(int valence);

	//void getDebugInfo();

	virtual void getDebugInfo();

	virtual MWBase::BOReturn update(float time, MWWorld::Ptr ownerptr);

	virtual MWBase::BOReturn start();

	virtual ~BOCushionSit() {

	};

private:

	BOCushionSit(BOCushionSit const &instance)
	{

	};

};