#include "loadland.hpp"

#include <utility>

#include "esmreader.hpp"
#include "esmwriter.hpp"
#include "defs.hpp"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <osg/Plane>

std::map<float, std::map<float, float>> ESM::Land::GreylockLand::sLandHeights;

float ESM::Land::GreylockLand::sCenterY;

namespace ESM
{
    unsigned int Land::sRecordId = REC_LAND;

    Land::Land()
        : mFlags(0)
        , mX(0)
        , mY(0)
        , mPlugin(0)
        , mDataTypes(0)
        , mLandData(NULL)
    {
    }

    void transposeTextureData(const uint16_t *in, uint16_t *out)
    {
        int readPos = 0; //bit ugly, but it works
        for ( int y1 = 0; y1 < 4; y1++ )
            for ( int x1 = 0; x1 < 4; x1++ )
                for ( int y2 = 0; y2 < 4; y2++)
                    for ( int x2 = 0; x2 < 4; x2++ )
                        out[(y1*4+y2)*16+(x1*4+x2)] = in[readPos++];
    }

    Land::~Land()
    {
        delete mLandData;
    }

    void Land::load(ESMReader &esm, bool &isDeleted)
    {
        isDeleted = false;

        mPlugin = esm.getIndex();

        bool hasLocation = false;
        bool isLoaded = false;
        while (!isLoaded && esm.hasMoreSubs())
        {
            esm.getSubName();
            switch (esm.retSubName().intval)
            {
                case ESM::FourCC<'I','N','T','V'>::value:
                    esm.getSubHeaderIs(8);
                    esm.getT<int>(mX);
                    esm.getT<int>(mY);
                    hasLocation = true;
                    break;
                case ESM::FourCC<'D','A','T','A'>::value:
                    esm.getHT(mFlags);
                    break;
                case ESM::SREC_DELE:
                    esm.skipHSub();
                    isDeleted = true;
                    break;
                default:
                    esm.cacheSubName();
                    isLoaded = true;
                    break;
            }
        }

        if (!hasLocation)
            esm.fail("Missing INTV subrecord");

        mContext = esm.getContext();

        mLandData = NULL;

        // Skip the land data here. Load it when the cell is loaded.
        while (esm.hasMoreSubs())
        {
            esm.getSubName();
            switch (esm.retSubName().intval)
            {
                case ESM::FourCC<'V','N','M','L'>::value:
                    esm.skipHSub();
                    mDataTypes |= DATA_VNML;
                    break;
                case ESM::FourCC<'V','H','G','T'>::value:
                    esm.skipHSub();
                    mDataTypes |= DATA_VHGT;
                    break;
                case ESM::FourCC<'W','N','A','M'>::value:
                    esm.getHExact(mWnam, sizeof(mWnam));
                    mDataTypes |= DATA_WNAM;
                    break;
                case ESM::FourCC<'V','C','L','R'>::value:
                    esm.skipHSub();
                    mDataTypes |= DATA_VCLR;
                    break;
                case ESM::FourCC<'V','T','E','X'>::value:
                    esm.skipHSub();
                    mDataTypes |= DATA_VTEX;
                    break;
                default:
                    esm.fail("Unknown subrecord");
                    break;
            }
        }
    }

