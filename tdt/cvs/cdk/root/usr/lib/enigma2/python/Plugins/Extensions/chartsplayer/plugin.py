# -*- coding: utf-8 -*-
#
# This plugin is open source but it is NOT free software.
#
# This plugin may only be distributed to and executed on hardware which
# is licensed by Dream Multimedia GmbH.
# In other words:
# It's NOT allowed to distribute any parts of this plugin or its source code in ANY way
# to hardware which is NOT licensed by Dream Multimedia GmbH.
# It's NOT allowed to execute this plugin and its source code or even parts of it in ANY way
# on hardware which is NOT licensed by Dream Multimedia GmbH.
#  
from Components.ActionMap import *
from Components.Label import Label
from Components.ScrollLabel import ScrollLabel
from Components.MenuList import MenuList
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmap, MultiContentEntryPixmapAlphaTest
from Components.GUIComponent import GUIComponent
from Components.config import config
from enigma import ePicLoad
from Tools.LoadPixmap import LoadPixmap
from Components.Pixmap import Pixmap
from Components.AVSwitch import AVSwitch
from Screens.InfoBar import MoviePlayer
from Components.PluginComponent import plugins
from Components.Button import Button
from Screens.Screen import Screen
from Plugins.Plugin import PluginDescriptor
from twisted.web.client import getPage
from twisted.web.client import downloadPage

from Screens.InfoBarGenerics import *
from Components.ServiceEventTracker import ServiceEventTracker, InfoBarBase

from Components.config import *
from Components.ConfigList import *

from Screens.HelpMenu import HelpableScreen
from Screens.InputBox import InputBox
from Screens.ChoiceBox import ChoiceBox
from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.InfoBar import MoviePlayer, InfoBar

from enigma import eListboxPythonMultiContent, eListbox, gFont, RT_HALIGN_LEFT, RT_HALIGN_RIGHT, RT_HALIGN_CENTER, loadPNG, RT_WRAP, eServiceReference, getDesktop, loadJPG
from Tools.Directories import pathExists, fileExists, SCOPE_SKIN_IMAGE, resolveFilename
import sys, os, base64, re, urllib2, random
from twisted.internet import defer
from twisted.web import client, error as weberror
from twisted.internet import reactor
from base64 import b64decode
from binascii import unhexlify
import hashlib, re, urllib, os

from urllib import unquote, urlencode
from time import strptime, mktime

config.plugins.chartsplayerconfig = ConfigSubsection()
config.plugins.chartsplayerconfig.repeat = ConfigSelection(default="0", choices = [("0",_("All")),("1",_("1"))])
config.plugins.chartsplayerconfig.random = ConfigSelection(default="0", choices = [("1",_("An")),("0",_("Aus"))])
config.plugins.chartsplayerconfig.startwith = ConfigSelection(default="0", choices = [("0",_("Aus")),("1",_("Top 100 Single Charts")),("2",_("Top 100 Pop")),("3",_("Top 100 Rock")),("4",_("Top 100 Rnb/Rap")),("5",_("Top 100 Filme")),("6",_("Top 100 Musik Clips")),("7",_("Top 100 Entertainment")),("8",_("Top 100 Diverse"))])
 
class charts(MenuList):
        def __init__(self, list):
                MenuList.__init__(self, list, False, eListboxPythonMultiContent)
                self.l.setFont(0, gFont("Regular", 14))
                self.l.setFont(1, gFont("Regular", 16))
                self.l.setFont(2, gFont("Regular", 18))
                self.l.setFont(3, gFont("Regular", 17))
                self.l.setFont(4, gFont("Regular", 20))
                self.l.setFont(5, gFont("Regular", 24))
                self.l.setFont(6, gFont("Regular", 26))
                self.l.setFont(7, gFont("Regular", 20))

### Own Menulist for the Playlist				
class playlist_show(GUIComponent, object):
	GUI_WIDGET = eListbox
	
	def __init__(self):
		GUIComponent.__init__(self)
		self.l = eListboxPythonMultiContent()
		self.l.setBuildFunc(self.buildList)
		self.l.setFont(0, gFont("Regular", 14))
                self.l.setFont(1, gFont("Regular", 16))
                self.l.setFont(2, gFont("Regular", 18))
                self.l.setFont(3, gFont("Regular", 17))
                self.l.setFont(4, gFont("Regular", 20))
                self.l.setFont(5, gFont("Regular", 24))
                self.l.setFont(6, gFont("Regular", 26))
                self.l.setFont(7, gFont("Regular", 20))
		self.l.setItemHeight(130)

	def buildList(self, entry):
		print entry
		width = self.l.getItemSize().width()
		res = [ None ]
		(token, title, place, jpg_store, runtime) = entry
		jpg = loadJPG(jpg_store)
#		infos = re.findall('(.*?).-.(.*[a-zA-Z0-9])', title, re.S)
                infos = re.findall('(.*?)(.*[a-zA-Z0-9])', title, re.S)
		(artist, songname) = infos[0]
		place = "%s." % str(place)
