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
#include <components/esm/loadland.hpp>

//TERMS
//terrain index: the OpenMW style of storing terrains height in a cell, 65*65, with an associated world X and Y
//gismap: the xyz stored into a map in memory
//gissource: the .xyz file
//cellsamples. set up and ready to go heightmap, already interpolated for the given resolution.
std::map<int, std::map<int, std::vector<float>>> ESM::Land::sPreparedHeightMap;

osg::Vec2f MWWorld::WorldGen::GreylockLand::terrainIndexToXYZ(int cellx, int celly, int index)
{
	float xcenter = mCenterX;
	float ycenter = mCenterY;
	//float cellwidth = (8192 / 69.99) / 4.f;

	//assumes rectangle
	//how many meters is a cell?
	//117 meters per side
	//aka 128 yards
	//find where on map cellx and celly are

	float xmeteroffset = cellx * CELL_WIDTH + xcenter;
	float ymeteroffset = celly * CELL_WIDTH + ycenter;

	//figure out where in cell to take measurement of height
	float targety = index / ESM::Land::LAND_SIZE;//(LAND_SIZE);
	float targetx = index % ESM::Land::LAND_SIZE;//(LAND_SIZE);
	float multiplier = 65.f / ESM::Land::LAND_SIZE;
	ymeteroffset += (targety * 3.65 / 8.f) * multiplier;
	xmeteroffset += (targetx * 3.65 / 8.f) * multiplier;
	return osg::Vec2f(xmeteroffset, ymeteroffset);
}



float MWWorld::WorldGen::GreylockLand::getHeightAtIndex(int cellx, int celly, int index)
{
	//use cell x, y, and index to figure out where on GIS map we are sampling
	osg::Vec2f xy = terrainIndexToXYZ(cellx, celly, index);
	float xmeteroffset = xy.x();
	float ymeteroffset = xy.y();

	//get closest bounds we can find on terrain map
	auto xBound1 = mGISMap.lower_bound(xmeteroffset);
	if (xBound1 == mGISMap.end())
	{
		xBound1--;
	}
	auto yBound1 = xBound1->second.lower_bound(ymeteroffset);
	if (yBound1 == xBound1->second.end())
	{
		yBound1--;
	}

	float x1 = xBound1->first;
	float y1 = yBound1->first;
	float z1 = yBound1->second;
	xBound1++;
	if (xBound1 == mGISMap.end())
		return 0;
	float x2 = xBound1->first;
	auto yBound2 = xBound1->second.lower_bound(ymeteroffset);
	if (yBound2 == xBound1->second.end())
		return 0;
	yBound2++;

	float y2 = yBound2->first;
	float z2 = yBound2->second;

	osg::Vec3f v0(x1, y1, mGISMap[x1][y1]);
	osg::Vec3f v1(x2, y1, mGISMap[x2][y1]);
	osg::Vec3f v2(x2, y2, mGISMap[x2][y2]);
	osg::Vec3f v3(x1, y2, mGISMap[x1][y2]);


	

	float nY = ymeteroffset;//targety / LAND_SIZE;
	float nX = xmeteroffset;//targetx / LAND_SIZE;

	float factor = ESM::Land::LAND_SIZE;//ESM::Land::LAND_SIZE - 1.0f;
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
		- plane[3]) / plane.getNormal().z();// *160 * 2;// *4;
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

	//ESM::Land;
	int count = ESM::Land::LAND_SIZE * ESM::Land::LAND_SIZE;
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
	if (mGISMap.size() > 0)
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
			mGISMap[x][y] = z;
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
		mCenterX = (mGISMap.begin()->first + mGISMap.rbegin()->first) / 2.0;
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
	z = z * 360;
	filestream << x << std::endl << y << std::endl << z << std::endl;
	mGISMap[x][y] = z;
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
mCenterX = (mGISMap.begin()->first + mGISMap.rbegin()->first) / 2.0;
}



bool MWWorld::WorldGen::startNewGame()
{
	/*if (mLand)
		delete mLand;*/

	const int yrange = 30;
	const int xrange = 30;


	mLand = new GreylockLand;
	bool loaded = mLand->loadPreparedHeights();
	
	if (!loaded)
	{
		mLand->buildLand();


		int cellsloaded = 0;

		int xload = -xrange;
		int yload = -yrange;
		while (xload <= xrange)
		{
			while (yload <= yrange)
			{
				ESM::Position pos;
				auto heights = mLand->buildCellHeights(xload, yload);
				mLand->mCellHeightsMap[xload][yload + 1] = heights;
				ESM::Land::sPreparedHeightMap[xload][yload + 1] = heights;
				MWBase::Environment::get().getWorld()->indexToPosition(xload, yload, pos.pos[0], pos.pos[1], true);
				MWBase::Environment::get().getSmartEntitiesManager()->initializeActiveCell();
				yload += 1;
				cellsloaded += 1;
			}
			yload = -yrange;
			xload += 1;
		}


		mLand->savePreparedHeights();



	}
	//save prepared heights
	return true;

}

bool MWWorld::WorldGen::GreylockLand::savePreparedHeights()
{

	std::cout << "writing prepared heights..." << std::endl;


	boost::filesystem::ofstream filestream(TERRAIN_PATH + ".psh", std::ios::binary);
	typedef std::map<int, std::map<int, std::vector<float>>> landmap;
	ESM::Land::sPreparedHeightMap;
	for (landmap::iterator itx = ESM::Land::sPreparedHeightMap.begin(); itx != ESM::Land::sPreparedHeightMap.end(); itx++)
	{
		for (std::map<int, std::vector<float>>::iterator ity = (*itx).second.begin(); ity != (*itx).second.end(); ity++)
		{
			filestream << itx->first << std::endl << ity->first << std::endl;
			for (std::vector<float>::iterator itz = ity->second.begin(); itz != ity->second.end(); itz++)
			{
				filestream << (*itz) << std::endl;
			}
		}
	}


	filestream.close();
	return true;

	//filestream << x << std::endl << y << std::endl << z << std::endl;

}

bool MWWorld::WorldGen::GreylockLand::loadPreparedHeights()
{
	std::ifstream inc(TERRAIN_PATH + ".psh");

	if (inc.is_open())
	{
		std::cout << "loading prepared land heights....." << std::endl;
		while (!inc.eof())
		{
			
			
			float x;
			float y;
			inc >> x >> y;
			std::vector<float> zlist;
			int itx = 0;
			while (itx < ESM::Land::LAND_SIZE*ESM::Land::LAND_SIZE)
			{
				float z;
				inc >> z;
				zlist.push_back(z);
				itx += 1;
			}
		
			ESM::Land::sPreparedHeightMap[x][y] = zlist;

		
		}
	}
	else
	{
		return false;
	}

	


	return true;


}

std::vector<float> MWWorld::WorldGen::getCellHeights(int x, int y)
{
	
	return mLand->getCellHeights(x, y);

}
