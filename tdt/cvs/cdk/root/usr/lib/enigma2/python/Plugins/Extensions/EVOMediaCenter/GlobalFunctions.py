from enigma import eTimer, eWidget, eRect, eServiceReference, iServiceInformation, iPlayableService
from Screens.Screen import Screen
from Screens.ServiceInfo import ServiceInfoList, ServiceInfoListEntry
from Components.ActionMap import ActionMap, NumberActionMap
from Components.Pixmap import Pixmap, MovingPixmap
from Components.Label import Label
from Components.Button import Button
from Components.Sources.List import List
from Screens.MessageBox import MessageBox
from Components.ServicePosition import ServicePositionGauge
from Components.ServiceEventTracker import ServiceEventTracker
from Components.Sources.StaticText import StaticText
from Components.ConfigList import ConfigList, ConfigListScreen
from Components.config import *
from Tools.Directories import resolveFilename, fileExists, pathExists, createDir, SCOPE_MEDIA, SCOPE_SKIN_IMAGE
from Components.FileList import FileList
from Components.AVSwitch import AVSwitch
from Screens.InfoBar import MoviePlayer
from Plugins.Plugin import PluginDescriptor
#import os
from os import path as os_path
class EVOMC_AudioInfoView(Screen):
	skin = """
		<screen position="80,130" size="560,320" title="View Audio Info" >
			<widget name="infolist" position="5,5" size="550,310" selectionDisabled="1" />
		</screen>"""
	def __init__(self, session, fullname, name, ref):
		self.skin = EVOMC_AudioInfoView.skin
		Screen.__init__(self, session)
		self["actions"] = ActionMap(["OkCancelActions"],
		{
			"cancel": self.close,
			"ok": self.close
		}, -1)
		tlist = [ ]
		self["infolist"] = ServiceInfoList(tlist)
		currPlay = self.session.nav.getCurrentService()
		if currPlay is not None:
			stitle = currPlay.info().getInfoString(iServiceInformation.sTagTitle)
			if stitle == "":
				stitle = currPlay.info().getName().split('/')[-1]
			tlist.append(ServiceInfoListEntry("Title: ", stitle))
			tlist.append(ServiceInfoListEntry("Artist: ", currPlay.info().getInfoString(iServiceInformation.sTagArtist)))
			tlist.append(ServiceInfoListEntry("Album: ", currPlay.info().getInfoString(iServiceInformation.sTagAlbum)))
			tlist.append(ServiceInfoListEntry("Genre: ", currPlay.info().getInfoString(iServiceInformation.sTagGenre)))
			tlist.append(ServiceInfoListEntry("Year: ", currPlay.info().getInfoString(iServiceInformation.sTimeCreate)))
			tlist.append(ServiceInfoListEntry("Comment: ", currPlay.info().getInfoString(iServiceInformation.sTagComment)))
class EVOMC_VideoInfoView(Screen):
	skin = """
		<screen position="80,130" size="560,320" title="View Video Info" >
			<widget name="infolist" position="5,5" size="550,310" selectionDisabled="1" />
		</screen>"""
	def __init__(self, session, fullname, name, ref):
		self.skin = EVOMC_VideoInfoView.skin
		Screen.__init__(self, session)
		self["actions"] = ActionMap(["OkCancelActions"],
		{
			"cancel": self.close,
			"ok": self.close
		}, -1)
		tlist = [ ]
		self["infolist"] = ServiceInfoList(tlist)
		currPlay = self.session.nav.getCurrentService()
		if currPlay is not None:
			stitle = currPlay.info().getInfoString(iServiceInformation.sTitle)
			if stitle == "":
				stitle = currPlay.info().getName().split('/')[-1]
			tlist.append(ServiceInfoListEntry("Title: ", stitle))
			tlist.append(ServiceInfoListEntry("sNamespace: ", currPlay.info().getInfoString(iServiceInformation.sNamespace)))
			tlist.append(ServiceInfoListEntry("sProvider: ", currPlay.info().getInfoString(iServiceInformation.sProvider)))
			tlist.append(ServiceInfoListEntry("sTimeCreate: ", currPlay.info().getInfoString(iServiceInformation.sTimeCreate)))
			tlist.append(ServiceInfoListEntry("sVideoWidth: ", currPlay.info().getInfoString(iServiceInformation.sVideoWidth)))
			tlist.append(ServiceInfoListEntry("sVideoHeight: ", currPlay.info().getInfoString(iServiceInformation.sVideoHeight)))
			tlist.append(ServiceInfoListEntry("sDescription: ", currPlay.info().getInfoString(iServiceInformation.sDescription)))