#		res.append((eListboxPythonMultiContent.TYPE_PIXMAP_ALPHATEST, 0, 0, 200, 110, jpg))
		res.append((eListboxPythonMultiContent.TYPE_PIXMAP_ALPHATEST, 0, 0, 300, 210, jpg))
                res.append((eListboxPythonMultiContent.TYPE_TEXT, 300, 10, 65, 33, 6, RT_HALIGN_LEFT, place))
		res.append((eListboxPythonMultiContent.TYPE_TEXT, 300, 55, 700, 28, 4, RT_HALIGN_LEFT, artist))
		res.append((eListboxPythonMultiContent.TYPE_TEXT, 300, 55, 700, 28, 4, RT_HALIGN_LEFT, songname))
		res.append((eListboxPythonMultiContent.TYPE_TEXT, 795, 85, 120, 18, 2, RT_HALIGN_RIGHT, runtime))
		return res
		
	def getCurrent(self):
		cur = self.l.getCurrentSelection()
		return cur and cur[0]

	def postWidgetCreate(self, instance):
		instance.setContent(self.l)
		self.instance.setWrapAround(True)

	def preWidgetRemove(self, instance):
		instance.setContent(None)

	def setList(self, list):
		self.l.setList(list)

	def moveToIndex(self, idx):
		self.instance.moveSelectionTo(idx)


### Play modus: Repat, Random, todo: Schuffle
class playconfig(ConfigListScreen,Screen):
	skin = """
		<screen position="center,center" size="400,200" title="MyVideo Top 100 Einstellungen">
			<widget name="config" position="10,10" size="390,190" transparent="1" scrollbarMode="showOnDemand" />
		</screen>"""

	def __init__(self, session):
		Screen.__init__(self, session)
		self.session = session	

		self.list = []
		self.list.append(getConfigListEntry("Repeat", config.plugins.chartsplayerconfig.repeat))
		self.list.append(getConfigListEntry("Random", config.plugins.chartsplayerconfig.random))
		self.list.append(getConfigListEntry("Start", config.plugins.chartsplayerconfig.startwith))
		ConfigListScreen.__init__(self, self.list)

		self["setupActions"] = ActionMap(["SetupActions"],
		{
			"ok": self.save,
			"cancel": self.cancel,
		}, -2)

	def save(self):
		for x in self["config"].list:
			print "debug:", x[1].value
			x[1].save()
		print "Repeat:", config.plugins.chartsplayerconfig.repeat.value
		print "Random:", config.plugins.chartsplayerconfig.random.value
		self.close(config.plugins.chartsplayerconfig.repeat.value, config.plugins.chartsplayerconfig.random.value)

	def cancel(self):
		for x in self["config"].list:
			x[1].cancel()
		print "Repeat:", config.plugins.chartsplayerconfig.repeat.value
		print "Random:", config.plugins.chartsplayerconfig.random.value
		self.close(config.plugins.chartsplayerconfig.repeat.value, config.plugins.chartsplayerconfig.random.value)

