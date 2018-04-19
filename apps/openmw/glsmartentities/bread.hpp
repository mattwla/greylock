#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityBreadTemplate : public MWBase::SmartEntityTemplate {


	public:
		SmartEntityBreadTemplate::SmartEntityBreadTemplate();

		virtual ~SmartEntityBreadTemplate() {

		};

		virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

		virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

		virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

		//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, int pings);

		//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, int refnum, int pings);

};

class SmartEntityBreadInstance : public MWBase::SmartEntityInstance {


	public:

		SmartEntityBreadInstance::SmartEntityBreadInstance(const MWWorld::Ptr &ptr);

		SmartEntityBreadInstance::SmartEntityBreadInstance(std::string refid, ESM::RefNum refnum, int pings);

		virtual ~SmartEntityBreadInstance() {

		};

		//SmartEntityBreadInstance::SmartEntityBreadInstance(std::string refid, int refnum, int pings);

		virtual std::string getSaveString();

		virtual bool use(MWBase::Life * user);
};