// THIS IS A BASTARDIZED VERSION OF MHWEPG.C, HACKED TO SUPPORT NEUTRINO, AND
// MHWEPGv2. USE AT YOUR OWN RISK.
//
// MHWEPG v2 code is based on eepg-0.0.3 (Extended Epg plugin to VDR) written by
// Dingo35, and includes code from the LoadEpg plugin written by Luca De Pieri
// <dpluca@libero.it>
//
// Most of my changes are include in #ifdef NEUTRINO stanzas.
//
// If you're trying to use this with a system other than Evolux/Ntrino, take
// care that your sectionsd supports 32bit event ids.
//
// --martii
#ifndef NEUTRINO
#define NEUTRINO
#endif

//#define DEBUG
/*
 *  mhwepg.c	EPG for Canal+ bouquets
 *
 *  (C) 2002, 2003 Jean-Claude Repetto <mhwepg@repetto.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 * 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef NEUTRINO
#include <iconv.h>
#include <errno.h>
#endif

#include <linux/dvb/dmx.h>
#include "mhwepg.h"

static int debug = 0;

const char *day_names[] = {
   "Sun","Mon","Tue","Wed","Thu","Fri","Sat"};


#define MAX_SECTION_BUFFER 4096
unsigned char buffer[MAX_SECTION_BUFFER+1];	/* General data buffer */

#define MAX_CHANNELS 180

char channels[MAX_CHANNELS][50];
#ifdef NEUTRINO
char channel_ids[MAX_CHANNELS][100];
#else
char channel_ids[MAX_CHANNELS][50];
#endif
#ifdef NEUTRINO
#define MAX_THEMES 2046
char themes[MAX_THEMES][256];
#else
char themes[256][16];
#endif
char channel_equiv[MAX_CHANNELS][2][50];
int nb_equiv=0;

#ifdef NEUTRINO
#define MAX_PROGRAMS 262144
#else
#define MAX_PROGRAMS 25000
#endif
struct program * programs[MAX_PROGRAMS];
struct summary * summaries[MAX_PROGRAMS];

char device [30];
char source [50];

int nb_channels = 0;
int nb_programs = 0;
int nb_summaries = 0;
int hour_index [24*7];

#ifdef NEUTRINO
int opt_n = 0;
#endif

/* Local Time offsets */
int current_offset, next_offset;
time_t time_of_change;
int time_offset_set = 0;
int yesterday;	/* Day of the week of yesterday */
time_t yesterday_epoch;	/* Epoch of yesterday 00:00 in provider local time */

/*
 *	Removes trailing spaces from a string
 */

void strclean (char *s)
{
#ifdef NEUTRINO
   char * end = NULL;
#else
   char * end;
#endif
   while (*s)
   {
      if (!isspace (*s))
         end = s+1;
      s++;
   }
#ifdef NEUTRINO
 if (end)
#endif
   *end = 0;
}

/*
 *	Compare country code and region
 */

int timezonecmp(item_local_time_offset_t *item_local_time_offset,
   			 const char * country, int region)
{
   return item_local_time_offset->country_code1 == country[0] &&
          item_local_time_offset->country_code2 == country[1] &&
          item_local_time_offset->country_code3 == country[2] &&
          item_local_time_offset->region_id == region ? 0 : 1;
}

/*
 *	Set a filter for a DVB stream
 */

void SetFiltSection (int fd, unsigned short pid, unsigned char tid)
{
	struct dmx_sct_filter_params sctFilterParams;

	if (ioctl(fd, DMX_SET_BUFFER_SIZE, 188*1024)<0) {
          perror("DMX SET BUFFER SIZE");
          exit(0);
        } 

	memset(&sctFilterParams, 0, sizeof(sctFilterParams));
	sctFilterParams.pid     = pid;
	sctFilterParams.timeout = 60000;
	sctFilterParams.flags   = DMX_IMMEDIATE_START;

    sctFilterParams.filter.filter[0] = tid;
    sctFilterParams.filter.mask[0] = 0xFF;
        
	if (ioctl(fd, DMX_SET_FILTER, &sctFilterParams) < 0)  
    {
		perror("DMX SET FILTER:");
        exit (0);
    }
}

/*
 *	Get channel names (PID=0xD3, TID=0x91)
 */

int GetChannelNames(int list_channels)
{
   int fd,i,n;
   unsigned short sid;
   channel_name_t *channel_name;
   
   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd, 211, 0x91);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   if (n <= 0)
   {
      fprintf (stderr, "Error while reading channels names\n");
      perror(":");
      return -1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);

   channel_name = (channel_name_t *) (buffer+4);
   nb_channels = (n-4)/sizeof(channel_name_t);
   if (nb_channels >= MAX_CHANNELS)
   {
      fprintf (stderr, "Channels Table overflow (%d names needed)\n", nb_channels);
      return -1;
   }

   for (i=0; i<nb_channels; i++)
   {
      sid = HILO (channel_name->channel_id);

      memcpy (channels[i], channel_name -> name, 16);
      channels[i][16] = 0;
      /* Remove trailing blanks */
      strclean (channels[i]);
#ifdef NEUTRINO
      snprintf (channel_ids[i], sizeof(channel_ids[i]),
		"original_network_id=\"%.4x\" "
		"transport_stream_id=\"%.4x\" "
		"service_id=\"%.4x\"",
		HILO (channel_name->network_id), HILO (channel_name->transponder_id), HILO (channel_name->channel_id));
#else
      if (source[0] != 0)
	      sprintf (channel_ids[i], "%s-%d-%d-%d",source,HILO (channel_name->network_id),HILO (channel_name->transponder_id), HILO (channel_name->channel_id));
      else
	      sprintf (channel_ids[i], "%d", HILO (channel_name->channel_id));
#ifdef DEBUG
      fprintf (stderr,"SID = %s   CHANNEL = '%s'\n",channel_ids[i], channels[i]);
#endif
#endif
	if (list_channels)
	      fprintf (stderr,"%s %s\n",channel_ids[i], channels[i]);
      channel_name++;
   }
   return 0;
}

/*
 *	Get themes names (PID=0xD3, TID=0x92)
 */

int GetThemes (void)
{
   int fd,i,n,m,theme,nb;
   unsigned char * themes_index;
   typedef unsigned char theme_name[15];
   theme_name * themes_names;
   
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }

   SetFiltSection (fd, 211, 0x92);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   if (n <= 0)
   {
      fprintf (stderr,"Error while reading themes\n");
      perror(":");
      return -1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);

   themes_index = buffer+3;	/* Skip table ID and length */
   themes_names = (theme_name *) (buffer+19);	/* Start of names table */
   theme = 0;			
   m = 0;
   nb = (n-19)/15;	/* Number of themes */
   for (i=0; i<nb; i++)
   {
      if (themes_index[theme] == i)	/* New theme */
      {
         m = (m + 15) & 0xF0;
         theme++;
      }
      memcpy (themes[m], themes_names, 15);
      themes[m][15] = 0;
      strclean(themes[m]);
#ifdef NEUTRINO
      if(debug)
#endif
      fprintf (stderr,"%02X '%s'\n", m, themes[m]);

      m++;
      themes_names++;
   }
   return 0;
}

