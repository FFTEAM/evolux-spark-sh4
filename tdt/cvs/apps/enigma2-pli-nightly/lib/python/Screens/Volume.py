from Screen import Screen
from Components.VolumeBar import VolumeBar
from Components.Label import Label #NEW LINE		
class Volume(Screen):
	def __init__(self, session):
		Screen.__init__(self, session)
		self.volumeBar = VolumeBar()
		self["Volume"] = self.volumeBar
		self["VolumeText"] = Label("") #NEW LINE

	def setValue(self, vol):
		self.volumeBar.setValue(vol)
		self["VolumeText"].setText(str(vol)) #NEW LINE
