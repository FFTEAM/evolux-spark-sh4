from os import path, remove, listdir as os_listdir, system, walk
from Screens.Screen import Screen
from Screens.MessageBox import MessageBox
from Screens.ChoiceBox import ChoiceBox
from Screens.HelpMenu import HelpableScreen
from Screens.InfoBarGenerics import InfoBarSeek, InfoBarPVRState, InfoBarCueSheetSupport, InfoBarShowHide, InfoBarNotifications
from Components.ActionMap import ActionMap, NumberActionMap, HelpableActionMap
from Components.Label import Label
from Components.Pixmap import Pixmap, MovingPixmap
from Components.FileList import FileList
from Components.MenuList import MenuList
from Components.ServiceEventTracker import ServiceEventTracker, InfoBarBase
from Components.config import config
from Components.Harddisk import harddiskmanager
from enigma import eTimer, eWidget, eRect, eServiceReference, iServiceInformation, iPlayableService, ePicLoad, evfd, iServiceKeys, getDesktop, eServiceCenter, ePoint
from Screens.ServiceInfo import ServiceInfoList, ServiceInfoListEntry
from Components.Button import Button
from Components.Sources.List import List
from Components.ServicePosition import ServicePositionGauge
from Components.ServiceEventTracker import ServiceEventTracker
from Components.ConfigList import ConfigList, ConfigListScreen
from Components.config import *
from Tools.Directories import resolveFilename, fileExists, pathExists, createDir, SCOPE_MEDIA, SCOPE_SKIN_IMAGE
from Components.AVSwitch import AVSwitch
from Screens.InfoBar import MoviePlayer
from Plugins.Plugin import PluginDescriptor
from GlobalFunctions import EVOMC_FolderOptions, EVOMC_FavoriteFolders, EVOMC_FavoriteFolderAdd, EVOMC_FavoriteFolderEdit, EVOMC_AudioInfoView
from Components.MediaPlayer import PlayList
import gettext
import time
import random
config.plugins.EVOMC_ap = ConfigSubsection()
config.plugins.EVOMC_ap.showPreview = ConfigYesNo(default=False)
config.plugins.EVOMC_ap.preview_delay = ConfigInteger(default=5, limits=(1, 30))
config.plugins.EVOMC_ap.lastDir = ConfigText(default=resolveFilename(SCOPE_MEDIA))
config.plugins.EVOMC_all = ConfigSubsection()
config.plugins.EVOMC_all.showDimmer = ConfigYesNo(default=False)
config.plugins.EVOMC_all.dimmer_delay = ConfigInteger(default=20, limits=(10, 600))
config.plugins.EVOMC_saver = ConfigSubsection()
config.plugins.EVOMC_saver.myaudioScreenSaver = ConfigSelection(default="30", choices = [
    ("0","kein"),
    ("60","nach 1 Minute"),
    ("300","nach 5 Minuten"),
    ("600","nach 10 Minuten"),
    ("1800","nach 30 Minuten"),
    ("3600","nach 1 Stunde"),
    ("10","nach 10 Sek"),
    ("30","nach 30 Sek")])
onmyaudioScreenSaver = None
plugin_path = ""
def getAspect():
	val = AVSwitch().getAspectRatioSetting()
	return val/2
