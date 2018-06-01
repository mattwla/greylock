#ifndef GAME_MWMECHANICS_STATUSOBJECTS_H
#define GAME_MWMECHANICS_STATUSOBJECTS_H


#include "../mwbase/statusmanager.hpp"
#include "../mwworld/ptr.hpp"

namespace MWBase
{

	class StatusObject
	{

	protected:




	public:

		virtual MWBase::Status getStatusEnum() = 0;


	};


	class StatusObjectConstructor
	{

	public:

		virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei) = 0;

		virtual MWBase::Status getStatusEnum() = 0;
	};



	class FloatStatusObject : public StatusObject
	{


	public:

		static StatusObjectConstructor * getConstuctor()
		{
			class FloatStatusConstructor : public StatusObjectConstructor
			{

			public:

				virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei)
				{
					
					return new FloatStatusObject();
				}

				virtual MWBase::Status getStatusEnum()
				{
					return MWBase::FloatShroomPowdered;
				}
			};

			

			std::cout << "===========================returning float status constructor===================" << std::endl;
			return new FloatStatusConstructor();
		}

		virtual MWBase::Status getStatusEnum()
		{
			return MWBase::FloatShroomPowdered;
		}


	};



}


#endif
