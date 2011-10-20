from enigma import iPlayableService, eTimer, eWidget, eRect, eServiceReference, iServiceInformation, eAVSwitch, evfd, eServiceCenter
from Screens.Screen import Screen
from Screens.ServiceInfo import ServiceInfoList, ServiceInfoListEntry
from Screens.HelpMenu import HelpableScreen
from Components.ActionMap import ActionMap, NumberActionMap, HelpableActionMap
from Components.Pixmap import Pixmap, MovingPixmap
from Components.Label import Label
from Components.Button import Button
from Components.ServiceEventTracker import ServiceEventTracker, InfoBarBase
from Components.ConfigList import ConfigList, ConfigListScreen
from Components.config import *
from Screens.MessageBox import MessageBox
from Screens.ChoiceBox import ChoiceBox
from Tools.Directories import resolveFilename, fileExists, pathExists, createDir, SCOPE_MEDIA
from Components.FileList import FileList
from Components.AVSwitch import AVSwitch
from Plugins.Plugin import PluginDescriptor
from Components.Playlist import PlaylistIOInternal, PlaylistIOM3U, PlaylistIOPLS
from Components.MediaPlayer import PlayList
from GlobalFunctions import EVOMC_FolderOptions, EVOMC_FavoriteFolders, EVOMC_FavoriteFolderAdd, EVOMC_FavoriteFolderEdit, EVOMC_VideoInfoView
from Screens.InfoBarGenerics import InfoBarSeek, InfoBarAudioSelection, InfoBarCueSheetSupport, InfoBarNotifications, \
	InfoBarShowHide, InfoBarServiceErrorPopupSupport, \
	InfoBarPVRState, InfoBarSimpleEventView, InfoBarServiceNotifications, \
	InfoBarMoviePlayerSummarySupport, InfoBarSubtitleSupport, InfoBarTeletextPlugin
#import os
from os import path, remove as os_remove, listdir, system
import time
tmppolicy = "None"
config.plugins.EVOMC_vp = ConfigSubsection()
#config.plugins.EVOMC_vp.showPreview = ConfigYesNo(default=False)
#config.plugins.EVOMC_vp.preview_delay = ConfigInteger(default=5, limits=(1, 30))
config.plugins.EVOMC_vp.lastDir = ConfigText(default=resolveFilename(SCOPE_MEDIA))
config.plugins.EVOMC_all = ConfigSubsection()
config.plugins.EVOMC_all.showDimmer = ConfigYesNo(default=True)
config.plugins.EVOMC_all.dimmer_delay = ConfigInteger(default=60, limits=(10, 90))
def getAspect():
	val = AVSwitch().getAspectRatioSetting()
	return val/2
