#
# JobManager
#
# Coded by dre (c) 2012
# dirselect by DarkVolli
# Support: www.dreambox-tools.info
# E-Mail: dre@dreambox-tools.info
#
# This plugin is open source but it is NOT free software.
#
# This plugin may only be distributed to and executed on hardware which
# is licensed by Dream Multimedia GmbH.
# In other words:
# It's NOT allowed to distribute any parts of this plugin or its source code in ANY way
# to hardware which is NOT licensed by Dream Multimedia GmbH.
# It's NOT allowed to execute this plugin and its source code or even parts of it in ANY way
# on hardware which is NOT licensed by Dream Multimedia GmbH.
#
# If you want to use or modify the code or parts of it,
# you have to keep MY license and inform me about the modifications by mail.

from Components.ActionMap import *
from Components.config import config, ConfigSubsection, ConfigSelection, getConfigListEntry, ConfigText
from Components.ConfigList import ConfigListScreen
from Components.Label import Label
from Components.MenuList import MenuList
from Components.MultiContent import MultiContentEntryText, MultiContentEntryProgress
from Components.Pixmap import Pixmap
from Components.ProgressBar import ProgressBar
from Plugins.Plugin import PluginDescriptor
from Screens.HelpMenu import HelpableScreen
from Screens.Screen import Screen
from Tools.Directories import fileExists
from Tools.Downloader import HTTPProgressDownloader, downloadWithProgress
from dirSelect import dirSelectDlg
from enigma import RT_HALIGN_LEFT, RT_HALIGN_RIGHT, eListboxPythonMultiContent, gFont
import os

#TODO:
#Future developement:
#- add an option to automatically start downloads upon booting (up to a max of defined parallel downloads)
#- add an option to set a max of parallel downloads
#- add an option to remove finished job automatically from jobs
#- add an option to automatically refresh the gui every n seconds

#choices = [("/media/hdd", "/media/hdd"), ("/media/usb", "/media/usb"), ("/media/cf", "/media/cf")]

config.plugins.JobManager = ConfigSubsection()
#config.plugins.JobManager.dlpath = ConfigSelection(choices = choices, default = "/media/hdd")
config.plugins.JobManager.dlpath = ConfigText(default = "/media/hdd/movies")

class JobList(MenuList):
	def __init__(self,list):
		MenuList.__init__(self,list,False, eListboxPythonMultiContent)
		self.l.setItemHeight(50)
		self.l.setFont(0, gFont("Regular", 18))

def JobListEntry(name, status, totalSize=0, currentSize=0):
	res = [(name, status, totalSize, currentSize)]
	sizeInMB = int(totalSize)/1024/1024
	if sizeInMB > 0:
		currentProgress = int(100*currentSize/sizeInMB)
	else:
		currentProgress = 0
	res.append(MultiContentEntryText(pos=(0, 0), size=(520, 25), font=0, text=name))
	res.append(MultiContentEntryText(pos=(0, 25), size=(120, 25), font=0, text=status))
	res.append(MultiContentEntryText(pos=(120,25), size=(100,25), font=0, text="%d MB" %(currentSize)))
	res.append(MultiContentEntryText(pos=(220,25), size=(100,25), font=0, text="%d MB" %(sizeInMB)))
	res.append(MultiContentEntryProgress(pos=(320,25), size=(150,25), percent=currentProgress))
	return res

def ErrorListEntry(name, error):
	res = [(name, error)]
	res.append(MultiContentEntryText(pos=(0, 0), size=(520, 25), font=0, text=name))
	res.append(MultiContentEntryText(pos=(0, 25), size=(520, 25), font=0, text=error))
	return res