#ifdef NEUTRINO
/**
 * \brief Get MHW2 Themes
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */

int nThemes = 0;

int GetThemesMHW2 (const u_char * Data, int Length)
{
    int p1;
    int p2;
    int pThemeName = 0;
    int pSubThemeName = 0;
    int lenThemeName = 0;
    int lenSubThemeName = 0;
    int pThemeId = 0;
    int i, ii;
    if (Length > 4) {
if(debug)
      fprintf(stderr, "-------------THEMES FOUND--------------");
      for (i = 0; i < Data[4]; i++) {
        p1 = ((Data[5 + i * 2] << 8) | Data[6 + i * 2]) + 3;
        if (Length > p1) {
          for (ii = 0; ii <= (Data[p1] & 0x3f); ii++) {
            p2 = ((Data[p1 + 1 + ii * 2] << 8) | Data[p1 + 2 + ii * 2]) + 3;
            if (Length > p2) {
              if (ii == 0) {
                pThemeName = p2 + 1;
                lenThemeName = Data[p2] & 0x1f;
                lenSubThemeName = 0;
              } else {
                pSubThemeName = p2 + 1;
                lenSubThemeName = Data[p2] & 0x1f;
              }
              if (Length >= (pThemeName + lenThemeName)) {
                pThemeId = ((i & 0x3f) << 6) | (ii & 0x3f);
                if (pThemeId > MAX_THEMES) {
if(debug)
                  fprintf(stderr, "Error, something wrong with themes id calculation MaxThemes: %i pThemeID:%d\n", MAX_THEMES, pThemeId);
                  return 0; //fatal error
                }
                if ((lenThemeName + 2) < 256) {
                  memcpy (themes[pThemeId], &Data[pThemeName], lenThemeName);
                  if (Length >= (pSubThemeName + lenSubThemeName))
                    if (lenSubThemeName > 0)
                      if ((lenThemeName + lenSubThemeName + 2) < 256) {
                        themes[pThemeId][lenThemeName] = ' ';
                        memcpy (&themes[pThemeId][lenThemeName + 1], &Data[pSubThemeName], lenSubThemeName);
                      }
                  strclean (themes[pThemeId]);
if(debug)
                  fprintf (stderr, "Added theme %d: >>%s<<\n", pThemeId, themes[pThemeId]);
                  nThemes++;
                  if (nThemes > MAX_THEMES) {
                    fprintf(stderr, "Error, %i themes found more than %i\n", nThemes, MAX_THEMES);
                    return 0; //fatal error
                  }
                }
              } else
                return 1; //I assume non fatal error or success
            } else
              return 1;  //I assume non fatal error or success
          }
        } else
          return 1;  //I assume non fatal error or success
      }    //for loop
      //Del (Pid, Tid);
      return 2; //all themes read
    } //if length
  return 1; //non fatal or success
}

//FIXME -- may as well list channels
int GetChannelsMHW2 (const u_char * Data, int Length)
{
//    sChannelMHW1 *Channel;
   channel_name_t *channel_name;
    int Size, Off;
    Size = sizeof (channel_name_t);
    Off = 4;

      if (Length > 120)
        nb_channels = Data[120];
      else {
        fprintf(stderr, "Error, channels packet too short for MHW2.");
        return 0;
      }
      int pName = ((nb_channels * 8) + 121);
      if (Length > pName) {
        //Channel = (sChannelMHW1 *) (Data + 120);
        Size -= 14; //MHW2 is 14 bytes shorter
        Off = 121;  //and offset differs
      } else {
        fprintf(stderr, "Error, channels length does not match pname.");
        return 0;
      }

    if (nb_channels > MAX_CHANNELS) {
      fprintf(stderr, "EEPG: Error, %i channels found more than %i", nb_channels, MAX_CHANNELS);
      return 0;
    } else {
      int pName = ((nb_channels * 8) + 121); //TODO double ...
	int i;
      for (i = 0; i < nb_channels; i++) {
   	channel_name = (channel_name_t *) (Data+Off);

          int lenName = Data[pName] & 0x0f;
          memcpy (channels[i], &Data[pName + 1], lenName);
	  channels[i][lenName] = 0;
          strclean (channels[i]);
          pName += (lenName + 1);
      snprintf (channel_ids[i], sizeof(channel_ids[i]),
		"original_network_id=\"%.4x\" "
		"transport_stream_id=\"%.4x\" "
		"service_id=\"%.4x\"",
		HILO (channel_name->network_id), HILO (channel_name->transponder_id), HILO (channel_name->channel_id));

	if(debug)
	      fprintf (stderr,"SID = %s   CHANNEL = '%s'\n",channel_ids[i], channels[i]);
        Off += Size;
      } //for loop
    }   //else nb_channels > MAX_CHANNELS
//    LoadEquivalentChannels ();
//    GetLocalTimeOffset (); //reread timing variables, only used for MHW
    return 2; //obviously, when you get here, channels are read succesfully, but since all channels are sent at once, you can stop now
}

/*
 *	Get themes and channels (PID=0x231, TID=0xc8)
 */

int GetThemesAndChannels (void)
{
   int fd,n;
   int got_themes = 0;
   int got_channels = 0;
   
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }

   SetFiltSection (fd, 0x231, 0xc8);
   while (!got_themes || !got_channels) {
	   n = read(fd,buffer,MAX_SECTION_BUFFER);
	   if (n <= 0)
	   {
	      fprintf (stderr,"Error while reading themes and channels\n");
	      perror(":");
	      return -1;
	   }
	   if (n < 4)
		continue;
	   if (!got_channels && buffer[3] == 0) {
		// Channels
		if (1 != GetChannelsMHW2 (buffer, n))
			got_channels = 1;
	   } else if (!got_themes && buffer[3] == 1) {
		// Themes
		if (1 != GetThemesMHW2 (buffer, n))
			got_themes = 1;
	   }
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}

/**
 * \brief Get MHW2 Titles
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */

unsigned char InitialChannel[8];
unsigned char InitialTitle[64];
unsigned char InitialSummary[64];

