#include "worldgen.hpp"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include <osg/Plane>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include "refdata.hpp"
#include "ptr.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/smartentitiesmanager.hpp"




float MWWorld::WorldGen::GreylockLand::getHeightAtIndex(int cellx, int celly, int index)
{
	//first find center of terrain.
	float xcenter = mCenterX;
	float ycenter = mCenterY;
	float cellwidth = (8192 / 69.99) / 4.f;

	//assumes rectangle
	//how many meters is a cell?
	//117 meters per side
	//aka 128 yards
	//find where on map cellx and celly are


	float xmeteroffset = cellx * cellwidth + xcenter;
	float ymeteroffset = celly * cellwidth + ycenter;

	//figure out where in cell to take measurement of height
	float targety = index / 65;//(LAND_SIZE);
	float targetx = index % 65;//(LAND_SIZE);
	ymeteroffset += targety * 3.65 / 8.f;
	xmeteroffset += targetx * 3.65 / 8.f;


	//get closest bounds we can find on terrain map
	auto t1 = mLandHeights.lower_bound(xmeteroffset);
	if (t1 == mLandHeights.end())
	{
		t1--;
	}
	auto t2 = t1->second.lower_bound(ymeteroffset);
	if (t2 == t1->second.end())
	{
		t2--;
	}

	auto bound1 = mLandHeights.lower_bound(xmeteroffset);
	if (bound1 == mLandHeights.end())
		bound1--;
	auto bound1b = bound1->second.lower_bound(ymeteroffset);
	if (bound1b == bound1->second.end())
		bound1b--;

	float x1 = bound1->first;
	float y1 = bound1b->first;
	float z1 = bound1b->second;

	auto bound2 = bound1++;

	if (bound1 == mLandHeights.end())
		return 0;

	float x2 = bound1->first;

	bound1b = bound1->second.lower_bound(ymeteroffset);
	if (bound1b == bound1->second.end())
		return 0;
	auto bound2b = bound1b++;
	float y2 = bound1b->first;
	float z2 = bound1b->second;

	osg::Vec3f v0(x1, y1, mLandHeights[x1][y1]);
	osg::Vec3f v1(x2, y1, mLandHeights[x2][y1]);
	osg::Vec3f v2(x2, y2, mLandHeights[x2][y2]);
	osg::Vec3f v3(x1, y2, mLandHeights[x1][y2]);

	//get normalized position in cell

	float nY = ymeteroffset;//targety / LAND_SIZE;
	float nX = xmeteroffset;//targetx / LAND_SIZE;

	float factor = 65.0;//ESM::Land::LAND_SIZE - 1.0f;
	float invFactor = 1.0f / factor;

	float xParam = (nX - x1); //* factor;
	float yParam = (nY - y1); //* factor;

	osg::Plane plane;

	bool secondTri = ((yParam) > xParam);
	if (secondTri)
		plane = osg::Plane(v0, v1, v3);
	else
		plane = osg::Plane(v1, v2, v3);

	// Solve plane equation for z
	float z = (-plane.getNormal().x() * nX
		- plane.getNormal().y() * nY
		- plane[3]) / plane.getNormal().z() * 160 * 4;
	return z;
}


bool MWWorld::WorldGen::GreylockLand::isLandCached(int x, int y)
{
	if (mCellHeightsMap.count(x) == 0)
		return false;
	if (mCellHeightsMap[x].count(y) == 0)
		return false;
	return true;
}

void MWWorld::WorldGen::GreylockLand::saveCellHeights()
{
	//FORMAT:
	//x y numverts vertdata
	//Check to see if we already have a save
	std::ifstream save(TERRAIN_PATH + ".vhgt");
	if (save.is_open())
		return;

	boost::filesystem::ofstream filestream(TERRAIN_PATH + ".vhgt", std::ios::binary);

	for (std::map<int, std::map<int, std::vector<float>>>::iterator itx = mCellHeightsMap.begin(); itx != mCellHeightsMap.end(); itx++)
	{
		for (std::map<int, std::vector<float>>::iterator ity = (*itx).second.begin(); ity != (*itx).second.end(); ity++)
		{
			float x = itx->first;
			float y = ity->first;
			float count = ity->second.size();

			filestream << x << std::endl << y << std::endl << count << std::endl;

			for (std::vector<float>::iterator itf = ity->second.begin(); itf != ity->second.end(); itf++)
			{
				float z = *itf;
				filestream << z << std::endl;
			}
		}
	}
	filestream.close();
}

