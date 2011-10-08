# test example by emanuel@ihad.tv & JackDaniel@ihad.tv 2010
###########################################################################

from Screens.Screen import Screen
from Screens.Console import Console
from Components.Label import Label
from Components.ActionMap import ActionMap
from Components.config import config, ConfigSubsection, ConfigSelection, getConfigListEntry
from Components.ConfigList import ConfigListScreen
from Components.Harddisk import harddiskmanager
from Components.Language import language
from Plugins.Plugin import PluginDescriptor
from Tools.Directories import resolveFilename, SCOPE_PLUGINS, SCOPE_HDD, SCOPE_MEDIA
from os import environ as os_environ
import gettext
import os

# language
###########################################################################

def localeInit():
    # getLanguage returns e.g. "fi_FI" for "language_country"
    lang = language.getLanguage()[:2]
    # Enigma doesn't set this (or LC_ALL, LC_MESSAGES, LANG). gettext needs it!
    os_environ["LANGUAGE"] = lang
    print "[FlashBackup] found LANGUAGE=", lang
    gettext.bindtextdomain("FlashBackup", resolveFilename(SCOPE_PLUGINS, "Extensions/FlashBackup/locale"))

def _(txt):
    t = gettext.dgettext("FlashBackup", txt)
    if t == txt:
        print "[FlashBackup] fallback to default translation for", txt
        t = gettext.gettext(txt)
    return t

localeInit()
language.addCallback(localeInit)

###########################################################################

config.plugins.FlashBackup = ConfigSubsection()
config.plugins.FlashBackup.swap = ConfigSelection([("0", "auto"),("128", "128 MB"),("256", "256 MB"),("512", "512 MB")], default = "0")

###########################################################################

def getMountedDevs():
    def handleMountpoint(loc):
        mp = loc[0]
        desc = loc[1]
        return (mp, desc + " (" + mp + ")")

    mountedDevs = [(resolveFilename(SCOPE_MEDIA, "hdd"), _("Harddisk")),
                   (resolveFilename(SCOPE_MEDIA, "cf"), _("Compact Flash")),
                   (resolveFilename(SCOPE_MEDIA, "CF"), _("Compact Flash")),
                   (resolveFilename(SCOPE_MEDIA, "CF-Karte"), _("Compact Flash")),
                   (resolveFilename(SCOPE_MEDIA, "usb"), _("USB Device")),
                   (resolveFilename(SCOPE_MEDIA, "USB"), _("USB Device")),
                   (resolveFilename(SCOPE_MEDIA, "USB-Stick"), _("USB Device")),
                   (resolveFilename(SCOPE_MEDIA, "ba"), _("Barry Allen Device")),
                   (resolveFilename(SCOPE_MEDIA, "sd"), _("SD Device")),
                   (resolveFilename(SCOPE_MEDIA, "SD"), _("SD Device")),
                   (resolveFilename(SCOPE_MEDIA, "SD-Karte"), _("SD Device"))]
    mountedDevs += map(lambda p: (p.mountpoint, (_(p.description) if p.description else "")), harddiskmanager.getMountedPartitions(True))
    mountedDevs = filter(lambda path: os.path.isdir(path[0]) and os.access(path[0], os.W_OK|os.X_OK), mountedDevs)
    netDir = resolveFilename(SCOPE_MEDIA, "net")
    if os.path.isdir(netDir):
        mountedDevs += map(lambda p: (os.path.join(netDir, p), _("Network mount")), os.listdir(netDir))
    mountedDevs = map(handleMountpoint, mountedDevs)
    return mountedDevs
config.plugins.FlashBackup.devs = ConfigSelection(choices=getMountedDevs())

###########################################################################



class FlashBackup(ConfigListScreen,Screen):
    skin = """
        <screen position="center,center" size="620,155" title="1:1 Flashimage-Backup" >
            <widget name="config" position="5,5" size="610,340" scrollbarMode="showOnDemand" />
            <widget name="btnRed" position="10,110" size="150,40" backgroundColor="red" valign="center" halign="center" zPosition="2"  foregroundColor="white" font="Regular;18"/>
            <widget name="btnGreen" position="460,110" size="150,40" backgroundColor="green" valign="center" halign="center" zPosition="2"  foregroundColor="white" font="Regular;18"/>
        </screen>"""

    def __init__(self, session, args = 0):
        self.session = session
        self.flaschCom = ""
        getMountedDevs()
        Screen.__init__(self, session)
        
        self.list = []
        self.list.append(getConfigListEntry(_("Backup to: "), config.plugins.FlashBackup.devs))
        self.list.append(getConfigListEntry(_("Swapsize: "), config.plugins.FlashBackup.swap))
        ConfigListScreen.__init__(self, self.list)
        self["btnRed"] = Label(_("Cancel"))
        self["btnGreen"] = Label(_("Start"))
        self["myActionMap"] = ActionMap(["SetupActions"],
        {
            "save": self.save,
            "ok": self.save,
            "cancel": self.cancel,
        }, -2)

    def save(self):
        self.makeBackup()
        for x in self["config"].list:
            x[1].save()
        
    def makeBackup(self):
        if os_environ["LANGUAGE"] == "de":
            self.flaschCom = "/usr/bin/backup_pingulux_image.sh"
        else:
            self.flaschCom = "/usr/bin/backup_pingulux_image.sh"
            
        self.flaschCom += ' ' + config.plugins.FlashBackup.devs.value
        
        if config.plugins.FlashBackup.swap.value != "0":
            self.flaschCom += ' ' + config.plugins.FlashBackup.swap.value
        
        self.prombt(self.flaschCom)
    
    def prombt(self, com):
        self.session.open(Console,"Backup...", ["%s" % com])
    
    def cancel(self):
        print "[FlashBackup] - end"
        self.close(None)

###########################################################################

def main(session, **kwargs):
    print "[FlashBackup] main - start"
    session.open(FlashBackup)

###########################################################################

def Plugins(**kwargs):
        return PluginDescriptor(
            name="1:1 FlashBackup",
            description=_("1:1 Flashimage-Backup on HDD, CF, SD, USB or Networkmount"),
            icon="flashbackup.png",
            where = PluginDescriptor.WHERE_PLUGINMENU,
            fnc = main)
