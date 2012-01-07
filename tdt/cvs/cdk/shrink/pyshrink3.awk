{
 l=match($0,/[[:graph:]]/)
 la[l]=l
 i++
 fa[i]=$0
}

END {
 for(i=1; i<length(la); i++)
 {
  if(la[i] != "")
  {
   y++
   la[i]=y
  }
 }

 for(i=1; i<length(fa)+1; i++)
 {
  l=match(fa[i],/[[:graph:]]/)
  l=la[l];l--
  ls=""
  for(y=0; y<l; y++) {ls=ls" "}
  sub(/^[ ]*/,ls,fa[i])
  print fa[i]
 }
}
