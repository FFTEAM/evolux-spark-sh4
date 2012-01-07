BEGIN {
auf=0
ausgabe=1
}

/'''/ {
 if(match($0,/^[ 	]*'''/) != 0 && auf == 0)
 {
  t=$0
  gsub(/'''.*/, "", t)
  if(split($0, a ,/'''/) != 3) {print t"''"}
  ausgabe=0
 }
 if(auf == 1) {auf=0} else {auf=1}
 if(split($0, a ,/'''/) == 3) {auf=0;ausgabe=1}
}

/^[ 	]*'''.*'''[ 	]*$/ {
 if(auf == 0)
 {
  ausgabe=0
  t=$0
  gsub(/'''.*/, "", t)
  print t"''"
 }
}

{
 if(ausgabe == 1) {print $0}
 if(auf == 0) {ausgabe=1}
}
