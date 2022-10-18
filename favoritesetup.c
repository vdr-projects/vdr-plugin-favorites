/*
 * favoritesetup.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "favoritesetup.h"

cFavoriteSetup::cFavoriteSetup(void)
{
   CloseOnSwitch = config.closeonswitch;
   SortBy = config.sortby;
   HideMainMenuEntry = config.hidemainmenuentry;
   ShowEPG = config.showepg;
   ProgressView = config.progressview;

   SortOpt[0] = tr("as entered");
   SortOpt[1] = tr("none");
   SortOpt[2] = tr("name");

   ProgressViewtStrs[0] = tr("old text bar");
   ProgressViewtStrs[1] = tr("VDRSymbols");
   ProgressViewtStrs[2] = tr("Percent");

   Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &HideMainMenuEntry));
   Add(new cMenuEditBoolItem(tr("Close on switch"), &CloseOnSwitch));
   Add(new cMenuEditStraItem(tr("Sort by"), &SortBy, 3, SortOpt));
   Add(new cMenuEditBoolItem(tr("Show EPG"), &ShowEPG));
   Add(new cMenuEditStraItem(tr("Progress view"), &ProgressView, 3, ProgressViewtStrs));
}

void cFavoriteSetup::Store(void)
{
   SetupStore("CloseOnSwitch", config.closeonswitch = CloseOnSwitch);
   SetupStore("SortBy", config.sortby = SortBy);
   SetupStore("HideMainMenuEntry", config.hidemainmenuentry = HideMainMenuEntry);
   SetupStore("ShowEPG", config.showepg = ShowEPG);
   SetupStore("ProgressView", config.progressview = ProgressView);
}