class EVOMC_VideoPlayer(Screen, InfoBarBase, InfoBarSeek, InfoBarNotifications, HelpableScreen):
	def __init__(self, session):
		Screen.__init__(self, session)
		HelpableScreen.__init__(self)
		InfoBarNotifications.__init__(self)
		InfoBarBase.__init__(self)
		InfoBarSeek.__init__(self, actionmap = "MediaPlayerSeekActions")
		self.isVisible = True
		self.oldService = self.session.nav.getCurrentlyPlayingServiceReference()
		self.session.nav.stopService()
		self.playlistparsers = {}
		self.addPlaylistParser(PlaylistIOM3U, "m3u")
		self.playlist = MyPlayList()
		self["fileinfo"] = Label()
		self["key_red"] = Button(_("Delete"))
		self["key_green"] = Button(_(""))
		self["key_yellow"] = Button("Favorites")
		self["key_blue"] = Button(_("Settings"))
		self["currentfolder"] = Label("")
		self["currentfavname"] = Label("")
		self.currList = "filelist"
		self.curfavfolder = -1
		self["actions"] = HelpableActionMap(self, "EVOMC_VideoPlayerActions",
			{
				"ok": (self.KeyOk, "Play selected file"),
				"left": (self.leftUp, "List Top"),
				"right": (self.rightDown, "List Bottom"),
				"up": (self.up, "List up"),
				"down": (self.down, "List down"),
				"menu": (self.KeyMenu, "File / Folder Options"),
				"video": (self.visibility, "Show / Hide Player"),
				"nextBouquet": (self.NextFavFolder, "Next Favorite Folder"),
				"prevBouquet": (self.PrevFavFolder, "Previous Favorite Folder"),
				"stop": (self.StopPlayback, "Stop Playback"),
				"red": (self.deleteFile, "Delete File"),
				"yellow": (self.FavoriteFolders, "Favorite Folders"),
#				"green": (self.showPreview, "Preview"),
				"blue": (self.KeySettings, "Settings"),
			}, -2)
		self["InfobarShowHideActions"] = HelpableActionMap(self, "InfobarShowHideActions",
			{
				"toggleShow": (self.showFileInfo, "Show File Info"),
			}, -2)
		self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
			{
				"cancel": (self.Exit, "Exit Video Player"),
			}, -2)
		self.aspect = getAspect()
		currDir = config.plugins.EVOMC_vp.lastDir.value
		if not pathExists(currDir):
			currDir = "/"
		self["currentfolder"].setText(str(currDir))
		if pathExists("/usr/lib/enigma2/python/Plugins/Extensions/DVDPlayer") is True:
			self.filelist = FileList(currDir, useServiceRef = True, showDirectories = True, showFiles = True, matchingPattern = "(?i)^.*\.(mp4|ts|trp|wmv|mpg|vob|avi|mkv|dat|flac|m2ts|trp)")
		else:
			self.filelist = FileList(currDir, useServiceRef = True, showDirectories = True, showFiles = True, matchingPattern = "(?i)^.*\.(mp4|ts|trp|wmv|mpg|vob|avi|mkv|dat|flac|m2ts|trp)")
		self["filelist"] = self.filelist
		self["thumbnail"] = Pixmap()
		evfd.getInstance().vfd_write_string("EVO-VIDEOPLAYER")
#		self.filelist.refresh()
#		self.ThumbTimer = eTimer()
#		self.ThumbTimer.callback.append(self.showThumb)
#		self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
#		self.filelistTimer = eTimer()
#		self.filelistTimer.callback.append(self.filelist.refresh())
#		self.filelistTimer.start(60, True)
		self.DimmerTimer = eTimer()
		self.DimmerTimer.callback.append(self.showDimmer)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
		self.__event_tracker = ServiceEventTracker(screen=self, eventmap=
			{
				iPlayableService.evUser+11: self.__evDecodeError,
				iPlayableService.evUser+12: self.__evPluginError
			})
	def addPlaylistParser(self, parser, extension):
		self.playlistparsers[extension] = parser

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
		system("stfbcontrol a 255")
		self["filelist"].up()
#		self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def down(self):
		system("stfbcontrol a 255")
		self["filelist"].down()
#		self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def leftUp(self):
		system("stfbcontrol a 255")
		self["filelist"].pageUp()
#		self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def rightDown(self):
		system("stfbcontrol a 255")
		self["filelist"].pageDown()
#		self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
	def showDimmer(self):
		if config.plugins.EVOMC_all.showDimmer.getValue():
			system("stfbcontrol a 80")
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
#	def showPreview(self):
#		system("stfbcontrol a 255")
#		if self["filelist"].canDescent():
#			return
#		else:
#			if self["filelist"].getServiceRef() is not None:
#				self.DimmerTimer.stop()
#				self.session.nav.stopService()
#				self.session.nav.playService(self["filelist"].getServiceRef())
#	def showThumb(self):
#		if config.plugins.EVOMC_vp.showPreview.getValue() == False:
#			return
#		if self["filelist"].canDescent():
#			return
#		else:
#			if self["filelist"].getServiceRef() is not None:
#				self.DimmerTimer.stop()
#				self.session.nav.stopService()
#				self.session.nav.playService(self["filelist"].getServiceRef())
	def showFileInfo(self):
		if self["filelist"].canDescent():
			return
		else:
			system("stfbcontrol a 255")
