/******************************************************************************
 *                      <<< TuxTxt - Teletext Plugin >>>                      *
 *                                                                            *
 *             (c) Thomas "LazyT" Loewe 2002-2003 (LazyT@gmx.net)             *
 *                                                                            *
 *    continued 2004-2005 by Roland Meier <RolandMeier@Siemens.com>           *
 *                       and DBLuelle <dbluelle@blau-weissoedingen.de>        *
 *                                                                            *
 *              ported 2006 to Dreambox 7025 / 32Bit framebuffer              *
 *                   by Seddi <seddi@i-have-a-dreambox.com>                   *
 * 									      *
 *		ported 2009 to HD1 by Coolstream LTD 			      *	
 *                                                                            *
 ******************************************************************************/
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
//#include <system/set_threadname.h>
#include <unistd.h>
#include "teletext.h"
#include "tuxtxt.h"

#include <dmx_cs.h>

#include <driver/framebuffer.h>

#define KEY_TTTV	KEY_FN_1
#define KEY_TTZOOM	KEY_FN_2
#define KEY_REVEAL	KEY_FN_D

int log_level = 0;

#define debugf(i, arg...) \
	if (i < log_level) \
		fprintf(stderr, arg);


void getCurrentPIGSettings(int* left, int* top, int* width, int* height);
void getCurrentASPECTSettings();
void setCurrentPIGSettings(int left, int top, int width, int height);
void setCurrentASPECTSettings(int reset);
void getCurrentPOLICYSettings();
void setCurrentPOLICYSettings(int reset);

/*  BLITTING  */
/* current pig settings */
int left, top, width, height;
char aspect[16];
char policy[16];

// subs
static pthread_t ttx_sub_thread;
static int reader_running;
static int ttx_paused;
static int ttx_req_pause;
static int sub_pid, sub_page;
static bool use_gui;
static int cfg_national_subset;

void FillRect(int x, int y, int w, int h, int color)
{
	unsigned char *p = lfb + x*4 + y * CFrameBuffer::getInstance()->getStride();

	unsigned int col = bgra[color][3] << 24 | bgra[color][2] << 16 | bgra[color][1] << 8 | bgra[color][0];

	if (w > 0)
	{
		for (int count = 0; count < h; count++) 
		{
			unsigned int * dest0 = (unsigned int *)p;
			for (int i = 0; i < w; i++)
				*(dest0++) = col;
			p += CFrameBuffer::getInstance()->getStride();
		}
	}
}

void FillBorder(int color)
{
	int ys =  0;

	debugf(20, "%s: >\n", __func__);

	FillRect(0, ys, StartX, CFrameBuffer::getInstance()->getScreenHeight(true), color);
	FillRect(StartX, ys,displaywidth,StartY,color);
	FillRect(StartX, ys+StartY+25*fontheight,displaywidth, CFrameBuffer::getInstance()->getScreenHeight(true)-(StartY+25*fontheight), color);

	if (screenmode == 0 )
		FillRect(StartX + displaywidth, ys, CFrameBuffer::getInstance()->getScreenWidth(true) - (StartX+displaywidth),CFrameBuffer::getInstance()->getScreenHeight(true), color);

	debugf(20, "%s: <\n", __func__);
}

int getIndexOfPageInHotlist()
{
	int i;

	debugf(10, "%s: >\n", __func__);

	for (i = 0; i <= maxhotlist; i++)
	{
		if (tuxtxt_cache.page == hotlist[i])
			return i;
	}

	debugf(10, "%s: <\n", __func__);

	return -1;
}

void gethotlist()
{
	FILE *hl;
	char line[100];

	debugf(2, "%s: >\n", __func__);

	hotlistchanged = 0;
	maxhotlist = -1;
	sprintf(line, CONFIGDIR "/tuxtxt/hotlist%d.conf", tuxtxt_cache.vtxtpid);
#if TUXTXT_DEBUG
	printf("TuxTxt <gethotlist %s", line);
#endif
	if ((hl = fopen(line, "rb")) != 0)
	{
		do {
			if (!fgets(line, sizeof(line), hl))
				break;

			if (1 == sscanf(line, "%x", &hotlist[maxhotlist+1]))
			{
				if (hotlist[maxhotlist+1] >= 0x100 && hotlist[maxhotlist+1] <= 0x899)
				{
#if TUXTXT_DEBUG
					printf(" %03x", hotlist[maxhotlist+1]);
#endif
					maxhotlist++;
					continue;
				}
			}
#if TUXTXT_DEBUG
			else
				printf(" ?%s?", line);
#endif
		} while (maxhotlist < (int) (sizeof(hotlist)/sizeof(hotlist[0])-1));
		fclose(hl);
	}
#if TUXTXT_DEBUG
	printf(">\n");
#endif
	if (maxhotlist < 0) /* hotlist incorrect or not found */
	{
		hotlist[0] = 0x100; /* create one */
		hotlist[1] = 0x303;
		maxhotlist = 1;
	}

	debugf(2, "%s: <\n", __func__);
}

void savehotlist()
{
	FILE *hl;
	char line[100];
	int i;

	debugf(2, "%s: >\n", __func__);

	hotlistchanged = 0;
	sprintf(line, CONFIGDIR "/tuxtxt/hotlist%d.conf", tuxtxt_cache.vtxtpid);
#if TUXTXT_DEBUG
	printf("TuxTxt <savehotlist %s", line);
#endif
	if (maxhotlist != 1 || hotlist[0] != 0x100 || hotlist[1] != 0x303)
	{
		if ((hl = fopen(line, "wb")) != 0)
		{
			for (i = 0; i <= maxhotlist; i++)
			{
				fprintf(hl, "%03x\n", hotlist[i]);
#if TUXTXT_DEBUG
				printf(" %03x", hotlist[i]);
#endif
			}
			fclose(hl);
		}
	}
	else
	{
		unlink(line); /* remove current hotlist file */
#if TUXTXT_DEBUG
		printf(" (default - just deleted)");
#endif
	}
#if TUXTXT_DEBUG
	printf(">\n");
#endif

	debugf(2, "%s: <\n", __func__);
}

#define number2char(c) ((c) + (((c) <= 9) ? '0' : ('A' - 10)))
/* print hex-number into string, s points to last digit, caller has to provide enough space, no termination */
void hex2str(char *s, unsigned int n)
{
	debugf(20, "%s: >\n", __func__);

	do {
		char c = (n & 0xF);
		*s-- = number2char(c);
		n >>= 4;
	} while (n);

	debugf(20, "%s: <\n", __func__);
}

int toptext_getnext(int startpage, int up, int findgroup)
{
	int current, nextgrp, nextblk;

	int stoppage =  (tuxtxt_is_dec(startpage) ? startpage : startpage & 0xF00); // avoid endless loop in hexmode

	debugf(2, "%s: >\n", __func__);

	nextgrp = nextblk = 0;
	current = startpage;

	do {
		if (up)
			tuxtxt_next_dec(&current);
		else
			tuxtxt_prev_dec(&current);

		if (!tuxtxt_cache.bttok || tuxtxt_cache.basictop[current]) /* only if existent */
		{
			if (findgroup)
			{
				if (tuxtxt_cache.basictop[current] >= 6 && tuxtxt_cache.basictop[current] <= 7)
					return current;
				if (!nextgrp && (current&0x00F) == 0)
					nextgrp = current;
			}
			if (tuxtxt_cache.basictop[current] >= 2 && tuxtxt_cache.basictop[current] <= 5) /* always find block */
				return current;

			if (!nextblk && (current&0x0FF) == 0)
				nextblk = current;
		}
	} while (current != stoppage);

	debugf(2, "%s: <\n", __func__);

	if (nextgrp)
		return nextgrp;
	else if (nextblk)
		return nextblk;
	else
		return startpage;
}

void RenderClearMenuLineBB(char *p, tstPageAttr *attrcol, tstPageAttr *attr)
{
	int col;

	debugf(10, "%s: >\n", __func__);

	PosX = TOPMENUSTARTX;
	RenderCharBB(' ', attrcol); /* indicator for navigation keys */
#if 0
	RenderCharBB(' ', attr); /* separator */
#endif
	for(col = 0; col < TOPMENUCHARS; col++)
	{
		RenderCharBB(*p++, attr);
	}
	PosY += fontheight;
	memset(p-TOPMENUCHARS, ' ', TOPMENUCHARS); /* init with spaces */

	debugf(10, "%s: <\n", __func__);
}

void ClearBB(int color)
{
	debugf(20, "%s: >\n", __func__);

	CFrameBuffer::getInstance()->ClearFrameBuffer();
	
#ifdef FB_BLIT	
	CFrameBuffer::getInstance()->blit();
#endif	

	debugf(20, "%s: <\n", __func__);
}

void ClearFB(int color)
{
	debugf(20, "%s: >\n", __func__);

	CFrameBuffer::getInstance()->ClearFrameBuffer();
	
#ifdef FB_BLIT	
	CFrameBuffer::getInstance()->blit();
#endif	

	debugf(20, "%s: >\n", __func__);
}

void ClearB(int color)
{
	debugf(20, "%s: >\n", __func__);

	CFrameBuffer::getInstance()->ClearFrameBuffer();
	
#ifdef FB_BLIT	
	CFrameBuffer::getInstance()->blit();
#endif	

	debugf(20, "%s: <\n", __func__);
}

int  GetCurFontWidth()
{
	int mx = (displaywidth)%(40-nofirst); // # of unused pixels
	int abx = (mx == 0 ? displaywidth+1 : (displaywidth)/(mx+1));// distance between 'inserted' pixels
	int nx= abx+1-((PosX-sx) % (abx+1)); // # of pixels to next insert

	debugf(20, "%s: <>\n", __func__);

	return fontwidth+(((PosX+fontwidth+1-sx) <= displaywidth && nx <= fontwidth+1) ? 1 : 0);
}

void SetPosX(int column)
{
	PosX = StartX;
	int i;

	debugf(20, "%s: >\n", __func__);

	for (i = 0; i < column-nofirst; i++)
		PosX += GetCurFontWidth();

	debugf(20, "%s: <\n", __func__);
}

void setfontwidth(int newwidth)
{
	debugf(20, "%s: >\n", __func__);

	if (fontwidth != newwidth)
	{
		int i;
		fontwidth = newwidth;
		if (usettf)
			typettf.width  = (FT_UShort) fontwidth * TTFWidthFactor16 / 16;
		else
		{
			if (newwidth < 11)
				newwidth = 21;
			else if (newwidth < 14)
				newwidth = 22;
			else
				newwidth = 23;
			typettf.width  = typettf.height = (FT_UShort) newwidth;
		}
		for (i = 0; i <= 12; i++)
			axdrcs[i] = (fontwidth * i + 6) / 12;
	}

	debugf(20, "%s: <\n", __func__);
}

void setcolors(unsigned short *pcolormap, int offset, int number)
{
	int i,trans_tmp;
	int j = offset; /* index in global color table */

	debugf(20, "%s: >\n", __func__);

	trans_tmp=25-trans_mode;

	bgra[transp2][3]=((trans_tmp+7)<<11 | 0x7FF)>>8;

	for (i = 0; i < number; i++)
	{
		int r = (pcolormap[i] << 12 & 0xF000) >> 8;
		int g = (pcolormap[i] << 8 & 0xF000) >> 8;
		int b = (pcolormap[i] << 4 & 0xF000) >> 8;

		r = (r * (0x3f+(color_mode<<3))) >> 8;
		g = (g * (0x3f+(color_mode<<3))) >> 8;
		b = (b * (0x3f+(color_mode<<3))) >> 8;

		bgra[j][2]=r;
		bgra[j][1]=g;
		bgra[j][0]=b;
		
		j++;
	}

	debugf(20, "%s: <\n", __func__);
}


/* hexdump of page contents to stdout for debugging */
void dump_page()
{
	int r, c;
	char *p;
	unsigned char pagedata[23*40];

	if (!tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage])
		return;
	tuxtxt_decompress_page(tuxtxt_cache.page,tuxtxt_cache.subpage,pagedata);
	for (r=1; r < 24; r++)
	{
		p = (char *) pagedata+40*(r-1);
		for (c=0; c < 40; c++)
			printf(" %02x", *p++);
		printf("\n");
		p = (char *) page_char + 40*r;
		for (c=0; c < 40; c++)
			printf("  %c", *p++);
		printf("\n");
	}
}


/* get object data */
/* in: absolute triplet number (0..506, start at packet 3 byte 1) */
/* in: pointer to cache struct of page data */
/* out: 18 bit triplet data, <0 if invalid number, not cached, or hamming error */
int iTripletNumber2Data(int iONr, tstCachedPage *pstCachedPage, unsigned char* pagedata)
{
	if (iONr > 506 || 0 == pstCachedPage)
		return -1;

	unsigned char *p;
	int packet = (iONr / 13) + 3;
	int packetoffset = 3 * (iONr % 13);

	debugf(20, "%s: >\n", __func__);

	if (packet <= 23)
		p = pagedata + 40*(packet-1) + packetoffset + 1;
	else if (packet <= 25)
	{
		if (0 == pstCachedPage->pageinfo.p24)
			return -1;
		p = pstCachedPage->pageinfo.p24 + 40*(packet-24) + packetoffset + 1;
	}
	else
	{
		int descode = packet - 26;
		if (0 == pstCachedPage->pageinfo.ext)
			return -1;
		if (0 == pstCachedPage->pageinfo.ext->p26[descode])
			return -1;
		p = pstCachedPage->pageinfo.ext->p26[descode] + packetoffset;	/* first byte (=designation code) is not cached */
	}

	debugf(20, "%s: <\n", __func__);

	return deh24(p);
}

#define RowAddress2Row(row) ((row == 40) ? 24 : (row - 40))

/* dump interpreted object data to stdout */
/* in: 18 bit object data */
/* out: termination info, >0 if end of object */
void eval_object(int iONr, tstCachedPage *pstCachedPage,
					  unsigned char *pAPx, unsigned char *pAPy,
					  unsigned char *pAPx0, unsigned char *pAPy0,
					  tObjType ObjType, unsigned char* pagedata)
{
	int iOData;
	int iONr1 = iONr + 1; /* don't terminate after first triplet */
	unsigned char drcssubp=0, gdrcssubp=0;
	signed char endcol = -1; /* last column to which to extend attribute changes */
	tstPageAttr attrPassive = atrtable[ATR_PassiveDefault]; /* current attribute for passive objects */

	debugf(20, "%s: >\n", __func__);

	do
	{
		iOData = iTripletNumber2Data(iONr, pstCachedPage,pagedata);	/* get triplet data, next triplet */
		if (iOData < 0) /* invalid number, not cached, or hamming error: terminate */
			break;
#if TUXTXT_DEBUG
		if (dumpl25)
			printf("  t%03d ", iONr);
#endif
		if (endcol < 0)
		{
			if (ObjType == OBJ_ACTIVE)
			{
				endcol = 40;
			}
			else if (ObjType == OBJ_ADAPTIVE) /* search end of line */
			{
				int i;
				for (i = iONr; i <= 506; i++)
				{
					int iTempOData = iTripletNumber2Data(i, pstCachedPage,pagedata); /* get triplet data, next triplet */
					int iAddress = (iTempOData      ) & 0x3f;
					int iMode    = (iTempOData >>  6) & 0x1f;
					//int iData    = (iTempOData >> 11) & 0x7f;
					if (iTempOData < 0 || /* invalid number, not cached, or hamming error: terminate */
						 (iAddress >= 40	/* new row: row address and */
						 && (iMode == 0x01 || /* Full Row Color or */
							  iMode == 0x04 || /* Set Active Position */
							  (iMode >= 0x15 && iMode <= 0x17) || /* Object Definition */
							  iMode == 0x17))) /* Object Termination */
						break;
					if (iAddress < 40 && iMode != 0x06)
						endcol = iAddress;
				}
#if TUXTXT_DEBUG
				if (dumpl25)
					printf("  endcol %02d", endcol);
#endif
			}
		}
		iONr++;
	}
	while (0 == eval_triplet(iOData, pstCachedPage, pAPx, pAPy, pAPx0, pAPy0, &drcssubp, &gdrcssubp, &endcol, &attrPassive, pagedata)
			 || iONr1 == iONr); /* repeat until termination reached */

	debugf(20, "%s: <\n", __func__);
}

void eval_NumberedObject(int p, int s, int packet, int triplet, int high,
								 unsigned char *pAPx, unsigned char *pAPy,
								 unsigned char *pAPx0, unsigned char *pAPy0)
{
	if (!packet || 0 == tuxtxt_cache.astCachetable[p][s])
		return;

	unsigned char pagedata[23*40];
	tuxtxt_decompress_page(p, s,pagedata);
	int idata = deh24(pagedata + 40*(packet-1) + 1 + 3*triplet);
	int iONr;

	debugf(20, "%s: >\n", __func__);

	if (idata < 0)	/* hamming error: ignore triplet */
		return;
	if (high)
		iONr = idata >> 9; /* triplet number of odd object data */
	else
		iONr = idata & 0x1ff; /* triplet number of even object data */
	if (iONr <= 506)
	{
#if TUXTXT_DEBUG
		if (dumpl25)
			printf("P%xT%x%c %8s %c#%03d@%03d\n", packet, triplet, "LH"[!!high],	/* pointer pos, type, number, data pos */
					 ObjectType[triplet % 3], "PCD"[triplet % 3], 8*packet + 2*(triplet-1)/3, iONr);

#endif
		eval_object(iONr, tuxtxt_cache.astCachetable[p][s], pAPx, pAPy, pAPx0, pAPy0, (tObjType)(triplet % 3),pagedata);
	}

	debugf(20, "%s: <\n", __func__);
}

int eval_triplet(int iOData, tstCachedPage *pstCachedPage,
					  unsigned char *pAPx, unsigned char *pAPy,
					  unsigned char *pAPx0, unsigned char *pAPy0,
					  unsigned char *drcssubp, unsigned char *gdrcssubp,
					  signed char *endcol, tstPageAttr *attrPassive, unsigned char* pagedata)
{
	int iAddress = (iOData      ) & 0x3f;
	int iMode    = (iOData >>  6) & 0x1f;
	int iData    = (iOData >> 11) & 0x7f;

	debugf(20, "%s: >\n", __func__);

	if (iAddress < 40) /* column addresses */
	{
		int offset;	/* offset to page_char and page_atrb */

		if (iMode != 0x06)
			*pAPx = iAddress;	/* new Active Column */
		offset = (*pAPy0 + *pAPy) * 40 + *pAPx0 + *pAPx;	/* offset to page_char and page_atrb */
#if TUXTXT_DEBUG
		if (dumpl25)
			printf("  M%02xC%02xD%02x %d r:%d ch:%02x", iMode, iAddress, iData, *endcol,*pAPy0 + *pAPy,page_char[offset]);
#endif

		switch (iMode)
		{
		case 0x00:
			if (0 == (iData>>5))
			{
				int newcolor = iData & 0x1f;
				if (*endcol < 0) /* passive object */
					attrPassive->fg = newcolor;
				else if (*endcol == 40) /* active object */
				{
					tstPageAttr *p = &page_atrb[offset];
					int oldcolor = (p)->fg; /* current color (set-after) */
					int c = *pAPx0 + *pAPx;	/* current column absolute */
					do
					{
						p->fg = newcolor;
						p++;
						c++;
					} while (c < 40 && p->fg == oldcolor);	/* stop at change by level 1 page */
				}
				else /* adaptive object */
				{
					tstPageAttr *p = &page_atrb[offset];
					int c = *pAPx;	/* current column relative to object origin */
					do
					{
						p->fg = newcolor;
						p++;
						c++;
					} while (c <= *endcol);
				}
#if TUXTXT_DEBUG
				if (dumpl25)
					printf("  ,%02d FGCol T%x#%x", iAddress, (iData>>3)&0x03, iData&0x07);
#endif
			}
			break;
		case 0x01:
			if (iData >= 0x20)
			{
#if TUXTXT_DEBUG
				if (dumpl25)
					printf("  ,%02d BlockMosaic G1 #%02x", iAddress, iData);
#endif
				page_char[offset] = iData;
				if (*endcol < 0) /* passive object */
				{
					attrPassive->charset = C_G1C; /* FIXME: separated? */
					page_atrb[offset] = *attrPassive;
				}
				else if (page_atrb[offset].charset != C_G1S)
					page_atrb[offset].charset = C_G1C; /* FIXME: separated? */
			}
			break;
		case 0x02:
		case 0x0b:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d G3 #%02x f%db%d", iAddress, iData,attrPassive->fg, attrPassive->bg);
#endif
			page_char[offset] = iData;
			if (*endcol < 0) /* passive object */
			{
				attrPassive->charset = C_G3;
				page_atrb[offset] = *attrPassive;
			}
			else
				page_atrb[offset].charset = C_G3;
			break;
		case 0x03:
			if (0 == (iData>>5))
			{
				int newcolor = iData & 0x1f;
				if (*endcol < 0) /* passive object */
					attrPassive->bg = newcolor;
				else if (*endcol == 40) /* active object */
				{
					tstPageAttr *p = &page_atrb[offset];
					int oldcolor = (p)->bg; /* current color (set-after) */
					int c = *pAPx0 + *pAPx;	/* current column absolute */
					do
					{
						p->bg = newcolor;
						if (newcolor == black)
							p->IgnoreAtBlackBgSubst = 1;
						p++;
						c++;
					} while (c < 40 && p->bg == oldcolor);	/* stop at change by level 1 page */
				}
				else /* adaptive object */
				{
					tstPageAttr *p = &page_atrb[offset];
					int c = *pAPx;	/* current column relative to object origin */
					do
					{
						p->bg = newcolor;
						if (newcolor == black)
							p->IgnoreAtBlackBgSubst = 1;
						p++;
						c++;
					} while (c <= *endcol);
				}
#if TUXTXT_DEBUG
				if (dumpl25)
					printf("  ,%02d BGCol T%x#%x", iAddress, (iData>>3)&0x03, iData&0x07);
#endif
			}
			break;
		case 0x06:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  PDC");
#endif
			/* ignore */
			break;
		case 0x07:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d Flash M%xP%x", iAddress, iData & 0x03, (iData >> 2) & 0x07);
#endif
			if ((iData & 0x60) != 0) break; // reserved data field
			if (*endcol < 0) /* passive object */
			{
				attrPassive->flashing=iData & 0x1f;
				page_atrb[offset] = *attrPassive;
			}
			else
				page_atrb[offset].flashing=iData & 0x1f;
			break;
		case 0x08:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d G0+G2 set #%02x (p105)", iAddress, iData);
#endif
			if (*endcol < 0) /* passive object */
			{
				attrPassive->setG0G2=iData & 0x3f;
				page_atrb[offset] = *attrPassive;
			}
			else
				page_atrb[offset].setG0G2=iData & 0x3f;
			break;
		case 0x09:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d G0 #%02x '%c'", iAddress, iData, iData);
#endif
			page_char[offset] = iData;
			if (*endcol < 0) /* passive object */
			{
				attrPassive->charset = C_G0P; /* FIXME: secondary? */
				attrPassive->setX26  = 1;
				page_atrb[offset] = *attrPassive;
			}
			else
			{
				page_atrb[offset].charset = C_G0P; /* FIXME: secondary? */
				page_atrb[offset].setX26  = 1;
			}
			break;
//		case 0x0b: (see 0x02)
		case 0x0c:
		{
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d Attribute%s%s%s%s%s%s", iAddress,
						 (iData & 0x40) ? " DoubleWidth" : "",
						 (iData & 0x20) ? " UnderlineSep" : "",
						 (iData & 0x10) ? " InvColour" : "",
						 (iData & 0x04) ? " Conceal" : "",
						 (iData & 0x02) ? " Boxing" : "",
						 (iData & 0x01) ? " DoubleHeight" : "");
#endif
			int conc = (iData & 0x04);
			int inv  = (iData & 0x10);
			int dw   = (iData & 0x40 ?1:0);
			int dh   = (iData & 0x01 ?1:0);
			int sep  = (iData & 0x20);
			int bw   = (iData & 0x02 ?1:0);
			if (*endcol < 0) /* passive object */
			{
				if (conc)
				{
					attrPassive->concealed = 1;
					attrPassive->fg = attrPassive->bg;
				}
				attrPassive->inverted = (inv ? 1- attrPassive->inverted : 0);
				attrPassive->doubleh = dh;
				attrPassive->doublew = dw;
				attrPassive->boxwin = bw;
				if (bw) attrPassive->IgnoreAtBlackBgSubst = 0;
				if (sep)
				{
					if (attrPassive->charset == C_G1C)
						attrPassive->charset = C_G1S;
					else
						attrPassive->underline = 1;
				}
				else
				{
					if (attrPassive->charset == C_G1S)
						attrPassive->charset = C_G1C;
					else
						attrPassive->underline = 0;
				}
			}
			else
			{

				int c = *pAPx0 + (*endcol == 40 ? *pAPx : 0);	/* current column */
				int c1 = offset;
				tstPageAttr *p = &page_atrb[offset];
				do
				{
					p->inverted = (inv ? 1- p->inverted : 0);
					if (conc)
					{
						p->concealed = 1;
						p->fg = p->bg;
					}
					if (sep)
					{
						if (p->charset == C_G1C)
							p->charset = C_G1S;
						else
							p->underline = 1;
					}
					else
					{
						if (p->charset == C_G1S)
							p->charset = C_G1C;
						else
							p->underline = 0;
					}
					p->doublew = dw;
					p->doubleh = dh;
					p->boxwin = bw;
					if (bw) p->IgnoreAtBlackBgSubst = 0;
					p++;
					c++;
					c1++;
				} while (c < *endcol);
			}
			break;
		}
		case 0x0d:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d %cDRCS #%02x", iAddress, (iData & 0x40) ? ' ' : 'G', iData & 0x3f);
#endif
			page_char[offset] = iData & 0x3f;
			if (*endcol < 0) /* passive object */
			{
				attrPassive->charset = C_OFFSET_DRCS + ((iData & 0x40) ? (0x10 + *drcssubp) : *gdrcssubp);
				page_atrb[offset] = *attrPassive;
			}
			else
				page_atrb[offset].charset = C_OFFSET_DRCS + ((iData & 0x40) ? (0x10 + *drcssubp) : *gdrcssubp);
			break;
		case 0x0f:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  ,%02d G2 #%02x", iAddress, iData);
#endif
			page_char[offset] = iData;
			if (*endcol < 0) /* passive object */
			{
				attrPassive->charset = C_G2;
				page_atrb[offset] = *attrPassive;
			}
			else
				page_atrb[offset].charset = C_G2;
			break;
		default:
			if (iMode == 0x10 && iData == 0x2a)
				iData = '@';
			if (iMode >= 0x10)
			{
#if TUXTXT_DEBUG
				if (dumpl25)
					printf("  ,%02d G0 #%02x %c +diacr. %x", iAddress, iData, iData, iMode & 0x0f);
#endif
				page_char[offset] = iData;
				if (*endcol < 0) /* passive object */
				{
					attrPassive->charset = C_G0P;
					attrPassive->diacrit = iMode & 0x0f;
					attrPassive->setX26  = 1;
					page_atrb[offset] = *attrPassive;
				}
				else
				{
					page_atrb[offset].charset = C_G0P;
					page_atrb[offset].diacrit = iMode & 0x0f;
					page_atrb[offset].setX26  = 1;
				}
			}
			break; /* unsupported or not yet implemented mode: ignore */
		} /* switch (iMode) */
	}
	else /* ================= (iAddress >= 40): row addresses ====================== */
	{
#if TUXTXT_DEBUG
		if (dumpl25)
			printf("  M%02xR%02xD%02x", iMode, iAddress, iData);
#endif
		switch (iMode)
		{
		case 0x00:
			if (0 == (iData>>5))
			{
#if TUXTXT_DEBUG
				if (dumpl25)
					printf("  FScrCol T%x#%x", (iData>>3)&0x03, iData&0x07);
#endif
				FullScrColor = iData & 0x1f;
			}
			break;
		case 0x01:
			if (*endcol == 40) /* active object */
			{
				*pAPy = RowAddress2Row(iAddress);	/* new Active Row */

				int color = iData & 0x1f;
				int row = *pAPy0 + *pAPy;
				int maxrow;
#if TUXTXT_DEBUG
				if (dumpl25)
				{
					printf("  AP=%d,0", RowAddress2Row(iAddress));
					if (0 == (iData>>5))
						printf("  FRowCol T%x#%x", (iData>>3)&0x03, iData&0x07);
					else if (3 == (iData>>5))
						printf("  FRowCol++ T%x#%x", (iData>>3)&0x03, iData&0x07);
				}
#endif
				if (row <= 24 && 0 == (iData>>5))
					maxrow = row;
				else if (3 == (iData>>5))
					maxrow = 24;
				else
					maxrow = -1;
				for (; row <= maxrow; row++)
					FullRowColor[row] = color;
				*endcol = -1;
			}
			break;
		case 0x04:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf(" AP=%d,%d", RowAddress2Row(iAddress), iData);
#endif
			*pAPy = RowAddress2Row(iAddress); /* new Active Row */
			if (iData < 40)
				*pAPx = iData;	/* new Active Column */
			*endcol = -1; /* FIXME: check if row changed? */
			break;
		case 0x07:
#if TUXTXT_DEBUG
			if (dumpl25)
			{
				if (iAddress == 0x3f)
					printf("  AP=0,0");
				if (0 == (iData>>5))
					printf("  Address Display R0 FRowCol T%x#%x", (iData>>3)&0x03, iData&0x07);
				else if (3 == (iData>>5))
					printf("  Address Display R0->24 FRowCol T%x#%x", (iData>>3)&0x03, iData&0x07);
			}
#endif
			if (iAddress == 0x3f)
			{
				*pAPx = *pAPy = 0; /* new Active Position 0,0 */
				if (*endcol == 40) /* active object */
				{
					int color = iData & 0x1f;
					int row = *pAPy0; // + *pAPy;
					int maxrow;

					if (row <= 24 && 0 == (iData>>5))
						maxrow = row;
					else if (3 == (iData>>5))
						maxrow = 24;
					else
						maxrow = -1;
					for (; row <= maxrow; row++)
						FullRowColor[row] = color;
				}
				*endcol = -1;
			}
			break;
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
		case 0x0e:
		case 0x0f:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  PDC");
#endif
			/* ignore */
			break;
		case 0x10:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  AP=%d,%d  temp. Origin Modifier", iAddress - 40, iData);
#endif
			tAPy = iAddress - 40;
			tAPx = iData;
			break;
		case 0x11:
		case 0x12:
		case 0x13:
			if (iAddress & 0x10)	/* POP or GPOP */
			{
				unsigned char APx = 0, APy = 0;
				unsigned char APx0 = *pAPx0 + *pAPx + tAPx, APy0 = *pAPy0 + *pAPy + tAPy;
				int triplet = 3 * ((iData >> 5) & 0x03) + (iMode & 0x03);
				int packet = (iAddress & 0x03) + 1;
				int subp = iData & 0x0f;
				int high = (iData >> 4) & 0x01;


				if (APx0 < 40) /* not in side panel */
				{
#if TUXTXT_DEBUG
					if (dumpl25)
						printf("  Object Invocation %5s %8s S%xP%xT%x%c %c#%03d\n---",
								 ObjectSource[(iAddress >> 3) & 0x03], ObjectType[iMode & 0x03],
								 subp,
							 packet,
								 triplet,
								 "LH"[high], /* low/high */
								 "PCD"[triplet % 3],
								 8*packet + 2*(triplet-1)/3 + 1);
#endif
					eval_NumberedObject((iAddress & 0x08) ? gpop : pop, subp, packet, triplet, high, &APx, &APy, &APx0, &APy0);
#if TUXTXT_DEBUG
					if (dumpl25)
						printf("---");
#endif
				}
#if TUXTXT_DEBUG
				else if (dumpl25)
					printf("Object Invocation for Side Panel - ignored");
#endif
			}
			else if (iAddress & 0x08)	/* local: eval invoked object */
			{
				unsigned char APx = 0, APy = 0;
				unsigned char APx0 = *pAPx0 + *pAPx + tAPx, APy0 = *pAPy0 + *pAPy + tAPy;
				int descode = ((iAddress & 0x01) << 3) | (iData >> 4);
				int triplet = iData & 0x0f;

				if (APx0 < 40) /* not in side panel */
				{
#if TUXTXT_DEBUG
					if (dumpl25)
						printf("  local Object Invocation %5s %8s D%xT%x:\n---",
								 ObjectSource[(iAddress >> 3) & 0x03], ObjectType[iMode & 0x03], descode, triplet);
#endif
					eval_object(13 * 23 + 13 * descode + triplet, pstCachedPage, &APx, &APy, &APx0, &APy0, (tObjType)(triplet % 3), pagedata);
#if TUXTXT_DEBUG
					if (dumpl25)
						printf("---");
#endif
				}
#if TUXTXT_DEBUG
				else if (dumpl25)
					printf("local Object Invocation for Side Panel - ignored");
#endif
			}
			break;
		case 0x15:
		case 0x16:
		case 0x17:
			if (0 == (iAddress & 0x08))	/* Object Definition illegal or only level 3.5 */
				break; /* ignore */
#if TUXTXT_DEBUG
			if (dumpl25)
			{
				printf("  Object Definition %8s", ObjectType[iMode & 0x03]);
				{ /* *POP */
					int triplet = 3 * ((iData >> 5) & 0x03) + (iMode & 0x03);
					int packet = (iAddress & 0x03) + 1;
					printf("  S%xP%xT%x%c %c#%03d",
							 iData & 0x0f,	/* subpage */
							 packet,
							 triplet,
							 "LH"[(iData >> 4) & 0x01], /* low/high */
							 "PCD"[triplet % 3],
							 8*packet + 2*(triplet-1)/3 + 1);
				}
				{ /* local */
					int descode = ((iAddress & 0x03) << 3) | (iData >> 4);
					int triplet = iData & 0x0f;
					printf("  D%xT%x", descode, triplet);
				}
				putchar('\n');
			}
#endif
			tAPx = tAPy = 0;
			*endcol = -1;
			return 0xFF; /* termination by object definition */
			break;
		case 0x18:
			if (0 == (iData & 0x10)) /* DRCS Mode reserved or only level 3.5 */
				break; /* ignore */
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  %cDRCS S%x", (iData & 0x40) ? ' ' : 'G', iData & 0x0f);	/* subpage */
#endif
			if (iData & 0x40)
				*drcssubp = iData & 0x0f;
			else
				*gdrcssubp = iData & 0x0f;
			break;
		case 0x1f:
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("  Termination Marker %x\n", iData);	/* subpage */
#endif
			tAPx = tAPy = 0;
			*endcol = -1;
			return 0x80 | iData; /* explicit termination */
			break;
		default:
			break; /* unsupported or not yet implemented mode: ignore */
		} /* switch (iMode) */
	} /* (iAddress >= 40): row addresses */
