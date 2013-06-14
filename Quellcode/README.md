
##IRC-BOT##

	Bot - IRC Bot baut eine Verbindung mit IRC server
        

##SYNOPSIS

	Bot server port nick channel

##DESCRIPTION

- Dieser Bot starten eine Socket-Client-Verbindung. <br />
- port -und server-Parameter werden übergeben. <br />
- Mit nick und channel wird identifiziert. <br />

Der Bot reagiert auf folgende Meldungen: <br />
<pre><code> 	
   print            <== database auszugeben <br />
   start            <== loggin zu starten  <br />
   stop	            <== loggin zu stopen <br />
   lastseen [name]  <== letzte Nachricht von [name] <br />
   join [channel]   <== channel verbinden  <br />
   leave [channel]  <== channel verlassen <br />
   Botname: xxx	    <== eine einfache Nachricht ausgeben <br />
   name [name]	    <== aendert nickname zu [name] <br />
   exit		    <== irc bot auszuschalten <br />
</code></pre>


####AUTHOR

	Arkadij Doronin m1001200@fh-heidelberg.de 