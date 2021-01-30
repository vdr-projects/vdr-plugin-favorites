/*
 * Favorites.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "config.h"
#include "favoriteosd.h"
#include "favoritesetup.h"
#include "favoritechannel.h"

extern cFavoriteChannels FavoriteChannelsList;
extern cFavoriteChannels FavoriteChannelsListDisplay;

static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = trNOOP("Favorite Channels Menu");
static const char *MAINMENUENTRY  = trNOOP("Favorites");

class cPluginFavorites : public cPlugin
{
   private:
      // Add any member variables or functions you may need here.
   public:
      cPluginFavorites(void);
      virtual ~cPluginFavorites();
      virtual const char *Version(void) { return VERSION; }
      virtual const char *Description(void) { return tr(DESCRIPTION); }
      virtual const char *CommandLineHelp(void);
      virtual bool ProcessArgs(int argc, char *argv[]);
      virtual bool Start(void);
      virtual void Housekeeping(void);
      virtual const char *MainMenuEntry(void) { return config.hidemainmenuentry ? NULL : tr(MAINMENUENTRY); }
      virtual cOsdObject *MainMenuAction(void);
      virtual cMenuSetupPage *SetupMenu(void);
      virtual bool SetupParse(const char *Name, const char *Value);
};

cPluginFavorites::cPluginFavorites(void)
{
   // Initialize any member variables here.
   // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
   // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
   config.closeonswitch=1;
   config.sortby=1;
   config.hidemainmenuentry=0;
}


cPluginFavorites::~cPluginFavorites()
{
   // Clean up after yourself!
}


const char *cPluginFavorites::CommandLineHelp(void)
{
   // Return a string that describes all known command line options.
   return NULL;
}


bool cPluginFavorites::ProcessArgs(int argc, char *argv[])
{
   // Implement command line argument processing here if applicable.
   return true;
}


bool cPluginFavorites::Start(void)
{
   // Start any background activities the plugin shall perform.
   // Default values for setup
   return true;
}


void cPluginFavorites::Housekeeping(void)
{
   // Perform any cleanup or other regular tasks.
}


cOsdObject *cPluginFavorites::MainMenuAction(void)
{
   // Perform the action when selected from the main VDR menu.
   // Load favorite channels list
   if(!FavoriteChannelsList.Load(AddDirectory(ConfigDirectory(),"favorites.conf")))
   {
      // File load failed
      isyslog("Favorites.conf file not found");
   }
   // Create the "displayed" channel list (which can be sorted)
   if(!FavoriteChannelsListDisplay.Load(AddDirectory(ConfigDirectory(),"favorites.conf")))
   {
      // Saving after loading allows to clean the file
      // File load failed
      // But just log once
   }
   // Clean up the file with current VDR channels.conf
   FavoriteChannelsList.Save();
   return new cFavoriteOsd();
}


cMenuSetupPage *cPluginFavorites::SetupMenu(void)
{
   // Return a setup menu in case the plugin supports one.
   return new cFavoriteSetup;
}


bool cPluginFavorites::SetupParse(const char *Name, const char *Value)
{
   // Parse your own setup parameters and store their values.
   if (!strcasecmp(Name, "CloseOnSwitch"))          config.closeonswitch = atoi(Value);
   else if (!strcasecmp(Name, "SortBy"))            config.sortby = atoi(Value);
   else if (!strcasecmp(Name, "HideMainMenuEntry")) config.hidemainmenuentry = atoi(Value);
   else
      return false;

   return true;
}


sFavoriteConfig config;

                                 // Don't touch this!
VDRPLUGINCREATOR(cPluginFavorites);