#if TUXTXT_DEBUG
	if (dumpl25 && iAddress < 40)
		putchar('\n');
#endif
	if (iAddress < 40 || iMode != 0x10) /* leave temp. AP-Offset unchanged only immediately after definition */
		tAPx = tAPy = 0;


	debugf(20, "%s: <\n", __func__);

	return 0; /* normal exit, no termination */
}

int setnational(unsigned char sec)
{

	debugf(20, "%s: >\n", __func__);

	switch (sec)
	{
		case 0x08:
			return NAT_PL; //polish
		case 0x16:
		case 0x36:
			return NAT_TR; //turkish
		case 0x1d:
			return NAT_SR; //serbian, croatian, slovenian
		case 0x20:
		case 0x24:
		case 0x25:
			return NAT_RU; // cyrillic
		case 0x22:
			return NAT_ET; // estonian
		case 0x23:
			return NAT_LV; // latvian, lithuanian
		case 0x37:
			return NAT_GR; // greek
		case 0x47:
		case 0x57:
			// TODO : arabic
			break;
		case 0x55:
			// TODO : hebrew
			break;

	}

	debugf(20, "%s: <\n", __func__);

	return countryconversiontable[sec & 0x07];
}

/* evaluate level 2.5 information */
void eval_l25()
{
	debugf(20, "%s: >\n", __func__);

	memset(FullRowColor, 0, sizeof(FullRowColor));
	
	FullScrColor = black;

	if (!tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage])
		return;

	/* normal page */
	if (tuxtxt_is_dec(tuxtxt_cache.page))
	{
		unsigned char APx0, APy0, APx, APy;
		tstPageinfo *pi = &(tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage]->pageinfo);
		tstCachedPage *pmot = tuxtxt_cache.astCachetable[(tuxtxt_cache.page & 0xf00) | 0xfe][0];
		unsigned short *colortable = 0;
		int p26Received = 0;
		int BlackBgSubst = 0;
		int ColorTableRemapping = 0;

		pop = gpop = drcs = gdrcs = 0;

		if (pi->ext)
		{
			tstExtData *e = pi->ext;

			if (e->p26[0])
				p26Received = 1;

			if (e->p27)
			{
				tstp27 *p27 = e->p27;
				if (p27[0].l25)
					gpop = p27[0].page;
				if (p27[1].l25)
					pop = p27[1].page;
				if (p27[2].l25)
					gdrcs = p27[2].page;
				if (p27[3].l25)
					drcs = p27[3].page;
			}

			if (e->p28Received)
			{
				colortable = e->bgr;
				BlackBgSubst = e->BlackBgSubst;
				ColorTableRemapping = e->ColorTableRemapping;
				memset(FullRowColor, e->DefRowColor, sizeof(FullRowColor));
				FullScrColor = e->DefScreenColor;
				national_subset = setnational(e->DefaultCharset);
				national_subset_secondary = setnational(e->SecondCharset);
#if TUXTXT_DEBUG
				if (dumpl25)
				{
					int c; /* color */
					printf("p28/0: DefCharset %02x Sec %02x SidePanel %c%c%x DefScrCol %02x DefRowCol %02x BlBgSubst %x Map %x\n CBGR",
							 e->DefaultCharset,
							 e->SecondCharset,
							 e->LSP ? (e->SPL25 ? 'L' : 'l') : '-',	/* left panel (small: only in level 3.5) */
							 e->RSP ? (e->SPL25 ? 'R' : 'r') : '-',	/* right panel (small: only in level 3.5) */
							 e->LSPColumns,
							 e->DefScreenColor,
							 e->DefRowColor,
							 e->BlackBgSubst,
							 e->ColorTableRemapping);
					for (c = 0; c < 16; c++)
						printf(" %x%03x", c, e->bgr[c]);
					putchar('\n');
				}
#endif
			} /* e->p28Received */
		}

		if (!colortable && tuxtxt_cache.astP29[tuxtxt_cache.page >> 8])
		{
			tstExtData *e = tuxtxt_cache.astP29[tuxtxt_cache.page >> 8];
			colortable = e->bgr;
			BlackBgSubst = e->BlackBgSubst;
			ColorTableRemapping = e->ColorTableRemapping;
			memset(FullRowColor, e->DefRowColor, sizeof(FullRowColor));
			FullScrColor = e->DefScreenColor;
			national_subset = setnational(e->DefaultCharset);
			national_subset_secondary = setnational(e->SecondCharset);
#if TUXTXT_DEBUG
			if (dumpl25)
			{
				int c; /* color */
				printf("p29/0: DefCharset %02x Sec %02x SidePanel %c%c%x DefScrCol %02x DefRowCol %02x BlBgSubst %x Map %x\n CBGR",
						 e->DefaultCharset,
						 e->SecondCharset,
						 e->LSP ? (e->SPL25 ? 'L' : 'l') : '-',	/* left panel (small: only in level 3.5) */
						 e->RSP ? (e->SPL25 ? 'R' : 'r') : '-',	/* right panel (small: only in level 3.5) */
						 e->LSPColumns,
						 e->DefScreenColor,
						 e->DefRowColor,
						 e->BlackBgSubst,
						 e->ColorTableRemapping);
				for (c = 0; c < 16; c++)
					printf(" %x%03x", c, e->bgr[c]);
				putchar('\n');
			}
#endif

		}

		if (ColorTableRemapping)
		{
			int i;
			for (i = 0; i < 25*40; i++)
			{
				page_atrb[i].fg += MapTblFG[ColorTableRemapping - 1];
				if (!BlackBgSubst || page_atrb[i].bg != black || page_atrb[i].IgnoreAtBlackBgSubst)
					page_atrb[i].bg += MapTblBG[ColorTableRemapping - 1];
			}
		}

		/* determine ?pop/?drcs from MOT */
		if (pmot)
		{
			unsigned char pmot_data[23*40];
			tuxtxt_decompress_page((tuxtxt_cache.page & 0xf00) | 0xfe,0,pmot_data);

			unsigned char *p = pmot_data; /* start of link data */
			int o = 2 * (((tuxtxt_cache.page & 0xf0) >> 4) * 10 + (tuxtxt_cache.page & 0x0f));	/* offset of links for current page */
			int opop = p[o] & 0x07;	/* index of POP link */
			int odrcs = p[o+1] & 0x07;	/* index of DRCS link */
			unsigned char obj[3*4*4]; // types* objects * (triplet,packet,subp,high)
			unsigned char type,ct, tstart = 4*4;
			memset(obj,0,sizeof(obj));


			if (p[o] & 0x08) /* GPOP data used */
			{
				if (!gpop || !(p[18*40] & 0x08)) /* no p27 data or higher prio of MOT link */
				{
					gpop = ((p[18*40] << 8) | (p[18*40+1] << 4) | p[18*40+2]) & 0x7ff;
					if ((gpop & 0xff) == 0xff)
						gpop = 0;
					else
					{
						if (gpop < 0x100)
							gpop += 0x800;
						if (!p26Received)
						{
							ct=2;
							while (ct)
							{
								ct--;
								type = (p[18*40+5] >> 2*ct) & 0x03;

								if (type == 0) continue;
							    obj[(type-1)*(tstart)+ct*4  ] = 3 * ((p[18*40+7+ct*2] >> 1) & 0x03) + type; //triplet
							    obj[(type-1)*(tstart)+ct*4+1] = ((p[18*40+7+ct*2] & 0x08) >> 3) + 1       ; //packet
							    obj[(type-1)*(tstart)+ct*4+2] = p[18*40+6+ct*2] & 0x0f                    ; //subp
							    obj[(type-1)*(tstart)+ct*4+3] = p[18*40+7+ct*2] & 0x01                    ; //high

#if TUXTXT_DEBUG
								if (dumpl25)
									printf("GPOP  DefObj%d S%xP%xT%x%c %c#%03d\n"
										,2-ct
										, obj[(type-1)*(tstart)+ct*4+2]
										, obj[(type-1)*(tstart)+ct*4+1]
										, obj[(type-1)*(tstart)+ct*4]
										, "LH"[obj[(type-1)*(tstart)+ct*4+3]]
										, "-CDP"[type]
										, 8*(obj[(type-1)*(tstart)+ct*4+1]-1) + 2*(obj[(type-1)*(tstart)+ct*4]-1)/3 + 1);
#endif
							}
						}
					}
				}
			}

			if (opop) /* POP data used */
			{
				opop = 18*40 + 10*opop;	/* offset to POP link */
				if (!pop || !(p[opop] & 0x08)) /* no p27 data or higher prio of MOT link */
				{
					pop = ((p[opop] << 8) | (p[opop+1] << 4) | p[opop+2]) & 0x7ff;
					if ((pop & 0xff) == 0xff)
						pop = 0;
					else
					{
						if (pop < 0x100)
							pop += 0x800;
						if (!p26Received)
						{
							ct=2;
							while (ct)
							{
								ct--;
								type = (p[opop+5] >> 2*ct) & 0x03;

								if (type == 0) continue;
							    obj[(type-1)*(tstart)+(ct+2)*4  ] = 3 * ((p[opop+7+ct*2] >> 1) & 0x03) + type; //triplet
							    obj[(type-1)*(tstart)+(ct+2)*4+1] = ((p[opop+7+ct*2] & 0x08) >> 3) + 1       ; //packet
							    obj[(type-1)*(tstart)+(ct+2)*4+2] = p[opop+6+ct*2]                           ; //subp
							    obj[(type-1)*(tstart)+(ct+2)*4+3] = p[opop+7+ct*2] & 0x01                    ; //high
#if TUXTXT_DEBUG
								if (dumpl25)
									printf("POP  DefObj%d S%xP%xT%x%c %c#%03d\n"
										, 2-ct
										, obj[(type-1)*(tstart)+(ct+2)*4+2]
										, obj[(type-1)*(tstart)+(ct+2)*4+1]
										, obj[(type-1)*(tstart)+(ct+2)*4]
										, "LH"[obj[(type-1)*(tstart)+(ct+2)*4+3]]
										, "-CDP"[type], 8*(obj[(type-1)*(tstart)+(ct+2)*4+1]-1) + 2*(obj[(type-1)*(tstart)+(ct+2)*4]-1)/3 + 1);
#endif
							}
						}
					}
				}
			}
			// eval default objects in correct order
			for (ct = 0; ct < 12; ct++)
			{
#if TUXTXT_DEBUG
								if (dumpl25)
									printf("eval  DefObjs : %d S%xP%xT%x%c %c#%03d\n"
										, ct
										, obj[ct*4+2]
										, obj[ct*4+1]
										, obj[ct*4]
										, "LH"[obj[ct*4+3]]
										, "-CDP"[obj[ct*4 % 3]]
										, 8*(obj[ct*4+1]-1) + 2*(obj[ct*4]-1)/3 + 1);
#endif
				if (obj[ct*4] != 0)
				{
					APx0 = APy0 = APx = APy = tAPx = tAPy = 0;
					eval_NumberedObject(ct % 4 > 1 ? pop : gpop, obj[ct*4+2], obj[ct*4+1], obj[ct*4], obj[ct*4+3], &APx, &APy, &APx0, &APy0);
				}
			}

			if (p[o+1] & 0x08) /* GDRCS data used */
			{
				if (!gdrcs || !(p[20*40] & 0x08)) /* no p27 data or higher prio of MOT link */
				{
					gdrcs = ((p[20*40] << 8) | (p[20*40+1] << 4) | p[20*40+2]) & 0x7ff;
					if ((gdrcs & 0xff) == 0xff)
						gdrcs = 0;
					else if (gdrcs < 0x100)
						gdrcs += 0x800;
				}
			}
			if (odrcs) /* DRCS data used */
			{
				odrcs = 20*40 + 4*odrcs;	/* offset to DRCS link */
				if (!drcs || !(p[odrcs] & 0x08)) /* no p27 data or higher prio of MOT link */
				{
					drcs = ((p[odrcs] << 8) | (p[odrcs+1] << 4) | p[odrcs+2]) & 0x7ff;
					if ((drcs & 0xff) == 0xff)
						drcs = 0;
					else if (drcs < 0x100)
						drcs += 0x800;
				}
			}
			if (tuxtxt_cache.astCachetable[gpop][0])
				tuxtxt_cache.astCachetable[gpop][0]->pageinfo.function = FUNC_GPOP;
			if (tuxtxt_cache.astCachetable[pop][0])
				tuxtxt_cache.astCachetable[pop][0]->pageinfo.function = FUNC_POP;
			if (tuxtxt_cache.astCachetable[gdrcs][0])
				tuxtxt_cache.astCachetable[gdrcs][0]->pageinfo.function = FUNC_GDRCS;
			if (tuxtxt_cache.astCachetable[drcs][0])
				tuxtxt_cache.astCachetable[drcs][0]->pageinfo.function = FUNC_DRCS;
		} /* if mot */

#if TUXTXT_DEBUG
		if (dumpl25)
			printf("gpop %03x pop %03x gdrcs %03x drcs %03x p28/0: Func %x Natvalid %x Nat %x Box %x\n",
					 gpop, pop, gdrcs, drcs,
					 pi->function, pi->nationalvalid, pi->national, pi->boxed);
#endif

		/* evaluate local extension data from p26 */
		if (p26Received)
		{
#if TUXTXT_DEBUG
			if (dumpl25)
				printf("p26/x:\n");
#endif
			APx0 = APy0 = APx = APy = tAPx = tAPy = 0;
			eval_object(13 * (23-2 + 2), tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage], &APx, &APy, &APx0, &APy0, OBJ_ACTIVE, &page_char[40]); /* 1st triplet p26/0 */
		}

		{
			int r, c;
			int o = 0;


			for (r = 0; r < 25; r++)
				for (c = 0; c < 39; c++)
				{
					if (BlackBgSubst && page_atrb[o].bg == black && !(page_atrb[o].IgnoreAtBlackBgSubst))
					{
						if (FullRowColor[r] == 0x08)
							page_atrb[o].bg = FullScrColor;
						else
							page_atrb[o].bg = FullRowColor[r];
					}
					o++;
				}
		}

		if (!hintmode)
		{
			int i;
			for (i = 0; i < 25*40; i++)
			{
				if (page_atrb[i].concealed) page_atrb[i].fg = page_atrb[i].bg;
			}
		}

		if (boxed || transpmode)
			//FullScrColor = transp;
			FillBorder(transp);
		//else
			//FillBorder(FullScrColor);
		else if(use_gui) 
		{
			FillBorder(FullScrColor);
		}
		
		if (colortable) /* as late as possible to shorten the time the old page is displayed with the new colors */
			setcolors(colortable, 16, 16); /* set colors for CLUTs 2+3 */
	} /* is_dec(page) */

	debugf(20, "%s: <\n", __func__);
}

/*
 * main loop
*/
//TEST
#if 1
static void cleanup_fb_pan()
{
#ifdef USE_FBPAN
	if (var_screeninfo.yoffset)
	{
		var_screeninfo.yoffset = 0;
		if (ioctl(fb, FBIOPAN_DISPLAY, &var_screeninfo) == -1)
			perror("TuxTxt <FBIOPAN_DISPLAY>");
	}
#endif
}

static void * reader_thread(void * /*arg*/)
{
	printf("TuxTxt subtitle thread started\n");
//	set_threadname("ttx_reader_thread");
	reader_running = 1;
	
	//ttx_paused = 0;
	while(reader_running) 
	{
		if(ttx_paused)
			usleep(10);
		else
			RenderPage();
#ifdef FB_BLIT
		CFrameBuffer::getInstance()->blit();
#endif
		if(ttx_req_pause) 
		{
			ttx_req_pause = 0;
			ttx_paused = 1;
		}
	}
	
	if(!ttx_paused)
		CleanUp();
	cleanup_fb_pan();
	tuxtxt_close();
	printf("TuxTxt subtitle thread stopped\n");
	pthread_exit(NULL);
}

void tuxtx_pause_subtitle(bool pause)
{
	if(!pause) {
		printf("TuxTxt subtitle unpause, running %d pid %d page %d\n", reader_running, sub_pid, sub_page);
		ttx_paused = 0;
		if(!reader_running && sub_pid && sub_page)
			tuxtx_main(0, sub_pid, sub_page);
	}
	else 
	{
		if(!reader_running)
			return;
		
		printf("TuxTxt subtitle asking to pause...\n");
		ttx_req_pause = 1;
		while(!ttx_paused)
			usleep(10);
		printf("TuxTxt subtitle paused\n");
		cleanup_fb_pan();
	}
}

void tuxtx_stop_subtitle()
{
	printf("TuxTxt stopping subtitle thread ...\n");
	reader_running = 0;
	if(ttx_sub_thread)
		pthread_join(ttx_sub_thread, NULL);
	ttx_sub_thread = 0;
	sub_pid = sub_page = 0;
	ttx_paused = 0;
}

void tuxtx_set_pid(int pid, int page, const char * cc)
{
	if(reader_running)
		tuxtx_stop_subtitle();

	sub_pid = pid;
	sub_page = page;

	cfg_national_subset = GetNationalSubset(cc);
	printf("TuxTxt subtitle set pid %d page %d lang %s (%d)\n", sub_pid, sub_page, cc, cfg_national_subset);
#if 0
	ttx_paused = 1;
	if(sub_pid && sub_page)
		tuxtx_main(0, sub_pid, sub_page);
#endif
}

