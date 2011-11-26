from Plugins.Extensions.WebInterface.WebChilds.Toplevel import addExternalChild
import os
if os.path.exists('/media/ba/BarryAllen/BarryAllen.py') == True:
   from Plugins.Extensions.BarryAllen.BarryAllen import BarryAllen
   addExternalChild( ("barryallen", BarryAllen()) )

