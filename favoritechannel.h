#ifndef __FAVORITECHANNEL_H
#define __FAVORITECHANNEL_H

#include <vdr/plugin.h>
#include <vdr/channels.h>

#define CHANNELSMAX 255

class cFavoriteChannel : public cListObject
{
   private:
      tChannelID Index;
   public:
      bool Parse(char *s);
      bool Save(FILE *f);
      const tChannelID GetIndex() { return Index; }
      void SetIndex(tChannelID index) { Index = index; }
      virtual int Compare(const cListObject &ListObject) const;
};

class cFavoriteChannels : public cConfig<cFavoriteChannel>
{
   private:
      const char *FileName_;
   public:
      static tChannelID channel_to_remove;
      virtual bool Load(const char *filename, bool dummy=false);
      bool Save(void);
      cFavoriteChannel *GetFavoriteChannelNumber(int number);
      void AddFavoriteChannel(const tChannelID Index);
      void RemoveFavoriteChannel(tChannelID number);
      tChannelID GetCurrentFavorite();
      void ReadFavoriteChannels(void);
};
#endif                           //__FAVORITECHANNEL_H
