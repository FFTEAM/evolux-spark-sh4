/*	yWeb by yjogol
 * 	Helper functions which requires prototype
	$Date: 2007-11-26 22:13:07 $
	$Revision: 1.23 $
*/

if(yweb)
	yweb.require_prototype('1.6',true);

function split_version(vstring,v){
	var l=vstring.split(".");
	v.set('major', (l.length>0)?l[0]:"0");
	v.set('minor', (l.length>1)?l[1]:"0");
	v.set('patch', (l.length>2)?l[2]:"0");
	v.set('pre', (l.length>3)?l[3]:"0");
}
function version_le(l, r) /* l<= r?*/{
	return 	(l.get('major') < r.get('major'))||
		((l.get('major') == r.get('major')) && (l.get('minor') < r.get('minor'))) ||
		((l.get('major') == r.get('major')) && (l.get('minor') == r.get('minor')) && (l.get('patch') < r.get('patch'))) ||
		((l.get('major') == r.get('major')) && (l.get('minor') == r.get('minor')) && (l.get('patch') == r.get('patch')) && (l.get('pre') <= r.get('pre')));
}