#			self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
			self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
			self.session.open(EVOMC_VideoInfoView, self["filelist"].getCurrentDirectory() + self["filelist"].getFilename() , self["filelist"].getFilename(), self["filelist"].getServiceRef())
	def KeyOk(self):
		system("stfbcontrol a 255")
		system("echo 3 -> /dev/ttyAS1")
		self.filelist.refresh()
		if self.isVisible == False:
			self.visibility()
			return
#		self.ThumbTimer.stop()
		self.DimmerTimer.stop()
		if self.filelist.canDescent():
			self.filelist.descent()
		else:
			evfd.getInstance().vfd_write_string("PLAY")
			if self.filelist.getServiceRef().type == 4098:
				ServiceRef = self.filelist.getServiceRef()
				extension = ServiceRef.getPath()[ServiceRef.getPath().rfind('.') + 1:]
				if self.playlistparsers.has_key(extension):
					playlist = self.playlistparsers[extension]()
					list = playlist.open(ServiceRef.getPath())
					for x in list:
						self.playlist.addFile(x.ref)
				self.playlist.updateList()
			else:
				self.playlist.addFile(self.filelist.getServiceRef())
				self.playlist.updateList()
			currref = self.playlist.getServiceRefList()[self.playlist.getCurrentIndex()]
			print "currref !!!!!!!!!!!!!!!!!!!!!!!!!", currref
			self.session.open(MoviePlayer, currref)
#			system("echo " + hex(config.plugins.EVOMC_globalsettings.dst_top.value)[2:] + " > /proc/stb/vmpeg/0/dst_top")
#			system("echo " + hex(config.plugins.EVOMC_globalsettings.dst_left.value)[2:] + " > /proc/stb/vmpeg/0/dst_left")
#			system("echo " + hex(config.plugins.EVOMC_globalsettings.dst_width.value)[2:] + " > /proc/stb/vmpeg/0/dst_width")
#			system("echo " + hex(config.plugins.EVOMC_globalsettings.dst_height.value)[2:] + " > /proc/stb/vmpeg/0/dst_height")
	def KeyMenu(self):
#		self.ThumbTimer.stop()
		if self["filelist"].canDescent():
			if self.filelist.getCurrent()[0][1]:
				self.currentDirectory = self.filelist.getCurrent()[0][0]
				foldername = self.currentDirectory.split('/')
				foldername = foldername[-2]
				self.session.open(EVOMC_FolderOptions,self.currentDirectory, foldername)
#	def StartThumb(self):
#		self.session.openWithCallback(self.returnVal, ThumbView, self.filelist.getFileList(), self.filelist.getFilename(), self.filelist.getCurrentDirectory())
#	def returnVal(self, val=0):
#		if val > 0:
#			for x in self.filelist.getFileList():
#				if x[0][1] == True:
#					val += 1
#			self.filelist.moveToIndex(val)
#	def StartExif(self):
#		if not self.filelist.canDescent():
#			self.session.open(ExifView, self.filelist.getCurrentDirectory() + self.filelist.getFilename(), self.filelist.getFilename())
	def visibility(self, force=1):
		if self.isVisible == True:
			self.isVisible = False
			self.hide()
		else:
			self.isVisible = True
			self.show()
	def StopPlayback(self):
#		self.ThumbTimer.stop()
		system("echo C -> /dev/ttyAS1")
		self.session.nav.stopService()
		self.session.nav.playService(self.oldService)