### Lyric
class lyrics(Screen):
	skin = """
		<screen position="center,center" size="650,550" title="">
			<widget name="info" position="10,3" size="600,28" zPosition="0" font="Regular;24" transparent="1" foregroundColor="white" />
			<widget name="lyric_info" position="10,30" size="635,520" zPosition="0" font="Regular;24" transparent="1" foregroundColor="white" />
			<widget name="list" position="10,30" size="635,520" transparent="1" scrollbarMode="showOnDemand" />
		</screen>"""
			
	def __init__(self, session, title):
		Screen.__init__(self, session)
		self.session = session
		self.title = title
		self["list"] = charts([])
		self["info"] = Label("")
		self.backinfo = "main"
		self.key_mode = "list"
		self["lyric_info"] = ScrollLabel()
		self["lyric_info"].hide()

		self["actions"] = ActionMap(["WizardActions", "MoviePlayerActions", "EPGSelectActions", "MediaPlayerSeekActions", "ColorActions"],
		{
			"cancel":	self.exit,
			"back":		self.exit,
			"ok":		self.ok,
			"up":		self.up,
			"down":		self.down,
		}, -1)

		self.onLayoutFinish.append(self.search_title)

	def up(self):
		print "bin UP"
		if self.key_mode == "list":
			self["list"].up()
		else:
			print "lyric upp ??"
			self["lyric_info"].pageUp()

	def down(self):
		print "bin DOWN"
		if self.key_mode == "list":
			self["list"].down()
		else:
			self["lyric_info"].pageDown()

	def search_title(self):
		print "starte suche"
		self["info"].setText("Suche..")
		url = "http://www.lyricsmania.com/searchnew.php"
		self.title = self.title.replace('--','').replace('  ',' ')
		print self.title
		info = urlencode({'k': self.title})
		getPage(url, method='POST', postdata=info, headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.search_post)
		
	def search_post(self, data):
		print "suche gefunden"
		info = re.findall('<h1><a href="(.*?)" title=".*?">(.*?)</a>', data, re.S)
		self.count = len(info)
		self.list = []
		self.info = info
		for each in self.info:
			(url, title) = each
			url = "http://www.lyricsmania.com%s" % url
			print "erg:", title, url
			self.list.append(self.list_lyrics(title, url))
			self["list"].setList(self.list)
			self["list"].l.setItemHeight(24)

		self["info"].setText("Found %s Lyrics" % self.count)
	
	def list_lyrics(self, title, url):
		res = [(title, url)]
		print "zeige"
		print title
		res.append(MultiContentEntryText(pos=(0, 0), size=(630, 24), font=4, text=title, flags=RT_HALIGN_LEFT))
		return res

	def ok(self):
		if not self.backinfo == "back":
			item = self["list"].getCurrent()
			url = item[0][1]
			name = item[0][0]
			self["info"].setText("Lade daten.. %s" % name)
			if url:
				print url
				self.backinfo = "back"
				self.key_mode = "lyric"
				getPage(url, method='GET', headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.get_lyric)

	def get_lyric(self, data):
		print "hole infos"
		info = re.findall("<div id='songlyrics_h' class='dn'>(.*?)</div>", data, re.S)
		if info:
			self["info"].setText("")
			print "infos da."
			self.list = []
			info = info[0].replace('<br />','')
			self["list"].hide()
			self["lyric_info"].setText(info)
			self["lyric_info"].show()
		else:
			self["info"].setText("Keine Lyric gefunden..")

	def exit(self):
		if self.backinfo == "back":
			self["lyric_info"].hide()
			self["list"].show()
			self.backinfo = "main"
			self.key_mode = "list"
			self.list = []
			for each in self.info:
                        	(url, title) = each
                        	url = "http://www.lyricsmania.com%s" % url
                        	print "erg:", title, url
                        	self.list.append(self.list_lyrics(title, url))
                        	self["list"].setList(self.list)
                        	self["list"].l.setItemHeight(22)
		else:
			self.close()

### Playlist: Show's Playlist while playing a video			
class playlist(Screen):
	if getDesktop(0).size().width() == 1280:
		skin = """
			<screen position="center,80" size="1180,590" title="MyVideo Top 100 Streamer v1.3">
				<widget name="list" position="5,48" scrollbarMode="showOnDemand" transparent="1" size="965,520" zPosition="4"/>
				<widget name="info" position="365,06" size="550,34" zPosition="8" font="Regular;23" transparent="1" foregroundColor="#E55451" />
			        <ePixmap position="985,530" size="200,28" pixmap="~/images/music.png" alphatest="blend" zPosition="5"/>
                        </screen>"""
			
	if getDesktop(0).size().width() == 1024:
		skin = """
			<screen position="center,center" size="650,420" title="MyVideo Top 100 Streamer v1.3">
				<ePixmap position="550,1" size="48,48" pixmap="~/images/music.png" alphatest="blend" />
				<widget name="list" position="0,50" size="650,330" transparent="1" scrollbarMode="showOnDemand" />
				<widget name="info" position="65,13" size="550,24" zPosition="0" font="Regular;24" transparent="1" foregroundColor="white" />
				<ePixmap position="5,8" size="32,32" pixmap="~/images/play.png" alphatest="blend" />
			</screen>"""
	
	if getDesktop(0).size().width() == 720:
		skin = """
			<screen position="center,center" size="650,420" title="MyVideo Top 100 Streamer v1.3">
				<ePixmap position="550,1" size="48,48" pixmap="~/images/music.png" alphatest="blend" />
				<widget name="list" position="0,50" size="650,330" transparent="1" scrollbarMode="showOnDemand" />
				<widget name="info" position="65,13" size="550,24" zPosition="0" font="Regular;24" transparent="1" foregroundColor="white" />
				<ePixmap position="5,8" size="32,32" pixmap="~/images/play.png" alphatest="blend" />
			</screen>"""

	def __init__(self, session, chartlist, plugin_path, current_index):
		Screen.__init__(self, session)
		self.session = session
		self.plugin_path = plugin_path
                self.skin_path = plugin_path
		self.chartlist = chartlist
		self.current_index = current_index

                self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
                {
                        "cancel":       self.exit,
                        "ok":           self.ok,
                }, -1)

                self["info"] = Label("")
                self["info"].setText(" ")

		self["list"] = playlist_show()
		self.onLayoutFinish.append(self.show_playlist)

	def show_playlist(self):
		print "test"
		entryList = []
		for each in self.chartlist:
			(data, token, title, place, jpg_store, runtime) = each
			entryList.append(((token, title, place, jpg_store, runtime),))

		self["list"].setList(entryList)
		self["list"].moveToIndex(int(self.current_index))

	def exit(self):
		self.close("noplay")

	def ok(self):
		item = self["list"].getCurrent()
		filename = item[1]
		cur_index = item[2]
		print filename, cur_index 
		self.close(str(cur_index))

