#ifndef GAME_GLMETABRAINS_METABRAIN_H
#define GAME_GLMETABRAINS_METABRAIN_H
#include <components/esm/defs.hpp>
#include <string>
#include <vector>
#include <memory>
#include "../mwworld/ptr.hpp"

namespace MWBase
{
	class MetaBrain
	{

	public:
		virtual void update(float duration) = 0;

	};







	class MetaBrainManager
	{
		typedef std::vector<std::shared_ptr<MetaBrain>> metabrainlist;

		metabrainlist mMetaBrainList;

	public:

		void update(float duration);

		void clear();

		void newGame();

		~MetaBrainManager(){
			//delete meta brain list
		};

	};

}




#endif