int tuxtx_subtitle_running(int *pid, int *page, int *running)
{
	int ret = 0;

	if(running)
		*running = reader_running;

	if(reader_running && (tuxtxt_cache.vtxtpid == *pid) && (tuxtxt_cache.page == *page))
	{
		ret = 1;
	}
	*pid = sub_pid;
	*page = sub_page;

	return ret;
}
#endif
//
static int ttx_repeat_blocker = 125000;
static int ttx_repeat_genericblocker = 250000;
int tuxtx_main(int _rc, int pid, int page, int source, int repeat_blocker, int repeat_genericblocker)
{
	char cvs_revision[] = "$Revision: 1.95 $";
	ttx_repeat_blocker = repeat_blocker * 2000;
	ttx_repeat_genericblocker = repeat_genericblocker * 2000;
	debugf(1, "%s: >\n", __func__);
	
	use_gui = 1;
	boxed = 0;

	// get pig/aspect/policy settings
	getCurrentPIGSettings(&left, &top, &width, &height);
        getCurrentASPECTSettings();
        getCurrentPOLICYSettings();
	
	// set aspect/policy settings
        setCurrentASPECTSettings(0);
	setCurrentPOLICYSettings(0);	

#if !TUXTXT_CFG_STANDALONE
	int initialized = tuxtxt_init();
	if ( initialized )
		tuxtxt_cache.page = 0x100;
	
	// page
	if(page) 
	{
		//tuxtxt_cache.page = page;
		sub_page = tuxtxt_cache.page = page;
		sub_pid = pid;
		use_gui = 0;
		boxed = 1;
	}
#endif

	// show versioninfo
	sscanf(cvs_revision, "%*s %s", versioninfo);
	printf("TuxTxt %s\n", versioninfo);
	printf("for 32bpp framebuffer\n");

	//rcinput
	rc = _rc;
	
	//fb
	lfb = (unsigned char *)CFrameBuffer::getInstance()->getFrameBufferPointer();

	tuxtxt_cache.vtxtpid = pid;

	if(tuxtxt_cache.vtxtpid == 0)
		printf("[tuxtxt] No PID given, so scanning for PIDs ...\n\n");
	else
		printf("[tuxtxt] using PID 0x%x\n", tuxtxt_cache.vtxtpid);

	fcntl(rc, F_SETFL, fcntl(rc, F_GETFL) | O_EXCL | O_NONBLOCK);
	
	// coordinate
	int x = CFrameBuffer::getInstance()->getScreenX();
	int y = CFrameBuffer::getInstance()->getScreenY();
	int w = CFrameBuffer::getInstance()->getScreenWidth();
	int h = CFrameBuffer::getInstance()->getScreenHeight();
	//

	int s_x = x;
	int s_y = y;
	int s_w = w;
	int s_h = h;

	sx = s_x;
	ex = s_x + s_w;
	sy = s_y;
	ey = s_y + s_h;
	

	fprintf(stderr, "stride=%d sx=%d ex=%d sy=%d ey=%d\n", CFrameBuffer::getInstance()->getStride(), sx, ex, sy, ey);

	//initialisations
	transpmode = 0;

	// init source
	if (Init(source) == 0)
		return 0;
	
	// create sub thread
	if(!use_gui)
	{
		pthread_create(&ttx_sub_thread, 0, reader_thread, (void *) NULL);
		return 1;
	}
	
	//main loop
#ifdef FB_BLIT	
	CFrameBuffer::getInstance()->blit();
#endif	

	do {
		//update page or timestring and lcd
		//RenderPage();

		if (GetRCCode() == 1)
		{
			if (transpmode == 2) // TV mode
			{
				switch (RCCode)
				{
//#if TUXTXT_DEBUG /* FIXME */
				case RC_OK:
					if (showhex)
					{
						dump_page(); /* hexdump of page contents to stdout for debugging */
					}
					continue; /* otherwise ignore key */
//#endif /* TUXTXT_DEBUG */
				case RC_UP:
				case RC_DOWN:
				case RC_0:
				case RC_1:
				case RC_2:
				case RC_3:
				case RC_4:
				case RC_5:
				case RC_6:
				case RC_7:
				case RC_8:
				case RC_9:
				case RC_GREEN:
				case RC_YELLOW:
				case RC_BLUE:
				case RC_PLUS:
				case RC_MINUS:
				case RC_DBOX:
				case RC_STANDBY:
					transpmode = 1; /* switch to normal mode */
					SwitchTranspMode();
					break;		/* and evaluate key */

				case RC_MUTE:		/* regular toggle to transparent */
				case RC_TEXT:
					break;

				case RC_HELP: /* switch to scart input and back */
				{
					continue; /* otherwise ignore exit key */
				}
				default:
					continue; /* ignore all other keys */
				}
			}

			switch (RCCode)
			{
			case RC_UP:
				GetNextPageOne(!swapupdown);
				break;

			case RC_DOWN:
				GetNextPageOne(swapupdown);
				break;

			case RC_RIGHT:	
				if (boxed)
				{
					subtitledelay++;				    
					// display subtitledelay
					PosY = StartY;
					char ns[10];
					SetPosX(1);
					sprintf(ns,"+%d    ",subtitledelay);
					RenderCharFB(ns[0],&atrtable[ATR_WB]);
					RenderCharFB(ns[1],&atrtable[ATR_WB]);
					RenderCharFB(ns[2],&atrtable[ATR_WB]);
					RenderCharFB(ns[4],&atrtable[ATR_WB]);
				}
				else
					GetNextSubPage(1);	
				break;

			case RC_LEFT:
				if (boxed)
				{
					subtitledelay--;
					if (subtitledelay < 0) 
						subtitledelay = 0;
					// display subtitledelay
					PosY = StartY;
					char ns[10];
					SetPosX(1);
					sprintf(ns,"+%d    ",subtitledelay);
					RenderCharFB(ns[0],&atrtable[ATR_WB]);
					RenderCharFB(ns[1],&atrtable[ATR_WB]);
					RenderCharFB(ns[2],&atrtable[ATR_WB]);
					RenderCharFB(ns[4],&atrtable[ATR_WB]);
				}
				else
					GetNextSubPage(-1);	
				break;

			case RC_OK:
				if (tuxtxt_cache.subpagetable[tuxtxt_cache.page] == 0xFF)
					continue;
				PageCatching();
				break;

			case RC_0:
			case RC_1:
			case RC_2:
			case RC_3:
			case RC_4:
			case RC_5:
			case RC_6:
			case RC_7:
			case RC_8:
			case RC_9:
				PageInput(RCCode - RC_0);
				break;

			case RC_RED:	 ColorKey(prev_100);		break;
			case RC_GREEN:	 ColorKey(prev_10);		break;
			case RC_YELLOW:  ColorKey(next_10);		break;
			case RC_BLUE:	 ColorKey(next_100);		break;
#if 1
			case RC_PLUS:	 SwitchZoomMode();		break;
#endif
			case RC_MINUS:	 SwitchScreenMode(-1);prevscreenmode = screenmode; break;
			case RC_MUTE:	 SwitchTranspMode();	break;
			case RC_TEXT:	 
				if(transpmode == 1)
					RCCode = RC_HOME;
				SwitchTranspMode();	
				break;
			case RC_HELP:	 SwitchHintMode();		break;
#if 1
			case RC_DBOX:	 ConfigMenu(0, source);	break;
#endif
			case RC_HOME:
				break;
			}
		}

		// update page or timestring and lcd
		RenderPage();
#ifdef FB_BLIT		
		CFrameBuffer::getInstance()->blit();
#endif		
	} while ((RCCode != RC_HOME) && (RCCode != RC_STANDBY));

	//exit
	CleanUp();

#if 0
	close(rc);
	close(lcd);
#endif

#if 1
	if ( initialized )
		tuxtxt_close();
#endif

 	printf("Tuxtxt: plugin ended\n");

	return 1;
}

/*
* MyFaceRequester
*/
FT_Error MyFaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface)
{
	FT_Error result;

	result = FT_New_Face(library, (char *) face_id, 0, aface);

#if TUXTXT_DEBUG
	if (!result)
		printf("TuxTxt <font %s loaded>\n", (char*)face_id);
	else
		printf("TuxTxt <open font %s failed>\n", (char*)face_id);
#endif

	return result;
}

/*
 * Init
*/
int Init( int source )
{
	int error, i;
	unsigned char magazine;

	/* init data */
 	//page_atrb[32] = transp<<4 | transp;
	inputcounter  = 2;

	for (magazine = 1; magazine < 9; magazine++)
	{
		tuxtxt_cache.current_page  [magazine] = -1;
		tuxtxt_cache.current_subpage [magazine] = -1;
	}
#if TUXTXT_CFG_STANDALONE
	/* init data */
	memset(&tuxtxt_cache.astCachetable, 0, sizeof(tuxtxt_cache.astCachetable));
	memset(&tuxtxt_cache.subpagetable, 0xFF, sizeof(tuxtxt_cache.subpagetable));
	memset(&tuxtxt_cache.astP29, 0, sizeof(tuxtxt_cache.astP29));

	memset(&tuxtxt_cache.basictop, 0, sizeof(tuxtxt_cache.basictop));
	memset(&tuxtxt_cache.adip, 0, sizeof(tuxtxt_cache.adip));
	memset(&tuxtxt_cache.flofpages, 0 , sizeof(tuxtxt_cache.flofpages));
	tuxtxt_cache.maxadippg  = -1;
	tuxtxt_cache.bttok      = 0;
	maxhotlist = -1;

	//page_atrb[32] = transp<<4 | transp;
	inputcounter  = 2;
	tuxtxt_cache.cached_pages  = 0;

	tuxtxt_cache.page_receiving = -1;
	tuxtxt_cache.page       = 0x100;
#endif
	lastpage   = tuxtxt_cache.page;
	prev_100   = 0x100;
	prev_10    = 0x100;
	next_100   = 0x100;
	next_10    = 0x100;
	tuxtxt_cache.subpage    = tuxtxt_cache.subpagetable[tuxtxt_cache.page];
	
	if (tuxtxt_cache.subpage == 0xff)
		tuxtxt_cache.subpage    = 0;
	
	tuxtxt_cache.pageupdate = 0;

	tuxtxt_cache.zap_subpage_manual = 0;

	subtitledelay = 0;
	delaystarted = 0;

	/* init lcd */
	UpdateLCD();

	/* config defaults */
	screenmode = 0;
	screen_mode1 = 0;
	screen_mode2 = 0;
	color_mode   = 10;
	trans_mode   = 10;
	menulanguage = 0;	/* german */
	national_subset = 0;/* default */
	auto_national   = 1;
	swapupdown      = 0;
	showhex         = 0;
	showflof        = 1;
	show39          = 1;
	showl25         = 1;
	dumpl25         = 0;
	usettf          = 0;
	TTFWidthFactor16  = 28;
	TTFHeightFactor16 = 16;
	TTFShiftX         = 0;
	TTFShiftY         = 0;

	// load config
	if ((conf = fopen(TUXTXTCONF, "rt")) == 0)
	{ 
	        printf("failed to open %s\n", TUXTXTCONF);
		perror("TuxTxt <fopen tuxtxt.conf>");
	}
	else
	{
		while(1)
		{
			char line[100];
			int ival;

			if (!fgets(line, sizeof(line), conf))
				break;

			if (1 == sscanf(line, "ScreenMode16x9Normal %i", &ival))
				screen_mode1 = ival & 1;
			else if (1 == sscanf(line, "ScreenMode16x9Divided %i", &ival))
				screen_mode2 = ival & 1;
			else if (1 == sscanf(line, "Brightness %i", &ival))
				color_mode = ival;
			else if (1 == sscanf(line, "AutoNational %i", &ival))
				auto_national = ival & 1;
			else if (1 == sscanf(line, "NationalSubset %i", &ival))
			{
				if (ival >= 0 && ival <= (int) MAX_NATIONAL_SUBSET)
					national_subset = ival;
			}
			else if (1 == sscanf(line, "MenuLanguage %i", &ival))
			{
				if (ival >= 0 && ival <= MAXMENULANGUAGE)
					menulanguage = ival;
			}
			else if (1 == sscanf(line, "SwapUpDown %i", &ival))
				swapupdown = ival & 1;
			else if (1 == sscanf(line, "ShowHexPages %i", &ival))
				showhex = ival & 1;
			else if (1 == sscanf(line, "Transparency %i", &ival))
				trans_mode = ival;
			else if (1 == sscanf(line, "TTFWidthFactor16 %i", &ival))
				TTFWidthFactor16 = ival;
			else if (1 == sscanf(line, "TTFHeightFactor16 %i", &ival))
				TTFHeightFactor16 = ival;
			else if (1 == sscanf(line, "TTFShiftX %i", &ival))
				TTFShiftX = ival;
			else if (1 == sscanf(line, "TTFShiftY %i", &ival))
				TTFShiftY = ival;
			else if (1 == sscanf(line, "Screenmode %i", &ival))
				screenmode = ival;
			else if (1 == sscanf(line, "ShowFLOF %i", &ival))
				showflof = ival & 1;
			else if (1 == sscanf(line, "Show39 %i", &ival))
				show39 = ival & 1;
			else if (1 == sscanf(line, "ShowLevel2p5 %i", &ival))
				showl25 = ival & 1;
			else if (1 == sscanf(line, "DumpLevel2p5 %i", &ival))
				dumpl25 = ival & 1;
			else if (1 == sscanf(line, "UseTTF %i", &ival))
				usettf = ival & 1;
			else if (1 == sscanf(line, "StartX %i", &ival))
				sx = ival;
			else if (1 == sscanf(line, "EndX %i", &ival))
				ex = ival;
			else if (1 == sscanf(line, "StartY %i", &ival))
				sy = ival;
			else if (1 == sscanf(line, "EndY %i", &ival))
				ey = ival;
		}
		fclose(conf);
	}

	saveconfig = 0;
	savedscreenmode = screenmode;
	national_subset_secondary = NAT_DEFAULT;

	// init fontlibrary
	if ((error = FT_Init_FreeType(&library)))
	{
		printf("TuxTxt <FT_Init_FreeType: 0x%.2X>", error);
		return 0;
	}

	if ((error = FTC_Manager_New(library, 7, 2, 0, &MyFaceRequester, NULL, &manager)))
	{
		FT_Done_FreeType(library);
		printf("TuxTxt <FTC_Manager_New: 0x%.2X>\n", error);
		return 0;
	}

	if ((error = FTC_SBitCache_New(manager, &cache)))
	{
		FTC_Manager_Done(manager);
		FT_Done_FreeType(library);
		printf("TuxTxt <FTC_SBitCache_New: 0x%.2X>\n", error);
		return 0;
	}

	fontwidth = 0;	/* initialize at first setting */

	/* calculate font dimensions */
	displaywidth = (ex - sx);
	fontheight = (ey - sy) / 25; //21;
	fontwidth_normal = (ex - sx) / 40;
	setfontwidth(fontwidth_normal);
	fontwidth_topmenumain = (TV43STARTX - sx) / 40;
	fontwidth_topmenusmall = (ex - TOPMENUSTARTX) / TOPMENUCHARS;
	fontwidth_small = (TV169FULLSTARTX - sx)  / 40;
	ymosaic[0] = 0; /* y-offsets for 2*3 mosaic */
	ymosaic[1] = (fontheight + 1) / 3;
	ymosaic[2] = (fontheight * 2 + 1) / 3;
	ymosaic[3] = fontheight;
	
	{
		int i;
		for (i = 0; i <= 10; i++)
			aydrcs[i] = (fontheight * i + 5) / 10;
	}

	/* center screen */
	//StartX = sx; //+ (((ex-sx) - 40*fontwidth) / 2)
	//test;
	StartX = sx+ (((ex-sx) - 40*fontwidth) / 2);
	StartY = sy + (((ey-sy) - 25*fontheight) / 2);

	if (usettf)
	{
		typettf.face_id = (FTC_FaceID) TUXTXTTTFVAR;
		typettf.height = (FT_UShort) fontheight * TTFHeightFactor16 / 16;
	}
	else
	{
		typettf.face_id = (FTC_FaceID) TUXTXTOTBVAR;
		typettf.width  = (FT_UShort) 23;
		typettf.height = (FT_UShort) 23;
	}

	typettf.flags = FT_LOAD_MONOCHROME;

	if ((error = FTC_Manager_LookupFace(manager, typettf.face_id, &face)))
	{
		if (usettf == 1)
			typettf.face_id = (FTC_FaceID) TUXTXTTTF;
		else 
			typettf.face_id = (FTC_FaceID) TUXTXTOTB;

		printf("font %s\n", (char*) typettf.face_id);

		if ((error = FTC_Manager_LookupFace(manager, typettf.face_id, &face)))
		{
			printf("TuxTxt <FTC_Manager_LookupFace failed with Errorcode 0x%.2X>\n", error);
			FTC_Manager_Done(manager);
			FT_Done_FreeType(library);
			return 0;
		}
	}
	
	ascender = (usettf ? fontheight * face->ascender / face->units_per_EM : 16);
#if TUXTXT_DEBUG
	printf("TuxTxt <fh%d fw%d fs%d tm%d ts%d ym%d %d %d sx%d sy%d a%d>\n",
			 fontheight, fontwidth, fontwidth_small, fontwidth_topmenumain, fontwidth_topmenusmall,
			 ymosaic[0], ymosaic[1], ymosaic[2], StartX, StartY, ascender);
#endif

	/* set new colormap */
	setcolors((unsigned short *)defaultcolors, 0, SIZECOLTABLE);

	ClearBB(transp); /* initialize backbuffer */

	for (i = 0; i < 40 * 25; i++)
	{
		page_char[i] = ' ';
		page_atrb[i].fg = transp;
		page_atrb[i].bg = transp;
		page_atrb[i].charset = C_G0P;
		page_atrb[i].doubleh = 0;
		page_atrb[i].doublew = 0;
		page_atrb[i].IgnoreAtBlackBgSubst = 0;
	}

	//  if no vtxtpid for current service, search PIDs
	if (tuxtxt_cache.vtxtpid == 0)
	{
		/* get all vtxt-pids */
		getpidsdone = -1;	 // don't kill thread

		if (GetTeletextPIDs(source) == 0)
		{
			FTC_Manager_Done(manager);
			FT_Done_FreeType(library);
			
			//munmap(lfb, fix_screeninfo.smem_len);		/* unmap framebuffer */

			return 0;
		}

		if (auto_national)
			national_subset = pid_table[0].national_subset;

		if (pids_found > 1)
			ConfigMenu(1, source);
		else
		{
			tuxtxt_cache.vtxtpid = pid_table[0].vtxt_pid;
			current_service = 0;
			RenderMessage(ShowServiceName);
		}
	}
	else
	{
		SDT_ready = 0;
		
		//getpidsdone = 0;
		//tuxtxt_cache.pageupdate = 1; /* force display of message page not found (but not twice) */

		if(auto_national && cfg_national_subset)
			national_subset = cfg_national_subset;
		printf("Tuxtxt: national_subset %d (cfg %d)\n", national_subset, cfg_national_subset);
	}

#if TUXTXT_CFG_STANDALONE
	tuxtxt_init_demuxer( source);
	tuxtxt_start_thread( source);
#else
	tuxtxt_start(tuxtxt_cache.vtxtpid, source);
#endif
	fcntl(rc, F_SETFL, O_NONBLOCK);
	gethotlist();
	
	//SwitchScreenMode(screenmode);
	if(use_gui)
		SwitchScreenMode(screenmode);
	else
		SwitchScreenMode(0);
	
	prevscreenmode = screenmode;
	
	printf("TuxTxt: init ok\n");

	/* init successfull */
	return 1;
}

/*
 * Cleanup
*/
void CleanUp()
{
	int curscreenmode = screenmode;

	/* hide and close pig */
	if (screenmode)
		SwitchScreenMode(0); /* turn off divided screen */
			
	setCurrentASPECTSettings(1);
	setCurrentPOLICYSettings(1);

#if TUXTXT_CFG_STANDALONE
	tuxtxt_stop_thread();
	tuxtxt_clear_cache();
	if (tuxtxt_cache.dmx != -1)
		close(tuxtxt_cache.dmx);
	tuxtxt_cache.dmx = -1;
#else
	//tuxtxt_stop();
#endif

	/* clear screen */
	CFrameBuffer::getInstance()->ClearFrameBuffer();
#ifdef FB_BLIT	
	CFrameBuffer::getInstance()->blit();
#endif	

	/* close freetype */
	FTC_Manager_Done(manager);
	FT_Done_FreeType(library);

	/* unmap framebuffer */
	//munmap(lfb, fix_screeninfo.smem_len);

	if (hotlistchanged)
		savehotlist();

	/* save config */
	if (saveconfig || curscreenmode != savedscreenmode)
	{
		if ((conf = fopen(TUXTXTCONF, "wt")) == 0)
		{
			perror("TuxTxt <fopen tuxtxt.conf>");
		}
		else
		{
			printf("TuxTxt <saving config>\n");
			fprintf(conf, "ScreenMode16x9Normal %d\n", screen_mode1);
			fprintf(conf, "ScreenMode16x9Divided %d\n", screen_mode2);
			fprintf(conf, "Brightness %d\n", color_mode);
			fprintf(conf, "MenuLanguage %d\n", menulanguage);
			fprintf(conf, "AutoNational %d\n", auto_national);
			fprintf(conf, "NationalSubset %d\n", national_subset);
			fprintf(conf, "SwapUpDown %d\n", swapupdown);
			fprintf(conf, "ShowHexPages %d\n", showhex);
			fprintf(conf, "Transparency 0x%X\n", trans_mode);
			fprintf(conf, "TTFWidthFactor16 %d\n", TTFWidthFactor16);
			fprintf(conf, "TTFHeightFactor16 %d\n", TTFHeightFactor16);
			fprintf(conf, "TTFShiftX %d\n", TTFShiftX);
			fprintf(conf, "TTFShiftY %d\n", TTFShiftY);
			fprintf(conf, "Screenmode %d\n", curscreenmode);
			fprintf(conf, "ShowFLOF %d\n", showflof);
			fprintf(conf, "Show39 %d\n", show39);
			fprintf(conf, "ShowLevel2p5 %d\n", showl25);
			fprintf(conf, "DumpLevel2p5 %d\n", dumpl25);
			fprintf(conf, "UseTTF %d\n", usettf);
#if 0
			fprintf(conf, "StartX %d\n", sx);
			fprintf(conf, "EndX %d\n", ex);
			fprintf(conf, "StartY %d\n", sy);
			fprintf(conf, "EndY %d\n", ey);
#endif
			fclose(conf);
		}
	}
}

/*
 * GetTeletextPIDs
*/
//TODO: multi demuxes needed
int GetTeletextPIDs(int source)
{
	int pat_scan, pmt_scan, sdt_scan, desc_scan, pid_test, byte, diff, first_sdt_sec;

	unsigned char bufPAT[1024];
	unsigned char bufSDT[1024];
	unsigned char bufPMT[1024];

	/* show infobar */
	RenderMessage(ShowInfoBar);

        unsigned char filter[DMX_FILTER_SIZE];
        unsigned char mask[DMX_FILTER_SIZE];
	int res;

        cDemux * dmx = new cDemux( LIVE_DEMUX );
	dmx->Open(DMX_PSI_CHANNEL, 1024, source );

        memset(filter, 0x00, DMX_FILTER_SIZE);
        memset(mask, 0x00, DMX_FILTER_SIZE);

        //filter[0] = 0x00;
        //mask[0] = 0xFF;
        mask[0] = 0xFF;
        mask[4] = 0xFF;

        dmx->sectionFilter(0, filter, mask, 1);
	res = dmx->Read(bufPAT, sizeof(bufPAT));
	dmx->Stop();
	if(res <= 0) {
		printf("TuxTxt <read PAT> failed");
		delete dmx;
		return 0;
	}

	/* scan each PMT for vtxt-pid */
	pids_found = 0;

	for (pat_scan = 0x0A; pat_scan < 0x0A + (((bufPAT[0x01]<<8 | bufPAT[0x02]) & 0x0FFF) - 9); pat_scan += 4)
	{
#if TUXTXT_DEBUG
		printf("PAT liefert:%04x, %04x \n",((bufPAT[pat_scan - 2]<<8) | (bufPAT[pat_scan - 1])),(bufPAT[pat_scan]<<8 | bufPAT[pat_scan+1]) & 0x1FFF);
#endif
		if (((bufPAT[pat_scan - 2]<<8) | (bufPAT[pat_scan - 1])) == 0)
			continue;

		int pid = (bufPAT[pat_scan]<<8 | bufPAT[pat_scan+1]) & 0x1FFF;
		filter[0] = 0x02;
		mask[0] = 0xFF;

		dmx->sectionFilter(pid, filter, mask, 1);
		res = dmx->Read(bufPMT, sizeof(bufPMT));
		dmx->Stop();
		if(res <= 0) {
			perror("TuxTxt <read PMT>");
			continue;
		}

		for (pmt_scan = 0x0C + ((bufPMT[0x0A]<<8 | bufPMT[0x0B]) & 0x0FFF);
			  pmt_scan < (((bufPMT[0x01]<<8 | bufPMT[0x02]) & 0x0FFF) - 7);
			  pmt_scan += 5 + bufPMT[pmt_scan + 4])
		{
			if (bufPMT[pmt_scan] == 6)
			{
				for (desc_scan = pmt_scan + 5;
					  desc_scan < pmt_scan + ((bufPMT[pmt_scan + 3]<<8 | bufPMT[pmt_scan + 4]) & 0x0FFF) + 5;
					  desc_scan += 2 + bufPMT[desc_scan + 1])
				{
					if (bufPMT[desc_scan] == 0x56)
					{
						char country_code[4];

						for (pid_test = 0; pid_test < pids_found; pid_test++)
							if (pid_table[pid_test].vtxt_pid == ((bufPMT[pmt_scan + 1]<<8 | bufPMT[pmt_scan + 2]) & 0x1FFF))
								goto skip_pid;

						pid_table[pids_found].vtxt_pid     = (bufPMT[pmt_scan + 1]<<8 | bufPMT[pmt_scan + 2]) & 0x1FFF;
						pid_table[pids_found].service_id = bufPMT[0x03]<<8 | bufPMT[0x04];
						if (bufPMT[desc_scan + 1] == 5)
						{
							country_code[0] = bufPMT[desc_scan + 2] | 0x20;
							country_code[1] = bufPMT[desc_scan + 3] | 0x20;
							country_code[2] = bufPMT[desc_scan + 4] | 0x20;
							country_code[3] = 0;
							pid_table[pids_found].national_subset = GetNationalSubset(country_code);
						}
						else
						{
							country_code[0] = 0;
							pid_table[pids_found].national_subset = NAT_DEFAULT; /* use default charset */
						}

#if TUXTXT_DEBUG
						printf("TuxTxt <Service %04x Country code \"%3s\" national subset %2d%s>\n",
								 pid_table[pids_found].service_id,
								 country_code,
								 pid_table[pids_found].national_subset,
								 (pid_table[pids_found].vtxt_pid == tuxtxt_cache.vtxtpid) ? " * " : ""
								 );
#endif

						pids_found++;
skip_pid:
					;
					}
				}
			}
		}
	}

	/* check for teletext */
	if (pids_found == 0)
	{
		printf("TuxTxt <no Teletext on TS found>\n");

		RenderMessage(NoServicesFound);
		sleep(3);
		delete dmx;
		return 0;
	}

	/* read SDT to get servicenames */
	SDT_ready = 0;

	filter[0] = 0x42;
	mask[0] = 0xFF;

	dmx->sectionFilter(0x0011, filter, mask, 1);

	first_sdt_sec = -1;

	while (1)
	{
		res = dmx->Read(bufSDT, sizeof(bufSDT));
		if(res <= 0) {
			perror("TuxTxt <read SDT>");
			delete dmx;
			RenderMessage(ShowServiceName);
			return 1;
		}

		if (first_sdt_sec == bufSDT[6])
			break;

		if (first_sdt_sec == -1)
			first_sdt_sec = bufSDT[6];

		/* scan SDT to get servicenames */
		for (sdt_scan = 0x0B; sdt_scan < ((bufSDT[1]<<8 | bufSDT[2]) & 0x0FFF) - 7; sdt_scan += 5 + ((bufSDT[sdt_scan + 3]<<8 | bufSDT[sdt_scan + 4]) & 0x0FFF))
		{
			for (pid_test = 0; pid_test < pids_found; pid_test++)
			{
				if ((bufSDT[sdt_scan]<<8 | bufSDT[sdt_scan + 1]) == pid_table[pid_test].service_id && bufSDT[sdt_scan + 5] == 0x48)
				{
					diff = 0;
					pid_table[pid_test].service_name_len = bufSDT[sdt_scan+9 + bufSDT[sdt_scan+8]];

					for (byte = 0; byte < pid_table[pid_test].service_name_len; byte++)
					{
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x5B;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x7B;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x5C;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x7C;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x5D;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x7D;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] == (unsigned char)'�')
							bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] = 0x7E;
						if (bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] >= 0x80 && bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte] <= 0x9F)
							diff--;
						else
							pid_table[pid_test].service_name[byte + diff] = bufSDT[sdt_scan+10 + bufSDT[sdt_scan + 8] + byte];
					}

					pid_table[pid_test].service_name_len += diff;
				}
			}
		}
	}
	delete dmx;
	SDT_ready = 1;

	/* show current servicename */
	current_service = 0;

	if (tuxtxt_cache.vtxtpid != 0)
	{
		while (pid_table[current_service].vtxt_pid != tuxtxt_cache.vtxtpid && current_service < pids_found)
			current_service++;

		if (auto_national && current_service < pids_found)
			national_subset = pid_table[current_service].national_subset;
		RenderMessage(ShowServiceName);
	}

	getpidsdone = 1;

	RenderCharLCD(pids_found/10,  7, 44);
	RenderCharLCD(pids_found%10, 19, 44);

	return 1;
}

