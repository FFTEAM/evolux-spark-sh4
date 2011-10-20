# -*- coding: utf-8 -*-
###############################################################################
from Screens.Screen import Screen
from Screens.Standby import Standby
from Screens.InfoBarGenerics import InfoBarShowHide
from Components.config import *
from Screens.ChoiceBox import ChoiceBox
from Screens.InputBox import InputBox
from Screens.MessageBox import MessageBox
from Components.ActionMap import ActionMap
from Components.ScrollLabel import ScrollLabel
from Components.Label import Label
from Components.MenuList import MenuList
from Components.Sources.StaticText import StaticText
from Components.MultiContent import MultiContentEntryText
from Components.Pixmap import Pixmap
from Components.Input import Input
from Components.ConfigList import ConfigListScreen, ConfigList
from enigma import eListboxPythonMultiContent, eListbox, gFont
from enigma import getDesktop, eTimer
from Plugins.Plugin import PluginDescriptor
from os import path as os_path, listdir as os_listdir
from EVOMC_ShoutcastPlayer import ShoutcastPlayer
from Tools.Import import my_import
from enigma import ePoint
from enigma import eDVBVolumecontrol
from ConfigParser import ConfigParser
import os
import sys
import random
from time import *
import time
import datetime
import Screens.Standby
config.plugins.EVOMC_ShoutcastRadio = ConfigSubsection()
config.plugins.EVOMC_ShoutcastRadio.shoutcastScreenSaver = ConfigSelection(default="0", choices = [
    ("0","kein"),
    ("60","nach 1 Minute"),
    ("300","nach 5 Minuten"),
    ("600","nach 10 Minuten"),
    ("1800","nach 30 Minuten"),
    ("3600","nach 1 Stunde"),
    ("10","nach 10 Sek"),
    ("30","nach 30 Sek")])