int ParseTitlesMHW2 (const u_char * Data, int Length)
{
  if (Length > 18) {
    int Pos = 18;
    int Len = 0;
    /*bool Check = false;
    while (Pos < Length) {
      Check = false;
      Pos += 7;
      if (Pos < Length) {
        Pos += 3;
        if (Pos < Length)
          if (Data[Pos] > 0xc0) {
            Pos += (Data[Pos] - 0xc0);
            Pos += 4;
            if (Pos < Length) {
              if (Data[Pos] == 0xff) {
                Pos += 1;
                Check = true;
              }
            }
          }
      }
      if (Check == false){
        isyslog ("EEPGDebug: Check==false");
        return 1; // I assume nonfatal error or success
        }
    }*/
    if (memcmp (InitialTitle, Data, 16) == 0) { //data is the same as initial title
      return 2; //last item processed
    } else {
      if (nb_programs == 0)
        memcpy (InitialTitle, Data, 16); //copy data into initial title
      //Pos = 18;
      while (Pos < Length) {
   	struct program * program = malloc (sizeof (struct program));
	 programs[nb_programs] = program;
        program->channel_id = Data[Pos];
        Pos+=11;//The date time starts here
        //isyslog ("EEPGDebug: ChannelID:%d", T->ChannelId);
        unsigned int MjdTime = (Data[Pos] << 8) | Data[Pos + 1];
        //T->MjdTime = 0; //not used for matching MHW2
        program->time = ((MjdTime - 40587) * 86400)
                       + (((((Data[Pos + 2] & 0xf0) >> 4) * 10) + (Data[Pos + 2] & 0x0f)) * 3600)
                       + (((((Data[Pos + 3] & 0xf0) >> 4) * 10) + (Data[Pos + 3] & 0x0f)) * 60);
        program->duration = (((Data[Pos + 5] << 8) | Data[Pos + 6]) >> 4) * 60;
        Len = Data[Pos + 7] & 0x3f;
	memcpy(program->title, &Data[Pos + 8], Len);
	program->title[Len] = 0;
	strclean(program->title);
        Pos += Len + 8; // Sub Theme starts here

        program->theme_id = ((Data[7] & 0x3f) << 6) | (Data[Pos] & 0x3f);
        program->id = (Data[Pos + 1] << 8) | Data[Pos + 2]; // event id, actually
        program->summary_available = (program->id != 0xFFFF);
	if(debug)
	    fprintf(stderr, "EventId %04x Titlenr %d:SummAv:%x,Name:%s.\n", program->id,
            	 nb_programs, program->summary_available, program->title);
        Pos += 3;
        nb_programs++;
        if (nb_programs > MAX_PROGRAMS) {
          fprintf(stderr, "Error, %i titles found more than %i\n", nb_programs, MAX_PROGRAMS);
          return 0; //fatal error
        }
      }
      return 1; //success
    } //else memcmp
  } //if length
  return 1; //non fatal error
}

int GetTitlesMHW2(void)
{
   int res = 1;
   int fd,n,i;

   nb_programs=0;
   for (i=0; i<24*7; i++)
      hour_index[i] = -1;

   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE ");
      return -1;
   }
   SetFiltSection(fd,0x234,0xe6);

   fprintf (stderr,"Reading titles ...\n");

   while (res == 1) {
      /* Read the next program title */
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         fprintf (stderr,"Error while reading titles\n");
         perror(":");
         return -1;
      }
	res = ParseTitlesMHW2(buffer, n);
	if (res == 0)
         	fprintf (stderr,"Error while parsing titles\n");
	
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return (res == 2) ? 0 : -1;
}

/**
 * \brief Get MHW2 Summaries
 *
 * \return 0 = fatal error, code 1 = success, code 2 = last item processed
 */
int ParseSummariesMHW2 (const u_char * Data, int Length)
{
  if (Length > (Data[14] + 17)) {
    if (memcmp (InitialSummary, Data, 16) == 0) //data is equal to initial buffer
      return 2;
    else {
      if (nb_summaries == 0)
        memcpy (InitialSummary, Data, 16); //copy this data in initial buffer
      if (nb_summaries < MAX_PROGRAMS) {
   struct summary * summary;
        int lenText = Data[14];
        int SummaryLength = lenText;
        int Pos = 15;
        int Loop = Data[Pos + SummaryLength] & 0x0f;
      summary = malloc (sizeof (struct summary));
      summaries[nb_summaries++] = summary;

        summary->id = (Data[3] << 8) | Data[4];
        unsigned char tmp[4096]; //TODO do this smarter
        memcpy (tmp, &Data[Pos], lenText);
        tmp[SummaryLength] = '\n';
        SummaryLength += 1;
        Pos += (lenText + 1);
        if (Loop > 0) {
          while (Loop > 0) {
            lenText = Data[Pos];
            Pos += 1;
            if ((Pos + lenText) < Length) {
              memcpy (&tmp[SummaryLength], &Data[Pos], lenText);
              SummaryLength += lenText;
              if (Loop > 1) {
                tmp[SummaryLength] = '\n';
                SummaryLength += 1;
              }
            } else
              break;
            Pos += lenText;
            Loop--;
          }
        }
        summary->text = malloc (SummaryLength + 2);
        if (summary->text == NULL) {
          fprintf(stderr, "Summaries memory allocation error.");
          return 0; //fatal error
        }
        summary->text[SummaryLength] = '\0'; //end string with NULL character
        memcpy (summary->text, tmp, SummaryLength);
        strclean (summary->text);
	if(debug)
        	fprintf(stderr, "EventId %08x Summnr %d:%.30s.", summary->id, nb_summaries, summary->text);
      } else {
        fprintf(stderr, "Error, %i summaries found more than %i", nb_summaries, MAX_PROGRAMS);
        return 0; //fatal error
      }
    } //else
  } //if length
  return 1; //succes or nonfatal error
}

int GetSummariesMHW2(void) {
   int fd,n;
   int res = 1;
   
   fprintf (stderr,"Reading summaries ...\n");
   nb_summaries=0;
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }
   SetFiltSection(fd,0x236,0x96);

   while (res == 1)
   {
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0) {
         perror("Error while reading summaries : ");
         exit(1);
      }
	res = ParseSummariesMHW2(buffer, n);

   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return (res == 2) ? 0 : -1;
}

#endif

/*
 *	Convert provider local time to UTC
 */
time_t ProviderLocalTime2UTC (time_t t)
{
   if (t < time_of_change)	/* ---- FIX ME ---- */
      return t - current_offset;
   else
      return t - next_offset;
}

/*
 *	Convert UTC to provider local time
 */
time_t UTC2ProviderLocalTime (time_t t)
{
   if (t < time_of_change)
      return t + current_offset;
   else
      return t + next_offset;
}

/*
 *	Convert local time to UTC
 */
time_t LocalTime2UTC (time_t t)
{
	struct tm *temp;

	temp = gmtime(&t);
	temp->tm_isdst = -1;
	return mktime (temp);
}

