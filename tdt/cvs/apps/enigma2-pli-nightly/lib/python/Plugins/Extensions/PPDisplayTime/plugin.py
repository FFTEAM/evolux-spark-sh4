from Plugins.Plugin import PluginDescriptor
from Tools.Directories import fileExists
from enigma import *
from Components.ActionMap import ActionMap, NumberActionMap
from Screens.Screen import Screen
from Components.Label import Label
#from Components.ProgressBar import ProgressBar
from Components.Pixmap import Pixmap
from Components.config import config, ConfigSubsection, getConfigListEntry, ConfigInteger, ConfigYesNo, ConfigSelection, ConfigText
from Components.ConfigList import ConfigListScreen
import ServiceReference
import os
from time import *
config.VFD  = ConfigSubsection()
config.VFD.ShowTime = ConfigYesNo(default=True)
config.VFD.RedLedDeep = ConfigYesNo(default=False)

class PPDisplayTimeConfigScreen(ConfigListScreen, Screen):
	Skin = """
	<screen position="160,110" size="400,350" title="  PPDisplayTime" >
		<widget name="config" position="15,36" size="360,310" scrollbarMode="showOnDemand" />
		<eLabel text="PPDisplayTime" position="190,320" size="420,25" font="Regular;16" transparent="1" />
		<ePixmap position="10,250" size="350,50" pixmap="/usr/lib/enigma2/python/Plugins/Extensions/PPDisplayTime/dmm_logo.png" zPosition="1" alphatest="blend" />
	</screen>"""
	SkinHD = """
	<screen position="440,180" size="400,330" title="  PPDisplayTime" >
		<widget name="config" position="15,36" size="360,310" scrollbarMode="showOnDemand" />
		<ePixmap position="10,230" size="300,50" pixmap="/usr/lib/enigma2/python/Plugins/Extensions/PPDisplayTime/dmm_logo.png" zPosition="1" alphatest="blend" />
	</screen>"""
	SkinMD = """
	<screen position="300,150" size="400,330" title="  PPDisplayTime" >
		<widget name="config" position="15,36" size="360,310" scrollbarMode="showOnDemand" />
		<eLabel text="PPDisplayTime" position="155,295" size="420,25" font="Regular;20" transparent="1" />
		<ePixmap position="10,230" size="345,50" pixmap="/usr/lib/enigma2/python/Plugins/Extensions/PPDisplayTime/dmm_logo.png" zPosition="1" alphatest="blend" />
	</screen>"""
	def __init__(self, session):
		try:
			skin_w = getDesktop(0).size().width()
			if skin_w == 1280:
				HDskin = True
				MDskin = False
				Skin = False

			if skin_w == 1024:
				MDskin = True
				HDskin = False
				Skin = False

			if skin_w == 720:
				Skin = True
				MDskin = False
				HDskin = False
		except:
			HDskin = False
			MDskin = False
		self.service = None
		if HDskin:
			self.skin = PPDisplayTimeConfigScreen.SkinHD
		if Skin:
			self.skin = PPDisplayTimeConfigScreen.Skin
		if MDskin:
			self.skin = PPDisplayTimeConfigScreen.SkinMD
		Screen.__init__(self, session)
		global Stime
		Stime = ConfigYesNo(default=True)

		self.Clist = []
		self.Clist.append(getConfigListEntry(_("Show Time on Display"), Stime))
		self.Clist.append(getConfigListEntry(_("Turn red-LED on (Deepstandby)"), config.VFD.RedLedDeep))
		self.Load_settings()
		ConfigListScreen.__init__(self, self.Clist)
		self["actions"] = ActionMap(["OkCancelActions", "DirectionActions", "ColorActions"],
		{
			"cancel": self.Exit,
			"left": self.left,
			"right": self.right,
			"ok": self.ok,
		}, -2)
		self.onLayoutFinish.append(self.layoutFinished)
	def layoutFinished(self):
		self["config"].l.setList(self.Clist)
	def Red(self):
		self.Exit()
	def Green(self):
		self.Exit()
	def Blue(self):
		pass
	def left(self):
		ConfigListScreen.keyLeft(self)
	def right(self):
		ConfigListScreen.keyRight(self)
	def Exit(self):
		self.Show_current_channel()
		self.close()
	def ok(self):
		self.Show_current_channel()
		self.Save_settings()
		self.close()
	def Show_current_channel(self):
#		if config.VFD.ShowTime.value is not False:
		tm=localtime()
		servicename=strftime("%H%M",tm)
#		else:
#			self.service = self.session.nav.getCurrentlyPlayingServiceReference()
#			service = self.service.toCompareString()
#			servicename = ServiceReference.ServiceReference(service).getServiceName().replace('\xc2\x87', '').replace('\xc2\x86', '').ljust(16)
		evfd.getInstance().vfd_write_string( servicename )
	def Save_settings(self):
		global Stime
		config.VFD.ShowTime.value = Stime.value
		config.VFD.save()
	def Load_settings(self):
		global Stime
		try:
			Stime.value = config.VFD.ShowTime.value
		except:
			pass
def mainB(session, **kwargs):
		session.open(PPDisplayTimeConfigScreen)
		evfd.getInstance().vfd_write_string( "PPDisplayTime" )
def VFDdisplay(menuid, **kwargs):
	if menuid == "system":
		return [("PPDisplayTime...", mainB, "display_time", 44)]
	else:
		return []
class DisplayTime:
	def __init__(self, session):
		self.session = session
		self.timer = eTimer()
		self.timer.timeout.get().append(self.timerEvent)
		self.service = None
	def startTimer(self):
		self.timer.startLongTimer(30)
	def StopTimer(self, result):
		if result:
			self.timer.stop()
			self.service = None
	def timerEvent(self):
		tm=localtime()
		if config.VFD.ShowTime.value is True:
#			self.service = self.session.nav.getCurrentlyPlayingServiceReference()
#			if not self.service is None:
			servicename = strftime("%H%M",tm) 
			evfd.getInstance().vfd_write_string(servicename[0:17])
		self.startTimer()
timerInstance = None
def main(session, **kwargs):
	global timerInstance
	if timerInstance is None:
		timerInstance = DisplayTime(session)
	timerInstance.startTimer()
	timerInstance.timerEvent()
def autostart(reason, **kwargs):
	global timerInstance
	if reason == 0:
		if timerInstance is None:
			timerInstance = DisplayTime()
		timerInstance.startTimer()
		timerInstance.timerEvent()
	elif reason == 1:
		timerInstance.StopTimer(True)
def Plugins(**kwargs):
	return [
	PluginDescriptor(name="PPDisplayTime", description="PPDisplayTime 2011-10-06", where = PluginDescriptor.WHERE_MENU, fnc=VFDdisplay),
	PluginDescriptor(name="PPDisplayTime", description="PPDisplayTime 2011-10-06", where = PluginDescriptor.WHERE_SESSIONSTART, fnc=main) ]
