#ifndef GAME_MWWORLD_CELLSTORE_H
#define GAME_MWWORLD_CELLSTORE_H

#include <deque>
#include <algorithm>
#include <stdexcept>

#include "livecellref.hpp"
#include "esmstore.hpp"
#include "cellreflist.hpp"

namespace ESM
{
    struct CellState;
}

namespace MWWorld
{
    class Ptr;

  /// \brief Mutable state of a cell
  class CellStore
  {
    public:

        enum State
        {
            State_Unloaded, State_Preloaded, State_Loaded
        };

    private:

        const ESM::Cell *mCell;
        State mState;
        std::vector<std::string> mIds;
        float mWaterLevel;

        CellRefList<ESM::Activator>         mActivators;
        CellRefList<ESM::Potion>            mPotions;
        CellRefList<ESM::Apparatus>         mAppas;
        CellRefList<ESM::Armor>             mArmors;
        CellRefList<ESM::Book>              mBooks;
        CellRefList<ESM::Clothing>          mClothes;
        CellRefList<ESM::Container>         mContainers;
        CellRefList<ESM::Creature>          mCreatures;
        CellRefList<ESM::Door>              mDoors;
        CellRefList<ESM::Ingredient>        mIngreds;
        CellRefList<ESM::CreatureLevList>   mCreatureLists;
        CellRefList<ESM::ItemLevList>       mItemLists;
        CellRefList<ESM::Light>             mLights;
        CellRefList<ESM::Lockpick>          mLockpicks;
        CellRefList<ESM::Miscellaneous>     mMiscItems;
        CellRefList<ESM::NPC>               mNpcs;
        CellRefList<ESM::Probe>             mProbes;
        CellRefList<ESM::Repair>            mRepairs;
        CellRefList<ESM::Static>            mStatics;
        CellRefList<ESM::Weapon>            mWeapons;

    public:

        CellStore (const ESM::Cell *cell_);

        const ESM::Cell *getCell() const;

        State getState() const;

        bool hasId (const std::string& id) const;
        ///< May return true for deleted IDs when in preload state. Will return false, if cell is
        /// unloaded.

        Ptr search (const std::string& id);
        ///< Will return an empty Ptr if cell is not loaded. Does not check references in
        /// containers.

        Ptr searchViaHandle (const std::string& handle);
        ///< Will return an empty Ptr if cell is not loaded.

        float getWaterLevel() const;

        void setWaterLevel (float level);

        int count() const;
        ///< Return total number of references, including deleted ones.


    void load (const MWWorld::ESMStore &store, std::vector<ESM::ESMReader> &esm);

    void preload (const MWWorld::ESMStore &store, std::vector<ESM::ESMReader> &esm);

    /// Call functor (ref) for each reference. functor must return a bool. Returning
    /// false will abort the iteration.
    /// \return Iteration completed?
    ///
    /// \note Creatures and NPCs are handled last.
    template<class Functor>
    bool forEach (Functor& functor)
    {
        return
            forEachImp (functor, mActivators) &&
            forEachImp (functor, mPotions) &&
            forEachImp (functor, mAppas) &&
            forEachImp (functor, mArmors) &&
            forEachImp (functor, mBooks) &&
            forEachImp (functor, mClothes) &&
            forEachImp (functor, mContainers) &&
            forEachImp (functor, mDoors) &&
            forEachImp (functor, mIngreds) &&
            forEachImp (functor, mItemLists) &&
            forEachImp (functor, mLights) &&
            forEachImp (functor, mLockpicks) &&
            forEachImp (functor, mMiscItems) &&
            forEachImp (functor, mProbes) &&
            forEachImp (functor, mRepairs) &&
            forEachImp (functor, mStatics) &&
            forEachImp (functor, mWeapons) &&
            forEachImp (functor, mCreatures) &&
            forEachImp (functor, mNpcs) &&
            forEachImp (functor, mCreatureLists);
    }

    bool operator==(const CellStore &cell) {
        return  mCell->mName == cell.mCell->mName &&
                mCell->mData.mX == cell.mCell->mData.mX &&
                mCell->mData.mY == cell.mCell->mData.mY;
    }

    bool operator!=(const CellStore &cell) {
        return !(*this == cell);
    }