class DownloadJob:
	def __init__(self,url, name, status="new", filesize=0, filetype="flv"):
		self.url = url
		self.name = name
		self.filetype = filetype
		self.filename = "%s/%s.%s" %(config.plugins.JobManager.dlpath.value, self.name, self.filetype)
		self.filesize=filesize
		self.filesizeInMB=0
		self.status = status
		self.percentageProgress=0
		
	def startDownload(self):
		self.status = "downloading"
		self.currentDownload = downloadWithProgress(self.url, self.filename)
		self.currentDownload.addProgress(self.setProgress)
		self.currentDownload.start().addCallback(self.downloadFinished).addErrback(self.downloadFailed)  
		
	def stopDownload(self):
		self.status = "stopped"
		self.currentDownload.stop()
	
	def setProgress(self, receivedBytes, totalBytes):
		self.percentageProgress = int(float(receivedBytes) / float(totalBytes) * 100)
		if self.filesize == 0:
			buf = []
			jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "r")
			for line in jobFile:
				linesplit = line.split('^^')
				if linesplit[1] == self.name and len(linesplit) == 3:
					line = line.replace('\n', '^^%s' %(totalBytes))
					line = line + "\n"
				buf.append(line)
				
			jobFile.close()
			
			newJobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "w")
			for line in buf:
				newJobFile.write(line)
			newJobFile.close()
		self.filesize=totalBytes
		self.filesizeInMB=int((totalBytes/1024/1024))
		#print "current percentage: ", self.percentageProgress
		
	def downloadFinished(self, string=None):
		if string is not None:
			print "finished but something is not good: ", string
			#replace status with return string
			buf = []
			jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "r")
			for line in jobFile:
				linesplit = line.split('^^')
				if linesplit[1] == self.name:
					line = line.replace(linesplit[2], string)
				buf.append(line)
				
			jobFile.close()
			
			newJobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "w")
			for line in buf:
				newJobFile.write(line)
			newJobFile.close()
		else:
			print "finished: ", string
			if self.status != "stopped":
				self.status = "finished"
			#change status in jobs to finished when download is finished
			buf = []
			jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "r")
			for line in jobFile:
				linesplit = line.split('^^')
				if linesplit[1] == self.name:
					line = line.replace(linesplit[2], self.status)
				buf.append(line)
				
			jobFile.close()
			
			newJobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "w")
			for line in buf:
				newJobFile.write(line)
			newJobFile.close()

	def downloadFailed(self, error=None, errorText=""):
		if error:
			if errorText == "":
				errorText = error.getErrorMessage()
			print "there's an error: ", errorText
			self.status = "error"
			
			#change status in jobs to error when an error occurred
			buf = []
			jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "r")
			for line in jobFile:
				linesplit = line.split('^^')
				if linesplit[1] == self.name:
					line = line.replace(linesplit[2], self.status)
				buf.append(line)
				
			jobFile.close()
			
			newJobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "w")
			for line in buf:
				newJobFile.write(line)
			newJobFile.close()
			
			errorFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/errors", "a")
			errorFile.write("%s^^%s\n" %(self.name, errorText))
			errorFile.close()

class JobManager:
	def __init__(self):
		self.jobList =[]
			
	def addJobToList(self, url, name):
		self.jobStatus = "new"
		self.newJob = [url, name, self.jobStatus]
		print "download added to list: ", self.newJob
		
		jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "a")
		jobFile.write("%s^^%s^^%s\n" %(url, name, self.jobStatus))
		jobFile.close()
		
		currentDownload = DownloadJob(url, name)
		self.jobList.append(currentDownload)
		
		return currentDownload
		
	def removeJobFromList(self, name):
		for i, job in enumerate(self.jobList):
			if job.name == name:
				del self.jobList[i]
				break

	def startDownload(self, job=None):
		if job == None:
			#start all jobs in status new or stopped when no job has been handed over
			for currentDownload in self.jobList:
				if currentDownload.status == "new" or currentDownload.status == "stopped":
					currentDownload.startDownload()
		else:
			#start job handed over when in status new or stopped
			if job.status == "new" or job.status == "stopped":
				job.startDownload()
				print "started download: ", job.name
			else:
				print "job not started", job.name, job.status

	def stopDownload(self, job=None):
		if job == None:
			#stop all jobs in status downloading when no job has been handed over
			for currentDownload in self.jobList:
				if currentDownload.status == "downloading":
					currentDownload.stopDownload()
		else:
			#stop job handed over when in status downloading
			if job.status == "downloading":
				job.stopDownload()
				print "stopped download: ", job.name
			else:
				print "job is not downloading", job.name, job.status

	#function is called upon starting dreambox
	def readJobs(self):
		print "[JobManager]: reading jobs"
		if fileExists("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs"):
			jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "r")
			for line in jobFile:
				linesplit = line.split('^^')
				status = linesplit[2].strip('\n\r')
				size=0
				if len(linesplit) == 4:
					size=linesplit[3]
				if linesplit[2] != "downloading":
					currentDownload = DownloadJob(linesplit[0], linesplit[1], status, size)
					self.jobList.append(currentDownload)
					#TODO: add option to config to start downloads upon starting dreambox
					#self.startDownload(currentDownload)
			jobFile.close()

	def getCurrentDownloads(self):
		self.currentDownloadsList = []
		
		for job in self.jobList:
			if job.status == "downloading":
				self.currentDownloadsList.append(job)

		return self.currentDownloadsList
	
	def getJob(self, name=None):
		if name is not None:
			for job in self.jobList:
				if job.name==name:
					return job
					break

