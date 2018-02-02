#include "awarenessreactionsmanagerimp.hpp"


#include <limits.h>
#include <iostream>
#include <fstream>

#include <components/misc/rng.hpp>

#include <components/esm/esmwriter.hpp>
#include <components/esm/stolenitems.hpp>

#include <components/sceneutil/positionattitudetransform.hpp>

#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"


#include "../mwworld/player.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/actionteleport.hpp"
#include "../mwworld/projectilemanager.hpp"
#include "../mwworld/weather.hpp"
#include "../mwworld/scene.hpp"
#include "../mwworld//worldimp.cpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

#include <osg/Group>
#include <osg/ComputeBoundsVisitor>


namespace MWAwarenessReactions
{
	AwarenessReactionsManager::AwarenessReactionsManager()
	{

	}

	void AwarenessReactionsManager::updateActiveAffordances()
	{
		MWWorld::Ptr npc;
		std::vector<MWWorld::Ptr> out;
		std::string lowerCaseName = "affordance";
		auto mActiveCells = MWBase::Environment::get().getWorld()->getActiveCells();
		//auto mCells = MWBase::Environment::get().getWorld()->getCells();
		//for (MWWorld::Scene::CellStoreCollection::const_iterator iter(mActiveCells.begin());
		//	iter != mActiveCells.end(); ++iter)
		//{
		//	// TODO: caching still doesn't work efficiently here (only works for the one CellStore that the reference is in)
		//	MWWorld::CellStore* cellstore = *iter;
		//	MWWorld::Ptr ptr = mCells.getPtr(lowerCaseName, *cellstore, false);

		//	if (!ptr.isEmpty())
		//		std::cout << "hi" << std::endl;
		//	//return ptr;
		//}

		const MWWorld::Scene::CellStoreCollection& collection = mActiveCells;
		for (MWWorld::Scene::CellStoreCollection::const_iterator cellIt = collection.begin(); cellIt != collection.end(); ++cellIt)
		{
			MWWorld::ListObjectsVisitor visitor;
			(*cellIt)->forEach(visitor);

			for (std::vector<MWWorld::Ptr>::iterator it = visitor.mObjects.begin(); it != visitor.mObjects.end(); ++it)
				//if (Misc::StringUtils::ciEqual(it->getCellRef().getOwner(), npc.getCellRef().getRefId()))
			{
				if(it->getClass().getTypeName() == typeid(ESM::NPC).name())
				{
					out.push_back(*it);
					std::cout << "found npc" << it->getCellRef().getRefId() << std::endl;
				}
			}
		}
	}
}