config.plugins.EVOMC_ShoutcastRadio.startplay = ConfigYesNo(default = False)
config.plugins.EVOMC_ShoutcastRadio.startindex = ConfigInteger(0, limits = (0, 999))
config.plugins.EVOMC_ShoutcastRadio.offtimer_time = ConfigNumber(default = 10)
config.plugins.EVOMC_ShoutcastRadio.offtimer_art = ConfigSelection(default="0", choices = [("0","Standby"),("1","DeepStandby")])
myname = "EVOMediaCenter"
myversion = "1.5"
streamingtargetfile = "/tmp/streaming.mpg"
valid_types = ("MP3")
streamtitel = ""
streamplayer = None
onshoutcastScreenSaver = None
plugin_path = ""
wahl = 1
session = None
startindex = 0
offtimer = "starten"
class ShoutcastRadioScreenBrowser(Screen):
    global HDSkn
    global MDSkn
    sz_w = getDesktop(0).size().width()
    if sz_w == 1280:
        HDSkn = True
    else:
        HDSkn = False
    if sz_w == 1024:
        MDSkn = True
    else:
        MDSkn = False
    if HDSkn:
        skin = """
            <screen name="EVOShoutcastRadio" position="110,50" size="1060,620" title="ShoutcastRadio - Favoriten" flags="wfNoBorder" backgroundColor="#485052" >
                <widget name="titeltext" position="10,5" zPosition="1" size="1000,30" font="Regular;20" backgroundColor="#9f1313" transparent="1" />
                <ePixmap pixmap="skin_default/buttons/key_red.png" position="15,548" size="40,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_green.png" position="185,548" size="40,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_yellow.png" position="415,548" size="40,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_blue.png" position="585,548" size="40,30" alphatest="on" />
                <widget name="key_red" position="55,548" zPosition="1" size="120,30" font="Regular;20" backgroundColor="#9f1313" transparent="1" />
                <widget name="key_green" position="225,548" zPosition="1" size="200,30" font="Regular;20" backgroundColor="#1f771f" transparent="1" />
                <widget name="key_yellow" position="455,548" zPosition="1" size="120,30" font="Regular;20" backgroundColor="#18188b" transparent="1" />
                <widget name="key_blue" position="625,548" zPosition="1" size="120,30" font="Regular;20" backgroundColor="#18188b" transparent="1" />
                <ePixmap pixmap="/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/images/key_menu.png" position="870,546" size="51,30" alphatest="on" />
                <ePixmap pixmap="/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/images/key_info.png" position="935,546" size="51,30" alphatest="on" />
                <ePixmap pixmap="/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/images/key_help.png" position="1000,546" size="51,30" alphatest="on" />
                <widget name="streamlist" position="5,30" size="1050,510" backgroundColor="#446678" scrollbarMode="showOnDemand" /> 
                <widget name="playbalken" position="0,580" zPosition="1" size="1060,40" font="Regular;20" backgroundColor="#000000" transparent="0" />
                <widget name="playtext" position="0,580" zPosition="2" size="1060,40" font="Regular;20" valign="center" halign="center" backgroundColor="#9f1313" transparent="1" />           
            </screen>"""
    elif MDSkn:
        skin = """
            <screen name="EVOShoutcastRadio" position="20,30" size="990,566" title="ShoutcastRadio - Favoriten" flags="wfNoBorder" backgroundColor="#485052" >
                <widget name="titeltext" position="10,5" zPosition="1" size="950,30" font="Regular;20" backgroundColor="#9f1313" transparent="1" />
                <ePixmap pixmap="skin_default/buttons/key_red.png" position="25,500" size="40,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_green.png" position="195,500" size="40,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_yellow.png" position="425,500" size="40,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_blue.png" position="595,500" size="40,30" alphatest="on" />
                <widget name="key_red" position="65,500" zPosition="1" size="120,30" font="Regular;20" backgroundColor="#9f1313" transparent="1" />
                <widget name="key_green" position="235,500" zPosition="1" size="200,30" font="Regular;20" backgroundColor="#1f771f" transparent="1" />
                <widget name="key_yellow" position="465,500" zPosition="1" size="120,30" font="Regular;20" backgroundColor="#18188b" transparent="1" />
                <widget name="key_blue" position="635,500" zPosition="1" size="120,30" font="Regular;20" backgroundColor="#18188b" transparent="1" />
                <ePixmap pixmap="/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/images/key_menu.png" position="780,498" size="51,30" alphatest="on" />
                <ePixmap pixmap="/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/images/key_info.png" position="845,498" size="51,30" alphatest="on" />
                <ePixmap pixmap="/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/skins/basic/images/key_help.png" position="910,498" size="51,30" alphatest="on" />
                <widget name="streamlist" position="5,30" size="1000,460" backgroundColor="#446678" scrollbarMode="showOnDemand" /> 
                <widget name="playbalken" position="0,530" zPosition="1" size="1000,40" font="Regular;20" backgroundColor="#000000" transparent="0" />
                <widget name="playtext" position="0,530" zPosition="2" size="1000,40" font="Regular;20" valign="center" halign="center" backgroundColor="#9f1313" transparent="1" />           
            </screen>"""
    else:
        skin = """
            <screen name="EVOShoutcastRadio" position="15,15" size="690,530" title="ShoutcastRadio - Favoriten" flags="wfNoBorder" backgroundColor="#485052" >
                <widget name="titeltext" position="10,5" zPosition="1" size="720,30" font="Regular;20" backgroundColor="#9f1313" transparent="1" />
                <ePixmap pixmap="skin_default/buttons/red.png" position="10,460" size="140,40" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/green.png" position="153,460" size="140,40" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/yellow.png" position="400,460" size="140,40" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/blue.png" position="543,460" size="140,40" alphatest="on" />
                <widget name="key_red" position="10,460" zPosition="1" size="140,40" font="Regular;16" valign="center" halign="center" transparent="1" />
                <widget name="key_green" position="153,460" zPosition="1" size="140,40" font="Regular;16" valign="center" halign="center" transparent="1" />
                <widget name="key_yellow" position="400,460" zPosition="1" size="140,40" font="Regular;16" valign="center" halign="center" transparent="1" />
                <widget name="key_blue" position="543,460" zPosition="1" size="140,40" font="Regular;16" valign="center" halign="center" transparent="1" />
                <widget name="streamlist" position="10,30" size="670,430" backgroundColor="#446678" scrollbarMode="showOnDemand" />
                <widget name="playbalken" position="0,500" zPosition="2" size="690,40" font="Regular;16" backgroundColor="#000000" transparent="0" />
                <widget name="playtext" position="0,500" zPosition="3" size="670,40" font="Regular;16" valign="center" halign="center" backgroundColor="#9f1313" transparent="1" />           
            </screen>"""
    streamlist = []
    currentPlugin = None
    def __init__(self, session, args = 0):

        self.skin = ShoutcastRadioScreenBrowser.skin
        self.session = session
        Screen.__init__(self, session)
        global streamplayer
        streamplayer = ShoutcastPlayer(session)
        self["streamlist"] = StreamMenu([])
        self["titelbalken"] = Label("")
        self["playbalken"] = Label("")
        self["playtext"] = Label("")
        self["titeltext"] = Label("EVOShoutcastRadio - Favoriten")
        self["key_red"] = Label("Setup")
        self["key_green"] = Label("Start Sleeptimer")
        self["key_yellow"] = Label("Genre-Liste")
        self["key_blue"] = Label("Favoriten")
        self["actions"] = ActionMap(["WizardActions", "DirectionActions","MenuActions","ShortcutActions","GlobalActions","HelpActions","EPGSelectActions"], 
            {
             "ok": self.ok,
             "cancel": self.exit,
             "back": self.exit,
             "red": self.showConfig,
             "green": self.green,
             "blue": self.favoriten,
             "yellow": self.genre,
             "menu": self.showMainMenu,
             "displayHelp": self.showHelp,
             "info": self.showInfo,
             }, -1)

        self.onClose.append(self.exit)
        self.num = 0
        #self.CurrentService = self.session.nav.getCurrentlyPlayingServiceReference()
        #self.session.nav.stopService()
        self.bScreen = self.session.instantiateDialog(Blackscreen)
        self.bScreen.show()
        self.getInterfaceList()
        for plugin in self.pluginlist:
            if plugin.nameshort == "Favorites":
                wahl = 1
                self.setCurrentPlugin(plugin)
        #self.onShown.append(self.updateTitle)
        self.onClose.append(self.disconnectFromMetadataUpdates)
        self.shoutcastScreenSaverconfig = int(config.plugins.EVOMC_ShoutcastRadio.shoutcastScreenSaver.value)
        self.shoutcastScreenSaverTimer = eTimer()
        self.shoutcastScreenSaverTimer.timeout.get().append(self.shoutcastScreenSaverTimer_Timeout)
        self.ResetshoutcastScreenSaverTimer()
        self.start_timer = eTimer()
        self.start_timer.timeout.get().append(self.onLayoutFinished) 
        self.standby_timer = eTimer()
        self.standby_timer.timeout.get().append(self.standby_on)
        self.standby_timer.stop()
        if config.plugins.EVOMC_ShoutcastRadio.startplay.value:
            if int(config.plugins.EVOMC_ShoutcastRadio.startplay.value) >0:
                self.onLayoutFinish.append(self.onLayoutFinished)
    def connectToMetadataUpdates(self):
        global streamplayer
        if streamplayer is not None:
             streamplayer.metadatachangelisteners.append(self.onMetadataChanged)
    def disconnectFromMetadataUpdates(self):
        global streamplayer
        try:
             streamplayer.metadatachangelisteners.remove(self.onMetadataChanged)
        except Exception,e:
            pass
    def onMetadataChanged(self,title):
        try:
            self["playtext"].setText("%s"%title)
        except Exception,e:
            self.disconnectFromMetadataUpdates()
            self["titeltext"].setText("%s"%streamtitel)
    def onLayoutFinished(self):
        global startindex
        self.startindex = startindex
        self.num += 1
        if startindex is not None and streamplayer is not None:
            self.start_timer.stop()
            self.starters()
        else:
            if self.num < 30:
               self.start_timer.start(2000)
            else:
                self.start_timer.stop()
                fehler = "Fehler beim automatischen Start"
                self.session.open(MessageBox,_("%s" %fehler), MessageBox.TYPE_INFO)
    def starters(self):
        global streamplayer
        global startindex
        global streamtitel
        if self["streamlist"].l.getCurrentSelection() is not None:
            self["streamlist"].instance.moveSelectionTo(startindex)
            stream = self["streamlist"].l.getCurrentSelection()[0]
            self.connectToMetadataUpdates()
            streamtitel = stream.getName().split('-')[0]
            self.setTitle("%s"%streamtitel)
            s_titeltext="akt. Stream: " +streamtitel
            self["titeltext"].setText("%s"%s_titeltext)
            streamplayer.play(stream)
    def getInterfaceList(self):
        self.pluginlist = []
        global plugin_path,myname
        interfacepath = "/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter/ShoutcastRadioInterface"
        for iface in os_listdir(interfacepath):
            if iface.endswith(".pyc") and not iface.startswith("_"):
                pluginp = '.'.join(["Plugins", "Extensions", myname, "ShoutcastRadioInterface",iface.replace(".pyc","")])
                plugin = my_import(pluginp)
                self.pluginlist.append(plugin.Interface(self.session,cbListLoaded=self.onStreamlistLoaded))
    def setCurrentPlugin(self, plugin):
        self.currentPlugin = plugin
        plugin.getList()
    def genre(self):
        for plugin in self.pluginlist:
            if plugin.nameshort == "SHOUTcast":
                global wahl
                wahl = 2
                plugin.getList()
                self["titeltext"].setText("%s (%s)"%(myname,"SHOUTcast"))
    def suche(self):
        self.suchstart()
    def suchstart(self):
        for plugin in self.pluginlist:
            if plugin.nameshort == "SHOUTcastsearch":
                self["titeltext"].setText("%s (Suchergebnis)"%myname)
                self.suchwort="*schlager*".lower()
                if self.suchwort:
                    global wahl
                    wahl = 2
                    Suchstring = "*" + self.suchwort.replace(' ','*') + "*".lower()
                    plugin.searchList(Suchstring)
    def favoriten(self):
        for plugin in self.pluginlist:
            if plugin.nameshort == "Favorites":
                global wahl
                wahl = 1
                plugin.getList()
                self["titeltext"].setText("%s (%s)"%(myname,"Favoriten1"))
    def ok(self):
        if self["streamlist"].l.getCurrentSelection() is not None:
            global streamplayer
            if streamplayer.is_playing:
                print "[",myname,"] stream_startstop -> stop"
                streamplayer.stop()
                self.disconnectFromMetadataUpdates()
                self.setTitle("%s (%s)"%(myname,self.currentPlugin.nameshort))
                stream = self["streamlist"].l.getCurrentSelection()[0]
                streamtitel = stream.getName().split('-')[0]
                self.setTitle("%s"%streamtitel)
                s_titeltext="akt. Stream: " +streamtitel
                self["titeltext"].setText("%s"%s_titeltext)
                self.connectToMetadataUpdates()
                streamplayer.play(stream)
            else:
                if self["streamlist"].l.getCurrentSelection() is not None:
                    stream = self["streamlist"].l.getCurrentSelection()[0]
                    streamtitel = stream.getName().split('-')[0]
                    self.setTitle("%s"%streamtitel)
                    s_titeltext="akt. Stream: " +streamtitel
                    self["titeltext"].setText("%s"%s_titeltext)
                    self.connectToMetadataUpdates()
                    streamplayer.play(stream)
    def green(self):
        global offtimer
        if offtimer == "starten":
            self.get_offtimer()
            self["key_green"].setText("Stop Sleeptimer")
            offtimer = "stoppen"
        else:
            if self.vol_down_timer.isActive():
                self.vol_down_timer.stop()
            if self.standby_timer.isActive():
                self.standby_timer.stop()
            self["key_green"].setText("Start Sleeptimer")
            offtimer = "starten"
    def meldung(self):
        self.session.open(MessageBox,_("test"), MessageBox.TYPE_INFO,timeout=3)
    def exit(self):
        if streamplayer.is_playing:
            global streamplayer
            if self.shoutcastScreenSaverTimer.isActive():
                self.shoutcastScreenSaverTimer.stop()
            streamplayer.stop()
            self.disconnectFromMetadataUpdates()
            self.setTitle("%s (%s)"%(myname,self.currentPlugin.nameshort))
            self["titeltext"].setText("%s (%s)"%(myname,self.currentPlugin.nameshort))
        else:
            if self.standby_timer.isActive():
                self.standby_timer.stop()
            if self.shoutcastScreenSaverTimer.isActive():
                self.shoutcastScreenSaverTimer.stop()
            global streamplayer
            streamplayer.stop()
            streamplayer.exit()
            self.close()
    def onStreamlistLoaded(self,list):
        if wahl == 1:
            self["streamlist"].buildList_favoriten(list)
        else:
            self["streamlist"].buildList_shoutcast(list)
    def showMainMenu(self):
        menu = []
        if self["streamlist"].l.getCurrentSelection() is not None:
             selectedStream = self["streamlist"].l.getCurrentSelection()[0]
             self.stream_name = selectedStream.getName()
        else:
             selectedStream = None
        global offtimer
        for p in self.pluginlist:
            for i in p.getMenuItems(selectedStream,generic=True):
                menu.append((i[0],i[1]))
        if self.currentPlugin is not None:
            for i in self.currentPlugin.getMenuItems(selectedStream):
                menu.append((i[0],i[1]))
        menu.append((_("Shoutcast durchsuchen"), self.suche));        menu.append((_("About"), self.showAbout));
        menu.append((_("Hilfe"), self.showHelp));
        menu.append((_("Abschalt-Timer: %s"%offtimer), self.get_offtimer));
        menu.append((_("Einstellungen"), self.showConfig));
        if wahl == 1:
            menu.append((_("Stream als Startstream setzen"), self.set_startstream));
            menu.append((_("Stream aus Favoriten entfernen"), self.stream2deleteSelected));
        self.session.openWithCallback(self.menuCallback, ChoiceBox, title=_("Menu"), list=menu)
    def menuCallback(self,choice):
        if choice is not None:
            choice[1]()
    def get_offtimer(self):
        global offtimer
        offtimer_time = str(config.plugins.EVOMC_ShoutcastRadio.offtimer_time.value)
        if offtimer == "starten":
            self.session.openWithCallback(
                self.exit_timer,
                InputBox,
                title = "Minuten bis zum Abschalten eingeben:",
                text = offtimer_time,
                maxSize = False,
                type=Input.NUMBER
            )
            offtimer = "stoppen"
            self["key_green"].setText("Stop Sleeptimer")
        else:
            if self.standby_timer.isActive():
                self.standby_timer.stop()
            if self.vol_down_timer.isActive():
                self.vol_down_timer.stop()
            self["key_green"].setText("Start Sleeptimer")
            offtimer = "starten"
    def showAbout(self):
        text2 = '\n\n'+'Autor: shadowrider' + '\n' + 'http://www.i-have-a-dreambox.com/'
        self.session.open(MessageBox,_("%s V%s (Enigma2 Plugin) %s" % (myname,myversion,text2)), MessageBox.TYPE_INFO)
    def showInfo(self):
        try:
            text2 = '\n\n'+'Station: '+self["streamlist"].l.getCurrentSelection()[0].getName()
            text2 = text2+'\n'+str(self["streamlist"].l.getCurrentSelection()[0].getDescription())
            text2 = text2+'\n'+'URL: '+str(self["streamlist"].l.getCurrentSelection()[0].getURL2())
            self.session.open(MessageBox,_("%s V%s (Enigma2 Plugin) %s" % (myname,myversion,text2)), MessageBox.TYPE_INFO)
        except:
            pass
    def showConfig(self):
        if self.shoutcastScreenSaverTimer.isActive():
            self.shoutcastScreenSaverTimer.stop()
        self.session.openWithCallback(self.showConfigDone, EVOShoutcastRadioSetup)
    def showConfigDone(self):
        self.shoutcastScreenSaverconfig = int(config.plugins.EVOMC_ShoutcastRadio.shoutcastScreenSaver.value)
        self.ResetshoutcastScreenSaverTimer()
    def shoutcastScreenSaverTimer_Timeout(self):
        if self.shoutcastScreenSaverTimer.isActive():
            self.shoutcastScreenSaverTimer.stop()
        if streamplayer.is_playing:
            if self["streamlist"].l.getCurrentSelection()[0].getName():
                text = self["streamlist"].l.getCurrentSelection()[0].getName()
                if len(text) != 0:
                    text = text
        else:
            text = "kein Stream gestartet"
        self.session.openWithCallback(self.ResetshoutcastScreenSaverTimer,shoutcastScreenSaver, text)
    def ResetshoutcastScreenSaverTimer(self):
        if onshoutcastScreenSaver:
            pass
        if self.shoutcastScreenSaverconfig != 0:
            if self.shoutcastScreenSaverTimer.isActive():
                self.shoutcastScreenSaverTimer.stop()
            self.shoutcastScreenSaverTimer.start(self.shoutcastScreenSaverconfig * 1000)
    def showHelp(self):
        self.session.open(ShoutcastRadioScreenHelp)
    def set_startstream (self):
        if self["streamlist"].l.getCurrentSelection() is not None:
            start_index = self["streamlist"].instance.getCurrentIndex()
            config.plugins.EVOMC_ShoutcastRadio.startindex.value = int(start_index)
            config.plugins.EVOMC_ShoutcastRadio.startindex.save()
    def stream2deleteSelected(self):
        self.configfile =  "/etc/ShoutcastRadio.conf"
        self.configparser = ConfigParser()
        self.configparser.read(self.configfile)
        if self["streamlist"].l.getCurrentSelection() is not None:
            selectedStream = self["streamlist"].l.getCurrentSelection()[0]
            self.stream_name = selectedStream.getName()
            self.stream2delete = self.stream_name #selectedstreamname
            self.session.openWithCallback(self.deluserIsSure,MessageBox,_("diesen Stream aus Favoriten entfernen?\n\n%s" % self.stream2delete), MessageBox.TYPE_YESNO, timeout=5 )
    def deluserIsSure(self,answer):
        if answer is None:
            self.close
        if answer is False:
            self.close
        else:
            self.configfile =  "/etc/ShoutcastRadio.conf"
            self.configparser = ConfigParser()
            self.configparser.read(self.configfile)
            self.configparser.remove_section(self.stream2delete)
            print "["+myname+"] writing config to "+self.configfile
            fp = open(self.configfile,"w")
            self.configparser.write(fp)
            fp.close()
        self.favoriten()
    def exit_timer(self, result):
        self.vol_down_timer = eTimer()
        self.vol_down_timer.callback.append(self.vol_down_on)
        self.vol_down_timer.stop()
        if result:
            config.plugins.EVOMC_ShoutcastRadio.offtimer_time.value = str(result)
            config.plugins.EVOMC_ShoutcastRadio.offtimer_time.save()
            standby_time = int(result)
            self.ResetshoutcastScreenSaverTimer()
            self.volctrl = eDVBVolumecontrol.getInstance()
            self.oldvol = int(self.volctrl.getVolume())
            voltime = self.oldvol*2000
            standby_time2 = standby_time*60000
            if standby_time2 > voltime:
                self.standby_timer.start(standby_time2-voltime)
            else:
                self.standby_timer.start(standby_time2)
        else:
            global offtimer
            offtimer = "starten"
            self["key_green"].setText("Start Sleeptimer")
    def standby_on(self):
        self.standby_timer.stop()
        if self.shoutcastScreenSaverTimer.isActive():
            self.shoutcastScreenSaverTimer.stop()
        self.offtimer_reaktion()
    def offtimer_reaktion(self):
        global offtimer
        offtimer = "starten"
        self.vol_down_on()
    def vol_down_on(self):
        self.vol_down_timer.stop()
        self.volctrl = eDVBVolumecontrol.getInstance()
        oldvol2 = self.volctrl.getVolume()
        if oldvol2 > 0:
            self.volctrl.volumeDown()
            self.vol_down_timer.startLongTimer(2)
        else:
            global streamplayer
            self.session.nav.stopService()
            streamplayer.stop()

            self.session.open(Screens.Standby.Standby)

            self.volctrl = eDVBVolumecontrol.getInstance()
            self.volctrl.setVolume(self.oldvol,self.oldvol)
            config.audio.volume.value = self.oldvol
            config.audio.volume.save()
            if int(config.plugins.EVOMC_ShoutcastRadio.offtimer_art.value) == 1:
                self.session.open(Screens.Standby.TryQuitMainloop,1)
            #self.close(self.session)
            #self.close()
            self.exit()