class EVOMC_FolderOptions(Screen):
	skin = """
		<screen position="160,200" size="400,200" title="Media Center - Folder Options" >
			<widget source="pathlabel" transparent="1" render="Label" zPosition="2" position="0,180" size="380,20" font="Regular;16" />
			<widget source="menu" render="Listbox" zPosition="5" transparent="1" position="10,10" size="380,160" scrollbarMode="showOnDemand" >
				<convert type="StringList" />
			</widget>
		</screen>"""
	def __init__(self, session, directory, dirname):
		self.skin = EVOMC_FolderOptions.skin
		Screen.__init__(self, session)
		self.dirname = dirname
		self.directory = directory
		list = []
		list.append(("Add Folder to Favorites", "addtofav", "menu_addtofav", "50"))
		self["menu"] = List(list)
		self["pathlabel"] = StaticText("Folder: " + self.directory)
		self["actions"] = ActionMap(["OkCancelActions"],
		{
			"cancel": self.Exit,
			"ok": self.okbuttonClick
		}, -1)
	def okbuttonClick(self):
		print "okbuttonClick"
		selection = self["menu"].getCurrent()
		if selection is not None:
			if selection[1] == "addtofav":
				self.session.openWithCallback(self.FolderAdded, EVOMC_FavoriteFolderAdd, self.directory, self.dirname)
			else:
				self.close()
		else:
			self.close()
	def FolderAdded(self, added):
		self.close()
	def Exit(self):
		self.close()
class EVOMC_FavoriteFolders(Screen):
	def __init__(self, session):
		Screen.__init__(self, session)
		self.list = []
		for i in range(0, config.plugins.EVOMC_favorites.foldercount.value):
			cfg = config.plugins.EVOMC_favorites.folders[i]
			self.list.append((cfg.name.value, int(i), cfg.basedir.value, "50"))
		self["favoritelist"] = List(self.list)
		self["title"] = StaticText("")
		self["key_red"] = Button(_("Delete"))
		self["key_green"] = Button(_("Add Folder"))
		self["key_yellow"] = Button(_("Edit Folder"))
		self["key_blue"] = Button(_("Reset All"))
		self["actions"] = ActionMap(["OkCancelActions", "ColorActions"],
		{
			"ok": self.JumpToFolder,
			"cancel": self.Exit,
			"red": self.keyDelete,
			"green": self.FavoriteFolderAdd,
			"yellow": self.FavoriteFolderEdit,
			"blue": self.ResetAll
		}, -1)
	def JumpToFolder(self):
		configfile.save()
		selection = self["favoritelist"].getCurrent()
		if selection is not None:
			self.close(selection[2])
	def FavoriteFolderEdit(self):
		selection = self["favoritelist"].getCurrent()
		if selection is None:
			return
		else:
			self.session.openWithCallback(self.conditionalEdit, EVOMC_FavoriteFolderEdit, selection[1])
	def FavoriteFolderAdd(self):
		self.session.openWithCallback(self.conditionalNew, EVOMC_FavoriteFolderAdd)
	def UpdateFavoriteFolders(self):
		for i in range(0, len(config.plugins.EVOMC_favorites.folders)):
			config.plugins.EVOMC_favorites.folders.append(ConfigSubsection())
			config.plugins.EVOMC_favorites.folders[i].name = ConfigText("", False)
			config.plugins.EVOMC_favorites.folders[i].basedir = ConfigText("/", False)
		config.plugins.EVOMC_favorites.foldercount.value = len(config.plugins.EVOMC_favorites.folders)
	def keyDelete(self):
		self.session.openWithCallback(self.deleteConfirm, MessageBox, _("Really delete this favorite?"))
	def deleteConfirm(self, result):
		if result:
			selection = self["favoritelist"].getCurrent()
			id = int(selection[1])
			del config.plugins.EVOMC_favorites.folders[id]
			config.plugins.EVOMC_favorites.foldercount.value -= 1
			configfile.save()
			self.list.pop(id)
			self["favoritelist"].setList(self.list)
	def ResetAll(self):
		config.plugins.EVOMC_favorites.foldercount.value = 0
		config.plugins.EVOMC_favorites.foldercount.save()
		for i in range(0, 100):
			try:
				del(config.plugins.EVOMC_favorites.folders[i])
			except Exception, e:
				print "EVOMC_ResetAll-DelaFavFailed"
		config.plugins.EVOMC_favorites.folders.save()
		try:
			del(config.plugins.EVOMC_favorites.folders)
		except Exception, e:
			print "EVOMC_DELFAVFOLDERS-FAILED"
		try:
			del(config.plugins.EVOMC_favorites.folders[0])
		except Exception, e:
			print "EVOMC_DELFAV0-FAILED"
		config.plugins.EVOMC_favorites.folders.save()
		configfile.save()
		self.close()
	def conditionalNew(self,added):
		if added == 0:
			return
		id = len(config.plugins.EVOMC_favorites.folders)-1
		self.list.insert(id, getConfigListEntry(str(config.plugins.EVOMC_favorites.folders[id].name.value), id))
	def conditionalEdit(self, id):
		self.list.pop(id)
		self.list.insert(id, getConfigListEntry(str(config.plugins.EVOMC_favorites.folders[id].name.value), id))
	def refresh(self):
		pass
	def Exit(self):
		configfile.save()
		self.close()
