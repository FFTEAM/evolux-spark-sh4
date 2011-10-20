from enigma import eServiceReference, iServiceInformation
import EVOMC_ShoutcastTimeoutSocket
from Screens.MessageBox import MessageBox
import urllib, socket

class ShoutcastPlayer:
	is_playing = False

	def __init__(self, session, args=0):
	    print "[shoutcastRadio.StreamPlayer] init StreamPlayer"
	    self.is_playing = False
	    self.session = session
	    self.oldService = self.session.nav.getCurrentlyPlayingServiceReference()
	    self.session.nav.event.append(self.__event)
	    self.metadatachangelisteners = []

	def __event(self, ev):
	    print "[shoutcastRadio.StreamPlayer] EVENT ==>", ev
	    if ev == 5: # can we use a constant here instead of just 5?
   			currentServiceRef = self.session.nav.getCurrentService()
   			if currentServiceRef is not None:
   				#it seems, that only Title is avaible for now
   				sTagTitle = currentServiceRef.info().getInfoString(iServiceInformation.sTagTitle)
				self._onMetadataChanged(sTagTitle)

	def _onMetadataChanged(self,title):
		for i in self.metadatachangelisteners:
			i(title)

	def play(self, stream):

		if self.is_playing:
			self.stop()
		stream.getURL(self._playURL)


	def _playURL(self, url=None):
	    EVOMC_ShoutcastTimeoutSocket.setDefaultSocketTimeout(10)
	    urlopener = urllib.URLopener() 

	    if not url:
	        print "no URL provided for play"
	        return
	    try:
	        con = urlopener.open(url)
	        print "[shoutcastRadio.StreamPlayer] playing stream", url
	        esref = eServiceReference("4097:0:0:0:0:0:0:0:0:0:%s" % url.replace(':', '%3a'))
	        try:
	            self.session.nav.playService(esref)
	            self.is_playing = True
	        except:
	            print "[shoutcastRadio.StreamPlayer] Could not play %s" % esref

	    except EVOMC_ShoutcastTimeoutSocket.Timeout:
	        self.session.open(MessageBox,_("Verbindungsfehler, Timout\n Zeitueberschreitung bei Vebindung"), MessageBox.TYPE_ERROR)
	    except IOError, e:
	        text = "Seite nicht erreichbar:\n" +str(e) +"\n(Adresse: "+url+" )"
	        self.session.open(MessageBox,_(text), MessageBox.TYPE_ERROR)
	    except:
	        self.session.open(MessageBox,_("anderer Fehler"), MessageBox.TYPE_ERROR)


	def stop(self, text=""):
	    if self.is_playing:
	        print "[shoutcastRadio.StreamPlayer] stop streaming", text
	        try:
	            self.is_playing = False
	            self.session.nav.stopService()
	            self.session.nav.playService(self.oldService)
	            self.exit()
	        except TypeError, e:
	            print " ERROR Z", e
	            self.exit()

	def exit(self):
	    self.stop()