class EVOMC_AudioPlayer(Screen, HelpableScreen):
	def __init__(self, session):
		Screen.__init__(self, session)
		HelpableScreen.__init__(self)
		self.bScreen = self.session.instantiateDialog(Blackscreen)
		self.bScreen.show()
		self.myaudioScreenSaverconfig = int(config.plugins.EVOMC_saver.myaudioScreenSaver.value)
		self.myaudioScreenSaverTimer = eTimer()
		self.myaudioScreenSaverTimer.timeout.get().append(self.myaudioScreenSaverTimer_Timeout)
		self.ResetmyaudioScreenSaverTimer()
		self.start_timer = eTimer()
		self.moveTimer = eTimer()
		self.start_timer.timeout.get().append(self.onLayoutFinished) 
		self.isVisible = True
		self.oldService = self.session.nav.getCurrentlyPlayingServiceReference()
		self.skindir = "/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic"
		self.coverArtFileName = ""
		self["PositionGauge"] = ServicePositionGauge(self.session.nav)
		self["key_red"] = Button(_("Delete"))
		self["key_green"] = Button(_("Play All"))
		self["key_yellow"] = Button(_("Favorites"))
		self["key_blue"] = Button(_("Settings"))
		self["fileinfo"] = Label()
		self["coverArt"] = MediaPixmap()
		self["currentfolder"] = Label()
		self["currentfavname"] = Label()
		self.playlist = MyPlayList()
		self.currList = "filelist"
		self.curfavfolder = -1
		self["play"] = Pixmap()
		self["stop"] = Pixmap()
		self["curplayingtitle"] = Label()
		self.currPlaying = 0
		self.PlaySingle = 0
		self.PlaySingleRUN = 0
		self.PlayAll = 0
		self.__event_tracker = ServiceEventTracker(screen=self, eventmap=
			{
				iPlayableService.evEOF: self.doEOF,
				iPlayableService.evStopped: self.StopPlayback,
				iPlayableService.evUser+11: self.__evDecodeError,
				iPlayableService.evUser+12: self.__evPluginError,
				iPlayableService.evUser+13: self["coverArt"].embeddedCoverArt
			})
		self["actions"] = HelpableActionMap(self, "EVOMC_AudioPlayerActions",
			{
				"ok": (self.KeyPlaySingle, "Play selected file"),
				"left": (self.leftUp, "List Top"),
				"right": (self.rightDown, "List Bottom"),
				"up": (self.up, "List up"),
				"down": (self.down, "List down"),
				"menu": (self.KeyMenu, "File / Folder Options"),
				"video": (self.visibility, "Show / Hide Player"),
				"nextBouquet": (self.NextFavFolder, "Next Favorite Folder"),
				"prevBouquet": (self.PrevFavFolder, "Previous Favorite Folder"),
				"red": (self.deleteFile, "Delete File"),
				"green": (self.KeyPlayAll, "Play All"),
				"yellow": (self.FavoriteFolders, "Favorite Folders"),
				"blue": (self.Settings, "Settings"),
			}, -2)
		self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
			{
				"cancel": (self.Exit, "Exit Audio Player"),
			}, -2)
		self["InfobarShowHideActions"] = HelpableActionMap(self, "InfobarShowHideActions",
			{
				"toggleShow": (self.showFileInfo, "Show File Info"),
			}, -2)
		self["MediaPlayerActions"] = HelpableActionMap(self, "MediaPlayerActions",
			{
				"stop": (self.StopPlayback, "Stop Playback"),
			}, -2)
		self.aspect = getAspect()
		currDir = config.plugins.EVOMC_ap.lastDir.value
		if not pathExists(currDir):
			currDir = "/"
		self["currentfolder"].setText(str(currDir))
		self.filelist = FileList(currDir, useServiceRef = True, showDirectories = True, showFiles = True, matchingPattern = "(?i)^.*\.(mp3|ogg|wma|wav|wave|flac|m4a)")
		self["filelist"] = self.filelist
		self["thumbnail"] = Pixmap()
		evfd.getInstance().vfd_write_string("EVO-AUDIOPLAYER")
		self.ThumbTimer = eTimer()
		self.ThumbTimer.callback.append(self.showThumb)
		self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
		self.DimmerTimer = eTimer()
		self.DimmerTimer.callback.append(self.showDimmer)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
		self.BlinkingPlayIconTimer = eTimer()
		self.BlinkingPlayIconTimer.callback.append(self.BlinkingPlayIcon)
		self.blinking=False
		self.FileInfoTimer = eTimer()
		self.FileInfoTimer.callback.append(self.updateFileInfo)
		self.myaudioScreenSaverconfig = int(config.plugins.EVOMC_saver.myaudioScreenSaver.value)
        	self.ResetmyaudioScreenSaverTimer()
	def onLayoutFinished(self):
        	global startindex
        	self.startindex = startindex
        	self.num += 1
        	if startindex is not None:
            		self.start_timer.stop()
        	else:
            		if self.num < 30:
               			self.start_timer.start(2000)
            		else:
                		self.start_timer.stop()
                		fehler = "Fehler beim automatischen Start"
                		self.session.open(MessageBox,_("%s" %fehler), MessageBox.TYPE_INFO)
	def myaudioScreenSaverTimer_Timeout(self):
        	if self.myaudioScreenSaverTimer.isActive():
        		self.myaudioScreenSaverTimer.stop()
        	self.session.openWithCallback(self.ResetmyaudioScreenSaverTimer,myaudioScreenSaver)
	def ResetmyaudioScreenSaverTimer(self):
        	if onmyaudioScreenSaver:
            		pass
        	if self.myaudioScreenSaverconfig != 0:
            		if self.myaudioScreenSaverTimer.isActive():
                		self.myaudioScreenSaverTimer.stop()
            		self.myaudioScreenSaverTimer.start(self.myaudioScreenSaverconfig * 1000)
	def deleteFile(self):
		if self.currList == "filelist":
			self.service = self.filelist.getServiceRef()
		else:
			self.service = self.playlist.getSelection()
		if self.service is None:
			return
		if self.service.type != 4098 and self.session.nav.getCurrentlyPlayingServiceReference() is not None:
			if self.service == self.session.nav.getCurrentlyPlayingServiceReference():
				self.stopEntry()

		serviceHandler = eServiceCenter.getInstance()
		offline = serviceHandler.offlineOperations(self.service)
		info = serviceHandler.info(self.service)
		name = info and info.getName(self.service)
		result = False
		if offline is not None:
			# simulate first
			if not offline.deleteFromDisk(1):
				result = True
		if result == True:
			self.session.openWithCallback(self.deleteConfirmed_offline, MessageBox, _("Do you really want to delete %s?") % (name))
		else:
			self.session.openWithCallback(self.close, MessageBox, _("You cannot delete this!"), MessageBox.TYPE_ERROR)      

	def deleteConfirmed_offline(self, confirmed):
		if confirmed:
			serviceHandler = eServiceCenter.getInstance()
			offline = serviceHandler.offlineOperations(self.service)
			result = False
			if offline is not None:
				# really delete!
				if not offline.deleteFromDisk(0):
					result = True
			if result == False:
				self.session.open(MessageBox, _("Delete failed!"), MessageBox.TYPE_ERROR)
			else:
				self.removeListEntry()

	def removeListEntry(self):
		self.savePlaylistOnExit = True
		currdir = self.filelist.getCurrentDirectory()
		self.filelist.changeDir(currdir)
		deleteend = False
		while not deleteend:
			index = 0
			deleteend = True
			if len(self.playlist) > 0:
				for x in self.playlist.list:
					if self.service == x[0]:
						self.playlist.deleteFile(index)
						deleteend = False
						break
					index += 1
		self.playlist.updateList()
		if self.currList == "playlist":
			if len(self.playlist) == 0:
				self.switchToFileList()
	def up(self):
		self["filelist"].up()
		system("stfbcontrol a 255")
		self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def down(self):
		self["filelist"].down()
		system("stfbcontrol a 255")
		self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def leftUp(self):
		self["filelist"].pageUp()
		system("stfbcontrol a 255")
		self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def rightDown(self):
		self["filelist"].pageDown()
		system("stfbcontrol a 255")
		self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def showDimmer(self):
		if config.plugins.EVOMC_all.showDimmer.value:
			system("stfbcontrol a 80")