class ShoutcastRadioScreenHelp(Screen):
    if HDSkn:
        skin = """
            <screen position="103,73" size="930,456" title="EVOShoutcastRadio Hilfe" >
                <widget name="help" position="0,0" size="930,456" font="Regular;18"/>            
            </screen>""" 
    elif MDSkn:
        skin = """
            <screen position="103,73" size="730,456" title="EVOShoutcastRadio Hilfe" >
                <widget name="help" position="0,0" size="730,456" font="Regular;18"/>            
            </screen>""" 
    else:
        skin = """
            <screen position="50,70" size="620,456" title="EVOShoutcastRadio Hilfe" >
                <widget name="help" position="0,0" size="620,456" font="Regular;16"/>            
            </screen>"""

    def __init__(self, session, args = 0):
        self.skin = ShoutcastRadioScreenHelp.skin
        Screen.__init__(self, session)
        global plugin_path
        readme = plugin_path+"/readme.txt"
        if os_path.exists(readme):
            fp = open(readme)
            text = fp.read()
            fp.close()
        else:
            text = "sorry, Hilfe-Datei nicht vorhanden oder fehlerhaft: "+readme
        self["help"] = ScrollLabel(text)
        self["actions"] = ActionMap(["WizardActions", "DirectionActions","MenuActions"], 
            {
             "ok": self.close,
             "back": self.close,
             "up": self["help"].pageUp,
             "down": self["help"].pageDown
             }, -1)
