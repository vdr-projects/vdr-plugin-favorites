/*
 * favoritechannel.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "favoritechannel.h"
#include "config.h"
#include <vdr/device.h>

// The favoriteed channels list (stored on disk)
cFavoriteChannels FavoriteChannelsList;

// The favoriteed channels list (displayed)
cFavoriteChannels FavoriteChannelsListDisplay;

int number=0;
tChannelID favoritechannels[CHANNELSMAX];

bool cFavoriteChannel::Parse(char *s)
{
   //let's use the tChannelID type
   Index = tChannelID::FromString(s);
   if ( Index.Valid() )
   {
      // Maybe control the uniqueness...
      return true;
   }
   else
      return false;
}


bool cFavoriteChannel::Save(FILE *f)
{
   /* Fix crash problem reported by Manuel Hartl    */
   /* when loading a non existing favoriteed channel  */
   cChannel *channel;
   channel = Channels.GetByChannelID(GetIndex(), true);
   if (!channel)
   {
      isyslog("Favorite channel '%s' does not exist anymore. Removing\n", *(Index.ToString()) );
      return true;
   }
   if (!(FavoriteChannelsList.channel_to_remove == GetIndex()))
      return fprintf(f, "%s\n", *(Index.ToString()) ) > 0;
   else
      return true;
}


int cFavoriteChannel::Compare(const cListObject &ListObject) const
{
   //< Must return 0 if this object is equal to ListObject, a positive value
   //< if it is "greater", and a negative value if it is "smaller".
   const cFavoriteChannel *l = (cFavoriteChannel *)&ListObject;
   if (config.sortby == 1)
   {
      /* Do not sort */
      return 0;
   }
   else if (config.sortby == 2)
   {
      /* Sort by same order as channels.conf */
      if (strcmp(Channels.GetByChannelID(l->Index)->Name(), Channels.GetByChannelID(Index)->Name()) == 0)
         return 0;
      for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel))
      {
         if (!channel->GroupSep() || *channel->Name())
         {
            if (!strcmp(channel->Name(), Channels.GetByChannelID(l->Index)->Name()))
            {
               return 1;
            }
            else if (!strcmp(channel->Name(), Channels.GetByChannelID(Index)->Name()))
            {
               return -1;
            }
         }
      }
      /* should never go there! */
      return 0;
   }
   else if (config.sortby == 3)
   {
      /* Sort by channel name */
      return strcasecmp( Channels.GetByChannelID(l->Index)->Name(), Channels.GetByChannelID(Index)->Name())*-1;
   }
   else
   {
      /* Default */
      return 0;
   }
}


tChannelID cFavoriteChannels::channel_to_remove = tChannelID::InvalidID;

bool cFavoriteChannels::Load(const char *filename, bool dummy)
{
   FileName_ = filename;
   if(cConfig<cFavoriteChannel>::Load(filename,true))
   {
      return true;
   }
   return false;
}


bool cFavoriteChannels::Save(void)
{
   if (cConfig<cFavoriteChannel>::Save())
   {
      // isyslog("saved setup to %s", FileName());
      channel_to_remove = tChannelID::InvalidID;
      //cConfig<cFavoriteChannel>::Load(FileName_,true);
      return true;
   }
   return false;
}


cFavoriteChannel *cFavoriteChannels::GetFavoriteChannelNumber(int number)
{
   if(number >= CHANNELSMAX)
      return NULL;
   cFavoriteChannel *l = First();
   for (int i=0; i<number; i++)
      l=Next(l);
   return l;
}


void cFavoriteChannels::AddFavoriteChannel(tChannelID Index)
{
   if(number >= CHANNELSMAX)
      return;
   cFavoriteChannel *FavoriteChannel = new cFavoriteChannel;
   FavoriteChannel->SetIndex(Index);
   Add(FavoriteChannel);
}


void cFavoriteChannels::RemoveFavoriteChannel(tChannelID removenumber)
{
   for (cFavoriteChannel *channel = First(); channel; channel = Next(channel))
   {
      if (channel->GetIndex() == removenumber) Del(channel, false);
   }
   channel_to_remove = removenumber;
}


tChannelID cFavoriteChannels::GetCurrentFavorite()
{
   cChannel *Channel = Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
   if (Channel)
   {
      tChannelID current_channel = Channel->GetChannelID();
      // Test if channels is already favoriteed
      for (int i=0; i<number; i++)
         if (current_channel == favoritechannels[i])
            return current_channel;
   }
   return tChannelID::InvalidID;
}