class EVOMC_FavoriteFolderAdd(Screen, ConfigListScreen):
	skin = """
		<screen position="160,220" size="400,120" title="Media Center - Add Favorite" >
			<widget name="config" position="10,10" size="380,100" />
		</screen>"""
	def __init__(self, session, directory = "/", name = ""):
		Screen.__init__(self, session)
		self["actions"] = NumberActionMap(["SetupActions","OkCancelActions"],
		{
			"ok": self.keyOK,
			"cancel": self.keyCancel
		}, -1)
		self.id = config.plugins.EVOMC_favorites.foldercount.value
		config.plugins.EVOMC_favorites.folders.append(ConfigSubsection())
		config.plugins.EVOMC_favorites.folders[self.id].name = ConfigText("", False)
		config.plugins.EVOMC_favorites.folders[self.id].basedir = ConfigText("", False)
		config.plugins.EVOMC_favorites.folders[self.id].name.value = name
		config.plugins.EVOMC_favorites.folders[self.id].basedir.value = directory
		self.list = []
		self.list.append(getConfigListEntry(_("Name:"), config.plugins.EVOMC_favorites.folders[self.id].name))
		self.list.append(getConfigListEntry(_("Directory:"), config.plugins.EVOMC_favorites.folders[self.id].basedir))
		ConfigListScreen.__init__(self, self.list, session)
	def keyOK(self):
		config.plugins.EVOMC_favorites.foldercount.value += 1
		config.plugins.EVOMC_favorites.foldercount.save()
		config.plugins.EVOMC_favorites.folders.save()
		self.close(1)
	def keyCancel(self):
		try:
			del(config.plugins.EVOMC_favorites.folders[self.id])
		except Exception, e:
			print "EVOMC_Settings_DelaFavFailed"
		self.close(0)
class EVOMC_FavoriteFolderEdit(Screen, ConfigListScreen):
	skin = """
		<screen position="160,220" size="400,120" title="Media Center - Edit Favorite" >
			<widget name="config" position="10,10" size="380,100" />
		</screen>"""
	def __init__(self, session, foldernum):
		Screen.__init__(self, session)
		self["actions"] = NumberActionMap(["SetupActions","OkCancelActions"],
		{
			"ok": self.keyOK,
			"cancel": self.keyOK
		}, -1)
		self.list = []
		i = foldernum
		self.fn = foldernum
		self.list.append(getConfigListEntry(_("Name"), config.plugins.EVOMC_favorites.folders[i].name))
		self.list.append(getConfigListEntry(_("Directory"), config.plugins.EVOMC_favorites.folders[i].basedir))
		ConfigListScreen.__init__(self, self.list, session)
	def keyOK(self):
		config.plugins.EVOMC_favorites.folders.save()
		self.close(self.fn)
