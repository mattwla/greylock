#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityBreadTemplate : public MWBase::SmartEntityTemplate {


	public:
		SmartEntityBreadTemplate::SmartEntityBreadTemplate();

		virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

		virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, int refnum, int pings);

};

class SmartEntityBreadInstance : public MWBase::SmartEntityInstance {


	public:

		SmartEntityBreadInstance::SmartEntityBreadInstance(const MWWorld::Ptr &ptr);

		SmartEntityBreadInstance::SmartEntityBreadInstance(std::string refid, int refnum, int pings);
};