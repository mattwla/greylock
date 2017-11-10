#include "travelnodesmanagerimp.hpp"

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

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>


namespace MWTravelNodes
{
	TravelNodesManager::TravelNodesManager()
	{
		mtravelNodeMap = buildTravelNodes();
		buildPathGrid(&mtravelPathGrid);
		mtravelPathGridGraph = MWMechanics::PathgridGraph(&mtravelPathGrid);
		mtravelPathGridGraph.load();

	}

	TravelNodesManager::~TravelNodesManager()
	{

	}

	std::map<int, MWBase::TravelNodesManager::TravelNode*> TravelNodesManager::buildTravelNodes()
	{
		std::map<int, TravelNode*> nodeMap;

		std::string nodelist = ("schedules/travelnodes.csv");
		std::ifstream in(nodelist.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << nodelist << std::endl;

		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

		std::vector<std::vector<std::string>> vecvec;

		std::string line;

		while (getline(in, line))
		{
			std::vector<std::string> vec;
			Tokenizer tok(line);
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)
			{
				vec.push_back(*it);
			}
			vecvec.push_back(vec);
		}

		for (unsigned int i = 0; i < vecvec.size(); i++)
		{
			MWBase::TravelNodesManager::TravelNode *tn = new MWBase::TravelNodesManager::TravelNode;
			tn->id = i;
			tn->marker = vecvec[i][0];
			MWWorld::Ptr markerPtr = MWBase::Environment::get().getWorld()->searchPtr(vecvec[i][0], false);
			ESM::Position markerPos = markerPtr.getRefData().getPosition();
			ESM::Pathgrid::Point *point = new ESM::Pathgrid::Point;
			point->mUnknown = i; //mUnknown seems unused, being used here to store the idx of point so we can look it up in nodeMap
			point->mX = markerPos.pos[0];
			point->mY = markerPos.pos[1];
			point->mZ = markerPos.pos[2];
			tn->point = *point;
			vecvec[i];
			nodeMap[i] = tn;
			//id,index,num of connections, connected to

			MWWorld::CellStore* store = markerPtr.getCell(); //Where will this ptr go? Probably away so figure out how to make this crash.
			mCellToNodeMap[store] = tn;


		}

		return nodeMap;
	}

	void TravelNodesManager::buildPathGrid(ESM::Pathgrid * grid)
	{
		for (unsigned int i = 0; i < mtravelNodeMap.size(); i++)
		{
			grid->mPoints.push_back(mtravelNodeMap[i]->point);
		}

		std::string nodelist = ("schedules/edges.csv");
		std::ifstream in(nodelist.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << nodelist << std::endl;

		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

		std::vector<std::vector<std::string>> vecvec;

		std::string line;

		while (getline(in, line))
		{
			std::vector<std::string> vec;
			Tokenizer tok(line);
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)
			{
				vec.push_back(*it);
			}
			vecvec.push_back(vec);
		}


		for (unsigned int i = 0; i < vecvec.size(); i++)
		{
			ESM::Pathgrid::Edge e;
			e.mV0 = std::stoi(vecvec[i][0]);
			e.mV1 = std::stoi(vecvec[i][1]);
			grid->mEdges.push_back(e);
		}


	}


}


