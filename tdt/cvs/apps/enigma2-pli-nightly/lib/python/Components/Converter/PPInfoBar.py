# -*- coding: utf-8 -*-
from Tools.Directories import fileExists
from Components.config import config, ConfigText
from Components.Converter.Converter import Converter
from Components.Element import cached
from ServiceReference import ServiceReference
from enigma import eServiceCenter, eServiceReference, iServiceInformation, iPlayableService, eConsoleAppContainer
from string import upper
from Tools.Transponder import ConvertToHumanReadable
import gettext
from os import system
from Tools import Notifications
from Screens.MessageBox import MessageBox
class PPInfoBar(Converter, object):
	SERVICENAME = 0
	SERVICENUMBER = 1
	ORBITALPOSITION = 2
	PROVIDER = 3
	TUNERINFO = 4
	CSSERVER = 5
#	EMU = 6
	ALLINC = 6
	ALL = 7
	def __init__(self, type):
		Converter.__init__(self, type)
		self.container = eConsoleAppContainer()
		self.getLists()
		if type == "ServiceName":
			self.type = self.SERVICENAME
		elif type == "ServiceNumber":
			self.type = self.SERVICENUMBER
		elif type == "OrbitalPosition":
			self.type = self.ORBITALPOSITION
		elif type == "Provider":
			self.type = self.PROVIDER
		elif type == "TunerInfo":
			self.type = self.TUNERINFO
		elif type == "CsServer":
			self.type = self.CSSERVER
#		elif type == "Emu":
#			self.type = self.EMU
		elif type == "AllInc":
			self.type = self.ALLINC
		else:
			self.type = (type, self.ALL)
	@cached
	def getText(self):
		service = self.source.service
		info = service and service.info()
		if not info:
			return ""
		text = ""
		name = info.getName().replace('\xc2\x86', '').replace('\xc2\x87', '')
		number = self.getServiceNumber(name, info.getInfoString(iServiceInformation.sServiceref))
		orbital = self.getOrbitalPosition(service)
		provider = info.getInfoString(iServiceInformation.sProvider)
		tunerinfo = self.getTunerInfo(service)
		csserver = self.getCSSERVER(info)
#		emu = self.getEMU(info)
		if self.type == self.CSSERVER:
			text = csserver
#		elif self.type == self.EMU:
#			text = emu
		elif self.type == self.SERVICENAME:
			text = name
		elif self.type == self.ALLINC:
			text = "%s.  %s  (%s  %s)" % (number, name, orbital, provider)
		elif self.type == self.SERVICENUMBER:
			text = "%s." % number
		elif self.type == self.ORBITALPOSITION:
			text = orbital
		elif self.type == self.TUNERINFO:
			text = tunerinfo
		elif self.type == self.PROVIDER:
			text = info.getInfoString(iServiceInformation.sProvider)
		return text
	text = property(getText)
	def changed(self, what):
		Converter.changed(self, what)
	def getListFromRef(self, ref):
		list = []
		serviceHandler = eServiceCenter.getInstance()
		services = serviceHandler.list(ref)
		bouquets = services and services.getContent("SN", True)
		for bouquet in bouquets:
			services = serviceHandler.list(eServiceReference(bouquet[0]))
			channels = services and services.getContent("SN", True)
			for channel in channels:
				if not channel[0].startswith("1:64:"):
					list.append(channel[1].replace('\xc2\x86', '').replace('\xc2\x87', ''))
		return list
	def getLists(self):
		self.tv_list = self.getListFromRef(eServiceReference('1:7:1:0:0:0:0:0:0:0:(type == 1) || (type == 17) || (type == 195) || (type == 25) FROM BOUQUET "bouquets.tv" ORDER BY bouquet'))
		self.radio_list = self.getListFromRef(eServiceReference('1:7:2:0:0:0:0:0:0:0:(type == 2) FROM BOUQUET "bouquets.radio" ORDER BY bouquet'))
	def getServiceNumber(self, name, ref):
		list = []
		if ref.startswith("1:0:2"):
			list = self.radio_list
		elif ref.startswith("1:0:1"):
			list = self.tv_list
		number = ""
		if name in list:
			for idx in range(1, len(list)):
				if name == list[idx-1]:
					number = str(idx)
					break
		return number
	def getOrbitalPosition(self, service):
		feinfo = service.frontendInfo()
		orbital = 0
		if feinfo is not None:
			frontendData = feinfo and feinfo.getAll(True)
			if frontendData is not None:
				if frontendData.has_key("tuner_type"):
					if frontendData["tuner_type"] == "DVB-S":
						orbital = int(frontendData["orbital_position"])
		if orbital > 1800:
			return str((float(3600 - orbital))/10.0) + "°W"
		elif orbital > 0:
			return str((float(orbital))/10.0) + "°E"
		return ""
	def getTunerInfo(self, service):
		tunerinfo = ""
		info = (service and service.info())
		Ret_Text = ""
		xresol = info.getInfo(iServiceInformation.sVideoWidth)
		yresol = info.getInfo(iServiceInformation.sVideoHeight)
		if info:
			try:
				Ret_Text = str(xresol) + "x" + str(yresol)
				transponderData = ConvertToHumanReadable(info.getInfoObject(iServiceInformation.sTransponderData))
				tunerType = transponderData.get("tuner_type")
				symbolRate = transponderData.get("symbol_rate", "")
				frequency = transponderData.get("frequency", "")
				polarization = transponderData.get("polarization", "").replace(_("Horizontal"), "H").replace(_("Vertical"), "V").replace(_("Circular left"), "CL").replace(_("Circular right"), "CR")
				fec = transponderData.get("fec_inner", "")
				if (tunerType == _("Satellite")):
					frequency = (str((int(frequency) / 1000)) + " MHz")
					symbolRate = (str((int(symbolRate) / 1000)))
					tunerinfo = (((((((Ret_Text + " ") + frequency + " ") + polarization) + " ") + fec) + " ") + symbolRate)
				return tunerinfo
			except:
				return ""