class StreamMenu(MenuList):
    def __init__(self, list, enableWrapAround = False):
        MenuList.__init__(self, list, enableWrapAround, eListboxPythonMultiContent)
        if HDSkn:
            self.l.setFont(0, gFont("Regular", 22))
            self.l.setFont(1, gFont("Regular", 18))
        else:
            self.l.setFont(0, gFont("Regular", 20))
            self.l.setFont(1, gFont("Regular", 16))
    def postWidgetCreate(self, instance):
        MenuList.postWidgetCreate(self, instance)
        self.instance = instance
        instance.setItemHeight(30)
    def buildList_favoriten(self,listnew):
        if self.instance is not None:
            self.instance.setItemHeight(30)
        list=[]
        for stream in listnew:
            res = [ stream ]
            res.append(MultiContentEntryText(pos=(5, 5), size=(920, 30), font=0, text=stream.getName()))
            res.append(MultiContentEntryText(pos=(5, 1), size=(0, 0), font=1, text=stream.getDescription()))
            list.append(res)
        list.sort(key=lambda x:x[1][7])
        self.l.setList(list)
        if config.plugins.EVOMC_ShoutcastRadio.startindex.value:
            try:
                if len(list) > 1:
                    max_index = len(list)+1
                    start_index = int(config.plugins.EVOMC_ShoutcastRadio.startindex.value)
                    if start_index > 0 and start_index < max_index:
                        s_index = start_index
                        global startindex
                        startindex= s_index
                    else:
                        startindex= 0
            except IndexError:
                pass 
    def buildList_shoutcast(self,listnew):
        if self.instance is not None:
            self.instance.setItemHeight(50)
        list=[]
        for stream in listnew:
            res = [ stream ]
            res.append(MultiContentEntryText(pos=(5, 5), size=(920, 30), font=0, text=stream.getName()))
            res.append(MultiContentEntryText(pos=(5, 26), size=(500, 24), font=1, text=stream.getDescription()))
            list.append(res)
        self.l.setList(list)
        self.moveToIndex(0)