jm = JobManager()

class JobManagerConfigScreen(Screen, HelpableScreen, ConfigListScreen):
	skin = """
		<screen position="center,center" size="550,400" title="JobManager - Config" >
			<widget itemHeight="25" name="config" position="10,10" size="520,300" scrollbarMode="showOnDemand"/>
			<widget name="ButtonGreen" position="10,320" pixmap="skin_default/buttons/key_green.png" zPosition="10" size="35,25" transparent="1" alphatest="on" />
			<widget name="TextGreen" position="50,320" size="220,20" valign="center" halign="left" zPosition="10" font="Regular;18" transparent="1" />
		</screen>"""
		
	def __init__(self, session):
		Screen.__init__(self, session)
		HelpableScreen.__init__(self)
		self.session = session
		self.skin = JobManagerConfigScreen.skin
		
		self["ButtonGreen"] = Pixmap()
		self["TextGreen"] = Label(_("Save"))
		
		self["ColorActions"] = HelpableActionMap(self, "ColorActions",
		{
			"green":	(self.saveConfig, _("Save settings")),
		}, -1)
		
		self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
		{
			"ok":		(self.openDirSelectScreen, _("Open directory selection screen")),
			"cancel":	(self.close, _("Close config")),
		}, -1)
		
		self.list = []
		
		ConfigListScreen.__init__(self, self.list)
		
		self.list.append(getConfigListEntry(_("Save downloads to: "), config.plugins.JobManager.dlpath))
		
		self["config"].setList(self.list)
		
	def saveConfig(self):
		for x in self.list:
			x[1].save()
		self.close()
		
	def openDirSelectScreen(self):
		self.session.openWithCallback(self.dirSelectDlgClosed, dirSelectDlg, "/media/dummy/", False)  # just add any (not even existing) subdir to start in /media

	def dirSelectDlgClosed(self, directory):
		if directory != False:
			config.plugins.JobManager.dlpath.value = directory

class JobManagerErrorScreen(Screen, HelpableScreen):
	skin = """
		<screen position="center,center" size="550,400" title="JobManager - Errors" >
			<widget name="menulist" position="10,10" size="520,300" scrollbarMode="showOnDemand" />
		</screen>"""
		
	def __init__(self, session):
		Screen.__init__(self, session)
		HelpableScreen.__init__(self)
		self.session = session
		self.skin = JobManagerErrorScreen.skin
		
		self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
		{
			"cancel":	(self.close, _("Close error log")),
		}, -1)
		
		self.menulist = []
		self["menulist"] = JobList([])
		
		self.getErrorList()
		
	def getErrorList(self):
		self.menulist = []
		
		if fileExists("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/errors"):
			errorFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/errors", "r")
			for line in errorFile:
				linesplit = line.split('^^')
				self.menulist.append(ErrorListEntry(str(linesplit[0]), str(linesplit[1].strip('\n'))))
			errorFile.close()
			
		self["menulist"].l.setList(self.menulist)