#	def getEMU(self, info):
#		if (info is not None):
#			if fileExists("/tmp/correctvol") is False:
#				corrVol = "isactive4=`cat /usr/local/share/enigma2/settings | grep config.audio.volume= | cut -d = -f2`;touch /tmp/correctvol;newvol=$((63 - (( $isactive4 * 63 ) / 100)));echo $newvol > /proc/stb/avs/0/volume &"
#				self.container.execute(corrVol)
#			if (info.getInfo(iServiceInformation.sIsCrypted) == 0):
#				EmuAutoOnOff = config.plugins.EVOAutoStart.Emu.value
#				if fileExists("/tmp/ecm.info") and EmuAutoOnOff == "camd3Auto":
#					cd3 = system("pgrep camd3")
#					if cd3 == 0:
#						cmd = "killall -15 camd3;rm /tmp/ecm.info &"
#						self.container.execute(cmd)
#				elif fileExists("/tmp/ecm.info") and EmuAutoOnOff == "incubusAuto":
#					inc = system("pgrep incubusCamd")
#					if inc == 0:
#						cmd = "killall -15 incubusCamd;rm /tmp/ecm.info &"
#						self.container.execute(cmd)
#				else:
#					pass
#			if (info.getInfo(iServiceInformation.sIsCrypted) == 1):
#				EmuAutoOnOff = config.plugins.EVOAutoStart.Emu.value
#				if fileExists("/tmp/ecm.info") is False and EmuAutoOnOff == "camd3Auto":
#					cd3 = system("pgrep camd3")
#					if cd3 == 256:
#						cmd = "echo emu=camd3 > /var/emu/emu.conf;/var/emu/camd3 /var/keys/camd3.config &"
#						self.container.execute(cmd)
#				elif fileExists("/tmp/ecm.info") is False and EmuAutoOnOff == "incubusAuto":
#					inc = system("pgrep incubusCamd")
#					if inc == 256:
#						cmd = "echo emu=incubus > /var/emu/emu.conf;/var/emu/incubusCamd &"
#						self.container.execute(cmd)
#						Notifications.AddPopup(text = "Starting INCUBUS now...", type = MessageBox.TYPE_INFO, timeout = 3)
#				else:
#					pass



#				camd = ""
#				if fileExists("/var/emu/emu.conf"):
#					try:
#						emu = open("/var/emu/emu.conf", "r")
#						emufile = emu.read()
#						emu.close()
#					except:
#						emufile = ""
#					emuInfo = emufile.split("\n")
#					for line in emuInfo:
#						line = line.split ("=")
#						line = line[1].strip()
#						line = line.split (".")
#						line = line[0].strip()
#						camd = line
#						return camd
#					else:
#						pass

	def CSSERVERLine(self, line):
		if line.__contains__(":"):
			idx = line.index(":")
			line = line[idx+1:]
			line = line.replace("\n", "")
			while line.startswith(" "):
				line = line[1:]
			while line.endswith(" "):
				line = line[:-1]
			return line
		else:
			pass
	def getCSSERVER(self, info):
		if (info is not None):
			if (info.getInfo(iServiceInformation.sIsCrypted) == 1):
				try:
					f = open("/tmp/ecm.info", "r")
					content = f.read()
					f.close()
				except:
					content = ""
				contentInfo = content.split("\n")
				for line in contentInfo:
					line = line.upper()
					if line.startswith("DECODE:") or line.startswith("FROM:") or line.startswith("ADDRESS:") or line.startswith("decode:") or line.startswith("from:") or line.startswith("address:"):
						decFrom = "From: CS"
						return decFrom
			else:
				pass
		else:
			pass