/*
 *	Convert UTC to local time
 */
time_t UTC2LocalTime (time_t t)
{
	return 2 * t - LocalTime2UTC(t);
}

/*
 *	Parse TOT
 */

#ifdef NEUTRINO
char lang[4];
#endif

int ParseTOT(void)
{
   int fd,n,i;
   u_char *ptr;
   tot_t *tot;	/* Time offset table */
   descr_local_time_offset_t *descriptor;
   item_local_time_offset_t *item_local_time_offset;
   time_t diff_time, utc_offset, local_time_offset, next_time_offset,current_time;
   struct tm *change_time;

   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd, 0x14, 0x73);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   if (n <= 0)
   {
      fprintf (stderr, "Error while reading Time Offset Table\n");
      return -1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);

   /* Some providers does not send a correct UTC time, vg Cyfra + */
   tot = (tot_t *) buffer;
   current_time = time(NULL);
   diff_time = MjdToEpochTime(tot->utc_mjd) + BcdTimeToSeconds(tot->utc_time)
      		- current_time;

   /* Round */
   utc_offset = ((diff_time + 1800) / 3600) * 3600;
   fprintf (stderr,"UTC Offset = %d\n",(int)utc_offset);

   /* Parse the descriptors */
   ptr = (u_char *) (tot+1);
   n -= TOT_LEN + 4;	/* Descriptors length */
   while (n > 0)
   {
      descriptor = CastLocalTimeOffsetDescriptor (ptr);
      if (descriptor->descriptor_tag == DESCR_LOCAL_TIME_OFF)
      {
         item_local_time_offset = CastItemLocalTimeOffset(descriptor+1);
         for (i=0; i < descriptor->descriptor_length/ITEM_LOCAL_TIME_OFFSET_LEN; i++)
         {			 /* Get the local time offset */
            fprintf (stderr,"Country = %c%c%c, Region = %d\n", item_local_time_offset->country_code1,
               		item_local_time_offset->country_code2, item_local_time_offset->country_code3,
                        item_local_time_offset->region_id);
#ifdef NEUTRINO
	lang[0] = item_local_time_offset->country_code1;
	lang[1] = item_local_time_offset->country_code2;
	lang[2] = item_local_time_offset->country_code3;
	lang[0] = tolower(lang[0]);
	lang[1] = tolower(lang[1]);
	lang[2] = tolower(lang[2]);
	lang[3] = 0;
#endif

            if (!timezonecmp(item_local_time_offset, "FRA", 0) ||
                !timezonecmp(item_local_time_offset, "POL", 0) ||
                !timezonecmp(item_local_time_offset, "NLD", 0) ||
                !timezonecmp(item_local_time_offset, "NED", 0) ||
                !timezonecmp(item_local_time_offset, "GER", 0) ||
                !timezonecmp(item_local_time_offset, "DEU", 0) ||
                !timezonecmp(item_local_time_offset, "ITA", 0) ||
                !timezonecmp(item_local_time_offset, "DNK", 0) ||
                !timezonecmp(item_local_time_offset, "SWE", 0) ||
                !timezonecmp(item_local_time_offset, "FIN", 0) ||
                !timezonecmp(item_local_time_offset, "NOR", 0) ||
                !timezonecmp(item_local_time_offset, "ESP", 1))
 
            {
               local_time_offset = 60 * BcdTimeToMinutes(item_local_time_offset->local_time_offset);
               next_time_offset = 60 * BcdTimeToMinutes(item_local_time_offset->next_time_offset);
               if (item_local_time_offset->local_time_offset_polarity)
               {
                  local_time_offset = -local_time_offset;
                  next_time_offset = -next_time_offset;
               }
               time_of_change = MjdToEpochTime(item_local_time_offset->toc_mjd)
                  		 + BcdTimeToSeconds(item_local_time_offset->toc_time);
            
               current_offset = local_time_offset + utc_offset;
               next_offset = next_time_offset + utc_offset;
	if(debug) {
               fprintf (stderr,"Local Time Offset = %d\n", current_offset);
               fprintf (stderr,"Next Local Time Offset = %d\n", next_offset);
               change_time = gmtime(&time_of_change);
               fprintf (stderr,"Date of change = %02d/%02d/%4d\n",change_time->tm_mday,
                  	change_time->tm_mon+1,change_time->tm_year+1900);
	}
               return 0;
            }
            item_local_time_offset++;
         }    
      }
      ptr += descriptor->descriptor_length+DESCR_LOCAL_TIME_OFFSET_LEN;
      n -= descriptor->descriptor_length+DESCR_LOCAL_TIME_OFFSET_LEN;
   }
   return -1;
}

/*
 *	Get Local Time Offset
 */

int GetTimeOffset(void)
{
   struct tm *cur_time;
   time_t yesterday_time;

   if (!time_offset_set)
   {
      if (ParseTOT() != 0)
      {
         printf ("No local time in the TOT table. Please use the -t option.\n");
         return -1;
      }
   }

   /* Get day of the week of yesterday (provider local time) */
// yesterday_time = UTC2ProviderLocalTime (time(NULL) - 86400);
   yesterday_time = UTC2LocalTime (time(NULL) - 86400);
   cur_time = gmtime(&yesterday_time);
   yesterday = cur_time->tm_wday;

   /* Get epoch of yesterday 00:00 (provider local time) */
   cur_time->tm_hour = 0;
   cur_time->tm_min = 0;
   cur_time->tm_sec = 0;
   cur_time->tm_isdst = -1;
// yesterday_epoch = UTC2ProviderLocalTime(mktime (cur_time));
   yesterday_epoch = UTC2LocalTime(mktime (cur_time));
   return 0;
}

/*
 *	Get Program Titles (PID=0xD2, TID=0x90)
 */

