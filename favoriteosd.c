/*
 * favoriteosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/device.h>
#include <vdr/osd.h>
#include <string>
#include <locale.h>
#include <langinfo.h>

#include "favoriteosd.h"
#include "config.h"
#include "favoritechannel.h"

#define SHORTTEXT(EVENT) \
    ((EVENT) && !isempty((EVENT)->ShortText())) ? " ~ ":"", \
((EVENT) && !isempty((EVENT)->ShortText())) ? (EVENT)->ShortText():""

// --- Icons ------------------------------------------------------------------
bool Icons::IsUTF8 = false;

void Icons::InitCharSet()
{
    // Taken from VDR's vdr.c
    char *CodeSet = NULL;
    if(setlocale(LC_CTYPE, ""))
        CodeSet = nl_langinfo(CODESET);
    else
    {
        char *LangEnv = getenv("LANG"); // last resort in case locale stuff isn't installed
        if(LangEnv)
        {
            CodeSet = strchr(LangEnv,'.');
            if( CodeSet )
                CodeSet++; // skip the dot
        }
    }

    if( CodeSet && strcasestr(CodeSet,"UTF-8") != 0 )
        IsUTF8 = true;
}

extern int number;
extern tChannelID favoritechannels[CHANNELSMAX];

extern cFavoriteChannels FavoriteChannelsList;
extern cFavoriteChannels FavoriteChannelsListDisplay;

// Constructor and destructor

cFavoriteOsd::cFavoriteOsd(void):cOsdMenu(tr("Favorite Channels"))
{
   lastChannel = tChannelID::InvalidID;
   now = true;
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
   {
        LOCK_CHANNELS_READ;
        const cChannel *Channel = Channels->GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
        if (Channel) {
            tChannelID myCurrent = Channel->GetChannelID();
            // Find the actually tuned channel
            int i = 0;
            while ((i < CHANNELSMAX) && (i < number) && !(favoritechannels[i] == myCurrent)) {
                i++;
            }
            current = ((number != 0) && (i != number)) ? i : 0;
        }
   }

   SetHelp(tr("Button$Remove"), tr("Button$Add"), tr("Button$Up"), tr("Button$Down"));
   DisplayFavorites();
}


void cFavoriteOsd::DisplayFavorites()
{
   Clear();

   SetCols(17, 5, 5, 4);

   for (int i=0; i<number; i++)
   {
      bool channelExists = false;
      {
        LOCK_CHANNELS_READ;
        channelExists = Channels->GetByChannelID(favoritechannels[i]) != NULL;
      }

      if (channelExists)
      {
         if (config.showepg) {
             AddMenuEntry(i);
         } else {
             LOCK_CHANNELS_READ;
             Add(new cOsdItem((Channels->GetByChannelID(favoritechannels[i]))->Name()));
         }
      }
   }
   Display();

   if(Get(current))
   {
      SetCurrent(Get(current));
      Display();
   }
}

void cFavoriteOsd::AddMenuEntry(int favId) {
    const cEvent *event = NULL;
    const cTimer* hasMatch = NULL;
    eTimerMatch timerMatch = tmNone;

    char t = ' ';
    char v = ' ';
    char r = ' ';
    char szEventDescr[100] = "";

    LOCK_TIMERS_READ;
    LOCK_CHANNELS_READ;
    const cChannel *channel = Channels->GetByChannelID(favoritechannels[favId]);

    if (channel) {
        LOCK_SCHEDULES_READ;
        const cSchedule *schedule = Schedules->GetSchedule(channel);
        if (schedule == NULL) {
            event = NULL;
        } else {
            if (now) {
                event = schedule->GetPresentEvent();
            } else {
                event = schedule->GetFollowingEvent();
            }

            if (event) {
                hasMatch = Timers->GetMatch(event, &timerMatch);
            }
        }

        char szChannelpart[20] = "";
        snprintf(szChannelpart, 20, "%s", channel->Name());

        char szProgressPart[50] = "";
        if (event) {
            strcpy(szProgressPart, "\t");

            int fracInt;
            float fracFloat;
            std::string ProgressBar;
            char szProgress[9] = "";

            switch (config.progressview) {
                case 1: // VDRSymbols
                    ProgressBar += Icons::ProgressStart();
                    fracInt = (int) roundf((float) (time(NULL) - event->StartTime()) / (float) (event->Duration()) * 10.0);
                    fracInt = min(10, max(0, fracInt));
                    for (int i = 0; i < 10; i++) {
                        if (i < fracInt)
                            ProgressBar += Icons::ProgressFilled();
                        else
                            ProgressBar += Icons::ProgressEmpty();
                    }
                    ProgressBar += Icons::ProgressEnd();
                    sprintf(szProgressPart, "%s\t", ProgressBar.c_str());
                    break;

                case 0:
                    fracInt = (int) roundf((float) (time(NULL) - event->StartTime()) / (float) (event->Duration()) * 8.0);
                    fracInt = min(8, max(0, fracInt));

                    for (int i = 0; i < fracInt; i++)
                        szProgress[i] = '|';
                    szProgress[fracInt] = 0;
                    sprintf(szProgressPart, "%c%-8s%c\t", '[', szProgress, ']');
                    break;

                default:
                    fracFloat = (int) roundf(
                            (float) (time(NULL) - event->StartTime()) / (float) (event->Duration()) * 100.0);
                    sprintf(szProgressPart, "%3.0f%%", fracFloat);
            }

            t = hasMatch ? ((timerMatch == tmFull) ? 'T' : 't') : ' ';
            v = event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
            r = event->IsRunning() ? '*' : ' ';

            snprintf(szEventDescr, 100, "%s%s%s",
                     event ? event->Title() : tr("no info"),
                     SHORTTEXT(event));
        }

        char *buffer = NULL;
        asprintf(&buffer, "%s\t%s\t%s %c%c%c \t%s",
                 szChannelpart,
                 event?*(event->GetTimeString() ):"",
                 szProgressPart,
                 t, v, r,
                 szEventDescr);

        Add (new cOsdItem(buffer));
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
         case k8:
             now = !now;
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
