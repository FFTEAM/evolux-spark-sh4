import Plugins.Plugin
from Tools.Directories import fileExists
#if fileExists("/usr/lib/enigma2/python/Plugins/Extensions/EVOPanel/EVO_url.pyc"):
from Components.config import config
from Components.config import ConfigSubsection
from Components.config import ConfigSelection
from Components.config import ConfigInteger
from Components.config import ConfigSubList
from Components.config import ConfigSubDict
from Components.config import ConfigText
from Components.config import configfile
from Components.config import ConfigYesNo
from skin import loadSkin
from enigma import getDesktop
currentmcversion = "097beta8EVOmod"
currentmcplatform = "sh4"
config.plugins.EVOMC_favorites = ConfigSubsection()
config.plugins.EVOMC_favorites.foldercount = ConfigInteger(0)
config.plugins.EVOMC_favorites.folders = ConfigSubList()
config.plugins.EVOMC_globalsettings = ConfigSubsection()
config.plugins.EVOMC_globalsettings.language = ConfigSelection(default="EN", choices = [("EN", _("English"))])
config.plugins.EVOMC_globalsettings.showinmainmenu = ConfigYesNo(default=True)
config.plugins.EVOMC_globalsettings.showinextmenu = ConfigYesNo(default=True)
config.plugins.EVOMC_globalsettings.currentversion = ConfigInteger(0, (0, 999))
config.plugins.EVOMC_globalsettings.currentplatform = ConfigText(default = currentmcplatform)
config.plugins.EVOMC_globalsettings.dst_top = ConfigInteger(0, (0, 999))
config.plugins.EVOMC_globalsettings.dst_left = ConfigInteger(0, (0, 999))
config.plugins.EVOMC_globalsettings.dst_width = ConfigInteger(720, (1, 720))
config.plugins.EVOMC_globalsettings.dst_height = ConfigInteger(576, (1, 576))
config.plugins.EVOMC_globalsettings.currentversion.value = currentmcversion
config.plugins.EVOMC_globalsettings.currentplatform.value = currentmcplatform
sz_w = getDesktop(0).size().width()
if sz_w == 1280:
	loadSkin("/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/skin_hd.xml")
if sz_w == 720:
	loadSkin("/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/skin_sd.xml")
if sz_w == 1024:
	loadSkin("/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/skin_md.xml")
def addFavoriteFolders():
	i = len(config.plugins.EVOMC_favorites.folders)
	config.plugins.EVOMC_favorites.folders.append(ConfigSubsection())
	config.plugins.EVOMC_favorites.folders[i].name = ConfigText("", False)
	config.plugins.EVOMC_favorites.folders[i].basedir = ConfigText("/", False)
	config.plugins.EVOMC_favorites.foldercount.value = i+1
	return i
for i in range(0, config.plugins.EVOMC_favorites.foldercount.value):
	addFavoriteFolders()
