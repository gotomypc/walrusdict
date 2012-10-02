var http = require('http'), url = require('url');
var ref = require('ref');
var ffi = require('ffi');
var Iconv= require('iconv').Iconv;
var iconv = new Iconv('ISO-8859-1','UTF-8');
var exec = require('child_process').exec;

var data = ref.types.void;
var dataPtr = ref.refType(data);
var stringPtr = ref.refType(ref.types.CString);

var libntree = ffi.Library('./libntree', {
    '_Z4initv': [ dataPtr, [] ],
    '_Z4loadP5NTreePcS1_': [ 'void', [ dataPtr, 'string', 'string' ] ],
    '_Z5queryP5NTreePcS1_': [ 'void', [dataPtr, 'string', stringPtr ] ],
    '_Z9full_sortP5NTree': [ 'void', [dataPtr] ]
});

var dbPtr = libntree._Z4initv()
var resPtr = ref.allocCString((new Buffer(24000).toString()));

/* with those 6 dicts and depth == 8 expect 1GB ram usage*/


libntree._Z4loadP5NTreePcS1_(dbPtr, "DE-IT","dicts/DE-IT-8859.txt");
libntree._Z4loadP5NTreePcS1_(dbPtr, "IT-DE","dicts/IT-DE-8859.txt");
libntree._Z4loadP5NTreePcS1_(dbPtr, "IT-EN","dicts/IT-EN-8859.txt");
libntree._Z4loadP5NTreePcS1_(dbPtr, "EN-IT","dicts/EN-IT-8859.txt");
libntree._Z4loadP5NTreePcS1_(dbPtr, "EN-DE","dicts/EN-DE-8859.txt");
libntree._Z4loadP5NTreePcS1_(dbPtr, "DE-EN","dicts/DE-EN-8859.txt");

libntree._Z9full_sortP5NTree(dbPtr);

var sanitize = function(s) {
    return String(s).replace(/(["!'$`\\])/g,'');
};

query = function(req, res) {
    /* search into the loaded dictionaries */
    res.writeHead(200, { 'Content-Type': 'text/json'});

    var val = req.params.query.val;
    if (!val) {
	res.end("[]");
	return;
    }

    libntree._Z5queryP5NTreePcS1_(dbPtr, req.params.query.val, resPtr);
    res.end(ref.readCString(resPtr,0));
}

ankipush = function(req, res) {
    /* push a card to the configured anki deck */
    res.writeHead(200, { 'Content-Type': 'text/json'});
    

    var front = req.params.query.front;
    var back = req.params.query.back;
    var deck = req.params.query.deck || "walrus";

    if (front === undefined || back === undefined) {	
	res.end('["error"]');
	return;
    }

    console.log("anki: " + sanitize(front) + " - " + sanitize(back));
    /* execute anki script */
    child = exec('python anki/rogueclient.py -D "' + sanitize(deck) + '" -F "' + sanitize(front) + '" -B "' + sanitize(back) + '"', function (error, stdout, stderr) {
	if (error !== null) {
	    console.log('exec error: ' + error);
	    console.log('stdout: ' + stdout);
	    console.log('stderr: ' + stderr);
	    res.end('["error"]');
	} else {
	    res.end('["card added"]');
	}
    });
}

ankisync = function(req, res) {
    /* sync anki decks */
    res.writeHead(200, { 'Content-Type': 'text/json'});
    var deck = req.params.query.deck || "walrus";

    console.log("syncing");
    /* execute anki script */
    child = exec('python anki/rogueclient.py -s -D "' + sanitize(deck) + '"',
		 function (error, stdout, stderr) {
		     if (error !== null) {
			 console.log('exec error: ' + error);
			 console.log('stdout: ' + stdout);
			 console.log('stderr: ' + stderr);
			 res.end('["sync error"]');
		     } else {			 
			 res.end('["synced"]');
		     }
		 });    
}

fail = function(req ,res) {
    console.log('in query');
    res.writeHead(404, { 'Content-Type': 'text/json'});
    res.end('["error" : "404 - Not Found"]');
}

var actions = {
    '/query': query,
    '/push': ankipush,
    '/sync': ankisync
}

var port = process.argv[2] || "3000";
console.log("all dicts loaded. starting service on port " + port + ".");

http.createServer(function(req,res) {
    req.params = url.parse(req.url, true);
    console.log(req.url);
    console.log(req.params);
    selected = actions[req.params.pathname] || fail;
    selected(req,res);
}).listen(port);