int GetTitles(void)
{
   int fd,n,d,h,i;
   char first_buffer[EPG_TITLE_LEN];
   short end_flag;
   struct program * program;
   epg_title_t *epgtitle = (epg_title_t *) buffer;
   time_t broadcast_time; 

   nb_programs=0;
   for (i=0; i<24*7; i++)
      hour_index[i] = -1;

   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE ");
      return -1;
   }
   SetFiltSection(fd,210,0x90);

   /* Skip also the titles until the next separator */
   do
   {
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         fprintf (stderr,"Error while reading titles\n");
         perror(":");
         return -1;
      }
      if (n != EPG_TITLE_LEN)
         fprintf (stderr,"Warning, wrong size table : %d\n",n);
   } while (n != EPG_TITLE_LEN || epgtitle->channel_id != 0xFF);

   fprintf (stderr,"Reading titles ...\n");

   /* Store the separator */
   memcpy (first_buffer, buffer, EPG_TITLE_LEN);
   end_flag =0;
   while (!end_flag)
   {
	if (n == EPG_TITLE_LEN)
	{
	   /* Get Day and Hour of the program */
	      d = epgtitle->day;
	      if (d == 7)
	         d = 0;
	      h = epgtitle->hours;
	      if (h>15)
        	 h = h-4;
	      else if (h>7)
        	 h = h-2;
	      else
        	 d = (d==6) ? 0 : d+1;
     
	      if (epgtitle->channel_id == 0xFF)		/* Separator */
	      {
	if(debug)
	      	  fprintf (stderr,"-------------------------------\n");
	         hour_index[d*24+h] = nb_programs;
	      }
	      else
	      {
	         program = malloc (sizeof (struct program));
	         programs[nb_programs++] = program;

        	 broadcast_time = (d-yesterday) * 86400 + h * 3600 + epgtitle->minutes * 60;
	         if (broadcast_time < 6 * 3600)		/* After yesterday 06:00 ? */
        	    broadcast_time += 7 * 86400;	/* Next week */
	         broadcast_time += yesterday_epoch;
//		 broadcast_time = ProviderLocalTime2UTC (broadcast_time);
		 broadcast_time = LocalTime2UTC (broadcast_time);

	         program -> channel_id = epgtitle->channel_id-1;
        	 program -> theme_id = epgtitle->theme_id;
	         program -> time = broadcast_time;
        	 program -> summary_available = epgtitle->summary_available;
	         program -> duration = HILO(epgtitle->duration) * 60;
        	 memcpy (program -> title, &epgtitle->title, 23);
	         program -> title[23] = 0;
        	 program -> ppv = HILO32 (epgtitle->ppv_id);
	         program -> id = HILO32 (epgtitle->program_id);

	if(debug) {
	         fprintf (stderr,"\n%s %s -  ",channel_ids[program -> channel_id], channels[program -> channel_id]);
        	 fprintf (stderr,"%s ", day_names[d]);
	         fprintf (stderr,"%2d:%02d ", h, epgtitle->minutes);

	         /* Duration */
        	 fprintf (stderr,"%4d' ", program -> duration / 60);
	         fprintf (stderr,"%10d ", program -> ppv);
        	 fprintf (stderr,"%10d ", program -> id);
	         fprintf (stderr,"Summary%s available\n", program -> summary_available ? "" : " NOT");
        	 fprintf (stderr,"%s", ctime(&program -> time));
         
	         fprintf (stderr,"%s - ", themes[program -> theme_id]);
        	 fprintf (stderr,"%s\n",program -> title);
	}
	      }
	}
      /* Read the next program title */
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         fprintf (stderr,"Error while reading titles\n");
         perror(":");
         return -1;
      }
      if (n != EPG_TITLE_LEN)
         fprintf (stderr,"Warning, wrong size table : %d\n",n);
      /* Check if we've got all the titles */
      if (!memcmp (first_buffer,buffer,n))
         end_flag=1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}

/*
 *	Get Program Summaries (PID=0xD3, TID=0x90)
 */

int GetSummaries(void)
{
   int fd,i,j,n;
   short end_flag;
   char first_buffer[EPG_SUMMARY_LEN];
   struct summary * summary;
   epg_summary_t *epgsummary = (epg_summary_t *) buffer;
   epg_replay_t *epg_replay;
   
   fprintf (stderr,"Reading summaries ...\n");
   nb_summaries=0;
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }
   SetFiltSection(fd,211,0x90);

   /* Read Summaries */
   do {
   	n = read(fd,buffer,MAX_SECTION_BUFFER);
        if (n <= 0)
        {
           fprintf (stderr,"Error while reading summaries\n");
           perror("read:");
           return -1;
        }
   }
   while (n < 12 || buffer[7] != 0xFF || buffer[8] != 0xFF || buffer[9] !=0xFF || buffer[10] >= 10);	/* Invalid Data */

   memcpy (first_buffer, buffer, EPG_SUMMARY_LEN);	/* Store the first summary */

   end_flag =0;
   while (!end_flag)
   {
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         perror("Error while reading summaries : ");
         exit(1);
      }
      if (n < 12 || buffer[7] != 0xFF || buffer[8] != 0xFF || buffer[9] !=0xFF || buffer[10] >= 10)	/* Invalid Data */
         continue;
         
      if (!memcmp (first_buffer,buffer,EPG_SUMMARY_LEN))  /* Got all summaries ? */
         end_flag=1;

      buffer[n] = 0;	/* String terminator */

      summary = malloc (sizeof (struct summary));
      summaries[nb_summaries++] = summary;

      summary -> id = HILO32 (epgsummary->program_id);
      /* Index of summary text beginning */
      j = EPG_SUMMARY_LEN + epgsummary->nb_replays*EPG_REPLAY_LEN;
      summary -> text = malloc (n+1 - j);

      if (summary -> text == NULL)
      {
         fprintf (stderr,"Memory allocation error %d\n",n+1-j);
	if(debug) {
         fprintf (stderr,"n=%d j=%d nb_replays=%d\n",n,j,epgsummary->nb_replays);
         for (i=0; i<n; i++)
            fprintf (stderr,"%02X ",buffer[i]);
         fprintf (stderr,"\n%s\n", buffer+10);
	}
         exit(1);
      }
#ifdef NEUTRINO
   if (opt_n)
      strcpy(summary->text, (char *)&buffer[j]);
   else {
#endif
      /* Copy the summary and replace line feeds by the "|" character */
      i=0;
      do
      {
         summary -> text[i] = buffer[j+i] == '\n' ? '|' : buffer[j+i];
         i++;
      }
      while (buffer[j+i-1] != 0);
#ifdef NEUTRINO
   }
#endif
      summary -> nb_replays = epgsummary->nb_replays;

      /* Get the times of replays */
      epg_replay = (epg_replay_t *) (epgsummary+1);
      for (i=0; i< summary -> nb_replays; i++)
      {
         summary -> time[i] = MjdToEpochTime(epg_replay->replay_mjd) +
            			BcdTimeToSeconds(epg_replay->replay_time);
//       summary -> time[i] = ProviderLocalTime2UTC (summary -> time[i]);
         summary -> time[i] = LocalTime2UTC (summary -> time[i]);
         summary -> channel_id[i] = epg_replay->channel_id-1;
         summary -> subtitles[i] = epg_replay->subtitles;
         summary -> vm[i] = epg_replay->vm;
         summary -> vo[i] = epg_replay->vo;
         summary -> last_replay[i] = epg_replay->last;
         epg_replay++;
      }
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}

static iconv_t conv_desc;