/*
 * GetNationalSubset
*/
int GetNationalSubset(const char *cc)
{
	if (memcmp(cc, "cze", 3) == 0 || memcmp(cc, "ces", 3) == 0 ||
	    memcmp(cc, "slo", 3) == 0 || memcmp(cc, "slk", 3) == 0)
		return 0;
	if (memcmp(cc, "eng", 3) == 0)
		return 1;
	if (memcmp(cc, "est", 3) == 0)
		return 2;
	if (memcmp(cc, "fre", 3) == 0 || memcmp(cc, "fra", 3) == 0)
		return 3;
	if (memcmp(cc, "ger", 3) == 0 || memcmp(cc, "deu", 3) == 0)
		return 4;
	if (memcmp(cc, "ita", 3) == 0)
		return 5;
	if (memcmp(cc, "lav", 3) == 0 || memcmp(cc, "lit", 3) == 0)
		return 6;
	if (memcmp(cc, "pol", 3) == 0)
		return 7;
	if (memcmp(cc, "spa", 3) == 0 || memcmp(cc, "por", 3) == 0)
		return 8;
	if (memcmp(cc, "rum", 3) == 0 || memcmp(cc, "ron", 3) == 0)
		return 9;
	if (memcmp(cc, "scc", 3) == 0 || memcmp(cc, "srp", 3) == 0 ||
	    memcmp(cc, "scr", 3) == 0 || memcmp(cc, "hrv", 3) == 0 ||
	    memcmp(cc, "slv", 3) == 0)
		return 10;
	if (memcmp(cc, "swe", 3) == 0 ||
	    memcmp(cc, "dan", 3) == 0 ||
	    memcmp(cc, "nor", 3) == 0 ||
	    memcmp(cc, "fin", 3) == 0 ||
	    memcmp(cc, "hun", 3) == 0)
		return 11;
	if (memcmp(cc, "tur", 3) == 0)
		return 12;
	if (memcmp(cc, "rus", 3) == 0 ||
	    memcmp(cc, "bul", 3) == 0 ||
	    memcmp(cc, "ser", 3) == 0 ||
	    memcmp(cc, "cro", 3) == 0 ||
	    memcmp(cc, "ukr", 3) == 0)
		return NAT_RU;
	if (memcmp(cc, "gre", 3) == 0)
		return NAT_GR;

	return NAT_DEFAULT;	/* use default charset */
}

/*
 * ConfigMenu
*/
#if TUXTXT_DEBUG
void charpage()
{
	PosY = StartY;
	PosX = StartX;
	char cachefill[100];
	int fullsize =0,hexcount = 0, col, p,sp;
	int escpage = 0;
	tstCachedPage* pg;
	ClearFB(black);

	int zipsize = 0;
	for (p = 0; p < 0x900; p++)
	{
		for (sp = 0; sp < 0x80; sp++)
		{
			pg = tuxtxt_cache.astCachetable[p][sp];
			if (pg)
			{

				fullsize+=23*40;
				zipsize += tuxtxt_get_zipsize(p,sp);
			}
		}
	}


	memset(cachefill,' ',40);
	sprintf(cachefill,"f:%d z:%d h:%d c:%d %03x",fullsize, zipsize, hexcount, tuxtxt_cache.cached_pages, escpage);

	for (col = 0; col < 40; col++)
	{
		RenderCharFB(cachefill[col], &atrtable[ATR_WB]);
	}

	tstPageAttr atr;
	memcpy(&atr,&atrtable[ATR_WB],sizeof(tstPageAttr));
	int row;
	atr.charset = C_G0P;
	PosY = StartY+fontheight;

	for (row = 0; row < 16; row++)
	{
		PosY+= fontheight;
		SetPosX(1);
		for (col=0; col < 6; col++)
		{
			RenderCharFB(col*16+row+0x20, &atr);
		}
	}

	atr.setX26 = 1;
	PosY = StartY+fontheight;

	for (row = 0; row < 16; row++)
	{
		PosY+= fontheight;
		SetPosX(10);
		for (col=0; col < 6; col++)
		{
			RenderCharFB(col*16+row+0x20, &atr);
		}
	}

	PosY = StartY+fontheight;
	atr.charset = C_G2;
	atr.setX26 = 0;

	for (row = 0; row < 16; row++)
	{
		PosY+= fontheight;
		SetPosX(20);
		for (col=0; col < 6; col++)
		{
			RenderCharFB(col*16+row+0x20, &atr);
		}
	}

	atr.charset = C_G3;
	PosY = StartY+fontheight;

	for (row = 0; row < 16; row++)
	{
		PosY+= fontheight;
		SetPosX(30);
		for (col=0; col < 6; col++)
		{
			RenderCharFB(col*16+row+0x20, &atr);
		}
	}

	do
	{
		GetRCCode();
	}
	while (RCCode != RC_OK && RCCode != RC_HOME);
}
#endif

void Menu_HighlightLine(char *menu, int line, int high)
{
	char hilitline[] = "0111111111111111111111111111102";
	int itext = Menu_Width*line; /* index start menuline */
	int byte;
	int national_subset_bak = national_subset;

	PosX = Menu_StartX;
	PosY = Menu_StartY + line*fontheight;
	if (line == MenuLine[M_NAT])
		national_subset = national_subset_bak;
	else
		national_subset = menusubset[menulanguage];

	for (byte = 0; byte < Menu_Width; byte++)
		RenderCharFB(menu[itext + byte],
						 high ?
						 &atrtable[hilitline[byte] - '0' + ATR_MENUHIL0] :
						 &atrtable[menuatr[itext + byte] - '0' + ATR_MENU0]);
	national_subset = national_subset_bak;
}

void Menu_UpdateHotlist(char *menu, int hotindex, int menuitem)
{
	int i, j, k;
	tstPageAttr *attr;

	PosX = Menu_StartX + 6*fontwidth;
	PosY = Menu_StartY + (MenuLine[M_HOT]+1)*fontheight;
	j = Menu_Width*(MenuLine[M_HOT]+1) + 6; /* start index in menu */

	for (i = 0; i <= maxhotlist+1; i++)
	{
		if (i == maxhotlist+1) /* clear last+1 entry in case it was deleted */
		{
			attr = &atrtable[ATR_MENU5];
			memset(&menu[j], ' ', 3);
		}
		else
		{
			if (i == hotindex)
				attr = &atrtable[ATR_MENU1];
			else
			attr = &atrtable[ATR_MENU5];
			hex2str(&menu[j+2], hotlist[i]);
		}

		for (k = 0; k < 3; k++)
			RenderCharFB(menu[j+k], attr);

		if (i == 4)
		{
			PosX = Menu_StartX + 6*fontwidth;
			PosY += fontheight;
			j += 2*Menu_Width - 4*4;
		}
		else
		{
			j += 4; /* one space distance */
			PosX += fontwidth;
		}
	}

	hex2str(&menu[Menu_Width*MenuLine[M_HOT] + hotlistpagecolumn[menulanguage]], (hotindex >= 0) ? hotlist[hotindex] : tuxtxt_cache.page);
	memcpy(&menu[Menu_Width*MenuLine[M_HOT] + hotlisttextcolumn[menulanguage]], &hotlisttext[menulanguage][(hotindex >= 0) ? 5 : 0], 5);
	PosX = Menu_StartX + 20*fontwidth;
	PosY = Menu_StartY + MenuLine[M_HOT]*fontheight;

	Menu_HighlightLine(menu, MenuLine[M_HOT], (menuitem == M_HOT) ? 1 : 0);
}

void Menu_Init(char *menu, int current_pid, int menuitem, int hotindex)
{
	int byte, line;
	int national_subset_bak = national_subset;

	memcpy(menu, configmenu[menulanguage], Menu_Height*Menu_Width);

	if (getpidsdone)
	{
		memset(&menu[MenuLine[M_PID]*Menu_Width+3], 0x20,24);
		if (SDT_ready)
			memcpy(&menu[MenuLine[M_PID]*Menu_Width+3+(24-pid_table[current_pid].service_name_len)/2], &pid_table[current_pid].service_name, pid_table[current_pid].service_name_len);
		else
			hex2str(&menu[MenuLine[M_PID]*Menu_Width + 13 + 3], tuxtxt_cache.vtxtpid);
	}
	if (!getpidsdone || current_pid == 0 || pids_found == 1)
		menu[MenuLine[M_PID]*Menu_Width +  1] = ' ';

	if (!getpidsdone || current_pid == pids_found - 1 || pids_found == 1)
		menu[MenuLine[M_PID]*Menu_Width + 28] = ' ';


	/* set 16:9 modi, colors & national subset */
	memcpy(&menu[Menu_Width*MenuLine[M_SC1] + Menu_Width - 5], &configonoff[menulanguage][screen_mode1  ? 3 : 0], 3);
	memcpy(&menu[Menu_Width*MenuLine[M_SC2] + Menu_Width - 5], &configonoff[menulanguage][screen_mode2  ? 3 : 0], 3);

	menu[MenuLine[M_COL]*Menu_Width +  1] = (color_mode == 1  ? ' ' : '�');
	menu[MenuLine[M_COL]*Menu_Width + 28] = (color_mode == 24 ? ' ' : '�');
	memset(&menu[Menu_Width*MenuLine[M_COL] + 3             ], 0x7f,color_mode);
	memset(&menu[Menu_Width*MenuLine[M_COL] + 3+color_mode  ], 0x20,24-color_mode);
//	memcpy(&menu[Menu_Width*MenuLine[M_COL] + Menu_Width - 5], &configonoff[menulanguage][color_mode    ? 3 : 0], 3);
	menu[MenuLine[M_TRA]*Menu_Width +  1] = (trans_mode == 1  ? ' ' : '�');
	menu[MenuLine[M_TRA]*Menu_Width + 28] = (trans_mode == 24 ? ' ' : '�');
	memset(&menu[Menu_Width*MenuLine[M_TRA] + 3             ], 0x7f,trans_mode);
	memset(&menu[Menu_Width*MenuLine[M_TRA] + 3+trans_mode  ], 0x20,24-trans_mode);

	memcpy(&menu[Menu_Width*MenuLine[M_AUN] + Menu_Width - 5], &configonoff[menulanguage][auto_national ? 3 : 0], 3);
	if (national_subset != NAT_DE)
		memcpy(&menu[Menu_Width*MenuLine[M_NAT] + 2], &countrystring[national_subset*COUNTRYSTRING_WIDTH], COUNTRYSTRING_WIDTH);
	if (national_subset == 0  || auto_national)
		menu[MenuLine[M_NAT]*Menu_Width +  1] = ' ';
	if (national_subset == MAX_NATIONAL_SUBSET || auto_national)
		menu[MenuLine[M_NAT]*Menu_Width + 28] = ' ';
	if (showhex)
		menu[MenuLine[M_PID]*Menu_Width + 27] = '?';
	/* render menu */
	PosY = Menu_StartY;
	for (line = 0; line < Menu_Height; line++)
	{
		PosX = Menu_StartX;
		if (line == MenuLine[M_NAT])
			national_subset = national_subset_bak;
		else
			national_subset = menusubset[menulanguage];

		if (line == Menu_Height-2)
			memcpy(&menu[line*Menu_Width + 21], versioninfo, 4);

		for (byte = 0; byte < Menu_Width; byte++)
			RenderCharFB(menu[line*Menu_Width + byte], &atrtable[menuatr[line*Menu_Width + byte] - '0' + ATR_MENU0]);

		PosY += fontheight;
	}
	national_subset = national_subset_bak;
	Menu_HighlightLine(menu, MenuLine[menuitem], 1);
	Menu_UpdateHotlist(menu, hotindex, menuitem);
}

void ConfigMenu(int Init, int source)
{
	printf("[tuxtxt] Menu\n");
	int val, menuitem = M_Start;
	int current_pid = 0;
	int hotindex;
	int oldscreenmode, oldtrans = 0;
	int i;
	int national_subset_bak = national_subset;
	char menu[Menu_Height*Menu_Width];

	if (auto_national && tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage] &&
		tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage]->pageinfo.nationalvalid)
		national_subset = countryconversiontable[tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage]->pageinfo.national];

	if (getpidsdone)
	{
		/* set current vtxt */
		if (tuxtxt_cache.vtxtpid == 0)
			tuxtxt_cache.vtxtpid = pid_table[0].vtxt_pid;
		else
			while(pid_table[current_pid].vtxt_pid != tuxtxt_cache.vtxtpid && current_pid < pids_found)
				current_pid++;
	}

	/* reset to normal mode */
	if (zoommode)
		zoommode = 0;

	if (transpmode)
	{
		oldtrans = transpmode;
		transpmode = 0;
		ClearBB(black);
	}

	oldscreenmode = screenmode;
	if (screenmode)
		SwitchScreenMode(0); /* turn off divided screen */

	hotindex = getIndexOfPageInHotlist();

	/* clear framebuffer */
	ClearFB(transp);
	clearbbcolor = black;
	Menu_Init(menu, current_pid, menuitem, hotindex);

	/* set blocking mode */
	val = fcntl(rc, F_GETFL);
	fcntl(rc, F_SETFL, val &~ O_NONBLOCK);

	/* loop */
	do {
		if (GetRCCode() == 1)
		{

			if (
#if (RC_1 > 0)
				RCCode >= RC_1 && /* generates a warning... */
#endif
				RCCode <= RC_1+M_MaxDirect) /* direct access */
			{
				Menu_HighlightLine(menu, MenuLine[menuitem], 0);
				menuitem = RCCode-RC_1;
				Menu_HighlightLine(menu, MenuLine[menuitem], 1);

				if (menuitem != M_PID) /* just select */
					RCCode = RC_OK;
			}

			switch (RCCode)
			{
			case RC_UP:
				Menu_HighlightLine(menu, MenuLine[menuitem], 0);
				if (--menuitem < 0)
					menuitem = M_Number-1;
				if (auto_national && (menuitem == M_NAT))
					menuitem--;
				Menu_HighlightLine(menu, MenuLine[menuitem], 1);
				break;

			case RC_DOWN:
				Menu_HighlightLine(menu, MenuLine[menuitem], 0);
				if (++menuitem > M_Number-1)
					menuitem = 0;
				if (auto_national && (menuitem == M_NAT))
					menuitem++;
				Menu_HighlightLine(menu, MenuLine[menuitem], 1);
				break;

			case RC_LEFT:
				switch (menuitem)
				{
				case M_COL:
					saveconfig = 1;
					color_mode--;
					if (color_mode < 1) color_mode = 1;
					menu[MenuLine[M_COL]*Menu_Width +  1] = (color_mode == 1  ? ' ' : '�');
					menu[MenuLine[M_COL]*Menu_Width + 28] = (color_mode == 24 ? ' ' : '�');
					memset(&menu[Menu_Width*MenuLine[M_COL] + 3             ], 0x7f,color_mode);
					memset(&menu[Menu_Width*MenuLine[M_COL] + 3+color_mode  ], 0x20,24-color_mode);
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);
					setcolors((unsigned short *)defaultcolors, 0, SIZECOLTABLE);
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
				case M_TRA:
					saveconfig = 1;
					trans_mode--;
					if (trans_mode < 1) trans_mode = 1;
					menu[MenuLine[M_TRA]*Menu_Width +  1] = (trans_mode == 1  ? ' ' : '�');
					menu[MenuLine[M_TRA]*Menu_Width + 28] = (trans_mode == 24 ? ' ' : '�');
					memset(&menu[Menu_Width*MenuLine[M_TRA] + 3             ], 0x7f,trans_mode);
					memset(&menu[Menu_Width*MenuLine[M_TRA] + 3+trans_mode  ], 0x20,24-trans_mode);
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);
					setcolors((unsigned short *)defaultcolors, 0, SIZECOLTABLE);
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
					
				case M_PID:
				{
					if (!getpidsdone)
					{
						GetTeletextPIDs(source); //FIXME source???
						ClearFB(transp);
						/* set current vtxt */
						if (tuxtxt_cache.vtxtpid == 0)
							tuxtxt_cache.vtxtpid = pid_table[0].vtxt_pid;
						else
							while(pid_table[current_pid].vtxt_pid != tuxtxt_cache.vtxtpid && current_pid < pids_found)
								current_pid++;
						Menu_Init(menu, current_pid, menuitem, hotindex);
					}
					if (current_pid > 0)
					{
						current_pid--;

						memset(&menu[MenuLine[M_PID]*Menu_Width + 3], ' ', 24);

						if (SDT_ready)
						{
							memcpy(&menu[MenuLine[M_PID]*Menu_Width+3+(24-pid_table[current_pid].service_name_len)/2],
							       &pid_table[current_pid].service_name,
							       pid_table[current_pid].service_name_len);
						}
						else
							hex2str(&menu[MenuLine[M_PID]*Menu_Width + 13 + 3], tuxtxt_cache.vtxtpid);

						if (pids_found > 1)
						{
							if (current_pid == 0)
							{
								menu[MenuLine[M_PID]*Menu_Width +  1] = ' ';
								menu[MenuLine[M_PID]*Menu_Width + 28] = '�';
							}
							else
							{
								menu[MenuLine[M_PID]*Menu_Width +  1] = '�';
								menu[MenuLine[M_PID]*Menu_Width + 28] = '�';
							}
						}

						Menu_HighlightLine(menu, MenuLine[menuitem], 1);

						if (auto_national)
						{
							national_subset = pid_table[current_pid].national_subset;

							memcpy(&menu[Menu_Width*MenuLine[M_NAT] + 2], &countrystring[national_subset*COUNTRYSTRING_WIDTH], COUNTRYSTRING_WIDTH);
							Menu_HighlightLine(menu, MenuLine[M_NAT], 0);
						}
					}
					break;
				}

				case M_NAT:
					saveconfig = 1;
					if (national_subset > 0)
					{
						national_subset--;

						if (national_subset == 0)
						{
							menu[MenuLine[M_NAT]*Menu_Width +  1] = ' ';
							menu[MenuLine[M_NAT]*Menu_Width + 28] = '�';
						}
						else
						{
							menu[MenuLine[M_NAT]*Menu_Width +  1] = '�';
							menu[MenuLine[M_NAT]*Menu_Width + 28] = '�';
						}

						Menu_Init(menu, current_pid, menuitem, hotindex);
					}
					break;

				case M_HOT: /* move towards top of hotlist */
					if (hotindex <= 0) /* if not found, start at end */
						hotindex = maxhotlist;
					else
						hotindex--;
					Menu_UpdateHotlist(menu, hotindex, menuitem);
					break;

				case M_LNG:
					saveconfig = 1;
					if (--menulanguage < 0)
						menulanguage = MAXMENULANGUAGE;
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
				} /* switch menuitem */
				break; /* RC_LEFT */

			case RC_RIGHT:
				switch (menuitem)
				{
				case M_COL:
					saveconfig = 1;
					color_mode++;
					if (color_mode > 24) color_mode = 24;
					menu[MenuLine[M_COL]*Menu_Width +  1] = (color_mode == 1  ? ' ' : '�');
					menu[MenuLine[M_COL]*Menu_Width + 28] = (color_mode == 24 ? ' ' : '�');
					memset(&menu[Menu_Width*MenuLine[M_COL] + 3             ], 0x7f,color_mode);
					memset(&menu[Menu_Width*MenuLine[M_COL] + 3+color_mode  ], 0x20,24-color_mode);
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);
					setcolors((unsigned short *)defaultcolors, 0, SIZECOLTABLE);
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
				case M_TRA:
					saveconfig = 1;
					trans_mode++;
					if (trans_mode > 24) trans_mode = 24;
					menu[MenuLine[M_TRA]*Menu_Width +  1] = (trans_mode == 1  ? ' ' : '�');
					menu[MenuLine[M_TRA]*Menu_Width + 28] = (trans_mode == 24 ? ' ' : '�');
					memset(&menu[Menu_Width*MenuLine[M_TRA] + 3             ], 0x7f,trans_mode);
					memset(&menu[Menu_Width*MenuLine[M_TRA] + 3+trans_mode  ], 0x20,24-trans_mode);
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);
					setcolors((unsigned short *)defaultcolors, 0, SIZECOLTABLE);
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
					
				case M_PID:
					if (!getpidsdone)
					{
						GetTeletextPIDs(source); //FIXME: set source also
						ClearFB(transp);
						/* set current vtxt */
						if (tuxtxt_cache.vtxtpid == 0)
							tuxtxt_cache.vtxtpid = pid_table[0].vtxt_pid;
						else
							while(pid_table[current_pid].vtxt_pid != tuxtxt_cache.vtxtpid && current_pid < pids_found)
								current_pid++;
						Menu_Init(menu, current_pid, menuitem, hotindex);
					}
					if (current_pid < pids_found - 1)
					{
						current_pid++;

						memset(&menu[MenuLine[M_PID]*Menu_Width + 3], ' ', 24);

						if (SDT_ready)
							memcpy(&menu[MenuLine[M_PID]*Menu_Width + 3 +
											 (24-pid_table[current_pid].service_name_len)/2],
									 &pid_table[current_pid].service_name,
									 pid_table[current_pid].service_name_len);
						else
							hex2str(&menu[MenuLine[M_PID]*Menu_Width + 13 + 3], pid_table[current_pid].vtxt_pid);

						if (pids_found > 1)
						{
							if (current_pid == pids_found - 1)
							{
								menu[MenuLine[M_PID]*Menu_Width +  1] = '�';
								menu[MenuLine[M_PID]*Menu_Width + 28] = ' ';
							}
							else
							{
								menu[MenuLine[M_PID]*Menu_Width +  1] = '�';
								menu[MenuLine[M_PID]*Menu_Width + 28] = '�';
							}
						}

						Menu_HighlightLine(menu, MenuLine[menuitem], 1);

						if (auto_national)
						{
							if (getpidsdone)
								national_subset = pid_table[current_pid].national_subset;
							memcpy(&menu[Menu_Width*MenuLine[M_NAT] + 2], &countrystring[national_subset*COUNTRYSTRING_WIDTH], COUNTRYSTRING_WIDTH);
							Menu_HighlightLine(menu, MenuLine[M_NAT], 0);
						}
					}
					break;

				case M_NAT:
					saveconfig = 1;
					if (national_subset < (int) MAX_NATIONAL_SUBSET)
					{
						national_subset++;

						if (national_subset == (int) MAX_NATIONAL_SUBSET)
						{
							menu[MenuLine[M_NAT]*Menu_Width +  1] = '�';
							menu[MenuLine[M_NAT]*Menu_Width + 28] = ' ';
						}
						else
						{
							menu[MenuLine[M_NAT]*Menu_Width +  1] = '�';
							menu[MenuLine[M_NAT]*Menu_Width + 28] = '�';
						}

						Menu_Init(menu, current_pid, menuitem, hotindex);
					}
					break;

				case M_HOT: /* select hotindex */
					if ((int)hotindex >= maxhotlist) /* if not found, start at 0 */
						hotindex = 0;
					else
						hotindex++;
					Menu_UpdateHotlist(menu, hotindex, menuitem);
					break;

				case M_LNG:
					saveconfig = 1;
					if (++menulanguage > MAXMENULANGUAGE)
						menulanguage = 0;
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
				}
				break; /* RC_RIGHT */
#if 0
			case RC_PLUS:
				switch (menuitem)
				{
				case M_HOT: /* move towards end of hotlist */
				{
					if (hotindex<0) /* not found: add page at end */
					{
						if (maxhotlist < (int) (sizeof(hotlist)/sizeof(hotlist[0])-1)) /* only if still room left */
						{
							hotindex = ++maxhotlist;
							hotlist[hotindex] = tuxtxt_cache.page;
							hotlistchanged = 1;
							Menu_UpdateHotlist(menu, hotindex, menuitem);
						}
					}
					else /* found */
					{
						if (hotindex < maxhotlist) /* not already at end */
						{
							int temp = hotlist[hotindex];
							hotlist[hotindex] = hotlist[hotindex+1];
							hotlist[hotindex+1] = temp;
							hotindex++;
							hotlistchanged = 1;
							Menu_UpdateHotlist(menu, hotindex, menuitem);
						}
					}
				}
				break;
				}
				break;  /* RC_PLUS */
#endif

			case RC_MINUS:
				switch (menuitem)
				{
				case M_HOT: /* move towards top of hotlist */
				{
					if (hotindex<0) /* not found: add page at top */
					{
						if (maxhotlist < (int) (sizeof(hotlist)/sizeof(hotlist[0])-1)) /* only if still room left */
						{
							for (hotindex = maxhotlist; hotindex >= 0; hotindex--) /* move rest of list */
							{
								hotlist[hotindex+1] = hotlist[hotindex];
							}
							maxhotlist++;
							hotindex = 0;
							hotlist[hotindex] = tuxtxt_cache.page;
							hotlistchanged = 1;
							Menu_UpdateHotlist(menu, hotindex, menuitem);
						}
					}
					else /* found */
					{
						if (hotindex > 0) /* not already at front */
						{
							int temp = hotlist[hotindex];
							hotlist[hotindex] = hotlist[hotindex-1];
							hotlist[hotindex-1] = temp;
							hotindex--;
							hotlistchanged = 1;
							Menu_UpdateHotlist(menu, hotindex, menuitem);
						}
					}
				}
				break;
				}
				break; /* RC_MINUS */

			case RC_HELP:
				switch (menuitem)
				{
				case M_HOT: /* current page is added to / removed from hotlist */
				{
					if (hotindex<0) /* not found: add page */
					{
						if (maxhotlist < (int) (sizeof(hotlist)/sizeof(hotlist[0])-1)) /* only if still room left */
						{
							hotlist[++maxhotlist] = tuxtxt_cache.page;
							hotindex = maxhotlist;
							hotlistchanged = 1;
							Menu_UpdateHotlist(menu, hotindex, menuitem);
						}
					}
					else /* found: remove */
					{
						if (maxhotlist > 0) /* don't empty completely */
						{
							int i;

							for (i=hotindex; i<maxhotlist; i++) /* move rest of list */
							{
								hotlist[i] = hotlist[i+1];
							}
							maxhotlist--;
							if (hotindex > maxhotlist)
								hotindex = maxhotlist;
							hotlistchanged = 1;
							Menu_UpdateHotlist(menu, hotindex, menuitem);
						}
					}
				}
				break;
				case M_PID:
					showhex ^= 1;
					menu[MenuLine[M_PID]*Menu_Width + 27] = (showhex ? '?' : ' ');
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);
				break;
#if TUXTXT_DEBUG
				case M_LNG:
					charpage();
					ClearFB(transp);
					Menu_Init(menu, current_pid, menuitem, hotindex);
				break;
