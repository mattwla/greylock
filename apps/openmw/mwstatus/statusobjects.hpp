#ifndef GAME_MWMECHANICS_STATUSOBJECTS_H
#define GAME_MWMECHANICS_STATUSOBJECTS_H


#include "../mwbase/statusmanager.hpp"
#include "../mwworld/ptr.hpp"
#include "../mwworld/manualref.hpp"

namespace MWBase
{

	class StatusObject
	{
	protected:

		MWBase::SmartEntityInstance * mSEI;

		bool mDone = false;

	public:

		virtual MWBase::Status getStatusEnum() = 0;

		virtual void update(float duration) = 0;

		virtual void init() = 0;

		virtual void end() = 0;

		virtual bool isDone()
		{
			return mDone;
		}
	};


	class StatusObjectConstructor
	{



	public:

		virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei) = 0;

		virtual MWBase::Status getStatusEnum() = 0;
	};



	//FLOAT STATUS OBJECT


	class FloatStatusObject : public StatusObject
	{

		float mTotalTime = 0.0f;


	public:

		FloatStatusObject(MWBase::SmartEntityInstance * sei)
		{
			mSEI = sei;
		}

		static StatusObjectConstructor * getConstuctor()
		{
			class FloatStatusConstructor : public StatusObjectConstructor
			{
			public:

				virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei)
				{
					
					return new FloatStatusObject(sei);
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

		virtual void update(float duration);

		virtual void init();

		virtual void end();
	};



	//ON FIRE STATUS OBJECT

	class OnFireStatusObject : public StatusObject
	{

		float mTotalTime = 0.0f;
		MWWorld::Ptr fireptr;
		MWBase::SmartEntityInstance * mFireSEI;
		MWWorld::ManualRef * mFireRef;


	public:

		OnFireStatusObject(MWBase::SmartEntityInstance * sei)
		{
			mSEI = sei;
		}

		static StatusObjectConstructor * getConstuctor()
		{
			class OnFireStatusConstructor : public StatusObjectConstructor
			{
			public:

				virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei)
				{

					return new OnFireStatusObject(sei);
				}

				virtual MWBase::Status getStatusEnum()
				{
					return MWBase::OnFire;
				}
			};

			std::cout << "===========================returning float status constructor===================" << std::endl;
			return new OnFireStatusConstructor();
		}

		virtual MWBase::Status getStatusEnum()
		{
			return MWBase::OnFire;
		}

		virtual void update(float duration);

		virtual void init();

		virtual void end();
	};


	//THIEF Status object

	class ThiefStatusObject : public StatusObject
	{

		float mTotalTime = 0.0f;
		MWWorld::Ptr thiefptr;


	public:

		ThiefStatusObject(MWBase::SmartEntityInstance * sei)
		{
			mSEI = sei;
		}

		static StatusObjectConstructor * getConstuctor()
		{
			class ThiefStatusConstructor : public StatusObjectConstructor
			{
			public:

				virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei)
				{

					return new ThiefStatusObject(sei);
				}

				virtual MWBase::Status getStatusEnum()
				{
					return MWBase::Thief;
				}
			};

			std::cout << "===========================returning float status constructor===================" << std::endl;
			return new ThiefStatusConstructor();
		}

		virtual MWBase::Status getStatusEnum()
		{
			return MWBase::Thief;
		}

		virtual void update(float duration);

		virtual void init();

		virtual void end();
	};


	//Assault Status object

	class AssaultStatusObject : public StatusObject
	{

		float mTotalTime = 0.0f;
		MWWorld::Ptr assaulterptr;


	public:

		AssaultStatusObject(MWBase::SmartEntityInstance * sei)
		{
			mSEI = sei;
		}

		static StatusObjectConstructor * getConstuctor()
		{
			class AssaultStatusConstructor : public StatusObjectConstructor
			{
			public:

				virtual StatusObject * getObject(MWBase::SmartEntityInstance * sei)
				{

					return new AssaultStatusObject(sei);
				}

				virtual MWBase::Status getStatusEnum()
				{
					return MWBase::Assaulter;
				}
			};

			std::cout << "===========================returning assault status constructor===================" << std::endl;
			return new AssaultStatusConstructor();
		}

		virtual MWBase::Status getStatusEnum()
		{
			return MWBase::Assaulter;
		}

		virtual void update(float duration);

		virtual void init();

		virtual void end();
	};





}


#endif