    void Land::save(ESMWriter &esm, bool isDeleted) const
    {
        esm.startSubRecord("INTV");
        esm.writeT(mX);
        esm.writeT(mY);
        esm.endRecord("INTV");

        esm.writeHNT("DATA", mFlags);

        if (isDeleted)
        {
            esm.writeHNCString("DELE", "");
            return;
        }

        if (mLandData)
        {
            if (mDataTypes & Land::DATA_VNML) {
                esm.writeHNT("VNML", mLandData->mNormals, sizeof(mLandData->mNormals));
            }
            if (mDataTypes & Land::DATA_VHGT) {
                VHGT offsets;
                offsets.mHeightOffset = mLandData->mHeights[0] / HEIGHT_SCALE;
                offsets.mUnk1 = mLandData->mUnk1;
                offsets.mUnk2 = mLandData->mUnk2;

                float prevY = mLandData->mHeights[0];
                int number = 0; // avoid multiplication
                for (int i = 0; i < LAND_SIZE; ++i) {
                    float diff = (mLandData->mHeights[number] - prevY) / HEIGHT_SCALE;
                    offsets.mHeightData[number] =
                        (diff >= 0) ? (int8_t) (diff + 0.5) : (int8_t) (diff - 0.5);

                    float prevX = prevY = mLandData->mHeights[number];
                    ++number;

                    for (int j = 1; j < LAND_SIZE; ++j) {
                        diff = (mLandData->mHeights[number] - prevX) / HEIGHT_SCALE;
                        offsets.mHeightData[number] =
                            (diff >= 0) ? (int8_t) (diff + 0.5) : (int8_t) (diff - 0.5);

                        prevX = mLandData->mHeights[number];
                        ++number;
                    }
                }
                esm.writeHNT("VHGT", offsets, sizeof(VHGT));
            }
        }

        if (mDataTypes & Land::DATA_WNAM) {
            esm.writeHNT("WNAM", mWnam, 81);
        }

        if (mLandData)
        {
            if (mDataTypes & Land::DATA_VCLR) {
                esm.writeHNT("VCLR", mLandData->mColours, 3*LAND_NUM_VERTS);
            }
            if (mDataTypes & Land::DATA_VTEX) {
                uint16_t vtex[LAND_NUM_TEXTURES];
                transposeTextureData(mLandData->mTextures, vtex);
                esm.writeHNT("VTEX", vtex, sizeof(vtex));
            }
        }

    }

    void Land::blank()
    {
        mPlugin = 0;

        for (int i = 0; i < LAND_GLOBAL_MAP_LOD_SIZE; ++i)
            mWnam[0] = 0;

        if (!mLandData)
            mLandData = new LandData;

        mLandData->mHeightOffset = 0;
        for (int i = 0; i < LAND_NUM_VERTS; ++i)
            mLandData->mHeights[i] = 0;
        mLandData->mMinHeight = 0;
        mLandData->mMaxHeight = 0;
        for (int i = 0; i < LAND_NUM_VERTS; ++i)
        {
            mLandData->mNormals[i*3+0] = 0;
            mLandData->mNormals[i*3+1] = 0;
            mLandData->mNormals[i*3+2] = 127;
        }
        for (int i = 0; i < LAND_NUM_TEXTURES; ++i)
            mLandData->mTextures[i] = 0;
        for (int i = 0; i < LAND_NUM_VERTS; ++i)
        {
            mLandData->mColours[i*3+0] = -1;
            mLandData->mColours[i*3+1] = -1;
            mLandData->mColours[i*3+2] = -1;
        }
        mLandData->mUnk1 = 0;
        mLandData->mUnk2 = 0;
        mLandData->mDataLoaded = Land::DATA_VNML | Land::DATA_VHGT | Land::DATA_WNAM |
            Land::DATA_VCLR | Land::DATA_VTEX;
        mDataTypes = mLandData->mDataLoaded;

        // No file associated with the land now
        mContext.filename.clear();
    }