### Movieplayer
class MusicChartPlayer(Screen, InfoBarBase, InfoBarSeek, InfoBarNotifications, InfoBarShowHide):
	ENABLE_RESUME_SUPPORT = True
	ALLOW_SUSPEND = True
		
	def __init__(self, session, chartlist, index, filename, plugin_path):
		Screen.__init__(self, session)
		self.session = session
                self.plugin_path = plugin_path
                self.skin_path = plugin_path

		InfoBarNotifications.__init__(self)
		InfoBarBase.__init__(self)
		InfoBarShowHide.__init__(self)

		self["actions"] = ActionMap(["WizardActions", "MoviePlayerActions", "EPGSelectActions", "MediaPlayerSeekActions", "ColorActions", "InfobarShowHideActions", "InfobarActions"],
		{
			"leavePlayer":		        self.leavePlayer,
			"back":				self.leavePlayer,
			"left":				self.previous,
			"right":			self.next,
			"info":				self.lyrics,
			"up":				self.playlist,
			"down":				self.random_now,
			"input_date_time":	        self.playconfig,
			"menu":	                        self.playconfig,

		}, -1)

		self.allowPiP = False
		InfoBarSeek.__init__(self, actionmap = "MediaPlayerSeekActions")

		self.returning = False
		self.skinName = "MoviePlayer"
		self.lastservice = session.nav.getCurrentlyPlayingServiceReference()

		self.filename = filename
		self.chartList = chartlist
		self.index = index
		print self.index

		self.repeat = config.plugins.chartsplayerconfig.repeat.value
		self.random = config.plugins.chartsplayerconfig.random.value

		self.GK = ('WXpnME1EZGhNRGhpTTJNM01XVmhOREU0WldNNVpHTTJOakpt'
			'TW1FMU5tVTBNR05pWkRaa05XRXhNVFJoWVRVd1ptSXhaVEV3'
			'TnpsbA0KTVRkbU1tSTRNdz09')

		self.onLayoutFinish.append(self.play)

	def __md5(self, s):
		return hashlib.md5(s).hexdigest()

	def __rc4crypt(self, data, key):
		x = 0
		box = range(256)
		for i in range(256):
			x = (x + box[i] + ord(key[i % len(key)])) % 256
			box[i], box[x] = box[x], box[i]
		x = 0
		y = 0
		out = []
		for char in data:
			x = (x + 1) % 256
			y = (y + box[x]) % 256
			box[x], box[y] = box[y], box[x]
			out.append(chr(ord(char) ^ box[(box[x] + box[y]) % 256]))
		return ''.join(out)	

	def play(self):
		self.onClose.append(self.__onClose)
		for each in self.chartList:
			if int(self.index) == int(each[3]):
				data = each[0]
				token = each[1]
				self.filename = each[2]
				place = each[3]
				title = "%s. %s" % (place, self.filename)
				enc_data_b = unhexlify(data)
				sk = self.__md5(b64decode(b64decode(self.GK)) + self.__md5(str(token)))
				dec_data = self.__rc4crypt(enc_data_b, sk)
				if dec_data:
					url = re.findall("connectionurl='(.*?)'", dec_data, re.S)
					source = re.findall("source='(.*?)'", dec_data, re.S)
					url =  unquote(url[0])
					source =  unquote(source[0])
					vorne = re.findall('(.*?)\.', source, re.S)
					hinten = re.findall('\.(.*[a-zA-Z0-9])', source, re.S)
					string23 = "/%s:%s" % (hinten[0], vorne[0])
					link = "%s%s" % (url, string23)
					ref = eServiceReference(0x1001, 0, link)
					ref.setName(title)
					self.session.nav.stopService()
					self.session.nav.playService(ref)

	def errorload(self, error):
		print "[ChartsPlayer]:", error

	def playlist(self):
		print "playlist."
		current_index = self.index - 1
		self.session.openWithCallback(self.danach, playlist, self.chartList, self.plugin_path, current_index)

	def lyrics(self):
		self.session.open(lyrics, self.filename)

	def playconfig(self):
		self.session.openWithCallback(self.playconfig_save, playconfig)

	def playconfig_save(self, repeat, random):
		self.repeat = repeat
		self.random = random

	def danach(self, data):
		if not data == "noplay":
			self.index = int(data)
			self.play()