bool MWWorld::WorldGen::GreylockLand::loadCellHeights()
{
	std::cout << "loading cell heights" << std::endl;
	std::ifstream save(TERRAIN_PATH + ".vhgt");
	if (!save.is_open())
		return false;
	std::cout << "OPEN!" << std::endl;
	while (!save.eof())
	{
		float x;
		float y;
		float count;
		std::vector<float> heights;
		save >> x >> y >> count;
		int itx = 0;
		while (itx < count)
		{
			float z;
			save >> z;
			heights.push_back(z);
			itx += 1;
		}
		mCellHeightsMap[x][y] = heights;
	}
	save.close();
	return true;
}

std::vector<float> MWWorld::WorldGen::GreylockLand::getCellHeights(int x, int y)
{

	bool cached = isLandCached(x, y);
	if (cached)
	{
		return mCellHeightsMap[x][y];
	}
	{
		std::cout << "ERROR: COULD NOT GET CELL HEIGHTS" << std::endl;
		return std::vector<float>();
	}
}

std::vector<float> MWWorld::WorldGen::GreylockLand::buildCellHeights(int x, int y)
{
	int count = 65 * 65;
	int itx = 0;
	std::vector<float> result;
	while (itx < count)
	{
		result.push_back(getHeightAtIndex(x, y, itx));
		itx+=1;
	}
	return result;
}




void MWWorld::WorldGen::GreylockLand::buildLand()
{
	if (mLandHeights.size() > 0)
		return;
	//check if cached version available:
	std::ifstream inc(TERRAIN_PATH + ".gll");
	bool firstiter = true;
	float miny = 0;
	float biggesty = 0;
	if (inc.is_open())
	{
		std::cout << "loading land....." << std::endl;
		while (!inc.eof())
		{
			float x;
			float y;
			float z;
			inc >> x >> y >> z;
			mLandHeights[x][y] = z;
			if (y < miny)
				miny = y;
			if (y > biggesty)
				biggesty = y;
			if (firstiter)
			{
				miny = y;
				biggesty = y;
				firstiter = false;
			}
		}
		mCenterY = (biggesty + miny) / 2.0;
		mCenterX = (mLandHeights.begin()->first + mLandHeights.rbegin()->first) / 2.0;
		return;
	}

	std::ifstream in(TERRAIN_PATH + ".xyz");
	if (!in.is_open())
		std::cout << "-----=====TERRAIN NOT OPEN====-----" << std::endl;
	else
		std::cout << "----=====TERRAIN OPEN======------" << std::endl;
	std::string line;
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
	boost::char_separator<char> sep("\t");

	int count = 0;
	float prevy = 0;
	float currenty = 0;
	std::map<float, std::map<float, float>> grid;
	std::map<float, float> currentxrow;
	boost::filesystem::ofstream filestream(TERRAIN_PATH + ".gll", std::ios::binary);
	while (getline(in, line))
	{
		Tokenizer tok(line, sep);
		int itx = 0;
		float x;
		float y;
		float z;
		for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it) //iterate through the line, values seperated by commas
		{
			if (itx == 0)
			{
				x = std::stof(*it);
			}
			else if (itx == 1)
			{
				y = std::stof(*it);
			}
			else
			{
				z = std::stof(*it);
			}
			itx += 1;
		}
		currenty = y;
		filestream << x << std::endl << y << std::endl << z << std::endl;
		mLandHeights[x][y] = z;
		if (y < miny)
			miny = y;
		if (y > biggesty)
			biggesty = y;
		if (firstiter)
		{
			miny = y;
			biggesty = y;
			firstiter = false;
		}
	}
	mCenterY = (biggesty + miny) / 2.0;
	mCenterX = (mLandHeights.begin()->first + mLandHeights.rbegin()->first) / 2.0;
}



bool MWWorld::WorldGen::startNewGame()
{
	/*if (mLand)
		delete mLand;*/
	
	mLand = new GreylockLand;

	mLand->buildLand();


	int cellsloaded = 0;
	/*const MWWorld::Store<ESM::Cell> &cells = mCells.getExteriorStore();
	MWWorld::Store<ESM::Cell>::iterator iter;*/
	int xload = -22;
	int yload = -22;
	while (xload <= 22)
	{
		while (yload <= 22)
		{
			ESM::Position pos;
			
		
			
			auto heights = mLand->buildCellHeights(xload, yload);
			mLand->mCellHeightsMap[xload][yload] = heights;
			//mLand->saveCellHeights();

			MWBase::Environment::get().getWorld()->indexToPosition(xload, yload, pos.pos[0], pos.pos[1], true);
			MWBase::Environment::get().getWorld()->changeToExteriorCell(pos, false);

			//saveCellTerrain

			//SEManager inits smartzones here.
			
			
			MWBase::Environment::get().getSmartEntitiesManager()->initializeActiveCell();


			yload += 1;
			cellsloaded += 1;
		}

		yload = -22;
		xload += 1;


	}








	return true;

}

std::vector<float> MWWorld::WorldGen::getCellHeights(int x, int y)
{
	
	return mLand->getCellHeights(x, y);

}