class JobManagerFrontEnd(Screen, HelpableScreen):
	skin = """
		<screen position="center,center" size="550,400" title="JobManager" >
			<widget name="menulist" position="10,10" size="520,300" scrollbarMode="showOnDemand" />
			<widget name="ButtonRed" position="10,320" pixmap="skin_default/buttons/key_red.png" zPosition="10" size="35,25" transparent="1" alphatest="on" />
			<widget name="TextRed" position="50,320" size="220,20" valign="center" halign="left" zPosition="10" font="Regular;18" transparent="1" />
			<widget name="ButtonGreen" position="265,320" pixmap="skin_default/buttons/key_green.png" zPosition="10" size="35,25" transparent="1" alphatest="on" />
			<widget name="TextGreen" position="305,320" size="220,20" valign="center" halign="left" zPosition="10" font="Regular;18" transparent="1" />
			<widget name="ButtonYellow" position="10,350" pixmap="skin_default/buttons/key_yellow.png" zPosition="10" size="35,25" transparent="1" alphatest="on" />
			<widget name="TextYellow" position="50,350" size="220,20" valign="center" halign="left" zPosition="10" font="Regular;18" transparent="1"/>
			<widget name="ButtonBlue" position="265,350" pixmap="skin_default/buttons/key_blue.png" zPosition="10" size="35,25" transparent="1" alphatest="on" />
			<widget name="TextBlue" position="305,350" size="220,20" valign="center" halign="left" zPosition="10" font="Regular;18" transparent="1" />
		</screen>"""

	def __init__(self, session):
		Screen.__init__(self, session)
		HelpableScreen.__init__(self)
		self.session = session
		self.skin = JobManagerFrontEnd.skin
			
		self["OkCancelActions"] = HelpableActionMap(self, "OkCancelActions",
		{
			"cancel":	(self.close, _("Close plugin")),
		}, -1)

		self["ColorActions"] = HelpableActionMap(self, "ColorActions",
		{
			"red":		(self.removeJob, _("Delete download")),
			"blue":		(self.stopSelectedJob, _("Stop download")),
			"green":	(self.startSelectedJob, _("Start download")),
			"yellow":	(self.getJobList, _("Refresh GUI")),
		}, -1)
		
		self["MovieSelectionActions"] = HelpableActionMap(self, "MovieSelectionActions",
		{
			"showEventInfo":	(self.showErrorLog, _("Show error log")),
			"contextMenu":		(self.showConfig, _("Show config")),
		}, -1)

		self.menulist = []
		
		self["ButtonRed"] = Pixmap()
		self["ButtonGreen"] = Pixmap()
		self["ButtonYellow"] = Pixmap()
		self["ButtonBlue"] = Pixmap()
		self["TextRed"] = Label(_("Remove download"))
		self["TextGreen"] = Label(_("Start download"))
		self["TextYellow"] = Label(_("Refresh GUI"))
		self["TextBlue"] = Label(_("Stop download"))
		self["menulist"] = JobList([])
		
		self.getJobList()
		
	def getJobList(self):
		self.menulist = []
		currentSize = 0

		for job in jm.jobList:
			try:
				currentSize = os.path.getsize("%s/%s.%s" %(config.plugins.JobManager.dlpath.value, job.name, job.filetype))/1024/1024
			except OSError:
				currentSize = 0
			self.menulist.append(JobListEntry(job.name, job.status, job.filesize, currentSize))

		self["menulist"].l.setList(self.menulist)
	
	def removeJob(self):
		currentEpisodeName = self["menulist"].getCurrent()[0][0]
		currentStatus = self["menulist"].getCurrent()[0][1]
		currentStatus = currentStatus.strip('\n\r')
		
		buf = []
		if currentStatus == "downloading":
			self.stopSelectedJob()
		jobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "r")
		for line in jobFile:
			linesplit = line.split('^^')
			if linesplit[1] != currentEpisodeName:
				buf.append(line)
		jobFile.close()
		
		newJobFile = open("/usr/lib/enigma2/python/Plugins/Extensions/JobManager/jobs", "w")
		for line in buf:
			newJobFile.write(line)
		newJobFile.close()
		
		currentJob = jm.getJob(currentEpisodeName)
		
		if fileExists("%s/%s.%s" %(config.plugins.JobManager.dlpath.value, currentEpisodeName, currentJob.filetype)) and currentStatus != "finished":
			os.system("rm %s/%s.%s" %(config.plugins.JobManager.dlpath.value, currentEpisodeName, currentJob.filetype))
		
		
		
		jm.removeJobFromList(currentEpisodeName)
		#print jm.jobList
		self.getJobList()
	
	def stopSelectedJob(self):
		currentEpisodeName = self["menulist"].getCurrent()[0][0]
		currentStatus = self["menulist"].getCurrent()[0][1]
		
		for currentDownload in jm.jobList:
			if currentDownload.name == currentEpisodeName:
				jm.stopDownload(currentDownload)
				self.getJobList()

	def startSelectedJob(self):
		currentEpisodeName = self["menulist"].getCurrent()[0][0]
		currentStatus = self["menulist"].getCurrent()[0][1]
		
		for currentDownload in jm.jobList:
			if currentDownload.name == currentEpisodeName:
				jm.startDownload(currentDownload)
				self.getJobList()
				
	def showErrorLog(self):
		self.session.open(JobManagerErrorScreen)
		
	def showConfig(self):
		self.session.open(JobManagerConfigScreen)

def main(session,**kwargs):
	session.open(JobManagerFrontEnd)

def autostart(reason, **kwargs):
	jm.readJobs()

def Plugins(**kwargs):
	return [PluginDescriptor(name="JobManager", description="FrontEnd for JobManager", where = [PluginDescriptor.WHERE_PLUGINMENU], fnc=main)
			,PluginDescriptor(where=[PluginDescriptor.WHERE_AUTOSTART], fnc=autostart)]