#		from Screens.InfoBar import InfoBar
#                if InfoBar and InfoBar.instance:
#                        InfoBar.hide(InfoBar.instance)

	def leavePlayer(self):
		self.close()

	def next(self):
		self.index += 1
		if len(self.chartList) < self.index -1:
			self.index = 1
			self.play()
		else:
			self.play()

	def previous(self):
		print "previous Song"
		self.index -= 1
		print self.index
		if int(self.index) == int(0):
			self.index = len(self.chartList)
			self.play()
		else:
			self.play()

	def random_now(self):
		print "Random now:", self.index
		self.index = random.randint(1, len(self.chartList))
		print "Random create:", self.index
		self.play()

	def doEofInternal(self, playing):		
		print "current song:", self.index
		if int(self.random) == int(1):
			print "Random now:", self.index
			self.index = random.randint(1, len(self.chartList))
			print "Random create:", self.index
			self.play()

		elif int(self.repeat) == int(1):
			print "Repeat now:", self.index
			self.play()
		        self.hide()
                else:
			print "next song now:", self.index
			self.index += 1
			print "next song create:", self.index
			if len(self.chartList) < self.index -1:
				self.close()
			else:
				self.play()
                                self.hide()
                                
	def __onClose(self):
                self.session.nav.playService(self.lastservice)