#		else:
#			pass
	def NextFavFolder(self):
		system("stfbcontrol a 255")
		if self.curfavfolder + 1 < config.plugins.EVOMC_favorites.foldercount.value:
			self.curfavfolder += 1
			self.favname = config.plugins.EVOMC_favorites.folders[self.curfavfolder].name.value
			self.folder = config.plugins.EVOMC_favorites.folders[self.curfavfolder].basedir.value
			self["currentfolder"].setText(("%s") % (self.folder))
			self["currentfavname"].setText(("%s") % (self.favname))
			if pathExists(self.folder) == True:
				self["filelist"].changeDir(self.folder)
		else:
			return
	def PrevFavFolder(self):
		system("stfbcontrol a 255")
		if self.curfavfolder <= 0:
			return
		else:
			self.curfavfolder -= 1
			self.favname = config.plugins.EVOMC_favorites.folders[self.curfavfolder].name.value
			self.folder = config.plugins.EVOMC_favorites.folders[self.curfavfolder].basedir.value
			self["currentfolder"].setText(("%s") % (self.folder))
			self["currentfavname"].setText(("%s") % (self.favname))
			if pathExists(self.folder) == True:
				self["filelist"].changeDir(self.folder)
	def KeyPlaySingle(self):
		filename = self["filelist"].getFilename()
		print "filename", filename
		print "self.PlaySingle", self.PlaySingle
		if self.PlaySingleRUN == 0:
			if self.PlaySingle == 1 or config.plugins.EVOMC_ap.showPreview.getValue():
				if filename.upper().endswith(".MP3") or filename.upper().endswith(".OGG") or filename.upper().endswith(".WAV") or filename.upper().endswith(".WAVE") or filename.upper().endswith(".FLAC") or filename.upper().endswith(".M4A"):
					return
		else:
			if config.plugins.EVOMC_ap.showPreview.getValue():
				if filename.upper().endswith(".MP3") or filename.upper().endswith(".OGG") or filename.upper().endswith(".WAV") or filename.upper().endswith(".WAVE") or filename.upper().endswith(".FLAC") or filename.upper().endswith(".M4A"):
					return
		system("stfbcontrol a 255")
		self.ThumbTimer.stop()
		if self["filelist"].canDescent():
			self["currentfavname"].setText("")
			self.curfavfolder = -1
			self.filelist.descent()
			self["currentfolder"].setText(str(self.filelist.getCurrentDirectory()))
		else:
			self.PlaySingle = 1
			self.PlaySingleRUN = 1
			self.PlayService()
			self.BlinkingPlayIconTimer.stop()
	def KeyPlayAll(self):
		self.ThumbTimer.stop()
		if not self["filelist"].canDescent():
			self.PlaySingle = 0
			self.PlayAll = 1
			self.PlayService()
			self.BlinkingPlayIconTimer.start(1000, True)
	def PlayService(self):
		system("stfbcontrol a 255")
		self.StopPlayback()
		self.filelist.refresh()
		self.currPlaying = 1
		if self.PlayAll == 1:
			self.BlinkingPlayIconTimer.start(1000, True)
		evfd.getInstance().vfd_write_string("PLAY")
		self.session.nav.playService(self["filelist"].getServiceRef())
		self.FileInfoTimer.start(2000, True)
		self["play"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/play_enabled.png")
		self["stop"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/stop_disabled.png")
		system("vfdctl +play")
		system("vfdctl +mp3")
		system("vfdctl +music")
		path = self["filelist"].getFilename()
		self["coverArt"].updateCoverArt(path)
	def JumpToFolder(self, jumpto = None):
		if jumpto is None:
			return
		else:
			self["filelist"].changeDir(jumpto)
			self["currentfolder"].setText(("%s") % (jumpto))
	def FavoriteFolders(self):
		self.session.openWithCallback(self.JumpToFolder, EVOMC_FavoriteFolders)
	def StartThumb(self):
		self.session.openWithCallback(self.returnVal, ThumbView, self.filelist.getFileList(), self.filelist.getFilename(), self.filelist.getCurrentDirectory())
	def showThumb(self):
		if config.plugins.EVOMC_ap.showPreview.getValue() == False:
			return
		if self["filelist"].canDescent():
			return
		else:
			if self["filelist"].getServiceRef() is not None:
				system("stfbcontrol a 255")
				self.ThumbTimer.stop()
				system("killall -9 showiframe")
				system("stfbcontrol a 255")
				self.filelist.refresh()
				self.session.nav.stopService()
				self.session.nav.playService(self["filelist"].getServiceRef())
				self.currPlaying = 1
				self["play"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/play_enabled.png")
				self["stop"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/stop_disabled.png")
				self.FileInfoTimer.start(2000, True)
				self.BlinkingPlayIconTimer.start(1000, True)
	def returnVal(self, val=0):
		if val > 0:
			for x in self.filelist.getFileList():
				if x[0][1] == True:
					val += 1
			self.filelist.moveToIndex(val)
	def StartExif(self):
		if not self.filelist.canDescent():
			self.session.open(ExifView, self.filelist.getCurrentDirectory() + self.filelist.getFilename(), self.filelist.getFilename())
	def visibility(self, force=1):
		if self.isVisible == True:
			self.isVisible = False
			self.hide()
		else:
			self.isVisible = True
			self.show()
	def BlinkingPlayIcon(self):
		if self.blinking:
			self.blinking=False
			self["play"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/play_disabled.png")
			self.BlinkingPlayIconTimer.start(1000, True)
		else:
			self.blinking=True
			self["play"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/play_enabled.png")
			self.BlinkingPlayIconTimer.start(1000, True)
	def StopPlayback(self):
		evfd.getInstance().vfd_write_string("STOP")
		if self.isVisible == False:
			self.show()
			self.isVisible = True
		if self.session.nav.getCurrentService() is None:
			return
		else:
			self.session.nav.stopService()
			self["play"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/play_disabled.png")
			self["stop"].instance.setPixmapFromFile(str(self.skindir) + "/images/icons/stop_enabled.png")
			self.currPlaying = 0
			self.BlinkingPlayIconTimer.stop()
			self.ThumbTimer.stop()
			system("killall -9 showiframe")
			system("stfbcontrol a 255")
			self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
			self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
			system("vfdctl -play")
			system("vfdctl -mp3")
			system("vfdctl -music")
			self.StopPlayback()
	def showFileInfo(self):
		if self["filelist"].canDescent():
			return
		else:
			system("stfbcontrol a 255")
			self.ThumbTimer.start(config.plugins.EVOMC_ap.preview_delay.getValue() * 1000, True)
			self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
			self.session.open(EVOMC_AudioInfoView, self["filelist"].getCurrentDirectory() + self["filelist"].getFilename() , self["filelist"].getFilename(), self["filelist"].getServiceRef())
	def updateFileInfo(self):
		if self["filelist"].canDescent():
			return
		currPlay = self.session.nav.getCurrentService()
		if currPlay is not None:
			stitle = currPlay.info().getInfoString(iServiceInformation.sTagTitle)
			sArtist = currPlay.info().getInfoString(iServiceInformation.sTagArtist)
			sAlbum = currPlay.info().getInfoString(iServiceInformation.sTagAlbum)
			sGenre = currPlay.info().getInfoString(iServiceInformation.sTagGenre)
			sYear = currPlay.info().getInfoString(iServiceInformation.sTimeCreate)
#			sComment = currPlay.info().getInfoString(iServiceInformation.sTagComment)
			if stitle == "":
				stitle = currPlay.info().getName().split('/')[-1]
			self["fileinfo"].setText("Title: " + stitle + "\nArtist: " +  sArtist + "\nAlbum: " + sAlbum + "\nGenre: " + sGenre)
			self["curplayingtitle"].setText(stitle)
		else:
			pass
	def doEOF(self):
		try:
			print "EVO-MediaCenter: EOF Event ..."
			if self.PlaySingle == 0:
				print "Play Next File ..."
				self.ThumbTimer.stop()
				self.down()
				try:
					self.PlayService()
				except:
					pass
			else:
				print "Stop Playback ..."
				self.StopPlayback()
				system("vfdctl -play")
				system("vfdctl -mp3")
				system("vfdctl -music")
		except:
			pass
	def __evDecodeError(self):
		currPlay = self.session.nav.getCurrentService()
		sAudioType = currPlay.info().getInfoString(iServiceInformation.sUser+10)
		print "[__evAudioDecodeError] audio-codec %s can't be decoded by hardware" % (sAudioType)
		self.session.open(MessageBox, _("This UFS can't decode %s streams!") % sAudioType, type = MessageBox.TYPE_INFO,timeout = 5 )
	def __evPluginError(self):
		currPlay = self.session.nav.getCurrentService()
		message = currPlay.info().getInfoString(iServiceInformation.sUser+12)
		print "[__evPluginError]" , message
		self.session.open(MessageBox, message, type = MessageBox.TYPE_INFO,timeout = 20 )
	def Playlists(self):
		evfd.getInstance().vfd_write_string("PLAYLIST")
		self.ThumbTimer.stop()
		self.session.open(MessageBox,"Coming soon ... :)",  MessageBox.TYPE_INFO)
	def KeyMenu(self):
		self.ThumbTimer.stop()
		if self["filelist"].canDescent():
			if self.filelist.getCurrent()[0][1]:
				self.currentDirectory = self.filelist.getCurrent()[0][0]
				self.foldername = self.currentDirectory.split('/')
				self.foldername = self.foldername[-2]
				self.session.open(EVOMC_FolderOptions, self.currentDirectory, self.foldername)
	def Settings(self):
		evfd.getInstance().vfd_write_string("SETTINGS")
		system("stfbcontrol a 255")
		self.ThumbTimer.stop()
		self.session.open(AudioPlayerSettings)
		config.plugins.EVOMC_ap.save()
		config.plugins.EVOMC_all.save()
	def Exit(self):
		if self.isVisible == False:
			self.visibility()
			return
		if self.filelist.getCurrentDirectory() is None:
			config.plugins.EVOMC_ap.lastDir.value = "/"
		else:
			config.plugins.EVOMC_ap.lastDir.value = self.filelist.getCurrentDirectory()
		system("killall -9 showiframe")
		system("stfbcontrol a 255")
		system("vfdctl -play")
		system("vfdctl -mp3")
		system("vfdctl -music")
		self.ThumbTimer.stop()
		self.DimmerTimer.stop()
		self.FileInfoTimer.stop()
		del self["coverArt"].picload
		config.plugins.EVOMC_ap.save()
		config.plugins.EVOMC_all.save()
		self.session.nav.stopService()
		evfd.getInstance().vfd_write_string("EVO-MediaCenter")
		self.close()
class MyPlayList(PlayList):
	def __init__(self):
		PlayList.__init__(self)
class MediaPixmap(Pixmap):
	def __init__(self):
		print "start MediaPixmap"
		Pixmap.__init__(self)
		self.coverArtFileName = ""
		self.picload = ePicLoad()
		self.picload.PictureData.get().append(self.paintCoverArtPixmapCB)
		self.coverFileNames = ["folder.png", "folder.jpg"]
	def applySkin(self, desktop, screen):
		print "start applySkin"
		from Tools.LoadPixmap import LoadPixmap
		noCoverFile = None
		if self.skinAttributes is not None:
			for (attrib, value) in self.skinAttributes:
				if attrib == "pixmap":
					noCoverFile = value
					break
		if noCoverFile is None:
			noCoverFile = resolveFilename(SCOPE_SKIN_IMAGE, "skin_default/no_coverArt.png")
		self.noCoverPixmap = LoadPixmap(noCoverFile)
		return Pixmap.applySkin(self, desktop, screen)
	def onShow(self):
		print "start onShow"
		Pixmap.onShow(self)
		sc = AVSwitch().getFramebufferScale()
		self.picload.setPara((self.instance.size().width(), self.instance.size().height(), sc[0], sc[1], False, 1, "#00000000"))
	def paintCoverArtPixmapCB(self, picInfo=None):
		ptr = self.picload.getData()
		if ptr != None:
			self.instance.setPixmap(ptr.__deref__())
	def updateCoverArt(self, path):
		while not path.endswith("/"):
			path = path[:-1]
		new_coverArtFileName = None
		for filename in self.coverFileNames:
			if pathExists(path + filename) is True:
				new_coverArtFileName = path + filename
		if self.coverArtFileName != new_coverArtFileName:
			self.coverArtFileName = new_coverArtFileName
			if new_coverArtFileName:
				self.picload.startDecode(self.coverArtFileName)
			else:
				self.showDefaultCover()
	def showDefaultCover(self):
		self.instance.setPixmap(self.noCoverPixmap)
	def embeddedCoverArt(self):
		print "[embeddedCoverArt] found"
		self.coverArtFileName = "/tmp/.id3coverart"
		self.picload.startDecode(self.coverArtFileName)
class AudioPlayerSettings(Screen):
	if (getDesktop(0).size().width() == 1280):
		skin = """
		<screen position="430,220" size="400,200" title="Audioplayer Settings" >
			<widget name="configlist" position="10,10" size="380,180" />
		</screen>"""
	elif (getDesktop(0).size().width() == 1024):
		skin = """
		<screen position="320,150" size="400,200" title="Audioplayer Settings" >
			<widget name="configlist" position="10,10" size="380,180" />
		</screen>"""
	elif (getDesktop(0).size().width() == 720):
		skin = """
		<screen position="160,220" size="400,200" title="Audioplayer Settings" >
			<widget name="configlist" position="10,10" size="380,180" />
		</screen>"""

	def __init__(self, session):
		Screen.__init__(self, session)
		self["actions"] = NumberActionMap(["SetupActions"],
		{
			"ok": self.keyOK,
			"cancel": self.keyOK,
			"left": self.keyLeft,
			"right": self.keyRight,
			"0": self.keyNumber,
			"1": self.keyNumber,
			"2": self.keyNumber,
			"3": self.keyNumber,
			"4": self.keyNumber,
			"5": self.keyNumber,
			"6": self.keyNumber,
			"7": self.keyNumber,
			"8": self.keyNumber,
			"9": self.keyNumber
		}, -1)
		self.list = []
		self["configlist"] = ConfigList(self.list)
		self.list.append(getConfigListEntry(_("Screensaver Interval"), config.plugins.EVOMC_saver.myaudioScreenSaver))
		self.list.append(getConfigListEntry(_("Autoplay Enable"), config.plugins.EVOMC_ap.showPreview))
		self.list.append(getConfigListEntry(_("Autoplay Delay"), config.plugins.EVOMC_ap.preview_delay))
		self.list.append(getConfigListEntry(_("Dimmer Enable"), config.plugins.EVOMC_all.showDimmer))
		self.list.append(getConfigListEntry(_("Dimmer Delay"), config.plugins.EVOMC_all.dimmer_delay))
	def keyLeft(self):
		system("stfbcontrol a 255")
		self["configlist"].handleKey(KEY_LEFT)
	def keyRight(self):
		system("stfbcontrol a 255")
		self["configlist"].handleKey(KEY_RIGHT)
	def keyNumber(self, number):
		system("stfbcontrol a 255")
		self["configlist"].handleKey(KEY_0 + number)
	def keyOK(self):
		system("stfbcontrol a 255")
		config.plugins.EVOMC_ap.save()
		config.plugins.EVOMC_all.save()
		config.plugins.EVOMC_saver.save()
		evfd.getInstance().vfd_write_string("EVO-AUDIOPLAYER")
		self.close()
class myaudioScreenSaver(Screen):
	global plugin_path
	if (getDesktop(0).size().width() == 1280):
		skin = """
		<screen name="myaudioScreenSaver" position="0,0" size="1280,720" flags="wfNoBorder" backgroundColor="#00000000" title="" >
		<widget name="display_titel" position="100,312" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#005B00" />
		<widget name="display_time" position="100,282" zPosition="1" size="1128,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#007300" />
		<widget name="curplayingtitle" position="100,372" zPosition="1" size="1128,100" font="Regular;26" valign="left" transparent="1" foregroundColor="#007300" />
		</screen>"""
	elif (getDesktop(0).size().width() == 1024):
		skin = """
		<screen name="myaudioScreenSaver" position="0,0" size="1024,576" flags="wfNoBorder" backgroundColor="#00000000" title="" >
		<widget name="display_titel" position="100,312" zPosition="1" size="920,30" font="Regular;22" valign="left" transparent="1" foregroundColor="#005B00" />
		<widget name="display_time" position="100,282" zPosition="1" size="920,30" font="Regular;22" valign="left" transparent="1" foregroundColor="#007300" />
		<widget name="curplayingtitle" position="95,372" zPosition="1" size="920,100" font="Regular;22" valign="left" transparent="1" foregroundColor="#007300" />
		</screen>"""
    	else:
        	skin = """
		<screen name="myaudioScreenSaver" position="0,0" size="720,576" flags="wfNoBorder" backgroundColor="#00000000" title="" >
		<widget name="display_titel" position="100,290" zPosition="1" size="720,28" font="Regular;20" valign="left" transparent="1" foregroundColor="#005B00" />
		<widget name="display_time" position="100,260" zPosition="1" size="720,28" font="Regular;20" valign="left" transparent="1" foregroundColor="#005B00" />
		<widget name="curplayingtitle" position="95,362" zPosition="1" size="720,100" font="Regular;20" valign="left" transparent="1" foregroundColor="#007300" />
		</screen>"""
	def __init__(self, session):
		global onmyaudioScreenSaver
		onmyaudioScreenSaver = True
		self.session = session
		Screen.__init__(self, session)
		self["curplayingtitle"] = Label("Title:\nArtist:\nAlbum:")
		self["display_titel"] = Label("EVO - SCREENSAVER - myAudio")
		self["display_time"] = Label("")
		self["actions"] = ActionMap(["WizardActions", "DirectionActions", "ColorActions", "EventViewActions"],
		{
		"back": self.close,
		"right": self.close,
		"left": self.close,
		"up": self.close,
		"down": self.close,
		"ok": self.close,
		"pageUp": self.close,
		"pageDown": self.close,
		"yellow": self.close,
		"blue": self.close,
		"red": self.close,
		"green": self.close,
		"right": self.close,
		"left": self.close,
		"prevBouquet": self.close,
		"nextBouquet": self.close,
		"info": self.close,
		}, -1)
		self.onClose.append(self.__onClose)
		self.onLayoutFinish.append(self.startRun)
		self.moveTimer = eTimer()
		self.moveTimer.timeout.get().append(self.moveTimer_Timeout)
		self.moveTimer.stop()

	def startRun(self):
		self.moveTimer.start(10000)
	def moveTimer_Timeout(self):
		currPlay = self.session.nav.getCurrentService()
		if currPlay is not None:
			stitle = currPlay.info().getInfoString(iServiceInformation.sTagTitle)
			sArtist = currPlay.info().getInfoString(iServiceInformation.sTagArtist)
			sAlbum = currPlay.info().getInfoString(iServiceInformation.sTagAlbum)
			if stitle == "":
				stitle = currPlay.info().getName().split('/')[-1]
			self["curplayingtitle"].setText("Title: " + stitle + "\nArtist: " +  sArtist + "\nAlbum: " + sAlbum)
		else:
			pass
		self.moveTimer.stop()
		tage = ['Montag','Dienstag','Mittwoch','Donnerstag','Freitag', 'Samstag', 'Sonntag']
		diezeit = time.localtime(time.time())
		self.TagesZahl = tage[diezeit[6]]
		uhrzeit = strftime("%H:%M",localtime())
		timewidget = self.TagesZahl +", " + strftime("%d.%m.%Y",localtime()) + "   " + uhrzeit
		self["display_time"].setText(timewidget)
		titeltext = uhrzeit
		self.setTitle("%s"%(titeltext))
		if (getDesktop(0).size().width() == 1280):
            		jetzt = time.time()
            		x = random.randint(40, 680)
           		y = random.randint(20, 550)
            		self["curplayingtitle"].move(ePoint(x,y+95))
            		self["display_titel"].move(ePoint(x,y+45))
            		self["display_time"].move(ePoint(x,y+10))
		elif (getDesktop(0).size().width() == 1024):
            		jetzt = time.time()
            		x = random.randint(30, 580)
           		y = random.randint(10, 450)
            		self["curplayingtitle"].move(ePoint(x,y+70))
            		self["display_titel"].move(ePoint(x,y+35))
            		self["display_time"].move(ePoint(x,y+05))
       	 	else:
            		x = random.randint(0, 482)
            		y = random.randint(0, 338)
            		self["curplayingtitle"].move(ePoint(x+20,y+100))
            		self["display_titel"].move(ePoint(x,y+40))
            		self["display_time"].move(ePoint(x,y+10))
        	self.moveTimer.start(10000)
	def __onClose(self):
        	if self.moveTimer.isActive():
            		self.moveTimer.stop()
        	global onmyaudioScreenSaver
        	onmyaudioScreenSaver = None
        	self.close()
class Blackscreen(Screen):
	def __init__(self, session, args = None):
		desktop_size = getDesktop(0).size()
		Blackscreen.skin = """<screen name="EVO-Mediacenter" position="0,0" size="%d,%d" flags="wfNoBorder" zPosition="-1" backgroundColor="black" />""" %(desktop_size.width(), desktop_size.height())
		Screen.__init__(self, session)