    void Land::loadData(int flags, LandData* target) const
    {
		
        // Create storage if nothing is loaded
        if (!target && !mLandData)
        {
            mLandData = new LandData;
        }

        if (!target)
            target = mLandData;

        // Try to load only available data
        flags = flags & mDataTypes;
        // Return if all required data is loaded
        if ((target->mDataLoaded & flags) == flags) {
            return;
        }

        // Copy data to target if no file
        if (mContext.filename.empty())
        {
            // Make sure there is data, and that it doesn't point to the same object.
            if (mLandData && mLandData != target)
                *target = *mLandData;

            return;
        }

        ESM::ESMReader reader;
        reader.restoreContext(mContext);

        if (reader.isNextSub("VNML")) {
            condLoad(reader, flags, target->mDataLoaded, DATA_VNML, target->mNormals, sizeof(target->mNormals));
        }

        if (reader.isNextSub("VHGT")) {
            VHGT vhgt;
            //actually get vhgt from looking up my csv
			//greylock terrain object

			//give x
			//give y
			//give number of points per side.
			//get cell data, formatted.
			//give row
			//give column
			//gives back MW friendly value
			
			
			if (condLoad(reader, flags, target->mDataLoaded, DATA_VHGT, &vhgt, sizeof(vhgt))) {
                target->mMinHeight = FLT_MAX;
                target->mMaxHeight = -FLT_MAX;
                float rowOffset = vhgt.mHeightOffset;
                for (int y = 0; y < LAND_SIZE; y++) {
                    rowOffset += vhgt.mHeightData[y * LAND_SIZE];

					target->mHeights[y * LAND_SIZE] = rowOffset * HEIGHT_SCALE;
					target->mHeights[y * LAND_SIZE] = ESM::Land::GreylockLand::getHeightAtIndex(mX, mY, y*LAND_SIZE);
                    if (rowOffset * HEIGHT_SCALE > target->mMaxHeight)
                        target->mMaxHeight = rowOffset * HEIGHT_SCALE;
                    if (rowOffset * HEIGHT_SCALE < target->mMinHeight)
                        target->mMinHeight = rowOffset * HEIGHT_SCALE;
					//mwx height
                    float colOffset = rowOffset;
                    for (int x = 1; x < LAND_SIZE; x++) {
                        colOffset += vhgt.mHeightData[y * LAND_SIZE + x];
						target->mHeights[x + y * LAND_SIZE] = colOffset * HEIGHT_SCALE;
						target->mHeights[x + y * LAND_SIZE] = ESM::Land::GreylockLand::getHeightAtIndex(mX, mY, x + y*LAND_SIZE);

                        if (colOffset * HEIGHT_SCALE > target->mMaxHeight)
                            target->mMaxHeight = colOffset * HEIGHT_SCALE;
                        if (colOffset * HEIGHT_SCALE < target->mMinHeight)
                            target->mMinHeight = colOffset * HEIGHT_SCALE;
                    }
                }
                target->mUnk1 = vhgt.mUnk1;
                target->mUnk2 = vhgt.mUnk2;
            }
        }

        if (reader.isNextSub("WNAM"))
            reader.skipHSub();

        if (reader.isNextSub("VCLR"))
            condLoad(reader, flags, target->mDataLoaded, DATA_VCLR, target->mColours, 3 * LAND_NUM_VERTS);
        if (reader.isNextSub("VTEX")) {
            uint16_t vtex[LAND_NUM_TEXTURES];
            if (condLoad(reader, flags, target->mDataLoaded, DATA_VTEX, vtex, sizeof(vtex))) {
                transposeTextureData(vtex, target->mTextures);
            }
        }
    }

    void Land::unloadData() const
    {
        if (mLandData)
        {
            delete mLandData;
            mLandData = NULL;
        }
    }

    bool Land::condLoad(ESM::ESMReader& reader, int flags, int& targetFlags, int dataFlag, void *ptr, unsigned int size) const
    {
        if ((targetFlags & dataFlag) == 0 && (flags & dataFlag) != 0) {
            reader.getHExact(ptr, size);
            targetFlags |= dataFlag;
            return true;
        }
        reader.skipHSubSize(size);
        return false;
    }

    bool Land::isDataLoaded(int flags) const
    {
        return mLandData && (mLandData->mDataLoaded & flags) == flags;
    }

    Land::Land (const Land& land)
    : mFlags (land.mFlags), mX (land.mX), mY (land.mY), mPlugin (land.mPlugin),
      mContext (land.mContext), mDataTypes (land.mDataTypes),
      mLandData (land.mLandData ? new LandData (*land.mLandData) : 0)
    {}

    Land& Land::operator= (Land land)
    {
        swap (land);
        return *this;
    }

    void Land::swap (Land& land)
    {
        std::swap (mFlags, land.mFlags);
        std::swap (mX, land.mX);
        std::swap (mY, land.mY);
        std::swap (mPlugin, land.mPlugin);
        std::swap (mContext, land.mContext);
        std::swap (mDataTypes, land.mDataTypes);
        std::swap (mLandData, land.mLandData);
    }

    const Land::LandData *Land::getLandData (int flags) const
    {
        if (!(flags & mDataTypes))
            return 0;

        loadData (flags);
        return mLandData;
    }

    const Land::LandData *Land::getLandData() const
    {
        return mLandData;
    }

    Land::LandData *Land::getLandData()
    {
        return mLandData;
    }

    void Land::add (int flags)
    {
        if (!mLandData)
            mLandData = new LandData;

        mDataTypes |= flags;
        mLandData->mDataLoaded |= flags;
    }