int fputs_utf8(const char *s, FILE *stream){
	if (!s)
		return -1;

	size_t len = strlen(s);
	size_t newlen = len + 1;
	int i;
	for(i = 0; i < len; i++) {
		switch (s[i]) {
		case '&': newlen += 4; break; // &amp;
		case '\'': newlen += 5; break; // &apos;
		case '"': newlen += 5; break; // &quot;
		case '<': newlen += 3; break; // &lt;
		case '>': newlen += 3; break; // &gt;
		//case '\n': newlen += 3; break; // &10; (newline)
		}
	}

	char *t = alloca(newlen);
	char *u = t;
	for(i = 0; i < len; i++) {
		switch (s[i]) {
		case '&':
			*u++ = '&'; *u++ = 'a'; *u++ = 'm'; *u++ = 'p'; *u++ = ';';
			break;
		case '\'':
			*u++ = '&'; *u++ = 'a'; *u++ = 'p'; *u++ = 'o'; *u++ = 's'; *u++ = ';';
			break;
		case '"':
			*u++ = '&'; *u++ = 'q'; *u++ = 'u'; *u++ = 'o'; *u++ = 't'; *u++ = ';';
			break;
		case '<':
			*u++ = '&'; *u++ = 'l'; *u++ = 't'; *u++ = ';';
			break;
		case '>':
			*u++ = '&'; *u++ = 'g'; *u++ = 't'; *u++ = ';';
			break;
		case '\n':
			//*u++ = '&'; *u++ = '1'; *u++ = '0'; *u++ = ';';
			*u++ = ' ';
			break;
		default:
			*u++ = s[i];
		}
	}
	*u = 0;

	size_t utf8len = 4 * newlen;
	char *utf8_start = alloca(utf8len);
	char *utf8 = utf8_start;
	if ((iconv(conv_desc, &t, &newlen, &utf8, &utf8len) == -1)) {
		fprintf(stderr, "iconv failure: %s\n", strerror(errno));
		exit (-1);
	} else
		fputs(utf8_start, stream);

	return 0;
}

