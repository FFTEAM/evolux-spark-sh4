from Components.config import config, ConfigSubsection, ConfigSlider, ConfigSelection, ConfigNothing, NoSave
from Tools.CList import CList
from os import path as os_path, system
# The "VideoEnhancement" is the interface to /proc/stb/vmpeg/0.

class VideoEnhancement:
	firstRun = True

	def __init__(self):
		self.last_modes_preferred =  [ ]
		self.createConfig()

	def createConfig(self, *args):
		config.psi = ConfigSubsection()
		config.psi.configsteps = NoSave(ConfigSelection(choices=[1, 5, 10, 25], default = 5))

		if os_path.exists("/proc/stb/video/plane/psi_contrast"):
			def setContrast(config):
				myval = int(config.value)
				try:
					print "--> setting contrast to: %d" % myval
					#open("/proc/stb/video/plane/psi_contrast", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_contrast) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_contrast."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.contrast = ConfigSlider(default=128, limits=(0,256))
			config.psi.contrast.addNotifier(setContrast)
		else:
			config.psi.contrast = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_saturation"):
			def setSaturation(config):
				myval = int(config.value)
				try:
					print "--> setting saturation to: %d" % myval
					#open("/proc/stb/video/plane/psi_saturation", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_saturation) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_saturaion."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.saturation = ConfigSlider(default=128, limits=(0,256))
			config.psi.saturation.addNotifier(setSaturation)
		else:
			config.psi.saturation = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_tint"):
			def setHue(config):
				myval = int(config.value)
				try:
					print "--> setting tint to: %d" % myval
					#open("/proc/stb/video/plane/psi_hue", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_tint) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_tint."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.hue = ConfigSlider(default=128, limits=(0,256))
			config.psi.hue.addNotifier(setHue)
		else:
			config.psi.hue = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_brightness"):
			def setBrightness(config):
				myval = int(config.value)
				try:
					print "--> setting brightness to: %d" % myval
					#open("/proc/stb/video/plane/psi_brightness", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_brightness) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_brightness."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.brightness = ConfigSlider(default=128, limits=(0,256))
			config.psi.brightness.addNotifier(setBrightness)
		else:
			config.psi.brightness = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_block_noise_reduction"):
			def setBlock_noise_reduction(config):
				myval = int(config.value)
				try:
					print "--> setting block_noise_reduction to: %d" % myval
					#open("/proc/stb/video/plane/psi_block_noise_reduction", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_block_noise_reduction) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_block_noise_reduction."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.block_noise_reduction = ConfigSlider(default=0, limits=(0,5))
			config.psi.block_noise_reduction.addNotifier(setBlock_noise_reduction)
		else:
			config.psi.block_noise_reduction = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_mosquito_noise_reduction"):
			def setMosquito_noise_reduction(config):
				myval = int(config.value)
				try:
					print "--> setting mosquito_noise_reduction to: %d" % myval
					#open("/proc/stb/video/plane/psi_mosquito_noise_reduction", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_mosquito_noise_reduction) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_mosquito_noise_reduction."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.mosquito_noise_reduction = ConfigSlider(default=0, limits=(0,5))
			config.psi.mosquito_noise_reduction.addNotifier(setMosquito_noise_reduction)
		else:
			config.psi.mosquito_noise_reduction = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_digital_contour_removal"):
			def setDigital_contour_removal(config):
				myval = int(config.value)
				try:
					print "--> setting digital_contour_removal to: %d" % myval
					#open("/proc/stb/video/plane/psi_digital_contour_removal", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_digital_contour_removal) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_digital_contour_removal."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.digital_contour_removal = ConfigSlider(default=0, limits=(0,5))
			config.psi.digital_contour_removal.addNotifier(setDigital_contour_removal)
		else:
			config.psi.digital_contour_removal = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_split"):
			def setSplitMode(config):
				try:
					print "--> setting splitmode to:",str(config.value)
					#open("/proc/stb/video/plane/psi_split", "w").write(str(config.value))
				except IOError:
					print "couldn't write psi_split."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.split = ConfigSelection(choices={
					"off": _("Off"),
					"left": _("Left"),
					"right": _("Right")},
					default = "off")
			config.psi.split.addNotifier(setSplitMode)
		else:
			config.psi.split = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_sharpness"):
			def setSharpness(config):
				myval = int(config.value)
				try:
					print "--> setting sharpness to: %d" % myval
					#open("/proc/stb/video/plane/psi_sharpness", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_sharpness) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_sharpness."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.sharpness = ConfigSlider(default=0, limits=(0,256))
			config.psi.sharpness.addNotifier(setSharpness)
		else:
			config.psi.sharpness = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_auto_flesh"):
			def setAutoflesh(config):
				myval = int(config.value)
				try:
					print "--> setting auto_flesh to: %d" % myval
					#open("/proc/stb/video/plane/psi_auto_flesh", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_auto_flesh) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_auto_flesh."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.auto_flesh = ConfigSlider(default=0, limits=(0,4))
			config.psi.auto_flesh.addNotifier(setAutoflesh)
		else:
			config.psi.auto_flesh = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_green_boost"):
			def setGreenboost(config):
				myval = int(config.value)
				try:
					print "--> setting green_boost to: %d" % myval
					#open("/proc/stb/video/plane/psi_green_boost", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_green_boost) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_green_boost."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.green_boost = ConfigSlider(default=0, limits=(0,4))
			config.psi.green_boost.addNotifier(setGreenboost)
		else:
			config.psi.green_boost = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_blue_boost"):
			def setBlueboost(config):
				myval = int(config.value)
				try:
					print "--> setting blue_boost to: %d" % myval
					#open("/proc/stb/video/plane/psi_blue_boost", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_blue_boost) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_blue_boost."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.blue_boost = ConfigSlider(default=0, limits=(0,4))
			config.psi.blue_boost.addNotifier(setBlueboost)
		else:
			config.psi.blue_boost = NoSave(ConfigNothing())

		if os_path.exists("/proc/stb/video/plane/psi_dynamic_contrast"):
			def setDynamic_contrast(config):
				myval = int(config.value)
				try:
					print "--> setting dynamic_contrast to: %d" % myval
					#open("/proc/stb/video/plane/psi_dynamic_contrast", "w").write("%d" % myval)
					cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_dynamic_contrast) &" % myval
					system(cmd)
				except IOError:
					print "couldn't write psi_dynamic_contrast."

				if not VideoEnhancement.firstRun:
					self.setConfiguredValues()

			config.psi.dynamic_contrast = ConfigSlider(default=0, limits=(0,256))
			config.psi.dynamic_contrast.addNotifier(setDynamic_contrast)
		else:
			config.psi.dynamic_contrast = NoSave(ConfigNothing())

		try:
			x = config.av.scaler_sharpness.value
		except KeyError:
			if os_path.exists("/proc/stb/video/plane/psi_scaler_sharpness"):
				def setScaler_sharpness(config):
					myval = int(config.value)
					try:
						print "--> setting scaler_sharpness to: %d" % myval
						#open("/proc/stb/video/plane/psi_scaler_sharpness", "w").write("%d" % myval)
						cmd = "(sleep 2;echo %d > /proc/stb/video/plane/psi_scaler_sharpness) &" % myval
						system(cmd)
					except IOError:
						print "couldn't write psi_scaler_sharpness."

					if not VideoEnhancement.firstRun:
						self.setConfiguredValues()

				config.av.scaler_sharpness = ConfigSlider(default=13, limits=(0,26))
				config.av.scaler_sharpness.addNotifier(setScaler_sharpness)
			else:
				config.av.scaler_sharpness = NoSave(ConfigNothing())

		if VideoEnhancement.firstRun:
			self.setConfiguredValues()

		VideoEnhancement.firstRun = False

	def setConfiguredValues(self):
		try:
			print "--> applying psi values"
			open("/proc/stb/video/plane/psi_apply", "w").write("1")
		except IOError:
			print "couldn't apply psi values."

VideoEnhancement()
