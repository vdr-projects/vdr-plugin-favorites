/*
 * favoriteosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/device.h>
#include <vdr/osd.h>

#include "favoriteosd.h"
#include "config.h"
#include "favoritechannel.h"

extern int number;
extern tChannelID favoritechannels[CHANNELSMAX];

extern cFavoriteChannels FavoriteChannelsList;
extern cFavoriteChannels FavoriteChannelsListDisplay;

// Constructor and destructor

cFavoriteOsd::cFavoriteOsd(void):cOsdMenu(tr("Favorite Channels"))
{
   lastChannel = tChannelID::InvalidID;
   Show();
}

cFavoriteOsd::~cFavoriteOsd(void)
{
   Clear();
}

void cFavoriteOsd::Show()
{
   Clear();
   // Reading the favoriteList

   ReadFavoriteChannels();

   // find the actual current
   LOCK_CHANNELS_READ;
   const  cChannel *Channel = Channels->GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
   if (Channel)
   {
      tChannelID myCurrent = Channel->GetChannelID();
      // Find the actually tuned channel
      int i=0;
      while( (i < CHANNELSMAX) && (i < number) && !(favoritechannels[i] == myCurrent) )
      {
         i++;
      }
      current = ( (number != 0) && (i != number) ) ? i : 0;
   }
   SetHelp(tr("Button$Remove"), tr("Button$Add"), tr("Button$Up"), tr("Button$Down"));
   DisplayFavorites();
}


void cFavoriteOsd::DisplayFavorites()
{
   Clear();
   for (int i=0; i<number; i++)
   {
      LOCK_CHANNELS_READ;
      if (Channels->GetByChannelID(favoritechannels[i]))
      {
         Add (new cOsdItem((Channels->GetByChannelID(favoritechannels[i]))->Name()));
      }
   }
   Display();

   if(Get(current))
   {
      SetCurrent(Get(current));
      Display();
   }
}


eOSState cFavoriteOsd::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);

   cOsdItem * item = Get(Current());
   if (item)
   {
      current=item->Index();
   }

   if (state == osUnknown)
   {
      switch (Key)
      {
         case kRed:
            RemoveChannel();
            break;
         case kGreen:
            AddChannel();
            break;
         case kYellow:
            MoveChannel(-1);
            break;
         case kBlue:
            MoveChannel(1);
            break;
         case kOk:
            CursorOK();
            if (config.closeonswitch)
               return osEnd;
            else
               break;
         case k0:
            LastChannel();
            break;
         case kBack:
            return osEnd;
         default:
            return state;
      }
      state = osContinue;
      DisplayFavorites();
   }
   return state;
}


// Other public methods

void cFavoriteOsd::ReadFavoriteChannels()
{
   int i=0;
   number = 0;
   cFavoriteChannel *FavoriteChannel;
   FavoriteChannelsListDisplay.Sort();
   if (config.sortby == 0)
      // If no sort, use the raw file
      FavoriteChannel = FavoriteChannelsList.GetFavoriteChannelNumber(i);
   else
      // else use the sorted file
      FavoriteChannel = FavoriteChannelsListDisplay.GetFavoriteChannelNumber(i);

   while ( FavoriteChannel != NULL )
   {
      if ( (i < CHANNELSMAX) && (FavoriteChannel != NULL))
      {
         favoritechannels[i] = FavoriteChannel->GetIndex();
         #ifdef DEBUG
         fprintf(stderr, "[%2.2d]->'%s'\n",\
            i, (Channels.GetByChannelID(favoritechannels[i]))->Name());
         #endif
      }
      number += 1;
      i++;

      if (config.sortby == 0)
         // If no sort, use the raw file
         FavoriteChannel = FavoriteChannelsList.GetFavoriteChannelNumber(i);
      else
         // else use the sorted file
         FavoriteChannel = FavoriteChannelsListDisplay.GetFavoriteChannelNumber(i);
   }
}


void cFavoriteOsd::CursorOK()
{
   /*
    * If current channel is a favoritered one we remember it as the lastChannel
    */
   lastChannel = FavoriteChannelsList.GetCurrentFavorite();
   LOCK_CHANNELS_READ;
   const cChannel *Channel = Channels->GetByChannelID(favoritechannels[current]);
   if (Channel)
      cDevice::PrimaryDevice()->SwitchChannel(Channel, true);
}


void cFavoriteOsd::LastChannel()
{
   if (lastChannel.Valid())
   {
      /*
       * Before return to lastChannel we have to rememeber the actually tuned one
       */
      tChannelID oldLastChannel = lastChannel;
      lastChannel = FavoriteChannelsList.GetCurrentFavorite();
      //Let's set the new current channel and press OK :)
      for(int i=0; i<CHANNELSMAX; i++)
         if(favoritechannels[i] == oldLastChannel)
      {
         current = i+1;
         CursorOK();
         return;
      }
   }
}


void cFavoriteOsd::AddChannel()
{
   if (number < CHANNELSMAX)
   {
      LOCK_CHANNELS_READ;
      const cChannel *Channel = Channels->GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
      if (Channel)
      {
         tChannelID Current_channel = Channel->GetChannelID();
         // Test if channels is already favoriteed
         for (int i=0; i<number; i++)
         {
            if (Current_channel == favoritechannels[i])
            {
               // Channel to add exist already -> go to it
               current = i+1;
               return;
            }
         }
         FavoriteChannelsList.AddFavoriteChannel(Current_channel);
         FavoriteChannelsList.Save();
         FavoriteChannelsListDisplay.AddFavoriteChannel(Current_channel);
         ReadFavoriteChannels();
         current=0;
         for (int i=0; i<number; i++)
            if (favoritechannels[i] == Current_channel)
               current = i+1;
      }
   }
}


void cFavoriteOsd::RemoveChannel()
{
   if (number > 0)
   {
      FavoriteChannelsList.RemoveFavoriteChannel(favoritechannels[current]);
      FavoriteChannelsList.Save();
      FavoriteChannelsListDisplay.RemoveFavoriteChannel(favoritechannels[current]);
      if (current>0)
         current = current-1;
      else
         current = 0;
      ReadFavoriteChannels();
   }
}


void cFavoriteOsd::MoveChannel(int places)
{
   if (((current+places) >= 0) && ((current + places) < number))
   {
      FavoriteChannelsList.Move((current), ((current)+places));
      FavoriteChannelsList.Save();
      FavoriteChannelsListDisplay.Move((current), ((current)+places));
      current += places;
      ReadFavoriteChannels();
   }
}
