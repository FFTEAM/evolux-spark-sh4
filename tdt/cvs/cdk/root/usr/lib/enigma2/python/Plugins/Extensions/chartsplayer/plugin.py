# -*- coding: utf-8 -*-
from Components.ActionMap import *
from Components.Label import Label
from Components.MenuList import MenuList
from Components.MultiContent import MultiContentEntryText, MultiContentEntryPixmap, MultiContentEntryPixmapAlphaTest
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

from Screens.HelpMenu import HelpableScreen
from Screens.InputBox import InputBox
from Screens.ChoiceBox import ChoiceBox
from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.InfoBar import MoviePlayer, InfoBar

from enigma import eListboxPythonMultiContent, eListbox, gFont, RT_HALIGN_LEFT, RT_HALIGN_RIGHT, RT_HALIGN_CENTER, loadPNG, RT_WRAP, eServiceReference, getDesktop, loadJPG
from Tools.Directories import pathExists, fileExists, SCOPE_SKIN_IMAGE, resolveFilename
import sys, os, base64, re
from twisted.web import client, error as weberror
from twisted.internet import reactor
from urllib import urlencode
from time import strptime, mktime

if fileExists("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/plugin.pyc"):

        from Plugins.Extensions.JobManager.plugin import *
        JobManagerInstalled = True
else:
        JobManagerInstalled = False

class charts(MenuList):
        def __init__(self, list):
                MenuList.__init__(self, list, False, eListboxPythonMultiContent)
                self.l.setFont(0, gFont("Regular", 14))
                self.l.setFont(1, gFont("Regular", 16))
                self.l.setFont(2, gFont("Regular", 18))
                self.l.setFont(3, gFont("Regular", 17))
                self.l.setFont(4, gFont("Regular", 22))
                self.l.setFont(5, gFont("Regular", 24))
                self.l.setFont(6, gFont("Regular", 26))
                self.l.setFont(7, gFont("Regular", 20))

class MusicChartPlayer(InfoBarBase, InfoBarShowHide, InfoBarSeek, InfoBarAudioSelection, InfoBarNotifications, InfoBarServiceNotifications, InfoBarPVRState, InfoBarMoviePlayerSummarySupport, Screen):
	ENABLE_RESUME_SUPPORT = True
	ALLOW_SUSPEND = True
		
	def __init__(self, session, chartlist, index):
		Screen.__init__(self, session)
		self.session = session
		self["actions"] = HelpableActionMap(self, "MoviePlayerActions",
			{
				"leavePlayer": (self.leavePlayer, _("leave movie player..."))
			})

		self.allowPiP = False
		for x in InfoBarShowHide, InfoBarBase, InfoBarSeek, \
				InfoBarAudioSelection, InfoBarNotifications, \
				InfoBarServiceNotifications, InfoBarPVRState,  \
				InfoBarMoviePlayerSummarySupport:
			x.__init__(self)

		self.returning = False
		self.skinName = "MoviePlayer"
		self.lastservice = session.nav.getCurrentlyPlayingServiceReference()

		self.chartList = chartlist

		self.index = index
		#print self.index
		url = self.chartList[self.index][2]
		self.load_file(url)

	def load_file(self, url):
		if url:
			print "URL:", url
			client.getPage(url, method='GET', headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.grab_file).addErrback(self.errorload)		
		self.onClose.append(self.__onClose)

	def grab_file(self, data):
		xml = re.findall('stream_h264_url":"(.*?)"', data, re.S)
		if xml:
			file = xml[0].replace('\\','')
			filename = re.findall('title":"(.*?)"', data, re.S)
		if re.match('.*?Inhalt abgelehnt',data, re.S):
			message = self.session.open(MessageBox, ("Video got removed.."), MessageBox.TYPE_INFO,timeout=5)
		print "TEST:", file, filename[0]
		ref = eServiceReference(0x1001, 0, file)
		ref.setName(filename[0])
		self.session.nav.playService(ref)

	def errorload(self, error):
		print "[ChartsPlayer]:", error
		
	def leavePlayer(self):
		self.close()

	def doEofInternal(self, playing):
		
		print "current song:", self.index
		self.index += 1
		print "next song:", self.index
		if len(self.chartList) < self.index -1:
			self.close()
		else:
			#ref = self.chartList[self.index]
			url = self.chartList[self.index][2]
			self.load_file(url)

	def __onClose(self):
		self.session.nav.playService(self.lastservice)