class chartsplayer(Screen):
	if getDesktop(0).size().width() == 1280:
		skin = """
				<screen position="center,80" size="1180,590" title="MyVideo Top 100 Streamer v1.3">
				<widget name="myTVmenulist" position="5,48" scrollbarMode="showOnDemand" transparent="1" size="965,520" zPosition="4"/>
				<widget name="info" position="365,06" size="550,34" zPosition="8" font="Regular;23" transparent="1" foregroundColor="#E55451" />
                                <ePixmap alphatest="on" pixmap="~/images/key_red.png" position="995,20" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_green.png" position="995,60" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_yellow.png" position="995,100" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_blue.png" position="995,140" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_1.png" position="995,180" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_2.png" position="995,220" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_3.png" position="995,260" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_4.png" position="995,300" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_5.png" position="995,340" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_6.png" position="995,380" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_7.png" position="995,420" size="35,25" zPosition="5"/>
				<ePixmap alphatest="on" pixmap="~/images/key_8.png" position="995,460" size="35,25" zPosition="5"/>
				<ePixmap position="985,530" size="200,28" pixmap="~/images/music.png" alphatest="blend" zPosition="5"/>
                                <eLabel font="Regular;18" halign="left" position="1040,20" size="140,25" text="Einstellungen" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,60" size="140,25" text="Download" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,100" size="140,25" text=" " transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,140" size="140,25" text=" " transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,180" size="140,25" text="Single Charts" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,220" size="140,25" text="POP" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,260" size="140,25" text="Rock" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,300" size="140,25" text="Rnb/Rap" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,340" size="140,25" text="Filme" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,380" size="140,25" text="Musik Clips" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,420" size="140,25" text="Entertainment" transparent="1" valign="center" zPosition="6"/>
				<eLabel font="Regular;18" halign="left" position="1040,460" size="140,25" text="Diverse" transparent="1" valign="center" zPosition="6"/>
				</screen>"""
			
	if getDesktop(0).size().width() == 1024:
		skin = """
			<screen position="center,center" size="650,430" title="MyVideo Top 100 Music Charts Streamer v1.0">
				<ePixmap position="450,1" size="200,48" pixmap="~/images/music.png" alphatest="blend" />
				<widget name="myTVmenulist" position="0,50" size="650,330" transparent="1" scrollbarMode="showOnDemand" />
				<widget name="info" position="65,13" size="550,24" zPosition="0" font="Regular;24" transparent="1" foregroundColor="white" />
				<ePixmap position="5,8" size="32,32" pixmap="~/images/play.png" alphatest="blend" />
				<widget name="ButtonRed" position="18,414" pixmap="~/images/red.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextRed" position="18,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
				<widget name="ButtonGreen" position="176,414" pixmap="~/images/green.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextGreen" position="176,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
				<widget name="ButtonYellow" position="334,414" pixmap="~/images/yellow.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextYellow" position="334,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
				<widget name="ButtonBlue" position="492,414" pixmap="~/images/blue.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextBlue" position="492,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
			</screen>"""
	
	if getDesktop(0).size().width() == 720:
		skin = """
			<screen position="center,center" size="650,430" title="MyVideo Top 100 Music Charts Streamer v1.0">
				<ePixmap position="450,1" size="200,48" pixmap="~/images/music.png" alphatest="blend" />
				<widget name="myTVmenulist" position="0,50" size="650,330" transparent="1" scrollbarMode="showOnDemand" />
				<widget name="info" position="65,13" size="550,24" zPosition="0" font="Regular;24" transparent="1" foregroundColor="white" />
				<ePixmap position="5,8" size="32,32" pixmap="~/images/play.png" alphatest="blend" />
				<widget name="ButtonRed" position="18,414" pixmap="~/images/red.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextRed" position="18,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
				<widget name="ButtonGreen" position="176,414" pixmap="~/images/green.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextGreen" position="176,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
				<widget name="ButtonYellow" position="334,414" pixmap="~/images/yellow.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextYellow" position="334,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
				<widget name="ButtonBlue" position="492,414" pixmap="~/images/blue.png" zPosition="10" size="140,8" transparent="1" alphatest="on" />
				<widget name="TextBlue" position="492,390" size="140,24" valign="center" halign="center" zPosition="10" font="Regular;18" transparent="1" />
			</screen>"""
		
	def __init__(self, session, plugin_path):
		Screen.__init__(self, session)
		self.session = session
		self.plugin_path = plugin_path
		self.skin_path = plugin_path

		self["info"] = Label("")
		self["info"].setText("Kategorie auswahl...")

		self["ButtonRed"] = Pixmap()
		self["TextRed"] = Label("Single")
		self["ButtonGreen"] = Pixmap()
		self["TextGreen"] = Label("Pop")
		self["ButtonYellow"] = Pixmap()
		self["TextYellow"] = Label("Rock")
		self["ButtonBlue"] = Pixmap()
		self["TextBlue"] = Label("Rnb/Rap")

		self["myTVmenulist"] = charts([])
		self.loading = 0
		self.filename = ""
		self.go = "yes"
		self.play = "play"
		self.exit_now = "exit"
		self.repeat = config.plugins.chartsplayerconfig.repeat.value
		self.random = config.plugins.chartsplayerconfig.random.value
		self.startwith = config.plugins.chartsplayerconfig.startwith.value

		self["InputActions"] = HelpableActionMap(self, "InputActions",
 		{
 			"1": self.top,
 			"2": self.pop,
 			"3": self.rock,
 			"4": self.rnb,
                        "5": self.top100filme,
 			"6": self.top100musikclips,
 			"7": self.top100entertainment,
 			"8": self.top100diverse,
 #			"9": self.keyNumberGlobal,
 #			"0": self.keyNumberGlobal
 		}, -1)
 		
                self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
		{
			"cancel":	self.exit,
			"ok":		self.ok,
		}, -1)

		self["ColorActions"] = HelpableActionMap(self, "ColorActions",
		{
			"red":		self.playconfig,
			"green":	self.downloadvid,
#			"yellow":	self.x,
#			"blue":		self.x,
		}, -1)

		self["MovieSelectionActions"] = HelpableActionMap(self, "MovieSelectionActions",
		{
			"contextMenu":		self.playconfig,
		}, -1)

		self.onLayoutFinish.append(self.dir)

		if self.startwith == "0":
			pass
		elif self.startwith == "1":
			self.onLayoutFinish.append(self.top)
		elif self.startwith == "2":
			self.onLayoutFinish.append(self.pop)
		elif self.startwith == "3":
			self.onLayoutFinish.append(self.rock)
		elif self.startwith == "4":
			self.onLayoutFinish.append(self.rnb)
                elif self.startwith == "5":
			self.onLayoutFinish.append(self.top100filme)
		elif self.startwith == "6":
			self.onLayoutFinish.append(self.top100musikclips)
		elif self.startwith == "7":
			self.onLayoutFinish.append(self.top100entertainment)
		elif self.startwith == "8":
			self.onLayoutFinish.append(self.top100diverse)
                        	
	def playconfig(self):
		self.session.openWithCallback(self.playconfig_save, playconfig)

	def playconfig_save(self, repeat, random):
		self.repeat = repeat
		self.random = random

	def top(self):
		if self.go == "yes":
			self.genre = "Top 100 Single Charts"
			self.list("1")
		else:
			message = self.session.open(MessageBox, _("lade Liste... Bitte warten"), MessageBox.TYPE_INFO, timeout=3)

	def pop(self):
		if self.go == "yes":
			self.genre = "Top 100 Pop"
			self.list("2")
		else:
			message = self.session.open(MessageBox, _("lade Liste... Bitte warten"), MessageBox.TYPE_INFO, timeout=3)

	def rock(self):
		if self.go == "yes":
			self.genre = "Top 100 Rock"
                	self.list("3")
		else:
			message = self.session.open(MessageBox, _("lade Liste... Bitte warten"), MessageBox.TYPE_INFO, timeout=3)
			
	def rnb(self):
		if self.go == "yes":
			self.genre = "Top 100 Rnb and Rap"
                	self.list("4")
		else:
			message = self.session.open(MessageBox, _("lade Liste... Bitte wartent"), MessageBox.TYPE_INFO, timeout=3)

	def top100filme(self):
		if self.go == "yes":
			self.genre = "Top 100 Filme"
                	self.list("5")
		else:
			message = self.session.open(MessageBox, _("lade Liste... Bitte wartent"), MessageBox.TYPE_INFO, timeout=3)
	
	def top100musikclips(self):
		if self.go == "yes":
			self.genre = "Top 100 Musik Clips"
                	self.list("6")
		else:
			message = self.session.open(MessageBox, _("lade Liste... Bitte warten"), MessageBox.TYPE_INFO, timeout=3)
	
	def top100entertainment(self):
		if self.go == "yes":
			self.genre = "Top 100 Entertainment"
                	self.list("7")
		else:
			message = self.session.open(MessageBox, _("lade Liste.... Bitte warten"), MessageBox.TYPE_INFO, timeout=3)
	
	def top100diverse(self):
		if self.go == "yes":
			self.genre = "Top 100 Diverse"
                	self.list("8")
		else:
			message = self.session.open(MessageBox, _("lade Liste.... Bitte warten"), MessageBox.TYPE_INFO, timeout=3)
	
	def downloadvid(self):                     
                self.session.open(MessageBox, _("Download ist verfÃ¼gbar ab der nÃ¤chsten version"), MessageBox.TYPE_INFO, timeout=6)

        def list(self, which):
		self.go = "no"
		self.play = "play"
		self.exit_now = "exit"
		self.myTVmenulist = []
		self.own_playlist = []
		if which == "1":
			self["info"].setText("Top 100 Single Charts")
			url = "http://www.myvideo.de/Top_100/Top_100_Single_Charts"
		elif which == "2":
			self["info"].setText("Top 100 Pop")
			url = "http://www.myvideo.de/Musik/Musik_Charts/Top_100_Pop"
		elif which == "3":
			self["info"].setText("Top 100 Rock")
			url = "http://www.myvideo.de/Musik/Musik_Charts/Top_100_Rock"
		elif which == "4":
			self["info"].setText("Top 100 Rnb/Rap")
			url = "http://www.myvideo.de/Musik/Musik_Charts/Top_100_Rap/R%26amp%3Bamp%3BB"
                elif which == "5":
			self["info"].setText("Top 100 Filme")
			url = "http://www.myvideo.de/Top_100/Top_100_Filme"
		elif which == "6":
			self["info"].setText("Top 100 Musik Clips")
			url = "http://www.myvideo.de/Top_100/Top_100_Musik_Clips"
		elif which == "7":
			self["info"].setText("Top 100 Entertainment")
			url = "http://www.myvideo.de/Top_100/Top_100_Entertainment"
                elif which == "8":
			self["info"].setText("Top 100 Diverse")
			url = "http://www.myvideo.de/Musik/Musik_Charts/Top_100_Diverse"
			
		getPage(url, method='GET', headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.charts_list).addErrback(self.errorload)

	def charts_list(self, data):