#		self.ThumbTimer.start(config.plugins.EVOMC_vp.preview_delay.getValue() * 1000, True)
		self.DimmerTimer.start(config.plugins.EVOMC_all.dimmer_delay.getValue() * 1000, True)
		system("stfbcontrol a 255")
		self.show()
	def JumpToFolder(self, jumpto = None):
		if jumpto is None:
			return
		else:
			self["filelist"].changeDir(jumpto)
			self["currentfolder"].setText(("%s") % (jumpto))
	def FavoriteFolders(self):
		self.session.openWithCallback(self.JumpToFolder, EVOMC_FavoriteFolders)
	def KeySettings(self):
		self.session.open(VideoPlayerSettings)
		config.plugins.EVOMC_vp.save()
		config.plugins.EVOMC_all.save()
	def __evDecodeError(self):
		currPlay = self.session.nav.getCurrentService()
		sVideoType = currPlay.info().getInfoString(iServiceInformation.sVideoType)
		print "[__evDecodeError] video-codec %s can't be decoded by hardware" % (sVideoType)
		self.session.open(MessageBox, _("This Dreambox can't decode %s video streams!") % sVideoType, type = MessageBox.TYPE_INFO,timeout = 10 )
	def __evPluginError(self):
		currPlay = self.session.nav.getCurrentService()
		message = currPlay.info().getInfoString(iServiceInformation.sUser+12)
		print "[__evPluginError]" , message
		self.session.open(MessageBox, ("GStreamer Error: missing %s") % message, type = MessageBox.TYPE_INFO,timeout = 20 )
	def Exit(self):
		system("stfbcontrol a 255")
		if self.isVisible == False:
			self.visibility()
			return
		if self.filelist.getCurrentDirectory() is None:
			config.plugins.EVOMC_vp.lastDir.value = "/"
		else:
			config.plugins.EVOMC_vp.lastDir.value = self.filelist.getCurrentDirectory()
		self.session.nav.stopService()
		self.DimmerTimer.stop()
		config.plugins.EVOMC_vp.save()
		config.plugins.EVOMC_all.save()
		self.close()
	def showAfterSeek(self):
		self.show()
	def checkSkipShowHideLock(self):
		self.updatedSeekState()
	def unPauseService(self):
		self.setSeekState(self.SEEK_STATE_PLAY)
	def updatedSeekState(self):
		if self.seekstate == self.SEEK_STATE_PAUSE:
			self.playlist.pauseFile()
		elif self.seekstate == self.SEEK_STATE_PLAY:
			self.playlist.playFile()
		elif self.isStateForward(self.seekstate):
			self.playlist.forwardFile()
		elif self.isStateBackward(self.seekstate):
			self.playlist.rewindFile()
	def addPlaylistParser(self, parser, extension):
		self.playlistparsers[extension] = parser
class VideoPlayerSettings(Screen, ConfigListScreen):
	skin = """
		<screen position="160,220" size="420,120" title="Media Center - VideoPlayer Settings" >
			<widget name="config" position="10,10" size="390,100" />
		</screen>"""
	def __init__(self, session):
		Screen.__init__(self, session)
		self["actions"] = NumberActionMap(["SetupActions","OkCancelActions"],
		{
			"ok": self.keyOK,
			"cancel": self.keyOK
		}, -1)
		self.list = []
#		self.list.append(getConfigListEntry(_("Autoplay Enable"), config.plugins.EVOMC_vp.showPreview))
		self.list.append(getConfigListEntry(_("Autoplay Delay"), config.plugins.EVOMC_vp.preview_delay))
		self.list.append(getConfigListEntry(_("Dimmer Enable"), config.plugins.EVOMC_all.showDimmer))
		self.list.append(getConfigListEntry(_("Dimmer Delay"), config.plugins.EVOMC_all.dimmer_delay))
		ConfigListScreen.__init__(self, self.list, session)
	def keyOK(self):
		config.plugins.EVOMC_vp.save()
		config.plugins.EVOMC_all.save()
		evfd.getInstance().vfd_write_string("EVO-VIDEOPLAYER")
		self.close()
class MyPlayList(PlayList):
	def __init__(self):
		PlayList.__init__(self)
