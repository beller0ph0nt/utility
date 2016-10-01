var _paq = _paq || [];
(function(){

_paq.push(['setSiteId', 1]);
_paq.push(['setTrackerUrl', '/piwik/piwik.php']);
_paq.push(['setRequestMethod', 'POST']);
_paq.push(['trackPageView']);
_paq.push(['enableLinkTracking']);
if(document.cookie)
{
	var re = new RegExp('bm-wikiPiwikUser=([^;]+)');
	var piwik_user = re.exec(document.cookie);
	if(piwik_user!=null)
		_paq.push(['setCustomVariable',1,'User',unescape(piwik_user[1]),'visit']);
}

var d=document, g=d.createElement('script');
g.type = 'text/javascript';
g.defer= true;
g.async= true;
g.src  = '/piwik/async.js';

s=d.getElementsByTagName('script')[0];
s.parentNode.insertBefore(g,s);

})();
