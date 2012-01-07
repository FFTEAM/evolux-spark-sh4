ARCHIVEPATH=/devel/E2/release1139

#list=`find $ARCHIVEPATH`

rm x.txt

for i in $list
do
 type=`file $i`
 echo $type | grep "timezone data$" >/dev/null
 if [ $? -eq 0 ]
 then
  echo $i >> x.txt
 fi
done

find $ARCHIVEPATH -name "*.py" -print >> x.txt

list=`cat x.txt`

for i in $list
do
 #file=`cat $i`

 # tr -d "\r"			-> lösche alle \r
 # sed s/"^\s*"//		-> lösche alle führenden Leerzeichen, Tabs ,...
 # sed s/"\s*$"//		-> lösche alle endenden Leerzeichen, Tabs ,...
 # sed /^$/d			-> lösche alle Leerzeilen
 # sed '/<!--/,/-->/ {/.*/d}'	-> lösche alles zwischen <!-- und -->
 # sed  sed '2,$ {/^#.*/d}'	-> lösche alle Zeilen die mit # beginnen, ausser erste
 # sed /^#.*/d			-> lösche alle Zeilen die mit # beginnen
 # sed s/" #.*"//g		-> lösche alles hinter einen leerzeichen# inkl. leerzeichen#
 # sed /"^\s$"/d		-> löscht alle Zeile die nur Leerzeichen, Tabs, ... enthalten

 #echo -E "$file" | tr -d "\r" | sed s/"^\s*"// | sed s/"\s*$"// | sed s/#.*//g | sed /^$/d | sed '/<!--/,/-->/ {/.*/d}' | sed '2,$ {/^#.*/d}'

 echo $i
 cat $i | tr -d "\r" | sed s/"\s*$"// | sed /^$/d | sed '/<!--/,/-->/ {/.*/d}' | sed /^#.*/d | sed /"^\s$"/d > $i.tmp
 mv $i.tmp $i
 

done

#alle dasdha #sdhashdask
# if this == "#": Problem
# svnver = '  #' + svnver
# raise SkinError("color '%s' must be #aarrggbb or valid named color" % (str) Problem
#__all__ = [
#    # public symbols
#    "Comment",
#    "dump",
#    "Element", "ElementTree",
#    "fromstring",


#/home/nit/p/e2ro/usr/share/zoneinfo/CET: timezone data
#/home/nit/p/e2ro/lib/lsb/splash-functions: ASCII text
#/home/nit/p/e2ro/lib/lsb/init-functions: ASCII English text
#/home/nit/p/e2ro/etc/fonts/fonts.conf: XML
#/home/nit/p/e2ro/etc/fonts/fonts.dtd: exported SGML document text
#/home/nit/p/e2ro/etc/init.d/portmap: POSIX shell script text executable
