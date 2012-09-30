var http = require('http'), url = require('url');
var ref = require('ref');
var ffi = require('ffi');
var Iconv= require('iconv').Iconv;
var iconv = new Iconv('ISO-8859-1','UTF-8');

var data = ref.types.void;
var dataPtr = ref.refType(data);
var stringPtr = ref.refType(ref.types.CString);

var libntree = ffi.Library('./libntree', {
    '_Z4initv': [ dataPtr, [] ],
    '_Z4loadP5NTreePcS1_': [ 'void', [ dataPtr, 'string', 'string' ] ],
    '_Z5queryP5NTreePcS1_': [ 'void', [dataPtr, 'string', stringPtr ] ]
});

var dbPtr = libntree._Z4initv()
var resPtr = ref.allocCString((new Buffer(8192).toString()));
libntree._Z4loadP5NTreePcS1_(dbPtr, "IT-EN","IT-EN-8859.txt")

query = function(req, res) {
    res.writeHead(200, { 'Content-Type': 'text/json'});

    var val = req.params.query.val;
    if (!val || val.length < 4) {
	res.end("[]");
	return;
    }

    libntree._Z5queryP5NTreePcS1_(dbPtr, req.params.query.val, resPtr);
//    res.end(iconv.aconvert(ref.readCString(resPtr,0),'utf8').toString());
    console.log(resPtr.toString());
    res.end(ref.readCString(resPtr,0));
}

fail = function(req ,res) {
    console.log('in query');
    res.writeHead(404, { 'Content-Type': 'text/json'});
    res.end('["error" : "404 - Not Found"]');
}

var actions = {
    '/query': query    
}

http.createServer(function(req,res) {
    req.params = url.parse(req.url, true);
    console.log(req.url);
    console.log(req.params);
    selected = actions[req.params.pathname] || fail;
    selected(req,res);
}).listen(3000);