    void Land::remove (int flags)
    {
        mDataTypes &= ~flags;

        if (mLandData)
        {
            mLandData->mDataLoaded &= ~flags;

            if (!mLandData->mDataLoaded)
            {
                delete mLandData;
                mLandData = 0;
            }
        }
    }


	float Land::GreylockLand::getHeightAtIndex(int cellx, int celly, int index)
	{
		
		
		//first find center of terrain.
		float xcenter = (sLandHeights.begin()->first + sLandHeights.rbegin()->first) / 2.0;
		/*
		auto sampleyrange = sLandHeights.rbegin()->second;
		return 0;
		float ycenter = (sampleyrange.begin()->first + sampleyrange.rbegin()->first) / 2.0f;*/
		
		//assumes rectangle
		//auto sampleyrange = sLandHeights.begin()->second;
		
		//69.99

		float cellwidth = 8192 / 69.99 * 2;

		float ycenter = sCenterY;

		//std::cout << "center is" << sCenterY << std::endl;
		

		//how many meters is a cell?
		//117 meters per side
		//aka 128 yards

		//find where on map cellx and celly are

		float targety = index / (LAND_SIZE);
		float targetx = index % (LAND_SIZE);
		

		
		float xmeteroffset = cellx * cellwidth + xcenter;
		float ymeteroffset = celly * cellwidth + ycenter;
		//std::cout << targety << std::endl;
		ymeteroffset += targety * 3.65;
		xmeteroffset += targetx * 3.65;


		//std::cout << "X IS..." << xmeteroffset << std::endl;
		//std::cout << "Y IS..." << ymeteroffset << std::endl;




		//get closest bounds we can find on terrain map
		//return 0;

		auto t1 = sLandHeights.lower_bound(xmeteroffset);
		if (t1 == sLandHeights.end())
		{
			t1--;
		}
		auto t2 = t1->second.lower_bound(ymeteroffset);
		if (t2 == t1->second.end())
		{
			t2--;
		}
		//std::cout << t2->second << std::endl;
		//return t2->second * 60.f;
		//return (*sLandHeights.lower_bound(xmeteroffset)).second.lower_bound(ymeteroffset)->second * 60;
		
		auto bound1 = sLandHeights.lower_bound(xmeteroffset);
		if (bound1 == sLandHeights.end())
			bound1--;
		auto bound1b = bound1->second.lower_bound(ymeteroffset);
		if (bound1b == bound1->second.end())
			bound1b--;

		float x1 = bound1->first;
		float y1 = bound1b->first;
		float z1 = bound1b->second;

		auto bound2 = bound1++;

		float x2 = bound1->first;

		bound1b = bound1->second.lower_bound(ymeteroffset);
		auto bound2b = bound1b++;
		float y2 = bound1b->first;
		float z2 = bound1b->second;


		/*std::cout << "X1 = " << x1 << std::endl;
		std::cout << "X2 = " << x2 << std::endl;
		std::cout << "Y1 = " << y1 << std::endl;
		std::cout << "Y2 = " << y2 << std::endl;
*/
		//return z2 * 60;

	





	/*	float x2 = sLandHeights.lower_bound(xmeteroffset)++->first;
		float y2 = sLandHeights.lower_bound(xmeteroffset)++->second.lower_bound(ymeteroffset)->first;
		float z2 = sLandHeights.lower_bound(xmeteroffset)++->second.lower_bound(ymeteroffset)->second;*/



/*


		auto xlb = sLandHeights.lower_bound(xmeteroffset);
		auto xub = sLandHeights.upper_bound(xmeteroffset);


		auto ylb = sLandHeights.lower_bound(ymeteroffset);
		ylb--;
		auto yub = sLandHeights.upper_bound(ymeteroffset);
		

		float lowx = xlb->first;
		float hix = xub->first;
		float hiy = yub->first;
		float lowy = ylb->first;*/



		osg::Vec3f v0(x1, y1, sLandHeights[x1][y1]);
		osg::Vec3f v1(x2, y1, sLandHeights[x2][y1]);
		osg::Vec3f v2(x2, y2, sLandHeights[x2][y2]);
		osg::Vec3f v3(x1, y2, sLandHeights[x1][y2]);
		//std::cout << "--====SAMPLE HEIGHT" << sLandHeights[hiy][lowx] << std::endl;




		//int cellxindex = xmeteroffset / 5;
		//int cellyindex = ymeteroffset / 5;
		//

		//get normalized position in cell

		float nY = ymeteroffset;//targety / LAND_SIZE;
		float nX = xmeteroffset;//targetx / LAND_SIZE;

		float factor = ESM::Land::LAND_SIZE - 1.0f;
		float invFactor = 1.0f / factor;

		float xParam = (nX - x1) * factor;
		float yParam = (nY - y1) * factor;

		osg::Plane plane;
		// FIXME: deal with differing triangle alignment
	/*	if (true)
		{*/
			// odd row
			bool secondTri = ((1.0 - yParam) > xParam);
			if (secondTri)
				plane = osg::Plane(v0, v1, v3);
			else
				plane = osg::Plane(v1, v2, v3);
		//}
		/*
		else
		{
		// even row
		bool secondTri = (yParam > xParam);
		if (secondTri)
		plane.redefine(v0, v2, v3);
		else
		plane.redefine(v0, v1, v2);
		}
		*/

		// Solve plane equation for z
		float z = (-plane.getNormal().x() * nX
			- plane.getNormal().y() * nY
			- plane[3]) / plane.getNormal().z() * 60;

	//std::cout << "--====RETURNING: " << z << std::endl;

	return z;

		
	}
	