# old V1.0	songs = re.findall("<a href='/watch/.*?' title='(.*?)'><img id='i(.*?)' src='(.*?)'.*?<span class='vViews'>(.*?)</span>.*?<span class='chartTop.*?'>(.*?)</span>", data, re.S)
                songs = re.findall("<a href='/watch/.*?' title='(.*?)'><img id='i(\d+)'.*?longdesc='(.*?.jpg)'.*?<span class='vViews'>(.*?)</span>.*?<span class='chartTop.*?'>(.*?)</span>", data, re.S)
		self.count = len(songs)
		self.loading = 0
		url_list = []
                
		for each in songs:
			(title, token, image, runtime, place) = each
			jpg_store = "/tmp/mtv/%s.jpg" % str(place)
			title = self.umlaute(title)
			url_list.append((token, title, place, jpg_store, runtime, image))

		if len(url_list) != 0:
			ds = defer.DeferredSemaphore(tokens=1)
			downloads = [ds.run(self.download,item[5], item[3]).addCallback(self.create_own_playlist, item[0], item[1], item[2], item[3], item[4]).addErrback(self.errorload) for item in url_list]
			finished = defer.DeferredList(downloads).addErrback(self.errorload)

	def download(self, image, jpg_store):
		print "[Chartsplyer]: download image..."
		return downloadPage(image, jpg_store)

	def create_own_playlist(self, data, token, title, place, jpg_store, runtime):
		print "[Chartsplyer]: Create links.."
		url = "http://www.myvideo.de/dynamic/get_player_video_xml.php?flash_playertype=D&ID=%s&_countlimit=4&autorun=yes" % token
		getPage(url, method='GET', headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.get_xml, token, title, place, jpg_store, runtime).addErrback(self.errorload)
		
	def get_xml(self, data, token, title, place, jpg_store, runtime):
		self.loading += 1
		data = data.replace("_encxml=","")
		self.own_playlist.append([data, token, title, place, jpg_store, runtime])
		self.myTVmenulist.append(self.list_charts(token, title, place, jpg_store, runtime))
		#self.myTVmenulist.sort(key=lambda x: int(x[0][2]))
		self["myTVmenulist"].setList(self.myTVmenulist)
		self["myTVmenulist"].l.setItemHeight(130)
		if int(self.loading) == int(self.count):
			self.go = "yes"
			self["info"].setText(self.genre)
		else:
			self["info"].setText("Lade.. %s von %s" % (self.loading, self.count))

	def ok(self):
		if self.play == "play":
			item = self["myTVmenulist"].getCurrent()
			filename = item[0][1]
			cur_index = item[0][2]
			print filename, cur_index
			self.session.open(MusicChartPlayer, self.own_playlist, int(cur_index), filename, self.plugin_path)
		
	def play(self, filename, cur_index):
                self.session.open(MusicChartPlayer, self.own_playlist, int(cur_index), filename, self.plugin_path)

	def list_charts(self, token, title, place, jpg_store, runtime):
		res = [ (token, title, place, jpg_store, runtime) ]
