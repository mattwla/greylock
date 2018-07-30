#ifndef GAME_MWWORLD_CELLGEN_H
#define GAME_MWWORLD_CELLGEN_H

#include <stdint.h>
#include <string>
#include <map>
#include <vector>

namespace osg
{
	class Vec3f;
	class Vec2f;
}


namespace MWWorld
{

	static const float CELL_WIDTH = (8192 / 69.99) / 4.f;

	class WorldGen
	{

		

		struct GreylockLand {

			



			float mCenterY;

			float mCenterX;

			std::map<float, std::map<float, float>> mGISMap;

			osg::Vec2f terrainIndexToXYZ(int cellx, int celly, int index);

			float getHeightAtIndex(int cellx, int celly, int index);

			bool isLandCached(int x, int y);

			void buildLand();

			//std::vector<float> getfloats(int x, int y, int numperside);

			std::map<int, std::map<int, std::vector<float>>> mCellHeightsMap;

			void saveCellHeights();

			bool loadCellHeights();

			const std::string TERRAIN_PATH = "terrain/testrange";

			std::vector<float> getCellHeights(int x, int y);

			std::vector<float> buildCellHeights(int x, int y);



		};

		//static GreylockLand sGreylockLand;

		GreylockLand * mLand = 0;

	public:


		bool startNewGame();

		std::vector<float> getCellHeights(int x, int y);


	


	};

}




#endif // GAME_MWWORLD_CELLGEN_H