	void Land::GreylockLand::buildLand()
	{

		//check if already built
		if (sLandHeights.size() > 0)
			return;

		std::ifstream in("terrain/testrange.xyz");
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
		
		bool firstiter = true;
		float biggesty = 0;
		float miny = 0;
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
			//parsing assumes Y axis is grouped together. 
			
			sLandHeights[x][y] = z;

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
			//if (prevy == currenty)
			//{
			//	currentxrow[x] = z;
			//}
			//else
			//{
			//	//std::cout << "y changed" << std::endl;
			//	grid[prevy] = currentxrow;
			//	currentxrow.clear();
			//	
			//	//grid.push_back(currentxrow);
			//	
			//	
			//	
			//	currentxrow[x] = z;
			//	prevy = currenty;
			//}
			//
		}

		//grid.push_back(currentxrow);
		//grid[prevy] = currentxrow;

		std::cout << "--===COUNT IS====----" << std::endl;
		std::cout << count << std::endl;

		std::cout << "--===Y COUNT IS====----" << std::endl;
		std::cout << grid.size() << std::endl;

		std::cout << "--===Y SIZE IS===---" << std::endl;
		std::cout << grid.size() * 5 << " meters" << std::endl;
		
		/*std::cout << "--===X COUNT IS====----" << std::endl;
		std::cout << grid.back().size() << std::endl;

		std::cout << "--===X COUNT IS====----" << std::endl;
		std::cout << grid.back().size() * 5 << " meters" << std::endl;
		*/
		std::cout << "MIN Y   " << miny << std::endl;

		std::cout << "BIG Y   " << biggesty << std::endl;

		sCenterY = (biggesty + miny) / 2.0;
		//sCenterY += .5;



		// sLandHeights = grid;

		//testmap;
		//open thing.


	}

	std::vector<float> Land::GreylockLand::getfloats(int x, int y, int numperside)
	{
		//get center of slandheights

		//LAND_SIZE;
		//int totalpoints = LAND_SIZE * LAND_SIZE;

		//int xcenter = sLandHeights.back().size() / 2;
		//int ycenter = sLandHeights.size() / 2;

		//int celllength = LAND_SIZE;
		//int yleft = celllength;

		//std::vector<float> result;

		//while (yleft > 0)
		//{
		//	int currentrow = celllength - yleft;
		//	std::vector<std::pair<float, float>> rowvec = sLandHeights[currentrow];
		//	//23 real world readings are needed to fill in 63 points in the terrain. Use each reading 3 times
		//	for (std::vector<std::pair<float, float>>::iterator it = rowvec.begin(); it != rowvec.end(); it++)
		//	{
		//		result.push_back((*it).second);
		//		result.push_back((*it).second);
		//		result.push_back((*it).second);
		//	}
		//	yleft -= 1;


		//}





		std::vector<float> result;
		return result;
	}

}