    bool isExterior() const {
        return mCell->isExterior();
    }

    Ptr searchInContainer (const std::string& id);

        void loadState (const ESM::CellState& state);

        void saveState (ESM::CellState& state) const;

        void writeReferences (ESM::ESMWriter& writer) const;

        void readReferences (ESM::ESMReader& reader, const std::map<int, int>& contentFileMap);

        template <class T>
        CellRefList<T>& get() {
            throw std::runtime_error ("Storage for this type not exist in cells");
        }

  private:

    template<class Functor, class List>
    bool forEachImp (Functor& functor, List& list)
    {
        for (typename List::List::iterator iter (list.mList.begin()); iter!=list.mList.end();
            ++iter)
        {
            if (!iter->mData.getCount())
                continue;
            if (!functor (MWWorld::Ptr(&*iter, this)))
                return false;
        }
        return true;
    }

    /// Run through references and store IDs
    void listRefs(const MWWorld::ESMStore &store, std::vector<ESM::ESMReader> &esm);

    void loadRefs(const MWWorld::ESMStore &store, std::vector<ESM::ESMReader> &esm);

        void loadRef (ESM::CellRef& ref, bool deleted, const ESMStore& store);
        ///< Make case-adjustments to \a ref and insert it into the respective container.
        ///
        /// Invalid \a ref objects are silently dropped.
  };

    template<>
    inline CellRefList<ESM::Activator>& CellStore::get<ESM::Activator>()
    {
        return mActivators;
    }

    template<>
    inline CellRefList<ESM::Potion>& CellStore::get<ESM::Potion>()
    {
        return mPotions;
    }

    template<>
    inline CellRefList<ESM::Apparatus>& CellStore::get<ESM::Apparatus>()
    {
        return mAppas;
    }

    template<>
    inline CellRefList<ESM::Armor>& CellStore::get<ESM::Armor>()
    {
        return mArmors;
    }

    template<>
    inline CellRefList<ESM::Book>& CellStore::get<ESM::Book>()
    {
        return mBooks;
    }

    template<>
    inline CellRefList<ESM::Clothing>& CellStore::get<ESM::Clothing>()
    {
        return mClothes;
    }

    template<>
    inline CellRefList<ESM::Container>& CellStore::get<ESM::Container>()
    {
        return mContainers;
    }

    template<>
    inline CellRefList<ESM::Creature>& CellStore::get<ESM::Creature>()
    {
        return mCreatures;
    }

    template<>
    inline CellRefList<ESM::Door>& CellStore::get<ESM::Door>()
    {
        return mDoors;
    }

    template<>
    inline CellRefList<ESM::Ingredient>& CellStore::get<ESM::Ingredient>()
    {
        return mIngreds;
    }

    template<>
    inline CellRefList<ESM::CreatureLevList>& CellStore::get<ESM::CreatureLevList>()
    {
        return mCreatureLists;
    }

    template<>
    inline CellRefList<ESM::ItemLevList>& CellStore::get<ESM::ItemLevList>()
    {
        return mItemLists;
    }

    template<>
    inline CellRefList<ESM::Light>& CellStore::get<ESM::Light>()
    {
        return mLights;
    }

    template<>
    inline CellRefList<ESM::Lockpick>& CellStore::get<ESM::Lockpick>()
    {
        return mLockpicks;
    }

    template<>
    inline CellRefList<ESM::Miscellaneous>& CellStore::get<ESM::Miscellaneous>()
    {
        return mMiscItems;
    }

    template<>
    inline CellRefList<ESM::NPC>& CellStore::get<ESM::NPC>()
    {
        return mNpcs;
    }

    template<>
    inline CellRefList<ESM::Probe>& CellStore::get<ESM::Probe>()
    {
        return mProbes;
    }

    template<>
    inline CellRefList<ESM::Repair>& CellStore::get<ESM::Repair>()
    {
        return mRepairs;
    }

    template<>
    inline CellRefList<ESM::Static>& CellStore::get<ESM::Static>()
    {
        return mStatics;
    }

    template<>
    inline CellRefList<ESM::Weapon>& CellStore::get<ESM::Weapon>()
    {
        return mWeapons;
    }
}

#endif