class EVOShoutcastRadioSetup(Screen, ConfigListScreen):
    global HDSkn
    global MDSkn
    if HDSkn:
        skin = """
            <screen name="ShoutcastRadioSetup" position="110,50" size="1060,610" title="ShoutcastRadio - Setup" flags="wfNoBorder" backgroundColor="#485052" >
                <widget name="titelbalken" position="0,0" zPosition="-1" size="1060,30" font="Regular;22" backgroundColor="#485052" transparent="0" />
                <widget name="config" position="5,30" size="1050,540" zPosition="1" transparent="0" backgroundColor="#446678" scrollbarMode="showOnDemand" />
                <ePixmap pixmap="skin_default/buttons/key_red.png" position="440,578" zPosition="2" size="30,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_green.png" position="634,578" zPosition="2" size="30,30" alphatest="on" />
                <widget name="key_red" position="480,578" zPosition="2" size="140,40" halign="left" font="Regular;21" transparent="1" foregroundColor="white" shadowColor="black" shadowOffset="-1,-1"  />
                <widget name="key_green" position="680,578" zPosition="2" size="140,40" halign="left" font="Regular;21" transparent="1" foregroundColor="white" shadowColor="black" shadowOffset="-1,-1"  />
            </screen>"""
    elif MDSkn:
        skin = """
            <screen name="ShoutcastRadioSetup" position="100,30" size="860,510" title="ShoutcastRadio - Setup" flags="wfNoBorder" backgroundColor="#485052" >
                <widget name="titelbalken" position="0,0" zPosition="-1" size="1060,30" font="Regular;22" backgroundColor="#485052" transparent="0" />
                <widget name="config" position="5,30" size="1050,540" zPosition="1" transparent="0" backgroundColor="#446678" scrollbarMode="showOnDemand" />
                <ePixmap pixmap="skin_default/buttons/key_red.png" position="440,578" zPosition="2" size="30,30" alphatest="on" />
                <ePixmap pixmap="skin_default/buttons/key_green.png" position="634,578" zPosition="2" size="30,30" alphatest="on" />
                <widget name="key_red" position="480,578" zPosition="2" size="140,40" halign="left" font="Regular;21" transparent="1" foregroundColor="white" shadowColor="black" shadowOffset="-1,-1"  />
                <widget name="key_green" position="680,578" zPosition="2" size="140,40" halign="left" font="Regular;21" transparent="1" foregroundColor="white" shadowColor="black" shadowOffset="-1,-1"  />
            </screen>"""
    else:
        skin = """
            <screen name="ShoutcastRadioSetup" position="15,15" size="690,545" title="ShoutcastRadio - Setup" flags="wfNoBorder" backgroundColor="#485052">
                <widget name="titelbalken" position="10,2" zPosition="-1" size="690,30" font="Regular;22" backgroundColor="#485052" transparent="0" />
                <widget name="config" position="10,30" size="670,470" zPosition="1" transparent="0" backgroundColor="#446678" scrollbarMode="showOnDemand" />
                <ePixmap pixmap="skin_default/buttons/red.png" position="114,505" size="140,40" alphatest="on" zPosition="1" />
                <ePixmap pixmap="skin_default/buttons/green.png" position="430,505" size="140,40" alphatest="on" zPosition="1" />
                <widget name="key_red" position="114,505" zPosition="2" size="140,40" font="Regular;20" valign="center" halign="center" transparent="1" />
                <widget name="key_green" position="430,505" zPosition="2" size="140,40" font="Regular;20" valign="center" halign="center" transparent="1" />
            </screen>"""
    def __init__(self, session):
        Screen.__init__(self, session)
        self["titel_label"] = Label(" EVOShoutcastRadio - Einstellungen")
        self["key_red"] = Label("Abbruch")
        self["key_green"] = Label("speichern")
        self["titelbalken"] = Label(" EVOShoutcastRadio - Einstellungen")
        self["actions"] = ActionMap(["SetupActions", "ColorActions"],
        {
            "green": self.save,
            "ok": self.save,
            "red": self.close,
            "cancel": self.close
        }, -2)
        EVOShoutcastRadioConfigList = []
        EVOShoutcastRadioConfigList.append(getConfigListEntry(" Zeit bis Bildschirmschoner (min):", config.plugins.EVOMC_ShoutcastRadio.shoutcastScreenSaver))
        EVOShoutcastRadioConfigList.append(getConfigListEntry(" bei Start automatisch streamen:", config.plugins.EVOMC_ShoutcastRadio.startplay))
        EVOShoutcastRadioConfigList.append((" Start-Stream-Index (000-xxx):", config.plugins.EVOMC_ShoutcastRadio.startindex))
        EVOShoutcastRadioConfigList.append((" Ausschalt-Timer (Minuten):", config.plugins.EVOMC_ShoutcastRadio.offtimer_time))
        EVOShoutcastRadioConfigList.append((" Ziel vom Ausschalt-Timer (Standby/Deepstandby):", config.plugins.EVOMC_ShoutcastRadio.offtimer_art))
        ConfigListScreen.__init__(self, EVOShoutcastRadioConfigList, session)
    def save(self):
        ConfigListScreen.keySave(self)
        configfile.save()
    def leer(self):
        pass