#endif
				}
				break; /* RC_MUTE */

			case RC_OK:
				switch (menuitem)
				{
				case M_PID:
					if (!getpidsdone)
					{
						GetTeletextPIDs(source); //FIXME: source???
						ClearFB(transp);
						/* set current vtxt */
						if (tuxtxt_cache.vtxtpid == 0)
							tuxtxt_cache.vtxtpid = pid_table[0].vtxt_pid;
						else
							while(pid_table[current_pid].vtxt_pid != tuxtxt_cache.vtxtpid && current_pid < pids_found)
								current_pid++;
						Menu_Init(menu, current_pid, menuitem, hotindex);
					}
					else if (pids_found > 1)
					{
							if (hotlistchanged)
								savehotlist();

						if (Init || tuxtxt_cache.vtxtpid != pid_table[current_pid].vtxt_pid)
							{
#if TUXTXT_CFG_STANDALONE
								tuxtxt_stop_thread();
								tuxtxt_clear_cache();
#else
								tuxtxt_stop();
							if (Init)
								tuxtxt_cache.vtxtpid = 0; // force clear cache
#endif
								/* reset data */


								//page_atrb[32] = transp<<4 | transp;
								inputcounter = 2;


								tuxtxt_cache.page     = 0x100;
								lastpage = 0x100;
								prev_100 = 0x100;
								prev_10  = 0x100;
								next_100 = 0x100;
								next_10  = 0x100;
								tuxtxt_cache.subpage  = 0;

								tuxtxt_cache.pageupdate = 0;
								tuxtxt_cache.zap_subpage_manual = 0;
								hintmode = 0;
								memset(page_char,' ',40 * 25);

								for (i = 0; i < 40*25; i++)
								{
									page_atrb[i].fg = transp;
									page_atrb[i].bg = transp;
								}
								ClearFB(transp);


								/* start demuxer with new vtxtpid */
								if (auto_national)
									national_subset = pid_table[current_pid].national_subset;

#if TUXTXT_CFG_STANDALONE
								tuxtxt_cache.vtxtpid = pid_table[current_pid].vtxt_pid;
								tuxtxt_start_thread(source); //FIXME: source???
#else
								tuxtxt_start(pid_table[current_pid].vtxt_pid, source); //FIXME: source
#endif
							}
//							tuxtxt_cache.pageupdate = 1;

							ClearBB(black);
							gethotlist();

						/* show new teletext */
						current_service = current_pid;
//						RenderMessage(ShowServiceName);

						fcntl(rc, F_SETFL, O_NONBLOCK);
						RCCode = -1;
						if (oldscreenmode)
							SwitchScreenMode(oldscreenmode); /* restore divided screen */

						printf("[tuxtxt] Menu return from M_PID\n");
						transpmode = oldtrans;
						return;
					}
					break;

				case M_SC1:
					saveconfig = 1;
					screen_mode1++;
					screen_mode1 &= 1;

					memcpy(&menu[Menu_Width*MenuLine[M_SC1] + Menu_Width - 5], &configonoff[menulanguage][screen_mode1  ? 3 : 0], 3);
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);

					break;

				case M_SC2:
					saveconfig = 1;
					screen_mode2++;
					screen_mode2 &= 1;

					memcpy(&menu[Menu_Width*MenuLine[M_SC2] + Menu_Width - 5], &configonoff[menulanguage][screen_mode2  ? 3 : 0], 3);
					Menu_HighlightLine(menu, MenuLine[menuitem], 1);
					break;

				case M_AUN:
					saveconfig = 1;
					auto_national++;
					auto_national &= 1;
					if (auto_national)
					{
					 	if (getpidsdone)
							national_subset = pid_table[current_pid].national_subset;
						else
						{
							if (tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage] &&
								tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage]->pageinfo.nationalvalid)
								national_subset = countryconversiontable[tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage]->pageinfo.national];
							else
								national_subset = national_subset_bak;
						}

					}
					Menu_Init(menu, current_pid, menuitem, hotindex);
					break;
				case M_HOT: /* show selected page */
				{
					if (hotindex >= 0) /* not found: ignore */
					{
						lastpage = tuxtxt_cache.page;
						tuxtxt_cache.page = hotlist[hotindex];
						tuxtxt_cache.subpage = tuxtxt_cache.subpagetable[tuxtxt_cache.page];
						inputcounter = 2;
						tuxtxt_cache.pageupdate = 1;
						RCCode = RC_HOME;		 /* leave menu */
					}
				}
				break;
				} /* RC_OK */
				break;
			}
		}
		UpdateLCD(); /* update number of cached pages */
		
#ifdef FB_BLIT	
		CFrameBuffer::getInstance()->blit();
#endif		
	} while ((RCCode != RC_HOME) && (RCCode != RC_DBOX) && (RCCode != RC_MUTE));

	/* reset to nonblocking mode */
	fcntl(rc, F_SETFL, O_NONBLOCK);
	tuxtxt_cache.pageupdate = 1;
	RCCode = -1;
	if (oldscreenmode)
		SwitchScreenMode(oldscreenmode); /* restore divided screen */

	transpmode = oldtrans;
	printf("[tuxtxt] Menu return\n");
}

/*
 * PageInput
*/
void PageInput(int Number)
{
	int zoom = 0;

	/* clear temp_page */
	if (inputcounter == 2)
		temp_page = 0;

	/* check for 0 & 9 on first position */
	if (Number == 0 && inputcounter == 2)
	{
		/* set page */
		temp_page = lastpage; /* 0 toggles to last page as in program switching */
		inputcounter = -1;
	}
	else if (Number == 9 && inputcounter == 2)
	{
		/* set page */
		temp_page = getIndexOfPageInHotlist(); /* 9 toggles through hotlist */

		if (temp_page<0 || temp_page==maxhotlist) /* from any (other) page go to first page in hotlist */
			temp_page = (maxhotlist >= 0) ? hotlist[0] : 0x100;
		else
			temp_page = hotlist[temp_page+1];

		inputcounter = -1;
	}

	/* show pageinput */
	if (zoommode == 2)
	{
		zoommode = 1;
		CopyBB2FB();
	}

	if (zoommode == 1)
		zoom = 1<<10;

	PosY = StartY;

	switch (inputcounter)
	{
		case 2:
			SetPosX(1);
			RenderCharFB(Number | '0', &atrtable[ATR_WB]);
			RenderCharFB('-', &atrtable[ATR_WB]);
			RenderCharFB('-', &atrtable[ATR_WB]);
			break;
	
		case 1:
			SetPosX(2);
			RenderCharFB(Number | '0', &atrtable[ATR_WB]);
			break;
	
		case 0:
			SetPosX(3);
			RenderCharFB(Number | '0', &atrtable[ATR_WB]);
			break;
	}

	/* generate pagenumber */
	temp_page |= Number << inputcounter*4;

	inputcounter--;

	if (inputcounter < 0)
	{
		/* disable subpage zapping */
		tuxtxt_cache.zap_subpage_manual = 0;

		/* reset input */
		inputcounter = 2;

		/* set new page */
		lastpage = tuxtxt_cache.page;

		tuxtxt_cache.page = temp_page;
		hintmode = 0;

		/* check cache */
		int subp = tuxtxt_cache.subpagetable[tuxtxt_cache.page];
		if (subp != 0xFF)
		{
			tuxtxt_cache.subpage = subp;
			tuxtxt_cache.pageupdate = 1;
#if TUXTXT_DEBUG
			printf("TuxTxt <DirectInput: %.3X-%.2X>\n", tuxtxt_cache.page, tuxtxt_cache.subpage);
#endif
		}
		else
		{
			tuxtxt_cache.subpage = 0;
//			RenderMessage(PageNotFound);
#if TUXTXT_DEBUG
			printf("TuxTxt <DirectInput: %.3X not found>\n", tuxtxt_cache.page);
#endif
		}
	}
}

/*
 * GetNextPageOne
*/
void GetNextPageOne(int up)
{
	/* disable subpage zapping */
	tuxtxt_cache.zap_subpage_manual = 0;

	/* abort pageinput */
	inputcounter = 2;

	/* find next cached page */
	lastpage = tuxtxt_cache.page;

	int subp;
	do {
		if (up)
			tuxtxt_next_dec(&tuxtxt_cache.page);
		else
			tuxtxt_prev_dec(&tuxtxt_cache.page);
		subp = tuxtxt_cache.subpagetable[tuxtxt_cache.page];
	} while (subp == 0xFF && tuxtxt_cache.page != lastpage);

	/* update page */
	if (tuxtxt_cache.page != lastpage)
	{
		if (zoommode == 2)
			zoommode = 1;

		tuxtxt_cache.subpage = subp;
		hintmode = 0;
		tuxtxt_cache.pageupdate = 1;
#if TUXTXT_DEBUG
		printf("TuxTxt <NextPageOne: %.3X-%.2X>\n", tuxtxt_cache.page, tuxtxt_cache.subpage);
#endif
	}
}

/* GetNextSubPage */
void GetNextSubPage(int offset)
{
	int loop;

	/* abort pageinput */
	inputcounter = 2;

	for (loop = tuxtxt_cache.subpage + offset; loop != tuxtxt_cache.subpage; loop += offset)
	{
		if (loop < 0)
			loop = 0x79;
		else if (loop > 0x79)
			loop = 0;
		if (loop == tuxtxt_cache.subpage)
			break;

		if (tuxtxt_cache.astCachetable[tuxtxt_cache.page][loop])
		{
			/* enable manual subpage zapping */
			tuxtxt_cache.zap_subpage_manual = 1;

			/* update page */
			if (zoommode == 2) /* if zoomed to lower half */
				zoommode = 1; /* activate upper half */

			tuxtxt_cache.subpage = loop;
			hintmode = 0;
			tuxtxt_cache.pageupdate = 1;
#if TUXTXT_DEBUG
			printf("TuxTxt <NextSubPage: %.3X-%.2X>\n", tuxtxt_cache.page, tuxtxt_cache.subpage);
#endif
			return;
		}
	}

#if TUXTXT_DEBUG
	printf("TuxTxt <NextSubPage: no other SubPage>\n");
#endif
}

/*
 * ColorKey
*/
void ColorKey(int target)
{
	if (!target)
		return;
	if (zoommode == 2)
		zoommode = 1;
	lastpage     = tuxtxt_cache.page;
	tuxtxt_cache.page         = target;
	tuxtxt_cache.subpage      = tuxtxt_cache.subpagetable[tuxtxt_cache.page];
	inputcounter = 2;
	hintmode     = 0;
	tuxtxt_cache.pageupdate   = 1;
#if TUXTXT_DEBUG
	printf("TuxTxt <ColorKey: %.3X>\n", tuxtxt_cache.page);
#endif
}

/*
 * PageCatching
*/
void PageCatching()
{
	int val, byte;
	int oldzoommode = zoommode;

	pagecatching = 1;

	/* abort pageinput */
	inputcounter = 2;

	/* show info line */
	zoommode = 0;
	PosX = StartX;
	PosY = StartY + 24*fontheight;
	for (byte = 0; byte < 40-nofirst; byte++)
		RenderCharFB(catchmenutext[menulanguage][byte], &atrtable[catchmenutext[menulanguage][byte+40] - '0' + ATR_CATCHMENU0]);
	zoommode = oldzoommode;

	/* check for pagenumber(s) */
	catch_row    = 1;
	catch_col    = 0;
	catched_page = 0;
	pc_old_row = pc_old_col = 0; /* no inverted page number to restore yet */
	CatchNextPage(0, 1);

	if (!catched_page)
	{
		pagecatching = 0;
		tuxtxt_cache.pageupdate = 1;
		return;
	}

	/* set blocking mode */
	val = fcntl(rc, F_GETFL);
	fcntl(rc, F_SETFL, val &~ O_NONBLOCK);
	
#ifdef FB_BLIT
	CFrameBuffer::getInstance()->blit();
#endif

	/* loop */
	do {
		GetRCCode();

		switch (RCCode)
		{
		case RC_LEFT:
			CatchNextPage(0, -1);
			break;
		case RC_RIGHT:
			CatchNextPage(0, 1);
			break;
		case RC_UP:
			CatchNextPage(-1, -1);
			break;
		case RC_DOWN:
			CatchNextPage(1, 1);
			break;
		case RC_0:
		case RC_1:
		case RC_2:
		case RC_3:
		case RC_4:
		case RC_5:
		case RC_6:
		case RC_7:
		case RC_8:
		case RC_9:
		case RC_RED:
		case RC_GREEN:
		case RC_YELLOW:
		case RC_BLUE:
		case RC_PLUS:
		case RC_MINUS:
		case RC_DBOX:
		case RC_HOME:
		case RC_HELP:
		case RC_MUTE:
			fcntl(rc, F_SETFL, O_NONBLOCK);
			tuxtxt_cache.pageupdate = 1;
			pagecatching = 0;
			RCCode = -1;
			return;
		}

		UpdateLCD();
		
#ifdef FB_BLIT
		CFrameBuffer::getInstance()->blit();
#endif

	} while (RCCode != RC_OK);

	/* set new page */
	if (zoommode == 2)
		zoommode = 1;

	lastpage     = tuxtxt_cache.page;
	tuxtxt_cache.page         = catched_page;
	hintmode = 0;
	tuxtxt_cache.pageupdate = 1;
	pagecatching = 0;

	int subp = tuxtxt_cache.subpagetable[tuxtxt_cache.page];
	if (subp != 0xFF)
		tuxtxt_cache.subpage = subp;
	else
		tuxtxt_cache.subpage = 0;

	/* reset to nonblocking mode */
	fcntl(rc, F_SETFL, O_NONBLOCK);
}

/*
 * CatchNextPage
*/
void CatchNextPage(int firstlineinc, int inc)
{
	int tmp_page, allowwrap = 1; /* allow first wrap around */

	/* catch next page */
	for(;;)
	{
		unsigned char *p = &(page_char[catch_row*40 + catch_col]);
		tstPageAttr a = page_atrb[catch_row*40 + catch_col];

		if (!(a.charset == C_G1C || a.charset == C_G1S) && /* no mosaic */
			 (a.fg != a.bg) && /* not hidden */
			 (*p >= '1' && *p <= '8' && /* valid page number */
			  *(p+1) >= '0' && *(p+1) <= '9' &&
			  *(p+2) >= '0' && *(p+2) <= '9') &&
			 (catch_row == 0 || (*(p-1) < '0' || *(p-1) > '9')) && /* non-numeric char before and behind */
			 (catch_row == 37 || (*(p+3) < '0' || *(p+3) > '9')))
		{
			tmp_page = ((*p - '0')<<8) | ((*(p+1) - '0')<<4) | (*(p+2) - '0');

#if 0
			if (tmp_page != catched_page)	/* confusing to skip identical page numbers - I want to reach what I aim to */
#endif
			{
				catched_page = tmp_page;
				RenderCatchedPage();
				catch_col += inc;	/* FIXME: limit */
#if TUXTXT_DEBUG
				printf("TuxTxt <PageCatching: %.3X\n", catched_page);
#endif
				return;
			}
		}

		if (firstlineinc > 0)
		{
			catch_row++;
			catch_col = 0;
			firstlineinc = 0;
		}
		else if (firstlineinc < 0)
		{
			catch_row--;
			catch_col = 37;
			firstlineinc = 0;
		}
		else
			catch_col += inc;

		if (catch_col > 37)
		{
			catch_row++;
			catch_col = 0;
		}
		else if (catch_col < 0)
		{
			catch_row--;
			catch_col = 37;
		}

		if (catch_row > 23)
		{
			if (allowwrap)
			{
				allowwrap = 0;
				catch_row = 1;
				catch_col = 0;
			}
			else
			{
#if TUXTXT_DEBUG
				printf("TuxTxt <PageCatching: no PageNumber>\n");
#endif
				return;
			}
		}
		else if (catch_row < 1)
		{
			if (allowwrap)
			{
				allowwrap = 0;
				catch_row = 23;
				catch_col =37;
			}
			else
			{
#if TUXTXT_DEBUG
				printf("TuxTxt <PageCatching: no PageNumber>\n");
#endif
				return;
			}
		}
	}
}

/*
 * RenderCatchedPage
*/
void RenderCatchedPage()
{
	int zoom = 0;

	/* handle zoom */
	if (zoommode)
		zoom = 1<<10;

	if (pc_old_row || pc_old_col) /* not at first call */
	{
		/* restore pagenumber */
		SetPosX(pc_old_col);

		if (zoommode == 2)
			PosY = StartY + (pc_old_row-12)*fontheight*((zoom>>10)+1);
		else
			PosY = StartY + pc_old_row*fontheight*((zoom>>10)+1);

		RenderCharFB(page_char[pc_old_row*40 + pc_old_col    ], &page_atrb[pc_old_row*40 + pc_old_col    ]);
		RenderCharFB(page_char[pc_old_row*40 + pc_old_col + 1], &page_atrb[pc_old_row*40 + pc_old_col + 1]);
		RenderCharFB(page_char[pc_old_row*40 + pc_old_col + 2], &page_atrb[pc_old_row*40 + pc_old_col + 2]);
	}

	pc_old_row = catch_row;
	pc_old_col = catch_col;

	/* mark pagenumber */
	if (zoommode == 1 && catch_row > 11)
	{
		zoommode = 2;
		CopyBB2FB();
	}
	else if (zoommode == 2 && catch_row < 12)
	{
		zoommode = 1;
		CopyBB2FB();
	}
	SetPosX(catch_col);


	if (zoommode == 2)
		PosY = StartY + (catch_row-12)*fontheight*((zoom>>10)+1);
	else
		PosY = StartY + catch_row*fontheight*((zoom>>10)+1);

	tstPageAttr a0 = page_atrb[catch_row*40 + catch_col    ];
	tstPageAttr a1 = page_atrb[catch_row*40 + catch_col + 1];
	tstPageAttr a2 = page_atrb[catch_row*40 + catch_col + 2];
	int t;

	/* exchange colors */
	t = a0.fg; a0.fg = a0.bg; a0.bg = t;
	t = a1.fg; a1.fg = a1.bg; a1.bg = t;
	t = a2.fg; a2.fg = a2.bg; a2.bg = t;

	RenderCharFB(page_char[catch_row*40 + catch_col    ], &a0);
	RenderCharFB(page_char[catch_row*40 + catch_col + 1], &a1);
	RenderCharFB(page_char[catch_row*40 + catch_col + 2], &a2);
}

/*
 * SwitchZoomMode
*/
void SwitchZoomMode()
{
	if (tuxtxt_cache.subpagetable[tuxtxt_cache.page] != 0xFF)
	{
		/* toggle mode */
		zoommode++;

		if (zoommode == 3)
			zoommode = 0;

#if TUXTXT_DEBUG
		printf("TuxTxt <SwitchZoomMode: %d>\n", zoommode);
#endif
		/* update page */
		tuxtxt_cache.pageupdate = 1; /* FIXME */
	}
}

/*
 * SwitchScreenMode
*/
void getCurrentPIGSettings(int* left, int* top, int* width, int* height)
{
	FILE* fd;
	
	fd = fopen("/proc/stb/vmpeg/0/dst_left", "r");
	fscanf(fd, "%x", left);
	fclose(fd);

	fd = fopen("/proc/stb/vmpeg/0/dst_top", "r");
	fscanf(fd, "%x", top);
	fclose(fd);

	fd = fopen("/proc/stb/vmpeg/0/dst_width", "r");
	fscanf(fd, "%x", width);
	fclose(fd);

	fd = fopen("/proc/stb/vmpeg/0/dst_height", "r");
	fscanf(fd, "%x", height);
	fclose(fd);

	fprintf(stderr, "%s: top %d, left %d, width %d, height %d\n", __func__, *left, *top, *width, *height);
}

void getCurrentPOLICYSettings()
{
	FILE* fd;
	fd = fopen("/proc/stb/video/policy", "r");
	fscanf(fd, "%s", policy);
	fclose(fd);
}

void getCurrentASPECTSettings()
{
	FILE* fd;
	
	fd = fopen("/proc/stb/video/aspect", "r");
	fscanf(fd, "%s", aspect);
	fclose(fd);
}

void setCurrentPIGSettings(int left, int top, int width, int height)
{
	int _x, _y, _w, _h;
	int osd_w = CFrameBuffer::getInstance()->getScreenWidth(true);
	int osd_h = CFrameBuffer::getInstance()->getScreenHeight(true);
	/* the target "coordinates" seem to be in a PAL sized plane
	 * TODO: check this in the driver sources */
	int xres = 720; /* proc_get_hex("/proc/stb/vmpeg/0/xres") */
	int yres = 576; /* proc_get_hex("/proc/stb/vmpeg/0/yres") */
	
	if (left == -1 && top == -1 && width == -1 && height == -1)
	{
		_w = xres;
		_h = yres;
		_x = 0;
		_y = 0;
	}
	else
	{
		_x = left * xres / osd_w;
		_w = width * xres / osd_w;
		_y = top * yres / osd_h;
		_h = height * yres / osd_h;
	}
	//
	FILE* fd;
	
	fprintf(stderr, "%s: top %d, left %d, width %d, height %d\n", __func__, left, top, width, height);

	fd = fopen("/proc/stb/vmpeg/0/dst_left", "w");
	fprintf(fd, "%x", _x/*left*/);
	fclose(fd);

	fd = fopen("/proc/stb/vmpeg/0/dst_top", "w");
	fprintf(fd, "%x", _y/*top*/);
	fclose(fd);

	fd = fopen("/proc/stb/vmpeg/0/dst_width", "w");
	fprintf(fd, "%x", _w/*width*/);
	fclose(fd);

	fd = fopen("/proc/stb/vmpeg/0/dst_height", "w");
	fprintf(fd, "%x", _h/*height*/);
	fclose(fd);
}

// new
void getCurrentPictureInfo(int &width, int &height, int &rate)
{
  	unsigned char buffer[10];
	int n, fd;	

	// framerate
	rate = 0;
	fd = open("/proc/stb/vmpeg/0/framerate", O_RDONLY);
	n = read(fd, buffer, 10);
	close(fd);

	if (n > 0) 
	{
#ifdef __sh__	  
		sscanf((const char*) buffer, "%X", &rate);
#else
		sscanf((const char*) buffer, "%d", &rate);
#endif		
		rate = rate/1000;
	}

	// width (xres)
	width = 0;
	fd = open("/proc/stb/vmpeg/0/xres", O_RDONLY);
	n = read(fd, buffer, 10);
	close(fd);

	if (n > 0) 
	{
		sscanf((const char*) buffer, "%X", &width);
	}

	// height  (yres)
	height = 0;
	fd = open("/proc/stb/vmpeg/0/yres", O_RDONLY);
	n = read(fd, buffer, 10);
	close(fd);

	if (n > 0) 
	{
		sscanf((const char*) buffer, "%X", &height);
	}
}

void setCurrentPOLICYSettings(int reset)
{
	FILE* fd;
	fd = fopen("/proc/stb/video/policy", "w");
	if (reset == 1)
	    fprintf(fd, "%s", policy);
	fclose(fd);
}

void setCurrentASPECTSettings(int reset)
{
	FILE* fd;
	
	fd = fopen("/proc/stb/video/aspect", "w");
	if (reset == 1)
	   fprintf(fd, "%s", aspect);
	else
	   fprintf(fd, "4:3");
	fclose(fd);
}

void SwitchScreenMode(int newscreenmode)
{

	//struct v4l2_format format;

	debugf(1, "%s: >\n", __func__);

	// reset transparency mode
	if (transpmode)
		transpmode = 0;

	if (newscreenmode < 0) // toggle mode
		screenmode++;
	else // set directly
		screenmode = newscreenmode;
	//if ((screenmode > (screen_mode2 ? 2 : 1)) || (screenmode < 0))
	if ((screenmode > 2) || (screenmode < 0))
		screenmode = 0;

#if TUXTXT_DEBUG
	printf("TuxTxt <SwitchScreenMode: %d>\n", screenmode);
#endif

	// update page
	tuxtxt_cache.pageupdate = 1;

	// clear back buffer
	//clearbbcolor = screenmode ? transp : FullScrColor;

	//ClearBB(clearbbcolor);
	//TEST
	/* clear back buffer */
	clearbbcolor = screenmode?transp:static_cast<int>(FullScrColor);

	if(use_gui)
		ClearBB(clearbbcolor);

	fprintf(stderr, "screenmode %d\n", screenmode); 

	// set mode
	if (screenmode)								 // split
	{
		fprintf(stderr, "[TuxTxt] Slit Mode (1.)\n");
		
		ClearFB(clearbbcolor);

		int fw, fh, tx, ty, tw, th;

		fprintf(stderr, "Settup video picture\n");
		
		// ugly hack
		// dont calcultae rate when we have HD until to fix this
		int xres, yres, framerate;

		if (screenmode==1) // split with topmenu
		{
			fw = fontwidth_topmenumain;
			fh = fontheight;

			tw = TV43WIDTH;
			displaywidth = (TV43STARTX - sx);
			StartX = sx;
			//test
			//StartX = sx + (((ex-sx) - (40*fw+2+tw)) / 2); 

			tx = TV43STARTX;
			ty = TV43STARTY;
			th = TV43HEIGHT;

			// ugly hack
			// dont calcultae rate when we have HD until to fix this
			//int xres, yres, framerate;
#if 0			
			getCurrentPictureInfo(xres, yres, framerate);
	
			if(xres < 1280)
#endif			  
				// set pig
				setCurrentPIGSettings(tx, ty, tw, th);
		}
		else  // 2: split with full height tv picture
		{
			fw = fontwidth_small;
			fh = fontheight;

			tx = TV169FULLSTARTX;
			ty = TV169FULLSTARTY;
			tw = TV169FULLWIDTH;
			th = TV169FULLHEIGHT;

			displaywidth= (TV169FULLSTARTX - sx);
			//displaywidth = (sx +(ex - tw));

#if 0			
			getCurrentPictureInfo(xres, yres, framerate);
	
			if(xres < 1280)
#endif			  
			// set pig	
			setCurrentPIGSettings(tx, ty, tw, th);
		}

		setfontwidth(fw);

	}
	else // not split
	{
		fprintf(stderr, "[TuxTxt] Full txt Screen (2).\n"); 

		// set pig
		setCurrentPIGSettings(-1, -1, -1, -1);		

		setfontwidth(fontwidth_normal);
		displaywidth= (ex - sx);
		StartX = sx;
	}

	debugf(1, "%s: <\n", __func__);
}

/*
 * SwitchTranspMode
*/
void SwitchTranspMode()
{
	debugf(1, "%s: >\n", __func__);

	if (screenmode)
	{
		prevscreenmode = screenmode;
		SwitchScreenMode(0); /* turn off divided screen */
	}

	/* toggle mode */
	if (!transpmode)
		transpmode = 2;
	else
		transpmode--; /* backward to immediately switch to TV-screen */

#if TUXTXT_DEBUG
	printf("TuxTxt <SwitchTranspMode: %d>\n", transpmode);
#endif

	/* set mode */
	if (!transpmode) /* normal text-only */
	{
		ClearBB(FullScrColor);
		tuxtxt_cache.pageupdate = 1;
	}
	else if (transpmode == 1) /* semi-transparent BG with FG text */
	{
		ClearBB(transp);
		tuxtxt_cache.pageupdate = 1;
	}
	else /* TV mode */
	{
		ClearFB(transp);
		clearbbcolor = FullScrColor;
	}

	debugf(1, "%s: <\n", __func__);
}

/*
 * SwitchHintMode
*/
void SwitchHintMode()
{
	/* toggle mode */
	hintmode ^= 1;
#if TUXTXT_DEBUG
	printf("TuxTxt <SwitchHintMode: %d>\n", hintmode);
#endif

	if (!hintmode)	/* toggle evaluation of level 2.5 information by explicitly switching off hintmode */
	{
		showl25 ^= 1;
#if TUXTXT_DEBUG
		printf("TuxTxt <ShowLevel2p5: %d>\n", showl25);
#endif
	}
	/* update page */
	tuxtxt_cache.pageupdate = 1;
}

