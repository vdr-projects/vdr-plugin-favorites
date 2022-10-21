#ifndef __FAVORITESETUP_H
#define __FAVORITESETUP_H

#include <vdr/plugin.h>

class cFavoriteSetup : public cMenuSetupPage
{
   private:
      const char *SortOpt[3];
      const char* ProgressViewtStrs[3];
      int HideMainMenuEntry;
      int CloseOnSwitch;
      int SortBy;
      int ShowEPG;
      int ProgressView;

   protected:
      virtual void Store(void);
   public:
      cFavoriteSetup(void);
};
#endif                           //__FAVORITESETUP_H