#old 		infos = re.findall('(.*?).-.(.*[a-zA-Z0-9])', title, re.S)      
                infos = re.findall('(.*?)(.*[a-zA-Z0-9])', title, re.S)
                (artist, songname) = infos[0]
		place = "%s." % str(place)
		if fileExists(jpg_store):
#old			res.append(MultiContentEntryPixmapAlphaTest(pos=(0, 0), size=(200, 110), png=loadJPG(jpg_store)))
			res.append(MultiContentEntryPixmapAlphaTest(pos=(0, 0), size=(300, 210), png=loadJPG(jpg_store)))
                res.append(MultiContentEntryText(pos=(300, 10), size=(65, 33), font=6, text=place, color=0x00dab329, flags=RT_HALIGN_LEFT))
		res.append(MultiContentEntryText(pos=(300, 55), size=(700, 28), font=4, text=artist, flags=RT_HALIGN_LEFT))
		res.append(MultiContentEntryText(pos=(300, 55), size=(700, 28), font=4, text=songname, flags=RT_HALIGN_LEFT))
		res.append(MultiContentEntryText(pos=(795, 85), size=(120, 18), font=2, text=runtime, flags=RT_HALIGN_RIGHT))
		return res

	def dir(self):
		if not pathExists("/tmp/mtv/"):
                        os.system("mkdir /tmp/mtv/")
			print "[MTV] /tmp/mtv/ erstellt."
		else:
			print "[MTV] /tmp/mtv/ vorhanden."

	def exit(self):
		if self.exit_now == "exit":
			self.close()
			
	def errorload(self, error):
		print "[ChartsPlayer]:", error

	def umlaute(self, text):
		text = text.replace('&nbsp;'," ").replace("&#39;","'").replace('&quot;','"').replace('&ndash;',"-")
		text = text.replace('&copy;.*'," ").replace('&amp;',"&")#.replace('&uuml;',"Ã¼").replace('&auml;',"Ã¤").replace('&ouml;',"Ã¶")
#		text = text.replace('&Uuml;',"Ã.").replace('&Auml;',"Ã.").replace('&Ouml;',"Ã.")
		text = text.replace("&#039;","'")
# fixed utf-8 umlaute/sonderzeichen --------------------------------------------------------------------------------------------------------------------------------------------#		
                text = text.replace('--',"-")
                text = text.replace('- -',"-")    
                text = text.replace("&agrave;","Ã ") #à 
		text = text.replace("&aacute;","Ã¡") #á
		text = text.replace("&acirc;","Ã¢")  #â
		text = text.replace("&auml;","Ã¤")   #ä
		text = text.replace('&aelig;',"Ã¦")  #æ
		text = text.replace('&ccedil;',"Ã§") #ç
		text = text.replace('&egrave;',"Ã¨") #è
		text = text.replace("&eacute;","Ã©") #é
		text = text.replace('&ecirc;',"Ãª")  #ê
		text = text.replace('&euml;',"Ã«")   #ë
		text = text.replace('&igrave;',"Ã¬") #ì
		text = text.replace('&iacute;',"Ã")  #í
		text = text.replace('&icirc;',"Ã®")  #î
		text = text.replace('&ntilde;',"Ã±") #ñ
		text = text.replace('&ograve;',"Ã²") #ò
		text = text.replace('&oacute;',"Ã³") #ó
		text = text.replace('&ocirc;',"Ã´")  #ô
		text = text.replace('&otilde;',"Ãµ") #õ
		text = text.replace('&ouml;',"Ã¶")   #ö
		text = text.replace('&oslash;',"Ã¸") #ø
		text = text.replace('&ugrave;',"Ã¹") #ù
		text = text.replace('&uacute',"Ãº")  #ú
		text = text.replace('&ucirc;',"Ã»")  #û
		text = text.replace('&uuml;',"Ã¼")   #ü
		text = text.replace('&Auml',"Ã„")    #Ä
		text = text.replace('&Ouml',"Ã–")    #Ö
		text = text.replace('&Uuml',"Ãœ")    #Ü
		text = text.replace('&szlig;',"ÃŸ")  #ß
                return text

def main(session, **kwargs):
	session.open(chartsplayer, plugin_path)

def Plugins(path, **kwargs):
	global plugin_path
	plugin_path = path
	return [
        	PluginDescriptor(name="MyVideo.de", description="Top 100", where = [PluginDescriptor.WHERE_EXTENSIONSMENU], fnc=main),		
# old V1.0      PluginDescriptor(name="MyVideo Top 100 Music Streamer", description="Stream Top Music Charts direct to your TV", where = [PluginDescriptor.WHERE_PLUGINMENU], fnc=main)
                PluginDescriptor(name="MyVideo.de", description="Top 100", where = [PluginDescriptor.WHERE_PLUGINMENU], icon="plugin.png", fnc=main)
		]
