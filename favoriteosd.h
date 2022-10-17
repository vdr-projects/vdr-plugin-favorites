#ifndef __FAVORITEOSD_H
#define __FAVORITEOSD_H

#include <vdr/menu.h>
#include <vdr/plugin.h>

class cFavoriteOsd : public cOsdMenu
{
   private:
      tChannelID lastChannel;
      void AddMenuEntry(int favId);

   public:
      cFavoriteOsd(void);
      ~cFavoriteOsd();
      virtual void Show(void);
      virtual eOSState ProcessKey(eKeys Key);
      void DisplayFavorites(void);
      void ReadFavoriteChannels(void);
      void CursorOK();
      void LastChannel();
      void AddChannel();
      void RemoveChannel();
      void MoveChannel(int places);

   private:
      int current;
      bool now;

};

#endif                           //__FAVORITEOSD_H
