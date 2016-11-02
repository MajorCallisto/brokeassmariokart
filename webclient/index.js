"use strict";
const  fs    = require('fs'),
        http = require("http");
var port = 3333;
http.createServer(function (request, response) {
  fs.createReadStream("index.html").pipe(response);
}).listen(port);

console.log('Listening on port '+port);