class shoutcastScreenSaver(Screen):
    global HDSkn
    global MDSkn
    global plugin_path
    if HDSkn:
        skin = """
            <screen name="shoutcastScreenSaver" position="0,0" size="1280,720" flags="wfNoBorder" backgroundColor="#00000000" title="" >
                <widget name="display_station" position="100,342" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#005B00" />
                <widget name="display_titel" position="100,312" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#005B00" />
                <widget name="display_time" position="100,282" zPosition="1" size="1128,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#007300" />
                <widget name="display_nplaying" position="100,372" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#007300" />
            </screen>"""
    elif MDSkn:
        skin = """
            <screen name="shoutcastScreenSaver" position="0,0" size="974,526" flags="wfNoBorder" backgroundColor="#00000000" title="" >
                <widget name="display_station" position="100,342" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#005B00" />
                <widget name="display_titel" position="100,312" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#005B00" />
                <widget name="display_time" position="100,282" zPosition="1" size="1128,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#007300" />
                <widget name="display_nplaying" position="100,372" zPosition="1" size="1028,30" font="Regular;26" valign="left" transparent="1" foregroundColor="#007300" />
            </screen>"""
    else:
        skin = """
            <screen name="shoutcastScreenSaver" position="0,0" size="720,576" flags="wfNoBorder" backgroundColor="#00000000" title="" >
                <widget name="display_station" position="100,320" zPosition="1" size="720,28" font="Regular;20" valign="left" transparent="1" foregroundColor="#005B00" />
                <widget name="display_titel" position="100,290" zPosition="1" size="720,28" font="Regular;20" valign="left" transparent="1" foregroundColor="#005B00" />
                <widget name="display_time" position="100,260" zPosition="1" size="720,28" font="Regular;20" valign="left" transparent="1" foregroundColor="#005B00" />
            </screen>"""
    def __init__(self, session, text):
        self.sender = text
        global onshoutcastScreenSaver
        onshoutcastScreenSaver = True
        self.title2 = self.sender.split('-')[0]
        self.session = session
        Screen.__init__(self, session)
        self["display_station"] = Label(self.sender)
        self["display_nplaying"] = Label("")
        self["display_titel"] = Label("EVOShoutcastRadio - Stream von Station:")
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
        self.onClose.append(self.disconnectFromMetadataUpdates)
        self.onLayoutFinish.append(self.startRun)
        self.moveTimer = eTimer()
        self.moveTimer.timeout.get().append(self.moveTimer_Timeout)
        self.moveTimer.stop()
    def connectToMetadataUpdates(self):
        global streamplayer
        if streamplayer is not None:
             streamplayer.metadatachangelisteners.append(self.onMetadataChanged)
    def disconnectFromMetadataUpdates(self):
        global streamplayer
        try:
             streamplayer.metadatachangelisteners.remove(self.onMetadataChanged)
        except Exception,e:
            pass
    def onMetadataChanged(self,title):
        try:
            self.streamtitel2= "Titel: " + title
            self["display_nplaying"].setText("%s"%self.streamtitel2)
        except Exception,e:
            self.disconnectFromMetadataUpdates()
            self["display_nplaying"].setText("%s"%self.streamtitel2)
    def startRun(self):
        self.connectToMetadataUpdates()
        self.moveTimer.start(10000)
    def moveTimer_Timeout(self):
        self.moveTimer.stop()
        tage = ['Montag','Dienstag','Mittwoch','Donnerstag','Freitag', 'Samstag', 'Sonntag']
        diezeit = time.localtime(time.time())
        self.TagesZahl = tage[diezeit[6]]
        uhrzeit = strftime("%H:%M",localtime())
        timewidget = self.TagesZahl +", " + strftime("%d.%m.%Y",localtime()) + "   " + uhrzeit
        self["display_time"].setText(timewidget)
        titeltext = uhrzeit + "\n" + self.title2
        self.setTitle("%s"%(titeltext))
        if HDSkn:
            jetzt = time.time()
            x = random.randint(40, 680)
            y = random.randint(20, 550)
            self["display_nplaying"].move(ePoint(x,y+105))
            self["display_station"].move(ePoint(x,y+75))
            self["display_titel"].move(ePoint(x,y+45))
            self["display_time"].move(ePoint(x,y+10))
        else:
            x = random.randint(0, 482)
            y = random.randint(0, 338)
            self["display_nplaying"].move(ePoint(x+20,y+100))
            self["display_station"].move(ePoint(x,y+70))
            self["display_titel"].move(ePoint(x,y+40))
            self["display_time"].move(ePoint(x,y+10))
        self.moveTimer.start(10000)
    def __onClose(self):
        if self.moveTimer.isActive():
            self.moveTimer.stop()
        global onshoutcastScreenSaver
        onshoutcastScreenSaver = None
        self.close()
class Blackscreen(Screen):
	def __init__(self, session, args = None):
		desktop_size = getDesktop(0).size()
		Blackscreen.skin = """<screen name="EVOShoutcastRadio" position="0,0" size="%d,%d" flags="wfNoBorder" zPosition="-1" backgroundColor="black" />""" %(desktop_size.width(), desktop_size.height())
		Screen.__init__(self, session)