void RenderDRCS( //FIXME
	unsigned char *s,	/* pointer to char data, parity undecoded */
	unsigned char *d,	/* pointer to frame buffer of top left pixel */
	unsigned char *ax, /* array[0..12] of x-offsets, array[0..10] of y-offsets for each pixel */
	unsigned char fgcolor, unsigned char bgcolor)
{
	int bit, x, y, ltmp;
	unsigned char *ay = ax + 13; /* array[0..10] of y-offsets for each pixel */

	debugf(1, "FIXME: SETPIXEL: %s: >\n", __func__);

	for (y = 0; y < 10; y++) /* 10*2 bytes a 6 pixels per char definition */
	{
		unsigned char c1 = deparity[*s++];
		unsigned char c2 = deparity[*s++];
		int h = ay[y+1] - ay[y];

		if (!h)
			continue;
		if (((c1 == ' ') && (*(s-2) != ' ')) || ((c2 == ' ') && (*(s-1) != ' '))) /* parity error: stop decoding FIXME */
			return;
		for (bit = 0x20, x = 0;
			  bit;
			  bit >>= 1, x++)	/* bit mask (MSB left), column counter */
		{
			int i, f1, f2;

			f1 = (c1 & bit) ? fgcolor : bgcolor;
			f2 = (c2 & bit) ? fgcolor : bgcolor;

			for (i = 0; i < h; i++)
			{
				if (ax[x+1] > ax[x])
				{
//					memset(d + ax[x], f1, ax[x+1] - ax[x]);
					for (ltmp=0 ; ltmp < (ax[x+1]-ax[x]); ltmp++)
					{
						memcpy(d + ax[x]*4 +ltmp*4,bgra[f1],4);
					}
				}
				if (ax[x+7] > ax[x+6])
				{
					//memset(d + ax[x+6], f2, ax[x+7] - ax[x+6]); /* 2nd byte 6 pixels to the right */
					for (ltmp=0 ; ltmp < (ax[x+7]-ax[x+6]); ltmp++)
					{
						memcpy(d + ax[x+6]*4 +ltmp*4,bgra[f2],4);
					}

				}
				
				d += (CFrameBuffer::getInstance()->getScreenWidth(true) * 4);
			}
			
			d -= h *(CFrameBuffer::getInstance()->getScreenWidth(true) * 4);
		}
		
		d += h * (CFrameBuffer::getInstance()->getScreenWidth(true) * 4);
	}

	debugf(1, "%s: <\n", __func__);
}

void DrawVLine(int x, int y, int l, int color)
{
	#if 0
	int ytemp;

	debugf(20, "%s: >\n", __func__);

	for (ytemp = 0; ytemp < l ; ytemp++)
	{
		setPixel(x, y + ytemp, color);
	}
	#endif
	
	#if 1
	unsigned char *p = lfb + x*4 + y * CFrameBuffer::getInstance()->getStride();

	for ( ; l > 0 ; l--)
	{
		memmove(p,bgra[color],4);
		p += CFrameBuffer::getInstance()->getStride();
	}
	#endif

	debugf(20, "%s: <\n", __func__);
}

void DrawHLine(int x, int y, int l, int color)
{
	#if 0
	int ltmp;

	debugf(20, "%s: >\n", __func__);

	if (l > 0)
	{
		for (ltmp=0; ltmp < l; ltmp++)
		{
			setPixel(x + ltmp, y, color);
		}
	}
	#endif
	
	#if 1
	int ltmp;
	if (l > 0)
	{
		for (ltmp=0; ltmp <= l; ltmp++)
		{
			memmove(lfb + x*4 + ltmp*4 + y * CFrameBuffer::getInstance()->getStride(), bgra[color], 4);
		}
	}
	#endif

	debugf(20, "%s: <\n", __func__);
}

void FillRectMosaicSeparated(int x, int y, int w, int h, int fgcolor, int bgcolor, int set)
{
	debugf(20, "%s: >\n", __func__);

	FillRect(x, y, w, h, bgcolor);
	if (set)
	{
		FillRect(x+1, y+1, w-2, h-2, fgcolor);
	}

	debugf(20, "%s: <\n", __func__);
}

void FillTrapez(int x0, int y0, int l0, int xoffset1, int h, int l1, int color)
{
	unsigned char *p = lfb + x0*4 + y0 * (CFrameBuffer::getInstance()->getScreenWidth(true) *4);
	
	int xoffset, l;
	int yoffset;
	int ltmp;

	debugf(20, "%s: >\n", __func__);

	for (yoffset = 0; yoffset < h; yoffset++)
	{
		l = l0 + ((l1-l0) * yoffset + h/2) / h;
		xoffset = (xoffset1 * yoffset + h/2) / h;
		if (l > 0)
		{
			for (ltmp=0; ltmp < l; ltmp++)
			{
				//setPixel(xoffset + ltmp + x0, yoffset + y0, color);
				memcpy(p + xoffset * 4 + ltmp * 4, bgra[color], 4);
			}
		}

		p += (CFrameBuffer::getInstance()->getScreenWidth(true) * 4);
	}

	debugf(20, "%s: <\n", __func__);

}

void FlipHorz(int x, int y, int w, int h)
{
	unsigned char buf[w*4];
	
	unsigned char *p = lfb + x*4 + y * (CFrameBuffer::getInstance()->getScreenWidth(true)*4);

	int w1,h1;

	debugf(20, "%s: >\n", __func__);

	for (h1 = 0 ; h1 < h ; h1++)
	{
		memcpy(buf,p,w*4);
		for (w1 = 0 ; w1 < w ; w1++)
		{
			if (w1 + x > CFrameBuffer::getInstance()->getScreenWidth(true))
				fprintf(stderr, "%s !!!!!!!!! out of bounds x %d\n", __func__, w1 + x);
			memcpy(p+w1*4,buf+((w-w1)*4)-4,4);
		}
		
		p += (CFrameBuffer::getInstance()->getScreenWidth(true) * 4);

		if (h1 + y > CFrameBuffer::getInstance()->getScreenHeight(true))
			fprintf(stderr, "%s !!!!!!!!! out of bounds y %d\n", __func__, w1 + y);

	}

	debugf(20, "%s: <\n", __func__);
}

void FlipVert(int x, int y, int w, int h)
{
	unsigned char buf[w*4];
	
	unsigned char *p = lfb + x*4 + y * (CFrameBuffer::getInstance()->getScreenWidth(true) *4), *p1, *p2;
	int h1;

	debugf(20, "%s: >\n", __func__);

	for (h1 = 0 ; h1 < h/2 ; h1++)
	{
		p1 = (p+(h1*(CFrameBuffer::getInstance()->getScreenWidth(true)*4)));
	
		p2 = (p+(h-(h1+1))*(CFrameBuffer::getInstance()->getScreenWidth(true)*4));

		if (w + x > CFrameBuffer::getInstance()->getScreenWidth(true))
			fprintf(stderr, "%s !!!!!!!!! out of bounds x %d\n", __func__, w + x);

		if ((h-(h1+1) ) + y > CFrameBuffer::getInstance()->getScreenHeight(true))
			fprintf(stderr, "%s !!!!!!!!! out of bounds y %d\n", __func__, (h-(h1+1)) + y);

		if (h1 + y > CFrameBuffer::getInstance()->getScreenHeight(true))
			fprintf(stderr, "%s !!!!!!!!! out of bounds y1 %d\n", __func__, h1 + y);
		
		memcpy(buf,p1,w*4);
		memcpy(p1,p2,w*4);
		memcpy(p2,buf,w*4);
	}

	debugf(20, "%s: <\n", __func__);
}

int ShapeCoord(int param, int curfontwidth, int curfontheight)
{
	switch (param)
	{
		case S_W13:
			return curfontwidth/3;
		case S_W12:
			return curfontwidth/2;
		case S_W23:
			return curfontwidth*2/3;
		case S_W11:
			return curfontwidth;
		case S_WM3:
			return curfontwidth-3;
		case S_H13:
			return curfontheight/3;
		case S_H12:
			return curfontheight/2;
		case S_H23:
			return curfontheight*2/3;
		case S_H11:
			return curfontheight;
		default:
			return param;
	}
}

void DrawShape(int x, int y, int shapenumber, int curfontwidth, int curfontheight, int fgcolor, int bgcolor, int clear)
{
	if (shapenumber < 0x20 || shapenumber > 0x7e || (shapenumber == 0x7e && clear))
		return;

	unsigned char *p = aShapes[shapenumber - 0x20];

	if (*p == S_INV)
	{
		int t = fgcolor;
		fgcolor = bgcolor;
		bgcolor = t;
		p++;
	}

	if (clear)
		FillRect(x, y, curfontwidth, fontheight, bgcolor);
	
	while (*p != S_END)
	{
		switch (*p++)
		{
			case S_FHL:
			{
				int offset = ShapeCoord(*p++, curfontwidth, curfontheight);
				DrawHLine(x, y + offset, curfontwidth, fgcolor);
				break;
			}
			case S_FVL:
			{
				int offset = ShapeCoord(*p++, curfontwidth, curfontheight);
				DrawVLine(x + offset, y, fontheight, fgcolor);
				break;
			}
			case S_FLH:
				FlipHorz(x,y,curfontwidth, fontheight);
				break;
			case S_FLV:
				FlipVert(x,y,curfontwidth, fontheight);
				break;
			case S_BOX:
			{
				int xo = ShapeCoord(*p++, curfontwidth, curfontheight);
				int yo = ShapeCoord(*p++, curfontwidth, curfontheight);
				int w = ShapeCoord(*p++, curfontwidth, curfontheight);
				int h = ShapeCoord(*p++, curfontwidth, curfontheight);
				FillRect(x + xo, y + yo, w, h, fgcolor);
				break;
			}
			case S_TRA:
			{
				int x0 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int y0 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int l0 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int x1 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int y1 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int l1 = ShapeCoord(*p++, curfontwidth, curfontheight);
				FillTrapez(x + x0, y + y0, l0, x1-x0, y1-y0, l1, fgcolor);
				break;
			}
			case S_BTR:
			{
				int x0 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int y0 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int l0 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int x1 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int y1 = ShapeCoord(*p++, curfontwidth, curfontheight);
				int l1 = ShapeCoord(*p++, curfontwidth, curfontheight);
				FillTrapez(x + x0, y + y0, l0, x1-x0, y1-y0, l1, bgcolor);
				break;
			}
			case S_LNK:
			{
				DrawShape(x, y, ShapeCoord(*p, curfontwidth, curfontheight), curfontwidth, curfontheight, fgcolor, bgcolor, 0);
				//p = aShapes[ShapeCoord(*p, curfontwidth, curfontheight) - 0x20];
				break;
			}
			default:
				break;
		}
	}
}


/*
 * RenderChar
*/
/* Dagobert:
 * So in this function is the problem with "�� ...".
 * The tables in tuxtxt_def.h are of type unsigned int short int
 * which is propably more than a char. So getting chars out of this
 * array gives us values >=255 which are not what we want with our
 * fonts. ok I added "& 0xFF" to get the correct value but this
 * takes us internationalization but this is ok for me for the first
 * shot. we take a shorter look why this works for dreambox but
 * not for us.
 */

