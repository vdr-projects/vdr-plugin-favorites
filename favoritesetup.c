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
   SortBy      = config.sortby;
   HideMainMenuEntry=config.hidemainmenuentry;
   SortOpt[0] = tr("as entered");
   SortOpt[1] = tr("none");
   SortOpt[2] = tr("name");
   Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &HideMainMenuEntry));
   Add(new cMenuEditBoolItem(tr("Close on switch"), &CloseOnSwitch));
   Add(new cMenuEditStraItem(tr("Sort by"), &SortBy, 3, SortOpt));
}


void cFavoriteSetup::Store(void)
{
   SetupStore("CloseOnSwitch", config.closeonswitch = CloseOnSwitch);
   SetupStore("SortBy", config.sortby = SortBy);
   SetupStore("HideMainMenuEntry", config.hidemainmenuentry = HideMainMenuEntry);
}