class chartsplayer(Screen):
	if getDesktop(0).size().width() == 1280:
		skin = """
			<screen position="center,center" size="650,450" title="Music Charts Streamer v0.1">
				<ePixmap position="550,2" size="48,48" pixmap="~/images/music.png" alphatest="blend" />
				<widget name="myTVmenulist" position="0,50" size="650,360" transparent="1" scrollbarMode="showOnDemand" />
				<widget name="info" position="50,13" size="550,24" zPosition="0" font="Regular;22" transparent="1" foregroundColor="yellow" />
				<ePixmap position="15,17" size="16,16" pixmap="~/images/play.png" alphatest="blend" />
				<widget name="ButtonRed" position="500,420" pixmap="skin_default/buttons/key_red.png" zPosition="10" size="30,24" transparent="1" alphatest="on" />
				<widget name="TextRed" position="545,420" size="120,24" valign="center" halign="left" zPosition="10" font="Regular;18" transparent="1" />
			</screen>"""

	def __init__(self, session, plugin_path):
		Screen.__init__(self, session)
		self.session = session
		self.plugin_path = plugin_path
		self.skin_path = plugin_path

		self["info"] = Label("")
		self["info"].setText("Grab Top 100 Charts..")

		self["ButtonRed"] = Pixmap()
		self["ButtonRed"].hide()
		self["TextRed"] = Label("Download")
		self["TextRed"].hide()

		self.myTVmenulist = []
		self["myTVmenulist"] = charts([])
		self["myTVmenulist"].selectionEnabled(1)
		
		self.filename = ""

		self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
		{
			"cancel":	self.exit,
			"ok":		self.ok,
		}, -1)

		self["InfobarActions"] = HelpableActionMap(self, "InfobarActions",
		{
			"showMovies":	self.jobix,
		}, -1)

		self["ColorActions"] = HelpableActionMap(self, "ColorActions",
		{
			"red":	self.downi,
		}, -1)

		self.ok_info = "login"
		self.onLayoutFinish.append(self.list)

	def list(self):
		url = "http://www.music-clips.net/charts/"
		client.getPage(url, method='GET', headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.charts_list).addErrback(self.errorload)

	def charts_list(self, data):
		box = re.findall('<div class="video-portal"><h1>Dailymotion</h1></div>(.*?)<div id="sky"></div>', data, re.S)
		songs = re.findall('<div class="name">(.*?)</div>.*?<div class="value"><a href="(.*?)".*?title="(.*?)"', box[0], re.S)

		own_playlist = []
		for each in songs:
			(place, link, title) = each
			#print place, title, link
			#name = "%s %s" % (place, title)
			if not re.match('#',title):
				daily_id = link.strip('http://www.dailymotion.com/video/')
				youtube_link = "http://www.dailymotion.com/embed/video/%s" % daily_id
			else:
				youtube_link = link

			own_playlist.append([place, title, youtube_link])
			self.myTVmenulist.append(self.list_charts(place, title, youtube_link))
			self["myTVmenulist"].l.setList(self.myTVmenulist)
			self["myTVmenulist"].l.setItemHeight(24)

		self["info"].setText("Top 100 Charts listed.")
		self["ButtonRed"].show()
		self["TextRed"].show()
		self.own_playlist = own_playlist

	def jobix(self):
		self.session.open(JobManagerFrontEnd)
		
	def downi(self):
		item = self["myTVmenulist"].getCurrent()
		link = item[0][2]
		self.get_file(link, "down")
		
	def play(self, link, filename):
		cur_index = self["myTVmenulist"].getSelectedIndex()
		print "cur_index:", cur_index
		self.session.open(MusicChartPlayer, self.own_playlist, int(cur_index))

	def ok(self):
		item = self["myTVmenulist"].getCurrent()
		link = item[0][2]

		self.get_file(link, "play")
		
	def get_file(self, url, cat):
		print url
		client.getPage(url, method='GET', headers={'Content-Type':'application/x-www-form-urlencoded'}).addCallback(self.grab_file, cat).addErrback(self.errorload)

	def grab_file(self, data, cat):
		xml = re.findall('stream_h264_url":"(.*?)"', data, re.S)
		if xml:
			file = xml[0].replace('\\','')
			filename = re.findall('title":"(.*?)"', data, re.S)
		if re.match('.*?Inhalt abgelehnt',data, re.S):
			message = self.session.open(MessageBox, ("Video got removed.."), MessageBox.TYPE_INFO,timeout=5)
		else:
			if file and filename:
				if cat == "play":
					self.play(file, filename[0])
				elif cat == "down":
					self.currentDownload = ""
					self.currentDownload = jm.addJobToList(file, filename[0])
					jm.startDownload(self.currentDownload)
					message = self.session.open(MessageBox, ("Download started, Please check progress in JobManager with PVR Button."), MessageBox.TYPE_INFO,timeout=5)
			else:
				"[ChartsPlayer]: No Video Link found."

	def list_charts(self, place, title, link):
        	res = [ (place, title, link) ]
        	png = "/usr/lib/enigma2/python/Plugins/Extensions/chartsplayer/images/tv.png"
        	if fileExists(png):
                	res.append(MultiContentEntryPixmapAlphaTest(pos=(15, 0), size=(30, 30), png=loadPNG(png)))
        	res.append(MultiContentEntryText(pos=(50, 0), size=(35, 20), font=4, text=place, flags=RT_HALIGN_RIGHT))
		res.append(MultiContentEntryText(pos=(110, 0), size=(500, 20), font=4, text=title, flags=RT_HALIGN_LEFT))
		return res

	def exit(self):
		self.close()

        def errorload(self, error):
                print "[ChartsPlayer]:", error

def main(session, **kwargs):
	session.open(chartsplayer, plugin_path)

def Plugins(path, **kwargs):
	global plugin_path
	plugin_path = path
	return [
		PluginDescriptor(name="Music Charts Streamer", description="Stream Top Music Charts direct to your TV", where = [PluginDescriptor.WHERE_EXTENSIONSMENU], fnc=main),
		PluginDescriptor(name="Music Charts Streamer", description="Stream Top Music Charts direct to your TV", where = [PluginDescriptor.WHERE_PLUGINMENU], fnc=main)
		]