void RenderChar(int Char, tstPageAttr *Attribute, int zoom, int yoffset)
{
	int Row, Pitch, Bit;
	int error, glyph;
	int bgcolor, fgcolor;
	int factor, xfactor;
	int national_subset_local = national_subset;
	unsigned char *sbitbuffer;

	int curfontwidth = GetCurFontWidth();

	debugf(20, "%s: >\n", __func__);

	if (Attribute->setX26)
	{
		national_subset_local = 0; // no national subset
	}

	// G0+G2 set designation
	if (Attribute->setG0G2 != 0x3f)
	{
		switch (Attribute->setG0G2)
		{
			case 0x20 :
			case 0x24 :
			case 0x25 :
				national_subset_local = NAT_RU;
				break;
			case 0x37:
				national_subset_local = NAT_GR;
				break;
				//TODO: arabic and hebrew
			default:
				national_subset_local = countryconversiontable[Attribute->setG0G2 & 0x07];
				break;

		}

	}
	
	if (Attribute->charset == C_G0S) // use secondary charset
		national_subset_local = national_subset_secondary;
	
	if (zoom && Attribute->doubleh)
		factor = 4;
	else if (zoom || Attribute->doubleh)
		factor = 2;
	else
		factor = 1;

	if (Attribute->doublew)
	{
		int t = curfontwidth;
		PosX += t;
		curfontwidth += GetCurFontWidth();
		PosX -= t;
		xfactor = 2;
	}
	else
		xfactor = 1;

	if (Char == 0xFF)	/* skip doubleheight chars in lower line */
	{
		PosX += curfontwidth;
		return;
	}

	/* get colors */
	if (Attribute->inverted)
	{
		int t = Attribute->fg;
		Attribute->fg = Attribute->bg;
		Attribute->bg = t;
	}
	
	fgcolor = Attribute->fg;
	
	if (transpmode == 1 && PosY < StartY + 24*fontheight)
	{
		if (fgcolor == transp) /* outside boxed elements (subtitles, news) completely transparent */
			bgcolor = transp;
		else
			bgcolor = transp2;
	}
	else
		bgcolor = Attribute->bg;

/*
fprintf(stderr, "charset = %d\n", Attribute->charset);		
fprintf(stderr, "national_subset_local = %d\n", national_subset_local);		
*/
	/* handle mosaic ->space*/
	if ((Attribute->charset == C_G1C || Attribute->charset == C_G1S) && ((Char&0xA0) == 0x20))
	{
		int w1 = (curfontwidth / 2 ) *xfactor;
		int w2 = (curfontwidth - w1) *xfactor;
		int y;

		Char = (Char & 0x1f) | ((Char & 0x40) >> 1);
		if (Attribute->charset == C_G1S) /* separated mosaic */
		{
			for (y = 0; y < 3; y++)
			{
				FillRectMosaicSeparated(PosX, PosY + yoffset + ymosaic[y]*factor, w1, (ymosaic[y+1] - ymosaic[y])*factor, fgcolor, bgcolor, Char & 0x01);
				FillRectMosaicSeparated(PosX + w1, PosY + yoffset + ymosaic[y]*factor, w2, (ymosaic[y+1] - ymosaic[y])*factor, fgcolor, bgcolor, Char & 0x02);
				Char >>= 2;
			}
		}
		else
		{
			for (y = 0; y < 3; y++)
			{
				FillRect(PosX,      PosY + yoffset + ymosaic[y]*factor, w1, (ymosaic[y+1] - ymosaic[y])*factor, (Char & 0x01) ? fgcolor : bgcolor);
				FillRect(PosX + w1, PosY + yoffset + ymosaic[y]*factor, w2, (ymosaic[y+1] - ymosaic[y])*factor, (Char & 0x02) ? fgcolor : bgcolor);
				Char >>= 2;
			}
		}
		PosX += curfontwidth;
		/*fprintf(stderr, "hier passierts 5\n");		*/
		return;
	}

	if (Attribute->charset == C_G3)
	{
		if (Char < 0x20 || Char > 0x7d)
		{
			Char = 0x20;
		}
		else
		{
			if (*aShapes[Char - 0x20] == S_CHR)
			{
				unsigned char *p = aShapes[Char - 0x20];
				Char = (*(p+1) <<8) + (*(p+2));
			}
			else if (*aShapes[Char - 0x20] == S_ADT)
			{
				int x,y,f,c;
				
				unsigned char* p = lfb + PosX*4 + (PosY+yoffset)* (CFrameBuffer::getInstance()->getScreenWidth(true)*4);
				
				for (y=0; y<fontheight;y++)
				{
					for (f=0; f<factor; f++)
					{
						for (x=0; x<curfontwidth*xfactor;x++)
						{
							c = (y&4 ? (x/3)&1 :((x+3)/3)&1);
							//setPixel(x + PosX, y + PosY + yoffset, (c ? fgcolor : bgcolor));
							memcpy((p+x*4),bgra[(c ? fgcolor : bgcolor)],4);
						}
						
						p += (CFrameBuffer::getInstance()->getScreenWidth(true)*4);
					}
				}
				PosX += curfontwidth;
				return;
			}
			else
			{
				DrawShape(PosX, PosY + yoffset, Char, curfontwidth, factor*fontheight, fgcolor, bgcolor,1);
				PosX += curfontwidth;
				return;
			}
		}
	}
	else if (Attribute->charset >= C_OFFSET_DRCS)
	{

		tstCachedPage *pcache = tuxtxt_cache.astCachetable[(Attribute->charset & 0x10) ? drcs : gdrcs][Attribute->charset & 0x0f];
		if (pcache)
		{
			unsigned char drcs_data[23*40];
			tuxtxt_decompress_page((Attribute->charset & 0x10) ? drcs : gdrcs,Attribute->charset & 0x0f,drcs_data);
			unsigned char *p;
			if (Char < 23*2)
				p = drcs_data + 20*Char;
			else if (pcache->pageinfo.p24)
				p = pcache->pageinfo.p24 + 20*(Char - 23*2);
			else
			{
				FillRect(PosX, PosY + yoffset, curfontwidth, factor*fontheight, bgcolor);
				PosX += curfontwidth;
				return;
			}
			
			axdrcs[12] = curfontwidth; /* adjust last x-offset according to position, FIXME: double width */
			
			RenderDRCS(p,lfb + PosX*4 + (yoffset + PosY) * (CFrameBuffer::getInstance()->getScreenWidth(true)*4), axdrcs, fgcolor, bgcolor);
		}
		else
		{
			FillRect(PosX, PosY + yoffset, curfontwidth, factor*fontheight, bgcolor);
		}
		PosX += curfontwidth;
		return;
	}
	else if (Attribute->charset == C_G2 && Char >= 0x20 && Char <= 0x7F)
	{
		if (national_subset_local == NAT_GR)
			Char = G2table[2][Char-0x20];
		else if (national_subset_local == NAT_RU)
			Char = G2table[1][Char-0x20];
		else
			Char = G2table[0][Char-0x20];

		if (Char == 0x7F)
		{
			FillRect(PosX, PosY + yoffset, curfontwidth, factor*ascender, fgcolor);
			FillRect(PosX, PosY + yoffset + factor*ascender, curfontwidth, factor*(fontheight-ascender), bgcolor);
			PosX += curfontwidth;
			return;
		}
	}
	else if (national_subset_local == NAT_GR && Char >= 0x40 && Char <= 0x7E)	/* remap complete areas for greek */
		Char += 0x390 - 0x40;
	else if (national_subset_local == NAT_GR && Char == 0x3c)
		Char = '�';
	else if (national_subset_local == NAT_GR && Char == 0x3e)
		Char = '�';
	else if (national_subset_local == NAT_GR && Char >= 0x23 && Char <= 0x24)
		Char = nationaltable23[NAT_DE][Char-0x23]; /* #$ as in german option */
	else if (national_subset_local == NAT_RU && Char >= 0x40 && Char <= 0x7E) /* remap complete areas for cyrillic */
		Char = G0tablecyrillic[Char-0x20];
	else
	{
		/* load char */
		switch (Char)
		{
			case 0x00:
			case 0x20:
				FillRect(PosX, PosY + yoffset, curfontwidth, factor*fontheight, bgcolor);
				PosX += curfontwidth;
				return;
			case 0x23:
			case 0x24:
				Char = nationaltable23[national_subset_local][Char-0x23] & 0xFF;
				break;
			case 0x40:
				Char = nationaltable40[national_subset_local] & 0xFF;
				break;
			case 0x5B:
			case 0x5C:
			case 0x5D:
			case 0x5E:
			case 0x5F:
			case 0x60:
				Char = nationaltable5b[national_subset_local][Char-0x5B] & 0xFF;
				break;
			case 0x7B:
			case 0x7C:
			case 0x7D:
			case 0x7E:
				Char = nationaltable7b[national_subset_local][Char-0x7B] & 0xFF;
				break;
			case 0x7F:
				FillRect(PosX, PosY + yoffset, curfontwidth, factor*ascender, fgcolor);
				FillRect(PosX, PosY + yoffset + factor*ascender, curfontwidth, factor*(fontheight-ascender), bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE0: /* |- */
				DrawHLine(PosX, PosY + yoffset, curfontwidth, fgcolor);
				DrawVLine(PosX, PosY + yoffset +1, fontheight -1, fgcolor);
				FillRect(PosX +1, PosY + yoffset +1, curfontwidth-1, fontheight-1, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE1: /* - */
				DrawHLine(PosX, PosY + yoffset, curfontwidth, fgcolor);
				FillRect(PosX, PosY + yoffset +1, curfontwidth, fontheight-1, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE2: /* -| */
				DrawHLine(PosX, PosY + yoffset, curfontwidth, fgcolor);
				DrawVLine(PosX + curfontwidth -1, PosY + yoffset +1, fontheight -1, fgcolor);
				FillRect(PosX, PosY + yoffset +1, curfontwidth-1, fontheight-1, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE3: /* |  */
				DrawVLine(PosX, PosY + yoffset, fontheight, fgcolor);
				FillRect(PosX +1, PosY + yoffset, curfontwidth -1, fontheight, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE4: /*  | */
				DrawVLine(PosX + curfontwidth -1, PosY + yoffset, fontheight, fgcolor);
				FillRect(PosX, PosY + yoffset, curfontwidth -1, fontheight, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE5: /* |_ */
				DrawHLine(PosX, PosY + yoffset + fontheight -1, curfontwidth, fgcolor);
				DrawVLine(PosX, PosY + yoffset, fontheight -1, fgcolor);
				FillRect(PosX +1, PosY + yoffset, curfontwidth-1, fontheight-1, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE6: /* _ */
				DrawHLine(PosX, PosY + yoffset + fontheight -1, curfontwidth, fgcolor);
				FillRect(PosX, PosY + yoffset, curfontwidth, fontheight-1, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE7: /* _| */
				DrawHLine(PosX, PosY + yoffset + fontheight -1, curfontwidth, fgcolor);
				DrawVLine(PosX + curfontwidth -1, PosY + yoffset, fontheight -1, fgcolor);
				FillRect(PosX, PosY + yoffset, curfontwidth-1, fontheight-1, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xE8: /* Ii */
				FillRect(PosX +1, PosY + yoffset, curfontwidth -1, fontheight, bgcolor);
				for (Row=0; Row < curfontwidth/2; Row++)
					DrawVLine(PosX + Row, PosY + yoffset + Row, fontheight - Row, fgcolor);
				PosX += curfontwidth;
				return;
			case 0xE9: /* II */
				FillRect(PosX, PosY + yoffset, curfontwidth/2, fontheight, fgcolor);
				FillRect(PosX + curfontwidth/2, PosY + yoffset, (curfontwidth+1)/2, fontheight, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xEA: /* °  */
				FillRect(PosX, PosY + yoffset, curfontwidth, fontheight, bgcolor);
				FillRect(PosX, PosY + yoffset, curfontwidth/2, curfontwidth/2, fgcolor);
				PosX += curfontwidth;
				return;
			case 0xEB: /* ¬ */
				FillRect(PosX, PosY + yoffset +1, curfontwidth, fontheight -1, bgcolor);
				for (Row=0; Row < curfontwidth/2; Row++)
					DrawHLine(PosX + Row, PosY + yoffset + Row, curfontwidth - Row, fgcolor);
				PosX += curfontwidth;
				return;
			case 0xEC: /* -- */
				FillRect(PosX, PosY + yoffset, curfontwidth, curfontwidth/2, fgcolor);
				FillRect(PosX, PosY + yoffset + curfontwidth/2, curfontwidth, fontheight - curfontwidth/2, bgcolor);
				PosX += curfontwidth;
				return;
			case 0xED:
			case 0xEE:
			case 0xEF:
			case 0xF0:
			case 0xF1:
			case 0xF2:
			case 0xF3:
			case 0xF4:
			case 0xF5:
			case 0xF6:
				Char = arrowtable[Char - 0xED];
				break;
			default:
				break;
		}
	}
	
	if (Char <= 0x20)
	{
#if TUXTXT_DEBUG
		printf("TuxTxt found control char: %x \"%c\" \n", Char, Char);
#endif
		FillRect(PosX, PosY + yoffset, curfontwidth, factor*fontheight, bgcolor);
		PosX += curfontwidth;
		return;
	}


	if (!(glyph = FT_Get_Char_Index(face, Char)))
	{
		fprintf(stderr, "TuxTxt <FT_Get_Char_Index for Char %d %x \"%c\" failed\n", Char, Char, Char & 0xFF );

		FillRect(PosX, PosY + yoffset, curfontwidth, factor*fontheight, bgcolor);
		PosX += curfontwidth;
		return;
	}

	if ((error = FTC_SBitCache_Lookup(cache, &typettf, glyph, &sbit, NULL)) != 0)
	{
		fprintf(stderr, "TuxTxt <FTC_SBitCache_Lookup: 0x%x> c%x a%x g%x w%d h%d x%d y%d\n",
				 error, Char, Attribute, glyph, curfontwidth, fontheight, PosX, PosY);

		FillRect(PosX, PosY + yoffset, curfontwidth, fontheight, bgcolor);
		PosX += curfontwidth;
		return;
	}

	/* render char */
	sbitbuffer = sbit->buffer;
	unsigned char localbuffer[1000]; // should be enough to store one character-bitmap...
	
	// add diacritical marks
	if (Attribute->diacrit)
	{
		FTC_SBit        sbit_diacrit;

		if (national_subset_local == NAT_GR)
			Char = G2table[2][0x20+ Attribute->diacrit];
		else if (national_subset_local == NAT_RU)
			Char = G2table[1][0x20+ Attribute->diacrit];
		else
			Char = G2table[0][0x20+ Attribute->diacrit];
		
		if ((glyph = FT_Get_Char_Index(face, Char)))
		{
			if ((error = FTC_SBitCache_Lookup(cache, &typettf, glyph, &sbit_diacrit, NULL)) == 0)
			{
					sbitbuffer = localbuffer;
					memcpy(sbitbuffer,sbit->buffer,sbit->pitch*sbit->height);

					for (Row = 0; Row < sbit->height; Row++)
					{
						for (Pitch = 0; Pitch < sbit->pitch; Pitch++)
						{
							if (sbit_diacrit->pitch > Pitch && sbit_diacrit->height > Row)
								sbitbuffer[Row*sbit->pitch+Pitch] |= sbit_diacrit->buffer[Row*sbit->pitch+Pitch];
						}
					}
				}
			}
		}

		unsigned char *p;
		int f; /* running counter for zoom factor */

		Row = factor * (ascender - sbit->top + TTFShiftY);
		FillRect(PosX, PosY + yoffset, curfontwidth, Row, bgcolor); /* fill upper margin */

		if (ascender - sbit->top + TTFShiftY + sbit->height > fontheight)
			sbit->height = fontheight - ascender + sbit->top - TTFShiftY; /* limit char height to defined/calculated fontheight */

		//fprintf(stderr, "PosX = %d, yoffset = %d, PosY %d, Row %d\n", PosX, yoffset, PosY, Row);

		p = lfb + PosX*4 + (yoffset + PosY + Row) * (CFrameBuffer::getInstance()->getScreenWidth(true) *4); /* running pointer into framebuffer */

		int saveRow = Row;
		for (Row = sbit->height; Row; Row--) /* row counts up, but down may be a little faster :) */
		{
			int pixtodo = (usettf ? sbit->width : curfontwidth);
			unsigned char *pstart = p;
			int x = PosX;

			for (Bit = xfactor * (sbit->left + TTFShiftX); Bit > 0; Bit--) /* fill left margin */
			{
				for (f = factor-1; f >= 0; f--)
					//setPixel(x, saveRow + f + yoffset + PosY + ((sbit->height - Row) * factor), bgcolor);
					memcpy((p + f * CFrameBuffer::getInstance()->getStride() ), bgra[bgcolor], 4);/*bgcolor*/
					//memmove((p + f*fix_screeninfo.line_length),bgra[bgcolor],4);/*bgcolor*/
				p += 4;
				x += 1;
				if (!usettf)
					pixtodo--;
			}

			for (Pitch = sbit->pitch; Pitch; Pitch--)
			{
				for (Bit = 0x80; Bit; Bit >>= 1)
				{
					int color;

					if (--pixtodo < 0)
						break;

					if (*sbitbuffer & Bit) /* bit set -> foreground */
						color = fgcolor;
					else /* bit not set -> background */
						color = bgcolor;

					for (f = factor-1; f >= 0; f--)
						//setPixel(x, saveRow + f + yoffset + PosY + ((sbit->height - Row) * factor), color);
						memcpy((p + f * CFrameBuffer::getInstance()->getStride() ),bgra[color],4);
					p+=4;
					x+=1;
					if (xfactor > 1) /* double width */
					{
						for (f = factor-1; f >= 0; f--)
							//setPixel(x, saveRow + f + yoffset + PosY + ((sbit->height - Row) * factor), color);
							memcpy((p + f * CFrameBuffer::getInstance()->getStride() ),bgra[color],4);
						p+=4;
						x+=1;

						if (!usettf)
							pixtodo--;
					}
				}
				sbitbuffer++;
			}
			
			for (Bit = (usettf ? (curfontwidth - xfactor*(sbit->width + sbit->left + TTFShiftX)) : pixtodo); Bit > 0; Bit--) /* fill rest of char width */
			{
				for (f = factor-1; f >= 0; f--)
					//setPixel(x, saveRow + f + yoffset + PosY + ((sbit->height - Row) * factor), bgcolor);
					memcpy((p + f * CFrameBuffer::getInstance()->getStride()),bgra[bgcolor],4);
				p+=4;
				x+=1;
			}

			p = pstart + factor*(CFrameBuffer::getInstance()->getScreenWidth(true) *4);
		}

		Row = ascender - sbit->top + sbit->height + TTFShiftY;
		FillRect(PosX, PosY + yoffset + Row*factor, curfontwidth, (fontheight - Row) * factor, bgcolor); /* fill lower margin */
		if (Attribute->underline)
			FillRect(PosX, PosY + yoffset + (fontheight-2)* factor, curfontwidth,2*factor, fgcolor); /* underline char */

		PosX += curfontwidth;
	//}

	debugf(20, "%s: <\n", __func__);
}

/*
 * RenderCharFB
*/
void RenderCharFB(int Char, tstPageAttr *Attribute)
{
	debugf(20, "%s: >\n", __func__);
	RenderChar(Char, Attribute, zoommode, 0);
	debugf(20, "%s: <\n", __func__);
}

/*
 * RenderCharBB
*/
void RenderCharBB(int Char, tstPageAttr *Attribute)
{
	debugf(20, "%s: >\n", __func__);

	RenderChar(Char, Attribute, 0, 0);

	debugf(20, "%s: <\n", __func__);
}

/*
 * RenderCharLCD
*/
void RenderCharLCD(int Digit, int XPos, int YPos)
{
#if 0
	int x, y;

	/* render digit to lcd backbuffer */
	for (y = 0; y < 15; y++)
	{
		for (x = 0; x < 10; x++)
		{
			if (lcd_digits[Digit*15*10 + x + y*10])
				lcd_backbuffer[XPos + x + ((YPos+y)/8)*120] |= 1 << ((YPos+y)%8);
			else
				lcd_backbuffer[XPos + x + ((YPos+y)/8)*120] &= ~(1 << ((YPos+y)%8));
		}
	}
#endif
}

#if 0
void RenderCharLCDsmall(int Char, int XPos, int YPos)
{
	int old_width = fontwidth;
	int old_height = fontheight;
	setfontwidth(fontwidth_small_lcd);
	typettf.font.pix_height = fontheight = fontwidth_small_lcd;
	RenderChar(Char, 0, 0, -(YPos<<8 | XPos));
	setfontwidth(old_width);
	typettf.font.pix_height = fontheight = old_height;
}
#endif

/*
 * RenderMessage
*/
void RenderMessage(int Message)
{
	int byte;
	int fbcolor, timecolor, menuatr;
	int pagecolumn;
	const char *msg;


/*                     00000000001111111111222222222233333333334 */
/*                     01234567890123456789012345678901234567890 */
	char message_1[] = "�������� www.tuxtxt.com x.xx ���������";
	char message_2[] = "�                                   ��";
/* 	char message_3[] = "�   suche nach Teletext-Anbietern   ��"; */
	char message_4[] = "�                                   ��";
	char message_5[] = "��������������������������������������";
	char message_6[] = "��������������������������������������";

/* 	char message_7[] = "� kein Teletext auf dem Transponder ��"; */
/* 	char message_8[] = "�  warte auf Empfang von Seite 100  ��"; */
/* 	char message_9[] = "�     Seite 100 existiert nicht!    ��"; */

	memcpy(&message_1[24], versioninfo, 4);
	/* reset zoom */
	zoommode = 0;

	/* set colors */
	fbcolor   = transp;
	timecolor = transp<<4 | transp;
	menuatr = ATR_MSG0;

	/* clear framebuffer */
	ClearFB(fbcolor);

	/* hide header */
	page_atrb[32].fg = transp;
	page_atrb[32].bg = transp;


	/* set pagenumber */
	if (Message == ShowServiceName)
	{
		pagecolumn = message8pagecolumn[menulanguage];
		msg = message_8[menulanguage];
		memcpy(&message_4, msg, sizeof(message_4));
		hex2str(message_4+pagecolumn, tuxtxt_cache.page);

		if (SDT_ready)
			memcpy(&message_2[2 + (35 - pid_table[current_service].service_name_len)/2],
					 &pid_table[current_service].service_name, pid_table[current_service].service_name_len);
		else if (Message == ShowServiceName)
			hex2str(&message_2[17+3], tuxtxt_cache.vtxtpid);

		msg = &message_3_blank[0];
	}
	else if (Message == NoServicesFound)
		msg = &message_7[menulanguage][0];
	else
		msg = &message_3[menulanguage][0];

	/* render infobar */
	PosX = StartX + fontwidth+5;
	PosY = StartY + fontheight*16;
	
	for (byte = 0; byte < 37; byte++)
		RenderCharFB(message_1[byte], &atrtable[menuatr + ((byte >= 9 && byte <= 27) ? 1 : 0)]);
	RenderCharFB(message_1[37], &atrtable[menuatr + 2]);

	PosX = StartX + fontwidth+5;
	PosY = StartY + fontheight*17;
	RenderCharFB(message_2[0], &atrtable[menuatr + 0]);
	for (byte = 1; byte < 36; byte++)
		RenderCharFB(message_2[byte], &atrtable[menuatr + 3]);
	RenderCharFB(message_2[36], &atrtable[menuatr + 0]);
	RenderCharFB(message_2[37], &atrtable[menuatr + 2]);

	PosX = StartX + fontwidth+5;
	PosY = StartY + fontheight*18;
	RenderCharFB(msg[0], &atrtable[menuatr + 0]);
	for (byte = 1; byte < 36; byte++)
		RenderCharFB(msg[byte], &atrtable[menuatr + 3]);
	RenderCharFB(msg[36], &atrtable[menuatr + 0]);
	RenderCharFB(msg[37], &atrtable[menuatr + 2]);

	PosX = StartX + fontwidth+5;
	PosY = StartY + fontheight*19;
	RenderCharFB(message_4[0], &atrtable[menuatr + 0]);
	for (byte = 1; byte < 36; byte++)
		RenderCharFB(message_4[byte], &atrtable[menuatr + 3]);
	RenderCharFB(message_4[36], &atrtable[menuatr + 0]);
	RenderCharFB(message_4[37], &atrtable[menuatr + 2]);

	PosX = StartX + fontwidth+5;
	PosY = StartY + fontheight*20;
	for (byte = 0; byte < 37; byte++)
		RenderCharFB(message_5[byte], &atrtable[menuatr + 0]);
	RenderCharFB(message_5[37], &atrtable[menuatr + 2]);

	PosX = StartX + fontwidth+5;
	PosY = StartY + fontheight*21;
	for (byte = 0; byte < 38; byte++)
		RenderCharFB(message_6[byte], &atrtable[menuatr + 2]);
	
#ifdef FB_BLIT
	CFrameBuffer::getInstance()->blit();
#endif	
}

/*
 * RenderPage
*/
void DoFlashing(int startrow)
{
	int row, col;
	/* get national subset */
	if (auto_national &&
		 national_subset <= NAT_MAX_FROM_HEADER && /* not for GR/RU as long as line28 is not evaluated */
		 pageinfo && pageinfo->nationalvalid) /* individual subset according to page header */
	{
		national_subset = countryconversiontable[pageinfo->national];
	}
	/* Flashing */
	tstPageAttr flashattr;
	char flashchar;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	long flashphase = (tv.tv_usec / 1000) % 1000;
	int srow = startrow;
	int erow = 24;
	int factor=1;
	switch (zoommode)
	{
		case 1: erow = 12; factor=2;break;
		case 2: srow = 12; factor=2;break;
	}
	PosY = StartY + startrow*fontheight*factor;
	for (row = srow; row < erow; row++)
	{
		int index = row * 40;
		int dhset = 0;
		int incflash = 3;
		int decflash = 2;
		PosX = StartX;
		for (col = nofirst; col < 40; col++)
		{
			if (page_atrb[index + col].flashing && page_char[index + col] > 0x20 && page_char[index + col]!= 0xff )
			{
				SetPosX(col);
				flashchar = page_char[index + col];
				int doflash = 0;
				memcpy(&flashattr,&page_atrb[index + col],sizeof(tstPageAttr));
				switch (flashattr.flashing &0x1c) // Flash Rate
				{
					case 0x00 :	// 1 Hz
						if (flashphase>500) doflash = 1;
						break;
					case 0x04 :	// 2 Hz  Phase 1
						if (flashphase<250) doflash = 1;
						break;
					case 0x08 :	// 2 Hz  Phase 2
						if (flashphase>=250 && flashphase<500) doflash = 1;
						break;
					case 0x0c :	// 2 Hz  Phase 3
						if (flashphase>=500 && flashphase<750) doflash = 1;
						break;
					case 0x10 :	// incremental flash
						incflash++;
						if (incflash>3) incflash = 1;
						switch (incflash)
						{
							case 1: if (flashphase<250) doflash = 1; break;
							case 2: if (flashphase>=250 && flashphase<500) doflash = 1;break;
							case 3: if (flashphase>=500 && flashphase<750) doflash = 1;
						}
						break;
					case 0x11 :	// decremental flash
						decflash--;
						if (decflash<1) decflash = 3;
						switch (decflash)
						{
							case 1: if (flashphase<250) doflash = 1; break;
							case 2: if (flashphase>=250 && flashphase<500) doflash = 1;break;
							case 3: if (flashphase>=500 && flashphase<750) doflash = 1;
						}
						break;

				}

				switch (flashattr.flashing &0x03) // Flash Mode
				{
					case 0x01 :	// normal Flashing
						if (doflash) flashattr.fg = flashattr.bg;
						break;
					case 0x02 :	// inverted Flashing
						doflash = 1-doflash;
						if (doflash) flashattr.fg = flashattr.bg;
						break;
					case 0x03 :	// color Flashing
						if (doflash) flashattr.fg = flashattr.fg + (flashattr.fg > 7 ? (-8) : 8);
						break;

				}
				RenderCharFB(flashchar,&flashattr);
				if (flashattr.doublew) col++;
				if (flashattr.doubleh) dhset = 1;
			}
		}
		if (dhset)
		{
			row++;
			PosY += fontheight*factor;
		}
		PosY += fontheight*factor;
	}

}

void RenderPage()
{
	int row, col, byte, startrow = 0;;
	int national_subset_bak = national_subset;

	/* update lcd */
	UpdateLCD();

	if (transpmode != 2 && delaystarted)
	{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		if (tv.tv_sec - tv_delay.tv_sec < subtitledelay)
			return;
	}
	

	/* update page or timestring */
	if (transpmode != 2 && tuxtxt_cache.pageupdate && tuxtxt_cache.page_receiving != tuxtxt_cache.page && inputcounter == 2)
	{
		if (boxed && subtitledelay) 
		{
			if (!delaystarted)
			{
				gettimeofday(&tv_delay,NULL);
				delaystarted = 1;
				return;
			}
			else
				delaystarted = 0;
		}

		/* reset update flag */
		tuxtxt_cache.pageupdate = 0;

		/* decode page */
		if (tuxtxt_cache.subpagetable[tuxtxt_cache.page] != 0xFF)
			DecodePage();
		else
			startrow = 1;
		if (boxed)
		{ 
			if (screenmode != 0) 
				SwitchScreenMode(0); /* turn off divided screen */
		}
		else 
		{ 
			if (screenmode != prevscreenmode && !transpmode) 
				SwitchScreenMode(prevscreenmode);
		}

 		/* display first column?  */
		nofirst = show39;
		for (row = 1; row < 24; row++)
		{
			byte = page_char[row*40];
			if (byte != ' '  && byte != 0x00 && byte != 0xFF && page_atrb[row*40].fg != page_atrb[row*40].bg)
			{
				nofirst = 0;
				break;
			}
		}
		
		fontwidth_normal = (ex-sx) / (40-nofirst);
		setfontwidth(fontwidth_normal);
		fontwidth_topmenumain = (TV43STARTX-sx) / (40-nofirst);
		fontwidth_topmenusmall = (ex- TOPMENUSTARTX) / TOPMENUCHARS;
		fontwidth_small = (TV169FULLSTARTX-sx)  / (40-nofirst);
		
		switch(screenmode)
		{
			case 0:	setfontwidth(fontwidth_normal)     ; displaywidth= (ex             -sx);break;
			case 1:  setfontwidth(fontwidth_topmenumain); displaywidth= (TV43STARTX     -sx);break;
			case 2:  setfontwidth(fontwidth_small)      ; displaywidth= (TV169FULLSTARTX-sx);break;
		}
		
		if (transpmode || (boxed && !screenmode))
		{
			FillBorder(transp);//ClearBB(transp);
			clearbbcolor = transp;
		}

		/* get national subset */
		if (auto_national &&
			 national_subset <= NAT_MAX_FROM_HEADER && /* not for GR/RU as long as line28 is not evaluated */
			 pageinfo && pageinfo->nationalvalid) /* individual subset according to page header */
		{
			national_subset = countryconversiontable[pageinfo->national];
#if TUXTXT_DEBUG
			printf("p%03x b%d n%d v%d i%d\n", tuxtxt_cache.page,national_subset_bak, national_subset, pageinfo->nationalvalid, pageinfo->national);
#endif
		}
		
		/* render page */
		PosY = StartY + startrow*fontheight;
		for (row = startrow; row < 24; row++)
		{
			int index = row * 40;

			PosX = StartX;
			for (col = nofirst; col < 40; col++)
			{
				RenderCharBB(page_char[index + col], &page_atrb[index + col]);

				if (page_atrb[index + col].doubleh && page_char[index + col] != 0xff)	/* disable lower char in case of doubleh setting in l25 objects */
					page_char[index + col + 40] = 0xff;
				if (page_atrb[index + col].doublew)	/* skip next column if double width */
				{
					col++;
					if (page_atrb[index + col-1].doubleh && page_char[index + col] != 0xff)	/* disable lower char in case of doubleh setting in l25 objects */
						page_char[index + col + 40] = 0xff;
				}
			}
			PosY += fontheight;
		}
		DoFlashing(startrow);
		national_subset = national_subset_bak;

		/* update framebuffer */
		CopyBB2FB();
	}
	else if ( use_gui && transpmode != 2)
	{
		if (zoommode != 2)
		{
			PosY = StartY;
			if (tuxtxt_cache.subpagetable[tuxtxt_cache.page] == 0xff)
			{
				page_atrb[32].fg = yellow;
				page_atrb[32].bg = menu1;
				int showpage = tuxtxt_cache.page_receiving;
				int showsubpage = tuxtxt_cache.subpagetable[showpage];
				if (showsubpage!=0xff)
				{
					tstCachedPage *pCachedPage;
					pCachedPage = tuxtxt_cache.astCachetable[showpage][showsubpage];
					if (pCachedPage && tuxtxt_is_dec(showpage))
					{
						PosX = StartX;
						if (inputcounter == 2)
						{
							if (tuxtxt_cache.bttok && !tuxtxt_cache.basictop[tuxtxt_cache.page]) /* page non-existent according to TOP (continue search anyway) */
							{
								page_atrb[0].fg = white;
								page_atrb[0].bg = red;
							}
							else
							{
								page_atrb[0].fg = yellow;
								page_atrb[0].bg = menu1;
							}
							hex2str((char*) page_char+3, tuxtxt_cache.page);
							for (col = nofirst; col < 7; col++) // selected page
							{
								RenderCharFB(page_char[col], &page_atrb[0]);
							}
							RenderCharFB(page_char[col], &page_atrb[32]);
						}
						else
							SetPosX(8);

						memcpy(&page_char[8], pCachedPage->p0, 24); /* header line without timestring */
						for (col = 0; col < 24; col++)
						{
							RenderCharFB(pCachedPage->p0[col], &page_atrb[32]);
						}
					}
				}
			}
			/* update timestring */
			SetPosX(32);
			for (byte = 0; byte < 8; byte++)
			{
				if (!page_atrb[32+byte].flashing)
					RenderCharFB(tuxtxt_cache.timestring[byte], &page_atrb[32]);
				else
				{
					SetPosX(33+byte);
					page_char[32+byte] = page_char[32+byte];
				}


			}
		}
		DoFlashing(startrow);
		national_subset = national_subset_bak;
	}
	else if (transpmode == 2 && tuxtxt_cache.pageupdate == 2)
	{
#if TUXTXT_DEBUG
		printf("received Update flag for page %03x\n",tuxtxt_cache.page);
#endif
		// display pagenr. when page has been updated while in transparency mode
		PosY = StartY;

		char ns[3];
		SetPosX(1);
		hex2str(ns+2,tuxtxt_cache.page);

		RenderCharFB(ns[0],&atrtable[ATR_WB]);
		RenderCharFB(ns[1],&atrtable[ATR_WB]);
		RenderCharFB(ns[2],&atrtable[ATR_WB]);

		tuxtxt_cache.pageupdate=0;
	}
#ifdef FB_BLIT
	CFrameBuffer::getInstance()->blit();
#endif
}

/*
 * CreateLine25
*/
void showlink(int column, int linkpage)
{
	unsigned char *p, line[] = "   >???   ";
	int oldfontwidth = fontwidth;
	int yoffset;

	debugf(20, "%s: >\n", __func__);

	yoffset = 0;

	int abx = ((displaywidth)%(40-nofirst) == 0 ? displaywidth+1 : (displaywidth)/(((displaywidth)%(40-nofirst)))+1);// distance between 'inserted' pixels
	int width = displaywidth /4;

	PosY = StartY + 24*fontheight;

	if (boxed)
	{
		PosX = StartX + column*width;
		//debugf(20, "%s: x=%d(%d) y=%d(%d|%d) w=%d(%d) h=%d\n", __func__, PosX, abx, PosY+yoffset, PosY, yoffset, displaywidth, width, fontheight);
		FillRect(PosX, PosY+yoffset, width, fontheight, transp);
		//debugf(20, "%s: < boxed\n", __func__);
		return;
	}

	if (tuxtxt_cache.adip[linkpage][0])
	{
		PosX = StartX + column*width;
		int l = strlen((char*) tuxtxt_cache.adip[linkpage]);

		if (l > 9) /* smaller font, if no space for one half space at front and end */
			setfontwidth(oldfontwidth * 10 / (l+1));
		FillRect(PosX, PosY+yoffset, width+(displaywidth%4), fontheight, atrtable[ATR_L250 + column].bg);
		PosX += ((width) - (l*fontwidth+l*fontwidth/abx))/2; /* center */
		for (p = tuxtxt_cache.adip[linkpage]; *p; p++)
			RenderCharBB(*p, &atrtable[ATR_L250 + column]);
		setfontwidth(oldfontwidth);
	}
	else /* display number */
	{
		PosX = StartX + column*width;
		FillRect(PosX, PosY+yoffset, displaywidth+sx-PosX, fontheight, atrtable[ATR_L250 + column].bg);
		if (linkpage < tuxtxt_cache.page)
		{
			line[6] = '<';
			hex2str((char*) line + 5, linkpage);
		}
		else
			hex2str((char*) line + 6, linkpage);
		for (p = line; p < line+9; p++)
			RenderCharBB(*p, &atrtable[ATR_L250 + column]);
	}

	debugf(20, "%s: <\n", __func__);
}

void CreateLine25()
{

	if (!tuxtxt_cache.bttok)
		/* btt completely received and not yet decoded */
		tuxtxt_decode_btt();
	if (tuxtxt_cache.maxadippg >= 0)
		tuxtxt_decode_adip();

	if (!showhex && showflof &&
		 (tuxtxt_cache.flofpages[tuxtxt_cache.page][0] || tuxtxt_cache.flofpages[tuxtxt_cache.page][1] || tuxtxt_cache.flofpages[tuxtxt_cache.page][2] || tuxtxt_cache.flofpages[tuxtxt_cache.page][3])) // FLOF-Navigation present
	{
		int i;

		prev_100 = tuxtxt_cache.flofpages[tuxtxt_cache.page][0];
		prev_10  = tuxtxt_cache.flofpages[tuxtxt_cache.page][1];
		next_10  = tuxtxt_cache.flofpages[tuxtxt_cache.page][2];
		next_100 = tuxtxt_cache.flofpages[tuxtxt_cache.page][3];

		PosY = StartY + 24*fontheight;
		PosX = StartX;
		for (i=nofirst; i<40; i++)
			RenderCharBB(page_char[24*40 + i], &page_atrb[24*40 + i]);
	}
	else
	{
		/*  normal: blk-1, grp+1, grp+2, blk+1 */
		/*  hex:    hex+1, blk-1, grp+1, blk+1 */
		if (showhex)
		{
			/* arguments: startpage, up, findgroup */
			prev_100 = tuxtxt_next_hex(tuxtxt_cache.page);
			prev_10  = toptext_getnext(tuxtxt_cache.page, 0, 0);
			next_10  = toptext_getnext(tuxtxt_cache.page, 1, 1);
		}
		else
		{
			prev_100 = toptext_getnext(tuxtxt_cache.page, 0, 0);
			prev_10  = toptext_getnext(tuxtxt_cache.page, 1, 1);
			next_10  = toptext_getnext(prev_10, 1, 1);
		}
		next_100 = toptext_getnext(next_10, 1, 0);
		showlink(0, prev_100);
		showlink(1, prev_10);
		showlink(2, next_10);
		showlink(3, next_100);
	}

	if (//tuxtxt_cache.bttok &&
		 screenmode == 1) /* TOP-Info present, divided screen -> create TOP overview */
	{
		char line[TOPMENUCHARS];
		int current;
		int prev10done, next10done, next100done, indent;
		tstPageAttr *attrcol, *attr; /* color attribute for navigation keys and text */

		int olddisplaywidth = displaywidth;
		displaywidth = 1000*(40-nofirst); // disable pixelinsert;
		setfontwidth(fontwidth_topmenusmall);

		PosY = TOPMENUSTARTY;
		memset(line, ' ', TOPMENUCHARS); /* init with spaces */

		memcpy(line+TOPMENUINDENTBLK, tuxtxt_cache.adip[prev_100], 12);
		hex2str(&line[TOPMENUINDENTDEF+12+TOPMENUSPC+2], prev_100);
		RenderClearMenuLineBB(line, &atrtable[ATR_L250], &atrtable[ATR_TOPMENU2]);

/*  1: blk-1, grp-1, grp+1, blk+1 */
/*  2: blk-1, grp+1, grp+2, blk+1 */
#if (LINE25MODE == 1)
		current = prev_10 - 1;
#else
		current = tuxtxt_cache.page - 1;
#endif

		prev10done = next10done = next100done = 0;
		while (PosY <= (TOPMENUENDY-fontheight))
		{
			attr = 0;
			attrcol = &atrtable[ATR_WB];
			if (!next100done && (PosY > (TOPMENUENDY - 2*fontheight))) /* last line */
			{
				attrcol = &atrtable[ATR_L253];
				current = next_100;
			}
			else if (!next10done && (PosY > (TOPMENUENDY - 3*fontheight))) /* line before */
			{
				attrcol = &atrtable[ATR_L252];
				current = next_10;
			}
			else if (!prev10done && (PosY > (TOPMENUENDY - 4*fontheight))) /* line before */
			{
				attrcol = &atrtable[ATR_L251];
				current = prev_10;
			}
			else do
			{
				tuxtxt_next_dec(&current);
				if (current == prev_10)
				{
					attrcol = &atrtable[ATR_L251];
					prev10done = 1;
					break;
				}
				else if (current == next_10)
				{
					attrcol = &atrtable[ATR_L252];
					next10done = 1;
					break;
				}
				else if (current == next_100)
				{
					attrcol = &atrtable[ATR_L253];
					next100done = 1;
					break;
				}
				else if (current == tuxtxt_cache.page)
				{
					attr = &atrtable[ATR_TOPMENU0];
					break;
				}
			} while (tuxtxt_cache.adip[current][0] == 0 && (tuxtxt_cache.basictop[current] < 2 || tuxtxt_cache.basictop[current] > 7));

			if (!tuxtxt_cache.bttok || (tuxtxt_cache.basictop[current] >= 2 && tuxtxt_cache.basictop[current] <= 5)) /* block (also for FLOF) */
			{
				indent = TOPMENUINDENTBLK;
				if (!attr)
					attr = &atrtable[tuxtxt_cache.basictop[current] <=3 ? ATR_TOPMENU1 : ATR_TOPMENU2]; /* green for program block */
			}
			else if (tuxtxt_cache.basictop[current] >= 6 && tuxtxt_cache.basictop[current] <= 7) /* group */
			{
				indent = TOPMENUINDENTGRP;
				if (!attr)
					attr = &atrtable[ATR_TOPMENU3];
			}
			else
			{
				indent = TOPMENUINDENTDEF;
				if (!attr)
					attr = &atrtable[ATR_WB];
			}
			memcpy(line+indent, tuxtxt_cache.adip[current], 12);
			hex2str(&line[TOPMENUINDENTDEF+12+TOPMENUSPC+2], current);
			RenderClearMenuLineBB(line, attrcol, attr);
		}
		displaywidth = olddisplaywidth;
		setfontwidth(fontwidth_topmenumain);
	}
}

/*
 * CopyBB2FB
*/
void CopyBB2FB()
{
	debugf(2, "%s: >\n", __func__);

	/* line 25 */
	//if (!pagecatching)
		//CreateLine25();
	//TEST
	if (!pagecatching && use_gui)
		CreateLine25();
	
	/* copy backbuffer to framebuffer */
	if (!zoommode)
	{
		if (clearbbcolor >= 0)
		{
			clearbbcolor = -1;
		}

		return;
	}

	debugf(2, "%s: <\n", __func__);
}

/*
 * UpdateLCD
*/
void UpdateLCD()
{
#if 0
	static int init_lcd = 1, old_cached_pages = -1, old_page = -1, old_subpage = -1, old_subpage_max = -1, old_hintmode = -1;
	int  x, y, subpage_max = 0, update_lcd = 0;

	if (lcd == -1) return; // for Dreamboxes without LCD-Display (5xxx)
	/* init or update lcd */
	if (init_lcd)
	{
		init_lcd = 0;

		for (y = 0; y < 64; y++)
		{
			int lcdbase = (y/8)*120;
			int lcdmask = 1 << (y%8);

			for (x = 0; x < 120; )
			{
				int rommask;
				int rombyte = lcd_layout[x/8 + y*120/8];

				for (rommask = 0x80; rommask; rommask >>= 1)
				{
					if (rombyte & rommask)
						lcd_backbuffer[x + lcdbase] |= lcdmask;
					else
						lcd_backbuffer[x + lcdbase] &= ~lcdmask;
					x++;
				}
			}
		}

		write(lcd, &lcd_backbuffer, sizeof(lcd_backbuffer));

		for (y = 16; y < 56; y += 8)	/* clear rectangle in backbuffer */
			for (x = 1; x < 118; x++)
				lcd_backbuffer[x + (y/8)*120] = 0;

		for (x = 3; x <= 116; x++)
			lcd_backbuffer[x + (39/8)*120] |= 1 << (39%8);

		for (y = 42; y <= 60; y++)
			lcd_backbuffer[35 + (y/8)*120] |= 1 << (y%8);

		for (y = 42; y <= 60; y++)
			lcd_backbuffer[60 + (y/8)*120] |= 1 << (y%8);

		RenderCharLCD(10, 43, 20);
		RenderCharLCD(11, 79, 20);

		return;
	}
	else
	{
		int p;

		if (inputcounter == 2)
			p = tuxtxt_cache.page;
		else
			p = temp_page + (0xDD >> 4*(1-inputcounter)); /* partial pageinput (filled with spaces) */

		/* page */
		if (old_page != p)
		{
			RenderCharLCD(p>>8,  7, 20);
			RenderCharLCD((p&0x0F0)>>4, 19, 20);
			RenderCharLCD(p&0x00F, 31, 20);

			old_page = p;
			update_lcd = 1;
		}

		/* current subpage */
		if (old_subpage != tuxtxt_cache.subpage)
		{
			if (!tuxtxt_cache.subpage)
			{
				RenderCharLCD(0, 55, 20);
				RenderCharLCD(1, 67, 20);
			}
			else
			{
				if (tuxtxt_cache.subpage >= 0xFF)
					tuxtxt_cache.subpage = 1;
				else if (tuxtxt_cache.subpage > 99)
					tuxtxt_cache.subpage = 0;

				RenderCharLCD(tuxtxt_cache.subpage>>4, 55, 20);
				RenderCharLCD(tuxtxt_cache.subpage&0x0F, 67, 20);
			}

			old_subpage = tuxtxt_cache.subpage;
			update_lcd = 1;
		}

		/* max subpage */
		for (x = 0; x <= 0x79; x++)
		{
			if (tuxtxt_cache.astCachetable[tuxtxt_cache.page][x])
				subpage_max = x;
		}

		if (old_subpage_max != subpage_max)
		{
			if (!subpage_max)
			{
				RenderCharLCD(0,  91, 20);
				RenderCharLCD(1, 103, 20);
			}
			else
			{
				RenderCharLCD(subpage_max>>4,  91, 20);
				RenderCharLCD(subpage_max&0x0F, 103, 20);
			}

			old_subpage_max = subpage_max;
			update_lcd = 1;
		}

		/* cachestatus */
		if (old_cached_pages != tuxtxt_cache.cached_pages)
		{
			#if 0
			int s;
			int p = tuxtxt_cache.cached_pages;
			for (s=107; s >= 107-4*fontwidth_small_lcd; s -= fontwidth_small_lcd)
			{
				int c = p % 10;
				if (p)
					RenderCharLCDsmall('0'+c, s, 44);
				else
					RenderCharLCDsmall(' ', s, 44);
				p /= 10;
			}
			#else
			RenderCharLCD(tuxtxt_cache.cached_pages/1000, 67, 44);
			RenderCharLCD(tuxtxt_cache.cached_pages%1000/100, 79, 44);
			RenderCharLCD(tuxtxt_cache.cached_pages%100/10, 91, 44);
			RenderCharLCD(tuxtxt_cache.cached_pages%10, 103, 44);
			#endif

			old_cached_pages = tuxtxt_cache.cached_pages;
			update_lcd = 1;
		}

		/* mode */
		if (old_hintmode != hintmode)
		{
			if (hintmode)
				RenderCharLCD(12, 43, 44);
			else
				RenderCharLCD(13, 43, 44);

			old_hintmode = hintmode;
			update_lcd = 1;
		}
	}

	if (update_lcd)
		write(lcd, &lcd_backbuffer, sizeof(lcd_backbuffer));
#endif
}

/*
 * DecodePage
*/
void DecodePage()
{
	int row, col;
	int hold, dhset;
	int foreground, background, doubleheight, doublewidth, charset, mosaictype, IgnoreAtBlackBgSubst, concealed, flashmode, boxwin;
	unsigned char held_mosaic, *p;
	tstCachedPage *pCachedPage;

	debugf(2, "%s: >\n", __func__);

	/* copy page to decode buffer */
	if (tuxtxt_cache.subpagetable[tuxtxt_cache.page] == 0xff) /* not cached: do nothing */
		return;
	if (tuxtxt_cache.zap_subpage_manual)
		pCachedPage = tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpage];
	else
		pCachedPage = tuxtxt_cache.astCachetable[tuxtxt_cache.page][tuxtxt_cache.subpagetable[tuxtxt_cache.page]];
	if (!pCachedPage)	/* not cached: do nothing */
		return;

	tuxtxt_decompress_page(tuxtxt_cache.page,tuxtxt_cache.subpage,&page_char[40]);

	memcpy(&page_char[8], pCachedPage->p0, 24); /* header line without timestring */

	pageinfo = &(pCachedPage->pageinfo);
	if (pageinfo->p24)
		memcpy(&page_char[24*40], pageinfo->p24, 40); /* line 25 for FLOF */

	/* copy timestring */
	memcpy(&page_char[32], &tuxtxt_cache.timestring, 8);

	/* check for newsflash & subtitle */
	if (pageinfo->boxed && tuxtxt_is_dec(tuxtxt_cache.page))
		boxed = 1;
	else
		boxed = 0;


	/* modify header */
	if (boxed)
		memset(&page_char, ' ', 40);
	else
	{
		memset(page_char, ' ', 8);
		hex2str((char*) page_char+3, tuxtxt_cache.page);
		if (tuxtxt_cache.subpage)
		{
			*(page_char+4) ='/';
			*(page_char+5) ='0';
			hex2str((char*) page_char+6, tuxtxt_cache.subpage);
		}

	}

	if (!tuxtxt_is_dec(tuxtxt_cache.page))
	{
		if (pageinfo->function == FUNC_MOT) /* magazine organization table */
		{
			printf("TuxTxt <decoding MOT %03x/%02x %d>\n", tuxtxt_cache.page, tuxtxt_cache.subpage, pageinfo->function);
			ClearBB(black);
			for (col = 0; col < 24*40; col++)
				page_atrb[col] = atrtable[ATR_WB];
			for (col = 40; col < 24*40; col++)
				page_char[col] = number2char(page_char[col]);
			boxed = 0;
			return ; /* don't interpret irregular pages */
		}
		else if (pageinfo->function == FUNC_GPOP || pageinfo->function == FUNC_POP) /* object definitions */
		{
			printf("TuxTxt <decoding *POP %03x/%02x %d>\n", tuxtxt_cache.page, tuxtxt_cache.subpage, pageinfo->function);
			ClearBB(black);
			for (col = 0; col < 24*40; col++)
				page_atrb[col] = atrtable[ATR_WB];
			p = page_char + 40;
			for (row = 1; row < 12; row++)
			{
				*p++ = number2char(row); /* first column: number (0-9, A-..) */
				for (col = 1; col < 40; col += 3)
				{
					int d = deh24(p);
					if (d < 0)
					{
						memcpy(p, "???", 3);
					p += 3;
					}
					else
					{
						*p++ = number2char((d >> 6) & 0x1f); /* mode */
						*p++ = number2char(d & 0x3f); /* address */
						*p++ = number2char((d >> 11) & 0x7f); /* data */
					}
				}
			}
			boxed = 0;
			return ; /* don't interpret irregular pages */
		}
		else if (pageinfo->function == FUNC_GDRCS || pageinfo->function == FUNC_DRCS) /* character definitions */
		{
			#define DRCSROWS 8
			#define DRCSCOLS (48/DRCSROWS)
			#define DRCSZOOMX 3
			#define DRCSZOOMY 5
			#define DRCSXSPC (12*DRCSZOOMX + 2)
			#define DRCSYSPC (10*DRCSZOOMY + 2)

			unsigned char ax[] = { /* array[0..12] of x-offsets, array[0..10] of y-offsets for each pixel */
				DRCSZOOMX * 0,
				DRCSZOOMX * 1,
				DRCSZOOMX * 2,
				DRCSZOOMX * 3,
				DRCSZOOMX * 4,
				DRCSZOOMX * 5,
				DRCSZOOMX * 6,
				DRCSZOOMX * 7,
				DRCSZOOMX * 8,
				DRCSZOOMX * 9,
				DRCSZOOMX * 10,
				DRCSZOOMX * 11,
				DRCSZOOMX * 12,
				DRCSZOOMY * 0,
				DRCSZOOMY * 1,
				DRCSZOOMY * 2,
				DRCSZOOMY * 3,
				DRCSZOOMY * 4,
				DRCSZOOMY * 5,
				DRCSZOOMY * 6,
				DRCSZOOMY * 7,
				DRCSZOOMY * 8,
				DRCSZOOMY * 9,
				DRCSZOOMY * 10
			};
			printf("TuxTxt <decoding *DRCS %03x/%02x %d>\n", tuxtxt_cache.page, tuxtxt_cache.subpage, pageinfo->function);
			ClearBB(black);
			for (col = 0; col < 24*40; col++)
				page_atrb[col] = atrtable[ATR_WB];


			for (row = 0; row < DRCSROWS; row++)
				for (col = 0; col < DRCSCOLS; col++)
					RenderDRCS(page_char + 20 * (DRCSCOLS * row + col + 2), lfb + (StartY + fontheight + DRCSYSPC * row + CFrameBuffer::getInstance()->getScreenHeight(true) ) * (CFrameBuffer::getInstance()->getScreenWidth(true) *4) + (StartX + DRCSXSPC * col)*4, ax, white, black);

			memset(page_char + 40, 0xff, 24*40); /* don't render any char below row 0 */
			boxed = 0;
			return ; /* don't interpret irregular pages */
		}
		else
		{
			printf("z\n");
			int i;
			int h, parityerror = 0;

			for (i = 0; i < 8; i++)
				page_atrb[i] = atrtable[ATR_WB];

			/* decode parity/hamming */
			for (i = 40; i < sizeof(page_char); i++)
			{
				page_atrb[i] = atrtable[ATR_WB];
				p = page_char + i;
				h = dehamming[*p];
				if (parityerror && h != 0xFF)	/* if no regular page (after any parity error) */
					hex2str((char*) p, h);	/* first try dehamming */
				else
				{
					if (*p == ' ' || deparity[*p] != ' ') /* correct parity */
						*p &= 127;
					else
					{
						parityerror = 1;
						if (h != 0xFF)	/* first parity error: try dehamming */
							hex2str((char*) p, h);
						else
							*p = ' ';
					}
				}
			}
			if (parityerror)
			{
				boxed = 0;
				return ; /* don't interpret irregular pages */
			}
		}
	}
	
	//printf("decode\n");
	
	/* decode */
	for (row = 0; row < ((showflof && pageinfo->p24) ? 25 : 24); row++)
	{
		/* start-of-row default conditions */
		foreground   = white;
		background   = black;
		doubleheight = 0;
		doublewidth  = 0;
		charset      = C_G0P;
		mosaictype   = 0;
		concealed    = 0;
		flashmode    = 0;
		hold         = 0;
		boxwin		 = 0;
		held_mosaic  = ' ';
		dhset        = 0;
		IgnoreAtBlackBgSubst = 0;

		if (boxed && memchr(&page_char[row*40], start_box, 40) == 0)
		{
			foreground = transp;
			background = transp;
		}

		for (col = 0; col < 40; col++)
		{
			int index = row*40 + col;

			page_atrb[index].fg = foreground;
			page_atrb[index].bg = background;
			page_atrb[index].charset = charset;
			page_atrb[index].doubleh = doubleheight;
			page_atrb[index].doublew = (col < 39 ? doublewidth : 0);
			page_atrb[index].IgnoreAtBlackBgSubst = IgnoreAtBlackBgSubst;
			page_atrb[index].concealed = concealed;
			page_atrb[index].flashing  = flashmode;
			page_atrb[index].boxwin    = boxwin;
			page_atrb[index].inverted  = 0; // only relevant for Level 2.5
			page_atrb[index].underline = 0; // only relevant for Level 2.5
			page_atrb[index].diacrit   = 0; // only relevant for Level 2.5
			page_atrb[index].setX26    = 0; // only relevant for Level 2.5
			page_atrb[index].setG0G2   = 0x3f; // only relevant for Level 2.5

			if (page_char[index] < ' ')
			{
				switch (page_char[index])
				{
				case alpha_black:
				case alpha_red:
				case alpha_green:
				case alpha_yellow:
				case alpha_blue:
				case alpha_magenta:
				case alpha_cyan:
				case alpha_white:
					concealed = 0;
					foreground = page_char[index] - alpha_black + black;
					if (col == 0 && page_char[index] == alpha_white)
						page_atrb[index].fg = black; // indicate level 1 color change on column 0; (hack)
					charset = C_G0P;
					break;

				case flash:
					flashmode = 1;
					break;
				case steady:
					flashmode = 0;
					page_atrb[index].flashing = 0;
					break;
				case end_box:
					boxwin = 0;
					IgnoreAtBlackBgSubst = 0;
/*					if (boxed)
					{
						foreground = transp;
						background = transp;
						IgnoreAtBlackBgSubst = 0;
					}
*/
					break;

				case start_box:
					if (!boxwin)
					{
						boxwin = 1;
						//background = 0x08;
					}
/*					if (boxed)
					{
						int rowstart = row * 40;
						if (col > 0)
							memset(&page_char[rowstart], ' ', col);
						for (clear = 0; clear < col; clear++)
						{
							page_atrb[rowstart + clear].fg = page_atrb[rowstart + clear].bg = transp;
							page_atrb[rowstart + clear].IgnoreAtBlackBgSubst = 0;
						}
					}
*/
					break;

				case normal_size:
					doubleheight = 0;
					doublewidth = 0;
					page_atrb[index].doubleh = doubleheight;
					page_atrb[index].doublew = doublewidth;
					break;

				case double_height:
					if (row < 23)
					{
						doubleheight = 1;
						dhset = 1;
					}
					doublewidth = 0;

					break;

				case double_width:
					if (col < 39)
						doublewidth = 1;
					doubleheight = 0;
					break;

				case double_size:
					if (row < 23)
					{
						doubleheight = 1;
						dhset = 1;
					}
					if (col < 39)
						doublewidth = 1;
					break;

				case mosaic_black:
				case mosaic_red:
				case mosaic_green:
				case mosaic_yellow:
				case mosaic_blue:
				case mosaic_magenta:
				case mosaic_cyan:
				case mosaic_white:
					concealed = 0;
					foreground = page_char[index] - mosaic_black + black;
					charset = mosaictype ? C_G1S : C_G1C;
					break;

				case conceal:
					page_atrb[index].concealed = 1;
					concealed = 1;
					if (!hintmode)
					{
						foreground = background;
						page_atrb[index].fg = foreground;
					}
					break;

				case contiguous_mosaic:
					mosaictype = 0;
					if (charset == C_G1S)
					{
						charset = C_G1C;
						page_atrb[index].charset = charset;
					}
					break;

				case separated_mosaic:
					mosaictype = 1;
					if (charset == C_G1C)
					{
						charset = C_G1S;
						page_atrb[index].charset = charset;
					}
					break;

				case esc:
					if (charset == C_G0P)
						charset = C_G0S;
					else if (charset == C_G0S)
						charset = C_G0P;
					break;

				case black_background:
					background = black;
					IgnoreAtBlackBgSubst = 0;
					page_atrb[index].bg = background;
					page_atrb[index].IgnoreAtBlackBgSubst = IgnoreAtBlackBgSubst;
					break;

				case new_background:
					background = foreground;
					if (background == black)
						IgnoreAtBlackBgSubst = 1;
					else
						IgnoreAtBlackBgSubst = 0;
					page_atrb[index].bg = background;
					page_atrb[index].IgnoreAtBlackBgSubst = IgnoreAtBlackBgSubst;
					break;

				case hold_mosaic:
					hold = 1;
					break;

				case release_mosaic:
					hold = 2;
					break;
				}

				/* handle spacing attributes */
				if (hold && (page_atrb[index].charset == C_G1C || page_atrb[index].charset == C_G1S))
					page_char[index] = held_mosaic;
				else
					page_char[index] = ' ';

				if (hold == 2)
					hold = 0;
			}
			else /* char >= ' ' */
			{
				/* set new held-mosaic char */
				if ((charset == C_G1C || charset == C_G1S) &&
					 ((page_char[index]&0xA0) == 0x20))
					held_mosaic = page_char[index];
				if (page_atrb[index].doubleh)
					page_char[index + 40] = 0xFF;

			}
			if (!(charset == C_G1C || charset == C_G1S))
				held_mosaic = ' '; /* forget if outside mosaic */

		} /* for col */

		/* skip row if doubleheight */
		if (row < 23 && dhset)
		{
			for (col = 0; col < 40; col++)
			{
				int index = row*40 + col;
				page_atrb[index+40].bg = page_atrb[index].bg;
				page_atrb[index+40].fg = white;
				if (!page_atrb[index].doubleh)
					page_char[index+40] = ' ';
				page_atrb[index+40].flashing = 0;
				page_atrb[index+40].charset = C_G0P;
				page_atrb[index+40].doubleh = 0;
				page_atrb[index+40].doublew = 0;
				page_atrb[index+40].IgnoreAtBlackBgSubst = 0;
				page_atrb[index+40].concealed = 0;
				page_atrb[index+40].flashing  = 0;
				page_atrb[index+40].boxwin    = page_atrb[index].boxwin;
			}
			row++;
		}
	} /* for row */
	FullScrColor = black;

	if (showl25)
		eval_l25();

	/* handle Black Background Color Substitution and transparency (CLUT1#0) */
	int r, c;
	int o = 0;
	char bitmask ;



	for (r = 0; r < 25; r++)
	{
		for (c = 0; c < 40; c++)
		{
			bitmask = (page_atrb[o].bg == 0x08 ? 0x08 : 0x00) | (FullRowColor[r] == 0x08 ? 0x04 : 0x00) | (page_atrb[o].boxwin <<1) | boxed;
			switch (bitmask)
			{
				case 0x08:
				case 0x0b:
					if (FullRowColor[r] == 0x08)
						page_atrb[o].bg = FullScrColor;
					else
						page_atrb[o].bg = FullRowColor[r];
					break;
					
				case 0x01:
				case 0x05:
				case 0x09:
				case 0x0a:
				case 0x0c:
				case 0x0d:
				case 0x0e:
				case 0x0f:
					page_atrb[o].bg = transp;
					break;
					
			}
			
			bitmask = (page_atrb[o].fg  == 0x08 ? 0x08 : 0x00) | (FullRowColor[r] == 0x08 ? 0x04 : 0x00) | (page_atrb[o].boxwin <<1) | boxed;
			switch (bitmask)
			{
				case 0x08:
				case 0x0b:
					if (FullRowColor[r] == 0x08)
						page_atrb[o].fg = FullScrColor;
					else
						page_atrb[o].fg = FullRowColor[r];
					break;
					
				case 0x01:
				case 0x05:
				case 0x09:
				case 0x0a:
				case 0x0c:
				case 0x0d:
				case 0x0e:
				case 0x0f:
					page_atrb[o].fg = transp;
					break;
			}
			o++;
		}
	}

	return ;

	debugf(2, "%s: <\n", __func__);
}

/******************************************************************************
 * GetRCCode                                                                  *
 ******************************************************************************/
#if 1
int GetRCCode()
{
	struct input_event ev;
	static __u16 rc_last_key = KEY_RESERVED;

        static uint64_t time_last = 0;
	RCCode = -1;


	int val = fcntl(rc, F_GETFL);
	if(!(val & O_NONBLOCK))
		printf("[tuxtxt] GetRCCode in blocking mode.\n");

	/* get code */
	if (read(rc, &ev, sizeof(ev)) == sizeof(ev))
	{
		if (ev.value)
		{

        		uint64_t time_now = (uint64_t) ev.time.tv_usec + (uint64_t)((uint64_t) ev.time.tv_sec * (uint64_t) 1000000);
			if (ev.code == rc_last_key && time_last + ttx_repeat_blocker> time_now
				&& (ev.code == KEY_DOWN || ev.code == KEY_UP || ev.code == KEY_LEFT || ev.code == KEY_RIGHT)) {
				// purge input buffer
				if(!(val & O_NONBLOCK))
					fcntl(rc, F_SETFL, val | O_NONBLOCK);
					while (read(rc, &ev, sizeof(ev)) == sizeof(ev));
				if(!(val & O_NONBLOCK))
					fcntl(rc, F_SETFL, val);
				return 0;
			}
			if (ev.code == rc_last_key && time_last + ttx_repeat_genericblocker > time_now) {
				// purge input buffer
				if(!(val & O_NONBLOCK))
					fcntl(rc, F_SETFL, val | O_NONBLOCK);
					while (read(rc, &ev, sizeof(ev)) == sizeof(ev));
				if(!(val & O_NONBLOCK))
					fcntl(rc, F_SETFL, val);
				return 0;
			}
			if (time_last + ttx_repeat_genericblocker < time_now)
				rc_last_key = KEY_RESERVED;
			time_last = time_now;

			{
				rc_last_key = ev.code;
				switch (ev.code)
				{
				case KEY_UP:		RCCode = RC_UP;		break;
				case KEY_DOWN:		RCCode = RC_DOWN;	break;
				case KEY_LEFT:		RCCode = RC_LEFT;	break;
				case KEY_RIGHT:		RCCode = RC_RIGHT;	break;
				case KEY_OK:		RCCode = RC_OK;		break;
				case KEY_0:		RCCode = RC_0;		break;
				case KEY_1:		RCCode = RC_1;		break;
				case KEY_2:		RCCode = RC_2;		break;
				case KEY_3:		RCCode = RC_3;		break;
				case KEY_4:		RCCode = RC_4;		break;
				case KEY_5:		RCCode = RC_5;		break;
				case KEY_6:		RCCode = RC_6;		break;
				case KEY_7:		RCCode = RC_7;		break;
				case KEY_8:		RCCode = RC_8;		break;
				case KEY_9:		RCCode = RC_9;		break;
				case KEY_RED:		RCCode = RC_RED;	break;
				case KEY_GREEN:		RCCode = RC_GREEN;	break;
				case KEY_YELLOW:	RCCode = RC_YELLOW;	break;
				case KEY_BLUE:		RCCode = RC_BLUE;	break;
				case KEY_VOLUMEUP:	RCCode = RC_PLUS;	break;
				case KEY_VOLUMEDOWN:	RCCode = RC_MINUS;	break;
				case KEY_MUTE:		RCCode = RC_MUTE;	break;

				/* on CS, change transparent mode with TEXT key */
				case KEY_TEXT:		RCCode = RC_TEXT;	break;

				case KEY_TTTV:		RCCode = RC_MUTE;	break;
				case KEY_TTZOOM:	RCCode = RC_PLUS;	break;
				case KEY_REVEAL:	RCCode = RC_HELP;	break;
				//case KEY_HELP:		RCCode = RC_HELP;	break;
				case KEY_INFO:		RCCode = RC_HELP;	break;
				case KEY_MENU:		RCCode = RC_DBOX;	break;
				case KEY_EXIT:		RCCode = RC_HOME;	break;

				case KEY_HOME:		RCCode = RC_HOME;	break;

				case KEY_POWER:		RCCode = RC_STANDBY;	break;
				}
printf("[tuxtxt] new key, code %X\n", RCCode);

				// purge input buffer
				if(!(val & O_NONBLOCK))
					fcntl(rc, F_SETFL, val | O_NONBLOCK);
				while (read(rc, &ev, sizeof(ev)) == sizeof(ev));
				if(!(val & O_NONBLOCK))
					fcntl(rc, F_SETFL, val);

				return 1;
			}
		}
	}


	// purge input buffer
	if(!(val & O_NONBLOCK))
		fcntl(rc, F_SETFL, val | O_NONBLOCK);
	while (read(rc, &ev, sizeof(ev)) == sizeof(ev));
	if(!(val & O_NONBLOCK))
		fcntl(rc, F_SETFL, val);

	usleep(1000000/25);

	return 0;
}
#else
/* this is obsolete and can soon be removed */
int GetRCCode()
{
	static unsigned short LastKey = -1;
	int count;
	if ((count = read(rc, &RCCode, 2)) != 2)
	{
		RCCode = -1;
		usleep(1000000/100);
		return 0;
	}

	fprintf(stderr, "rccode: %04x\n", RCCode);
	if (RCCode == LastKey &&
	    RCCode != 0x18 && RCCode != 0x19 && /* allow direction keys */
	    RCCode != 0x1b && RCCode != 0x1c)   /* to autorepeat...     */
	{
		RCCode = -1;
		return 1;
	}

	LastKey = RCCode;
	if ((RCCode & 0xFF00) == 0x0000)
	{
		switch (RCCode)
		{
		case 0x18:	RCCode = RC_UP;		break;
		case 0x1c:	RCCode = RC_DOWN;	break;
		case 0x19:	RCCode = RC_LEFT;	break;
		case 0x1b:	RCCode = RC_RIGHT;	break;
		case 0x1a:	RCCode = RC_OK;		break;
		case 0x0e:	RCCode = RC_0;		break;
		case 0x02:	RCCode = RC_1;		break;
		case 0x03:	RCCode = RC_2;		break;
		case 0x04:	RCCode = RC_3;		break;
		case 0x05:	RCCode = RC_4;		break;
		case 0x06:	RCCode = RC_5;		break;
		case 0x07:	RCCode = RC_6;		break;
		case 0x09:	RCCode = RC_7;		break;
		case 0x0a:	RCCode = RC_8;		break;
		case 0x0b:	RCCode = RC_9;		break;
		case 0x1f:	RCCode = RC_RED;	break;
		case 0x20:	RCCode = RC_GREEN;	break;
		case 0x21:	RCCode = RC_YELLOW;	break;
		case 0x22:	RCCode = RC_BLUE;	break;
		case 0x29:	RCCode = RC_PLUS;	break; // [=X=] key -> double height
		case 0x27:	RCCode = RC_MINUS;	break; // [txt] key -> split mode
		case 0x11:	RCCode = RC_MUTE;	break;
		case 0x28:	RCCode = RC_MUTE;	break; // [ /=] key
		case 0x14:	RCCode = RC_HELP;	break;
		case 0x2a:	RCCode = RC_HELP;	break; // [==?] key
		case 0x12:	RCCode = RC_DBOX;	break;
		case 0x15:	RCCode = RC_HOME;	break;
		case 0x01:	RCCode = RC_STANDBY;	break;
		}
		return 1;
	}
	return 1;
}
#endif
/* Local Variables: */
/* indent-tabs-mode:t */
/* tab-width:3 */
/* c-basic-offset:3 */
/* comment-column:0 */
/* fill-column:120 */
/* End: */