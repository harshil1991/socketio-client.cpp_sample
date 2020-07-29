'use strict';

const express     = require('express');
const app         = express();
const serverHttp  = require('http').Server(app); 
const io = require('socket.io')(serverHttp);

const port = 8081;

io.on('connection', function (socket) {   
    socket.on('message', function (data) {
        console.log("key received!!!" + data);
	socket.emit('server', 'hello socket io');
        console.log("sent server msg");
    });
});

serverHttp.listen(port, function() {  
    console.log("init!!!");    
});
