from enigma import eTimer, eAVSwitch, eConsoleAppContainer, evfd
from Screens.Screen import Screen
from Screens.ServiceInfo import ServiceInfoList, ServiceInfoListEntry
from Components.ActionMap import ActionMap, NumberActionMap
from Components.Pixmap import Pixmap, MovingPixmap
from Components.Label import Label
from Screens.MessageBox import MessageBox
from Components.Sources.List import List
from Components.Sources.StaticText import StaticText
from Components.ConfigList import ConfigList
from Components.config import config
from Tools.Directories import resolveFilename, fileExists, pathExists, createDir, SCOPE_MEDIA
from Components.FileList import FileList
from Components.AVSwitch import AVSwitch
from Plugins.Plugin import PluginDescriptor
try:
	from twisted.web.client import getPage
except Exception, e:
	print "Media Center: Import twisted.web.client failed"
from os import system, path
from EVOMC_AudioPlayer import EVOMC_AudioPlayer
from EVOMC_VideoPlayer import EVOMC_VideoPlayer
from EVOMC_ShoutcastRadio import ShoutcastRadioScreenBrowser
from EVOMC_PictureViewer import EVOMC_PictureViewer
from EVOMC_WeatherInfo import EVOMC_WeatherInfo
emu = "none"
import time
from subprocess import Popen,PIPE
tmppolicy = "None"
lastpath = "/hdd"
def getAspect():
	val = AVSwitch().getAspectRatioSetting()
	return val/2
class EVOMC_MainMenu(Screen):
	def __init__(self, session):
		Screen.__init__(self, session)
		self.oldService = self.session.nav.getCurrentlyPlayingServiceReference()
		self.session.nav.stopService()
		try:
			self.can_osd_alpha = open("/proc/stb/video/alpha", "r") and True or False
		except:
			self.can_osd_alpha = False
		if self.can_osd_alpha:
			open("/proc/stb/video/alpha", "w").write(str("255"))
		evfd.getInstance().vfd_write_string("EVOMEDIACENTER")
		list = []
		list.append(("My Music", "EVOMC_AudioPlayer", "menu_music", "50"))
		list.append(("My Videos", "EVOMC_VideoPlayer", "menu_video", "50"))
		list.append(("My Pictures", "EVOMC_PictureViewer", "menu_pictures", "50"))
		list.append(("DVD Player", "EVOMC_DVDPlayer", "menu_dvd", "50"))
#		list.append(("Shoutcast Radio", "EVOMC_ShoutcastRadio", "menu_shoutcastradio", "50"))
		list.append(("Weather Info", "EVOMC_WeatherInfo", "menu_weather", "50"))
		self["menu"] = List(list)
		self["title"] = StaticText("")
		self["welcomemessage"] = StaticText("")
		self["actions"] = ActionMap(["OkCancelActions"],
		{
			"cancel": self.Exit,
			"ok": self.okbuttonClick
		}, -1)
	def okbuttonClick(self):
		print "okbuttonClick"
		selection = self["menu"].getCurrent()
		if selection is not None:
			if selection[1] == "EVOMC_VideoPlayer":
				self.session.open(EVOMC_VideoPlayer)
			elif selection[1] == "EVOMC_PictureViewer":
				self.session.open(EVOMC_PictureViewer)
			elif selection[1] == "EVOMC_AudioPlayer":
				self.session.open(EVOMC_AudioPlayer)
			elif selection[1] == "EVOMC_RadioPlayer":
				self.session.open(EVOMC_RadioPlayer)
			elif selection[1] == "EVOMC_DVDPlayer":
				if pathExists("/usr/lib/enigma2/python/Plugins/Extensions/DVDPlayer/") == True:
					from Plugins.Extensions.DVDPlayer.plugin import FileBrowser, DVDSummary, DVDOverlay, ChapterZap, DVDPlayer
					evfd.getInstance().vfd_write_string("EVO-DVDPLAYER")
					self.session.open(DVDPlayer)
				else:
					self.session.open(MessageBox,"Error: DVD-Player Plugin not installed ...",  MessageBox.TYPE_INFO)
			elif selection[1] == "EVOMC_ShoutcastRadio":
				self.session.open(ShoutcastRadioScreenBrowser)
			elif selection[1] == "EVOMC_WeatherInfo":
				self.session.open(EVOMC_WeatherInfo)
			elif selection[1] == "Exit":
				self.Exit()
			else:
				self.session.open(MessageBox,("Error: Could not find plugin %s\ncoming soon ... :)") % (selection[1]),  MessageBox.TYPE_INFO)
	def error(self, error):
		self.session.open(MessageBox,("UNEXPECTED ERROR:\n%s") % (error),  MessageBox.TYPE_INFO)
	def Ok(self):
		self.session.open(MPD_PictureViewer)
	def Exit(self):
		self.session.nav.stopService()
		self.session.nav.playService(self.oldService)
		system("echo " + hex(0)[2:] + " > /proc/stb/vmpeg/0/dst_top")
		system("echo " + hex(0)[2:] + " > /proc/stb/vmpeg/0/dst_left")
		system("echo " + hex(720)[2:] + " > /proc/stb/vmpeg/0/dst_width")
		system("echo " + hex(576)[2:] + " > /proc/stb/vmpeg/0/dst_height")
		if self.can_osd_alpha:
			try:
				open("/proc/stb/video/alpha", "w").write(str(config.av.osd_alpha.value))
			except:
				print "Set OSD Transparacy failed"
		self.close()
def main(session, **kwargs):
	session.open(EVOMC_MainMenu)
def menu(menuid, **kwargs):
	if menuid == "mainmenu":
		return [(_("EVO Media Center"), main, "EVO_MC", 12)]
	return []
def Plugins(**kwargs):
 	return [
			PluginDescriptor(name="EVO MediaCenter", description=("EVO Media Center"), where = PluginDescriptor.WHERE_MENU, fnc = menu)]