int SaveEpgDataNeutrino(char * filename)
{
   FILE *file = NULL;
   FILE *indexxml = NULL;

   struct summary * summary;
   int i,j,/*n,*/ equiv_index;
   struct program * program;
//   struct tm *next_date;
   int first_program;
   int index;
   time_t current_time;
   char channel_id[100];
   char long_title[81];
   int summary_found;
   char *ptr;
   
	conv_desc = iconv_open("UTF-8", "ISO8859-1");
	if (conv_desc == (iconv_t)(-1)) {
		fprintf(stderr, "iconv_open failure: %s\n", strerror(errno));
		exit(1);
	}

   fprintf (stderr,"Writing EPG data to file ...\n");
   current_time = time(NULL);
#if 0
   /* Start from yesterday 6:00 */
   j = yesterday*24+6;
   for (i=0; i<24*7; i++)
      if ((first_program = hour_index[(i+j) % 24*7]) != -1)
         break;
#ifdef DEBUG
            fprintf (stderr,"First program = %d\n", first_program);
#endif
   if (first_program == -1)
      return -1;	/* No program */
#else
   first_program = 0;
#endif

   if (filename[0] != 0) {
	mkdir(filename, 0755);
	chdir(filename);
	indexxml = fopen ("index.xml","wb");
	if (!indexxml) {
		fprintf(stderr, "fopen(index.xml): %s\n", strerror(errno));
		exit(-1);
	}
   } else exit(-1);
fprintf(indexxml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<dvbepgfiles>\n");

 //  fprintf (stderr,"Nb of channels = %d\n", nb_channels);
   for (index = 0; index < nb_channels; index++)
   {
			char buf[30];
      strcpy (channel_id, channel_ids[index]);

			if (file) {
				fprintf(file, "\t</service>\n</dvbepg>\n");
				fclose(file);
				file = NULL;
			}
			snprintf(buf, sizeof(buf), "%.8x.xml", index);
			fprintf(indexxml, "\t\t<eventfile name=\"%s\"/>\n", buf);
			file = fopen(buf, "wb");
			fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!-- %s -->\n<dvbepg>\n\t<service %s>\n", channels[index], channel_ids[index]);

      for (equiv_index=-1; equiv_index<nb_equiv; equiv_index++)
      {
         if (equiv_index >= 0)
         {
if(debug)
            fprintf (stderr, "Equiv index = %d : '%s' '%s'\n",equiv_index,channel_ids[index], channel_equiv[equiv_index][0]);
            if (strcmp(channel_ids[index], channel_equiv[equiv_index][0]))
               continue;
            else
               strcpy (channel_id, channel_equiv[equiv_index][1]);
         }
         for (i=0; i<nb_programs; i++)
         {
            program = programs[(first_program+i)%nb_programs];

            if (program -> channel_id == index)
            {
               if (program->time + program->duration > current_time)
               {
//                  fprintf (file, "E %d %d %d 90\n", i, (int)(program->time), program->duration);

if(debug){
                  fprintf (stderr,"%s %s\n",channel_id, channels[index]);
                  fprintf (stderr,"%s", ctime(&program -> time));

                  /* Duration */
                  fprintf (stderr,"%4d' ", program -> duration / 60);
                  fprintf (stderr,"%10d ", program -> ppv);
                  fprintf (stderr,"%10d ", program -> id);
                  fprintf (stderr,"Summary%s available\n", program -> summary_available ? "" : " NOT");

                  fprintf (stderr,"%s\n",program -> title);
                  fprintf (stderr,"%s ", themes[program -> theme_id]);
}
                  summary_found = 0;
                  if (program -> summary_available)
                  {
                     for (j=0; j<nb_summaries; j++)
                        if (program -> id == summaries[j] -> id)
                        {
                           summary_found = 1;
                           summary = summaries[j];
                           strclean (summary -> text);

                           strncpy (long_title, program -> title, sizeof(long_title));
                           strclean (long_title);
                           if ((ptr = strstr (long_title, "...")) != NULL)
                             if (strlen(ptr) == 3)
                             {
                                *ptr = 0;    // Remove ...
                                if ((ptr=strstr (summary -> text, long_title)) != NULL)
                                {
                                   strncpy (long_title, ptr, sizeof(long_title));
                                   if ((ptr=strchr(long_title, '\n')) != NULL)
                                      *ptr=0;
                                   else
                                      long_title [sizeof(long_title) - 1] = 0;
                                }
                                else
                                   strncpy (long_title, program -> title, sizeof(long_title));
                             }

				  fprintf(file, "\t\t<event id=\"%.4x\">\n", program->id);

				fprintf(file, "\t\t\t<name lang=\"%s\" string=\"", lang);
				fputs_utf8(long_title, file);
				fputs("\"/>\n", file);
				fprintf(file, "\t\t\t<text lang=\"%s\" string=\"", lang);
				fputs_utf8(themes[program->theme_id], file);
				fputs("\"/>\n", file);
				fprintf(file, "\t\t\t<extended_text lang=\"%s\" string=\"", lang);
				fputs_utf8(summary->text, file);
				fputs("\"/>\n", file);
				fprintf(file, "\t\t\t<time start_time=\"%lu\" duration=\"%d\"/>\n", (u_long) program->time, program->duration);
			   	fputs("\t\t\t<content class=\"00\" user=\"00\"/>\n", file);
			  	 fprintf (file, "\t\t</event>\n");

#if 0
//FIXME ... missing
                           for (n=0; n<summary -> nb_replays; n++)
                           {
                              next_date = localtime (&summary->time[n]);
                              if (n == 0)
                                 fprintf (file,"|Rediffusions:");
                              fprintf (file,"|%02d/%02d/%4d ",next_date->tm_mday,next_date->tm_mon+1,next_date->tm_year+1900);
                              fprintf (file,"%02d:%02d",next_date->tm_hour, next_date->tm_min);
                              if (summary -> channel_id[n] < nb_channels)
	                              fprintf (file," %s",channels[summary -> channel_id[n]]);
                              if (summary -> vm[n])
                                 fprintf (file," - VM");
                              if (summary -> vo[n])
                                 fprintf (file," - VO");
                              if (summary -> subtitles[n])
                                 fprintf (file," - ST");
                              if (summary -> last_replay[n])
                                 fprintf (file," - Dernière diffusion");

#ifdef DEBUG
                              if (summary -> channel_id[n] < nb_channels)
	                              fprintf (stderr,"%s ",channels[summary -> channel_id[n]]);
                              fprintf (stderr,"%2d/%02d/%4d ",next_date->tm_mday,next_date->tm_mon+1,next_date->tm_year+1900);
                              fprintf (stderr,"%02d:%02d",next_date->tm_hour, next_date->tm_min);
                              if (summary -> vm[n])
                                 fprintf (stderr," - VM");
                              if (summary -> vo[n])
                                 fprintf (stderr," - VO");
                              if (summary -> subtitles[n])
                                 fprintf (stderr," - ST");
                                 if (summary -> last_replay[n])
                                    fprintf (stderr," - Last");
                                 fprintf (stderr,"\n");
#endif
                           }
                           fprintf (file, "\n");
#endif
                       }
                   }
#if 0
                   if (!summary_found)
                   {
			  fprintf(file, "\t\t<event id=\"%.4x\">\n", program->id);
			fprintf(file, "\t\t\t<name lang=\"%s\" string=\"", lang);
			fputs_utf8(program->title, file);
			fprintf(file, "\"/>\n\t\t\t<text lang=\"%s\" string=\"%s\"/>\n", lang, themes[program->theme_id]);
		   fputs("\t\t\t<content class=\"00\" user=\"00\"/>\n", file);
                   fprintf (file, "\t\t</event>\n");
                   }
#endif
               }
            }
         }
      }
   }
   if (file) {
	fprintf(file, "\t</service>\n</dvbepg>\n");
	fclose(file);
	file = NULL;
   }

   fprintf(indexxml, "</dvbepgfiles>\n");
   fclose(indexxml);
   iconv_close(conv_desc);
   return 0;
}

int SaveEpgData(char * filename)
{
   FILE *file;
   struct summary * summary;
   int i,j,n,equiv_index;
   struct program * program;
   struct tm *next_date;
   int first_program;
   int index;
   u_char new_channel;
   time_t current_time;
   char channel_id[50];
   char long_title[81];
   int summary_found;
   char *ptr;
   
   fprintf (stderr,"Writing EPG data to file ...\n");
   current_time = time(NULL);
   /* Start from yesterday 6:00 */
   j = yesterday*24+6;
   for (i=0; i<24*7; i++)
      if ((first_program = hour_index[(i+j) % 24*7]) != -1)
         break;
	if(debug)
            fprintf (stderr,"First program = %d\n", first_program);
   if (first_program == -1)
      return -1;	/* No program */

   if (filename[0] != 0)
	file = fopen (filename,"w");
   else
        file = stdout;
   fprintf (stderr,"Nb of channels = %d\n", nb_channels);
   for (index = 0; index < nb_channels; index++)
   {
      strcpy (channel_id, channel_ids[index]);
      for (equiv_index=-1; equiv_index<nb_equiv; equiv_index++)
      {
         if (equiv_index >= 0)
         {
	if(debug)
            fprintf (stderr, "Equiv index = %d : '%s' '%s'\n",equiv_index,channel_ids[index], channel_equiv[equiv_index][0]);
            if (strcmp(channel_ids[index], channel_equiv[equiv_index][0]))
               continue;
            else
               strcpy (channel_id, channel_equiv[equiv_index][1]);
         }
         new_channel = 1;
         for (i=0; i<nb_programs; i++)
         {
            program = programs[(first_program+i)%nb_programs];

            if (program -> channel_id == index)
            {
               if (program->time + program->duration > current_time)
               {
                  if (new_channel)
                  {
                     fprintf (file, "C %s %s\n", channel_id, channels[index]);
                     new_channel = 0;
                  }
                  fprintf (file, "E %d %d %d 90\n", i, (int)(program->time), program->duration);

	if(debug){
                  fprintf (stderr,"%s %s\n",channel_id, channels[index]);
                  fprintf (stderr,"%s", ctime(&program -> time));

                  /* Duration */
                  fprintf (stderr,"%4d' ", program -> duration / 60);
                  fprintf (stderr,"%10d ", program -> ppv);
                  fprintf (stderr,"%10d ", program -> id);
                  fprintf (stderr,"Summary%s available\n", program -> summary_available ? "" : " NOT");

                  fprintf (stderr,"%s\n",program -> title);
                  fprintf (stderr,"%s ", themes[program -> theme_id]);
	}
                  summary_found = 0;
                  if (program -> summary_available)
                  {
                     for (j=0; j<nb_summaries; j++)
                        if (program -> id == summaries[j] -> id)
                        {
                           summary_found = 1;
                           summary = summaries[j];
                           strclean (summary -> text);

                           strncpy (long_title, program -> title, sizeof(long_title));
                           strclean (long_title);
                           if ((ptr = strstr (long_title, "...")) != NULL)
                             if (strlen(ptr) == 3)
                             {
                                *ptr = 0;    // Remove ...
                                if ((ptr=strstr (summary -> text, long_title)) != NULL)
                                {
                                   strncpy (long_title, ptr, sizeof(long_title));
                                   if ((ptr=strchr(long_title, '|')) != NULL)
                                      *ptr=0;
                                   else
                                      long_title [sizeof(long_title) - 1] = 0;
                                }
                                else
                                   strncpy (long_title, program -> title, sizeof(long_title));
                             }

                           fprintf (file, "T %s\n", long_title);
                           fprintf (file, "S %s - %d'\n", themes[program -> theme_id], program -> duration / 60);

                           fprintf (file, "D %s", summary -> text);
			if(debug)
                           fprintf (stderr,"%s\n", summary -> text);
                           for (n=0; n<summary -> nb_replays; n++)
                           {
                              next_date = localtime (&summary->time[n]);
                              if (n == 0)
                                 fprintf (file,"|Rediffusions:");
                              fprintf (file,"|%02d/%02d/%4d ",next_date->tm_mday,next_date->tm_mon+1,next_date->tm_year+1900);
                              fprintf (file,"%02d:%02d",next_date->tm_hour, next_date->tm_min);
                              if (summary -> channel_id[n] < nb_channels)
	                              fprintf (file," %s",channels[summary -> channel_id[n]]);
                              if (summary -> vm[n])
                                 fprintf (file," - VM");
                              if (summary -> vo[n])
                                 fprintf (file," - VO");
                              if (summary -> subtitles[n])
                                 fprintf (file," - ST");
                              if (summary -> last_replay[n])
                                 fprintf (file," - Dernière diffusion");

			if(debug){
                              if (summary -> channel_id[n] < nb_channels)
	                              fprintf (stderr,"%s ",channels[summary -> channel_id[n]]);
                              fprintf (stderr,"%2d/%02d/%4d ",next_date->tm_mday,next_date->tm_mon+1,next_date->tm_year+1900);
                              fprintf (stderr,"%02d:%02d",next_date->tm_hour, next_date->tm_min);
                              if (summary -> vm[n])
                                 fprintf (stderr," - VM");
                              if (summary -> vo[n])
                                 fprintf (stderr," - VO");
                              if (summary -> subtitles[n])
                                 fprintf (stderr," - ST");
                                 if (summary -> last_replay[n])
                                    fprintf (stderr," - Last");
                                 fprintf (stderr,"\n");
			}
                           }
                           fprintf (file, "\n");
                       }
                   }
                   if (!summary_found)
                   {
                       fprintf (file, "T %s\n",program -> title);
                       fprintf (file, "S %s - %d'\n", themes[program -> theme_id], program -> duration / 60);
                   }
                   fprintf (file, "e\n");
               }
	if(debug)
               fprintf (stderr,"\n");

            }
         }
         if (!new_channel)
            fprintf (file, "c\n");
      }
   }
   if (filename[0] != 0)
	fclose (file);
   return 0;
}

/* Read channels equivalents */
int ReadEquiv (char *filename)
{
   FILE *file;
   char buffer [128];
   char * ptr;

   file = fopen (filename,"r");
   if (file == NULL)
   {
	fprintf (stderr, "Can't open equivalent channels file : %s\n", filename);
	return -1;
   }
   while (fgets (buffer, sizeof(buffer), file) > 0 && nb_equiv < 100)
   {
	ptr = strtok (buffer, " \t\n\r");
	strncpy (channel_equiv[nb_equiv][0], ptr, 50);
	ptr = strtok (NULL, " \t\n\r");
	strncpy (channel_equiv[nb_equiv][1], ptr, 50);
	nb_equiv++;	
   }
   fclose (file);
   fprintf (stderr, "Nb equivalents : %d\n",nb_equiv);
   return 0;
}


static char *usage_str =
    "\nusage: mhwepg [options]\n"
    "         get MediaHighway EPG data\n"
    "     -a number : use given adapter (default 0)\n"
    "     -d number : use given demux (default 0)\n"
    "     -s string : source (such as S19.2E)\n"
    "     -e string : list of equivalent channels\n"
    "     -t number : local time offset (in seconds)\n"
    "     -l        : send the channels list to stderr\n"
#ifdef NEUTRINO
    "     -n string : output to directory <string>, suitable for Neutrino\n"
#endif
    "     -o string : output to file <string> (default stdout)\n\n";

static void usage(void)
{
   fprintf(stderr, usage_str);
   exit(1);
}

int main(int argc, char *argv[])
{
   char filename [100];
   unsigned int adapter = 0, demux = 0;
   int opt;
#ifdef NEUTRINO
   int mhwepgversion = 1;
#endif
   int list_channels = 0;

   source [0] = 0;
   filename [0] = 0;
#ifdef NEUTRINO
   while ((opt = getopt(argc, argv, "?hls:o:a:d:e:t:n:12D")) != -1)
#else
   while ((opt = getopt(argc, argv, "?hls:o:a:d:e:t:")) != -1)
#endif
   {
      switch (opt)
      {
	 case '?':
	 case 'h':
	 default:
	    usage();
	 case 's':
	    strncpy(source, optarg, sizeof(source));
	    break;
	 case 'o':
	    strncpy(filename, optarg, sizeof(filename));
	    break;
#ifdef NEUTRINO
	 case 'n':
	    opt_n = 1;
	    strncpy(filename, optarg, sizeof(filename));
	    break;
	 case '1':
	 case '2':
	    mhwepgversion = opt - '0';
	    break;
	 case 'D':
	    debug = 1;
	    break;
#endif
	 case 'e':
	   if (ReadEquiv(optarg))
	       exit (1);
           break;
	 case 'a':
	    adapter = strtoul(optarg, NULL, 0);
	    break;
	 case 'd':
	    demux = strtoul(optarg, NULL, 0);
            break;
	 case 't':
	    current_offset = strtoul(optarg, NULL, 0);
            next_offset = current_offset;
            time_of_change = 0;
            time_offset_set = 1;
            break;
	 case 'l':
	    list_channels = 1;
	    break;
      }
   }
   sprintf (device, "/dev/dvb/adapter%d/demux%d", adapter, demux);

   if (GetTimeOffset())
      exit(1);

#ifdef NEUTRINO
   if (mhwepgversion == 1) {
#endif
   if (GetThemes())
      exit(1);

   if (GetChannelNames(list_channels))
      exit(1);

   if (GetTitles())
      exit(1);
   fprintf (stderr,"Nb of titles : %d\n",nb_programs);

   if (GetSummaries())
      exit(1);
#ifdef NEUTRINO
   } else {
	if (GetThemesAndChannels())
	    exit(1);

   	if (GetTitlesMHW2())
	    exit(1);
   fprintf (stderr,"Nb of titles : %d\n",nb_programs);

   	if (GetSummariesMHW2())
	    exit(1);
  }
#endif

   fprintf (stderr,"Nb of summaries : %d\n",nb_summaries);
   fprintf (stderr,"Nb of titles : %d\n",nb_programs);

#ifdef NEUTRINO
   if (opt_n) {
	if (SaveEpgDataNeutrino(filename))
	    exit(0);
   } else
#endif
   if (SaveEpgData(filename))
      exit(0);

   return 0;
}
