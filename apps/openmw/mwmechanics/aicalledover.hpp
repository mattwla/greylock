#ifndef GAME_MWMECHANICS_AICALLEDOVER_H
#define GAME_MWMECHANICS_AICALLEDOVER_H

#include "aipackage.hpp"

#include <string>

#include <components/esm/defs.hpp>

#include "pathfinding.hpp"

namespace ESM
{
	namespace AiSequence
	{
		struct AiCalledOver;
	}
}

namespace MWMechanics
{
	/// \brief AiPackage for an actor to follow another actor/the PC
	/** The AI will follow the target until a condition (time, or position) are set. Both can be disabled to cause the actor to follow the other indefinitely
	**/
	class AiCalledOver : public AiPackage
	{
	public:
		/// Follow Actor for duration or until you arrive at a world position
		AiCalledOver(const std::string &ActorId, float duration, float X, float Y, float Z);
		/// Follow Actor for duration or until you arrive at a position in a cell
		AiCalledOver(const std::string &ActorId, const std::string &CellId, float duration, float X, float Y, float Z);
		/// Follow Actor indefinitively
		AiCalledOver(const std::string &ActorId, bool commanded = false);

		AiCalledOver(const ESM::AiSequence::AiCalledOver *follow);

		MWWorld::Ptr getTarget() const;
		virtual bool sideWithTarget() const { return true; }
		virtual bool followTargetThroughDoors() const { return true; }
		virtual bool shouldCancelPreviousAi() const { return !mCommanded; }

		virtual AiCalledOver *clone() const;

		virtual bool execute(const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration);

		virtual int getTypeId() const;

		/// Returns the actor being followed
		std::string getFollowedActor();

		virtual void writeState(ESM::AiSequence::AiSequence& sequence) const;

		bool isCommanded() const;

		int getFollowIndex() const;

		void fastForward(const MWWorld::Ptr& actor, AiState& state);

	private:
		/// This will make the actor always follow.
		/** Thus ignoring mDuration and mX,mY,mZ (used for summoned creatures). **/
		bool mAlwaysFollow;
		bool mCommanded;
		float mDuration; // Hours
		float mRemainingDuration; // Hours
		float mX;
		float mY;
		float mZ;
		std::string mActorRefId;
		mutable int mActorId;
		std::string mCellId;
		bool mActive; // have we spotted the target?
		int mFollowIndex;

		static int mFollowIndexCounter;
	};
}
#endif