class MoviePlayer(InfoBarShowHide, \
		InfoBarSeek, InfoBarAudioSelection, HelpableScreen, InfoBarNotifications,
		InfoBarServiceNotifications, InfoBarPVRState, InfoBarCueSheetSupport, InfoBarSimpleEventView,
		InfoBarMoviePlayerSummarySupport, InfoBarSubtitleSupport, Screen, InfoBarTeletextPlugin,
		InfoBarServiceErrorPopupSupport):
	ENABLE_RESUME_SUPPORT = True
	ALLOW_SUSPEND = True
	def __init__(self, session, service):
		Screen.__init__(self, session)
		InfoBarSubtitleSupport.__init__(self)
		InfoBarAudioSelection.__init__(self)
		self["actions"] = HelpableActionMap(self, "MoviePlayerActions",
			{
				"leavePlayer": (self.leavePlayer, _("leave movie player..."))
			})
		self["MediaPlayerActions"] = HelpableActionMap(self, "MediaPlayerActions",
			{
				"previous": (self.previousMarkOrEntry, _("play from previous mark or playlist entry")),
				"next": (self.nextMarkOrEntry, _("play from next mark or playlist entry")),
				"subtitles": (self.subtitleSelection, _("Subtitle selection")),
			}, -2)
		self["ColorActions"] = HelpableActionMap(self, "ColorActions",
			{
				"blue": (self.sleepTimer, _("Sleep Timer")),
				"red": (self.change_ratio, _("Change Ratio old")),
			})
		for x in HelpableScreen, InfoBarShowHide, \
				InfoBarSeek, \
				InfoBarAudioSelection, InfoBarNotifications, InfoBarSimpleEventView, \
				InfoBarServiceNotifications, InfoBarPVRState, InfoBarCueSheetSupport,\
				InfoBarMoviePlayerSummarySupport, InfoBarSubtitleSupport, \
				InfoBarTeletextPlugin, InfoBarServiceErrorPopupSupport:
			x.__init__(self)
		self.session.nav.playService(service)
		self.returning = False
	def nextMarkOrEntry(self):
		if not self.jumpPreviousNextMark(lambda x: x):
			self.is_closing = True
			self.close(1)
	def previousMarkOrEntry(self):
		if not self.jumpPreviousNextMark(lambda x: -x-5*90000, start=True):
			self.is_closing = True
			self.close(-1)
	def leavePlayer(self):
		system("echo C -> /dev/ttyAS1")
		self.is_closing = True
		self.session.nav.stopService()
		self.close(0)
	def doEofInternal(self, playing):
		system("echo C -> /dev/ttyAS1")
		print "--- eofint movieplayer ---"
		self.is_closing = True
		self.close(1)
	def subtitleSelection(self):
		from Screens.Subtitles import Subtitles
		self.session.open(Subtitles)
	def sleepTimer(self):
		from Screens.SleepTimerEdit import SleepTimerEdit
		self.session.open(SleepTimerEdit)
	def change_ratio(self):
		print "change ratio by np"
		policy_input = open("/proc/stb/video/policy", "r")
		policy = policy_input.read()
	 	policy_input.close()
		print "VideoModeLight policy current", policy
		global tmppolicy
		print "VideoModeLight tmppolicy current", tmppolicy
		if tmppolicy == "None":
			tmppolicy = policy
		if tmppolicy.find("letterbox") != -1:
			print "VideoModeLight set policy letterbox"
			tmppolicy = "non"
			eAVSwitch.getInstance().setAspectRatio(0)
			self.session.open(MessageBox, 'LETTERBOX', MessageBox.TYPE_WARNING, timeout=2)
		elif tmppolicy.find("non") != -1:
			print "VideoModeLight set policy non"
			tmppolicy = "panscan"
			eAVSwitch.getInstance().setAspectRatio(3)
			self.session.open(MessageBox, 'NONLINEAR', MessageBox.TYPE_WARNING, timeout=2)
		elif tmppolicy.find("panscan") != -1:
			print "VideoModeLight set policy panscan"
			tmppolicy = "bestfit"
			eAVSwitch.getInstance().setAspectRatio(1)
			self.session.open(MessageBox, 'PANSCAN', MessageBox.TYPE_WARNING, timeout=2)
		elif tmppolicy.find("bestfit") != -1:
			print "VideoModeLight set policy bestfit"
			tmppolicy = "letterbox"
			eAVSwitch.getInstance().setAspectRatio(2)
			self.session.open(MessageBox, 'BESTFIT', MessageBox.TYPE_WARNING, timeout=2)
