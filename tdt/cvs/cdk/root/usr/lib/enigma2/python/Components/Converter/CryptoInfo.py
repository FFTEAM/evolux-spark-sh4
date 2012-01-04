from Tools.Directories import fileExists
from Components.Converter.Converter import Converter
from Components.Element import cached
from enigma import iServiceInformation, iPlayableService
from string import upper
import gettext

class CryptoInfo(Converter, object):
	ALL = 0
	IRDETO = 1
	SECA = 2
	NAGRA = 3
	VIA = 4
	CONAX = 5
	TUNNELDNAGRA = 6
	CRYPTO = 7

	def __init__(self, type):
		Converter.__init__(self, type)
		self.list = []
		if type == "Irdeto":
			self.type = self.IRDETO
		elif type == "Seca":
			self.type = self.SECA
		elif type == "Nagra":
			self.type = self.NAGRA
		elif type == "Via":
			self.type = self.VIA
		elif type == "Conax":
			self.type = self.CONAX
		elif type == "TunneldNagra":
			self.type = self.TUNNELDNAGRA
		elif type == "Crypto":
			self.type = self.CRYPTO
		else:
			self.type = self.ALL

	@cached
	def getText(self):
		service = self.source.service
		info = service and service.info()
		if not info:
			pass
		
		text = ""
		
		caidfrom = self.getCaidFrom(info)
		irdeto = self.getIRDETO(caidfrom)
		seca = self.getSECA(caidfrom)
		nagra = self.getNAGRA(caidfrom)
		via = self.getVIA(caidfrom)
		conax = self.getCONAX(caidfrom)
		tunneldnagra = self.getTUNNELDNAGRA(caidfrom)
		crypto = self.getCRYPTO(caidfrom)

		if self.type == self.IRDETO:
			text = irdeto
		elif self.type == self.SECA:
			text = seca
		elif self.type == self.NAGRA:
			text = nagra
		elif self.type == self.VIA:
			text = via
		elif self.type == self.CONAX:
			text = conax
		elif self.type == self.TUNNELDNAGRA:
			text = tunneldnagra
		elif self.type == self.CRYPTO:
			text = crypto
	
		return text

	text = property(getText)

	def changed(self, what):
		Converter.changed(self, what)

#############################################################
	def int2hex(self, int):
			return "%x" % int


	def getEcmID(self):
		ecminf = []
		service = self.source.service
		if service is not None:
			info = service and service.info()
			if info is not None:
				caids = info.getInfoObject(iServiceInformation.sCAIDs)
				if caids is not None:
					for ecm in caids:
						ecm = self.int2hex(ecm)
						ecmID = ecm
						if len(ecmID) == 3:
							ecmID = "0%s" % ecmID
						ecminf = ecmID
						return ecmID
		else:
			pass
#############################################################
	def getCaidFrom(self, info):
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
					if line.__contains__("CAID"):
						caid1 = line.upper()
						if caid1.__contains__("X"):
							idx = caid1.index("X")
							caid1 = caid1[idx+1:]
							if len(caid1) == 3:
								caid1 = "0%s" % caid1
							caid2 = caid1[:4]
							return caid2
				else:
					pass

	def getIRDETO(self, caidfrom):
		CaidRange = ("0600,06FF,I",)
		CaidFound = caidfrom
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
			try:
				if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
					CAID = (CaidList[2])
					return CAID
			except:
				pass

	def getSECA(self, caidfrom):
		CaidRange = ("0100,01FF,S",)
		CaidFound = caidfrom
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
			try:
				if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
					CAID = (CaidList[2])
					return CAID
			except:
				pass

	def getNAGRA(self, caidfrom):
		CaidRange = ("1700,1702,N", "1800,18FF,N")
		CaidFound = caidfrom
		CAID = ""
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
				try:
					if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
						CAID = (CaidList[2])
						return CAID
				except:
					pass

	def getVIA(self, caidfrom):
		CaidRange = ("0500,05FF,V",)
		CaidFound = caidfrom
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
			try:
				if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
					CAID = (CaidList[2])
					return CAID
			except:
				pass

	def getCONAX(self, caidfrom):
		CaidRange = ("0B00,0BFF,C",)
		CaidFound = caidfrom
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
			try:
				if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
					CAID = (CaidList[2])
					return CAID
			except:
				pass

	def getTUNNELDNAGRA(self, caidfrom):
		CaidRange = ("1701,1702,TN",)
		CaidFound = caidfrom
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
			try:
				if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
					CAID = (CaidList[2])
					return CAID
			except:
				pass

	def getCRYPTO(self, caidfrom):
		CaidRange = ("0D00,0DFF,CW",)
		CaidFound = caidfrom
		if (CaidFound != ""):
			for CaidOn in CaidRange:
				CaidList = CaidOn.split(",")
			try:
				if ((int(CaidFound, 16) >= int(CaidList[0], 16)) and (int(CaidFound, 16) <= int(CaidList[1], 16))):
					CAID = (CaidList[2])
					return CAID
			except:
				pass


