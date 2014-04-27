//npm install -g serialport
//doesn't work on my system
//I use https://github.com/voodootikigod/node-serialport
//Need to install gyp first: npm install -g gyp
//It seems to fuckup without visual studio
//I had to install Visual Studio Express - 2013
//Ultimately, I had to download from the git, copy the files into the folder and manually isntall
var util = require('util');
var SerialPort = require("serialport").SerialPort;
var express = require('express');
var app = express();
var http = require('http');
var path = require('path');


var config = {};
//Whatever port the bluetooth is on
config.comPort = "COM10";
config.baudrate = 115200;
config.databits = 8;
config.MaxArduinoAttempts = 3;
config.active_powerup = false;
config.active_invincibilty = false;
config.length_powerup = 10000;
config.length_invincibility = 15000;
config.setSpeedInterval = 0;
config.invincibleClientId = -1;

console.log("Broke-Ass Mariokart Node Application");

//all environments
app.set('port', process.env.PORT || 3000);
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

var server = http.createServer(app);
server.listen(app.get('port'), function(){
  console.log('Express server listening on port ' + app.get('port'));
});


var io = require('socket.io').listen(server, {log:false});
//console.log(io);
io.sockets.on('connection', function (socket) {
	socket.motor_speed = {"m1":0, "m2":0};
	socket.username = "";
	socket.strike = 0;
	socket.emit("brokeass-message-client", "connected");
	socket.on('brokeass-message', function (data){
		var dataStr = String(data);
		var obj = JSON.parse(dataStr);
		if (dataStr.indexOf("caught_powerup") != -1){
			console.log("config.active_powerup",config.active_powerup);
			if (config.active_powerup == true){
				config.active_powerup = false;
				config.active_invincibilty = true;
				config.invincibleClientId = socket.id;
				
				setTimeout(killInvincibility, config.length_invincibility);
				if (obj["username"]){
					socket.broadcast.emit("brokeass-message-client", "{\"missed_starman\":true, \"username\":\""+obj.username+"\"}");
				}else {
					socket.broadcast.emit("brokeass-message-client", "{\"missed_starman\":true, \"username\":\"\"}");
				}
				sendHardwareMSG("{\"i_t\":}");
				return;
			}else {
				socket_strike(socket);
				return;
			}
		}
		if (dataStr.indexOf("m1") == -1||dataStr.indexOf("m2") == -1){
			//Strike!
			socket_strike(socket);
			return;
		}
		obj.m1 = parseInt(obj.m1);
		obj.m2 = parseInt(obj.m2);
		if (isNaN(obj.m1) || isNaN(obj.m2)){
			//Strike!
			socket_strike(socket);
			return;
		}
		obj.m1 = map(obj.m1, -255,255);
		obj.m2 = map(obj.m2, -255,255);
		
		socket.username = String(obj.username);
		
		socket.motor_speed = obj;
		//var clients = io.sockets.clients();
		//sendHardwareMSG(data);
		//socket.broadcast.emit('amexMSG', data);
	});
	socket.on('disconnect', function() {
		var clients = io.sockets.clients();
		//console.log(clients.length);
		//console.log(clients);
		//console.log(util.inspect(clients, false, null));
	});
});
function socket_strike($socket){
	$socket.strike ++;

	if ($socket.strike > 2){
		$socket.emit("brokeass-message-client", "adequate_attempts");
		$socket.disconnect();
	}
}
function map($value, $min, $max){
	return Math.min(Math.max(parseInt($value), $min), $max);	
}

var arduino = new SerialPort(config.comPort, {
  baudrate: config.baudrate,
  databits: config.databits
}, false); // this is the openImmediately flag [default is true]

var arduino_openattempts = 0;

arduino.on('error', function(data){
	arduino_openattempts++
	if (arduino_openattempts<config.MaxArduinoAttempts){
		console.log("Arduino " + data);
	
		setTimeout(openArduino, 2000);
	}else{
		console.log("Unable to open connection. Please reset bluesmirf");	
		config.setSpeedInterval = setInterval(calculateAverage, 250);
	}
});
openArduino();
function openArduino(){
	
	arduino.open(function () {
	console.log('Arduino open');
	config.setSpeedInterval = setInterval(calculateAverage, 250);
	
	arduino_openattempts = 0;
	arduino.on('data', function(data) {
		console.log('data received: ' + data);
	});
	/*
	arduino.write("ls\n", function(err, results) {
	if (err != undefined){
		console.log('err ' + err);
	}
	if (results != undefined){
		console.log('results ' + results);
	}
	});
	*/
	});
}
function sendHardwareMSG($msg){
	arduino.write($msg + "\n", function(err, results) {
		if (err != undefined){
			//console.log('err ' + err);
		}
		//socket.emit('brokeass-message-client', "SUCCESS");
		if (results != undefined){
			//console.log('results ' + results);
		}
	});
}
function calculateAverage(){
		var clients = io.sockets.clients();
		//sendHardwareMSG(data);
		var average_m1 = 0;
		var average_m2 = 0;
		for (var i = 0; i < clients.length; i ++){
			if (config.active_invincibilty ==true && config.invincibleId == clients[i].id){
				average_m1 += clients[i].motor_speed.m1;
				average_m2 += clients[i].motor_speed.m1;
				break;	
			}
			average_m1 += clients[i].motor_speed.m1;
			average_m2 += clients[i].motor_speed.m2;
		}
		
		average_m1 /= -clients.length;
		average_m2 /= -clients.length;
		if (isNaN(average_m1)){
			average_m1 = 0;	
		}
		if (isNaN(average_m2)){
			average_m2 = 0;	
		}
		var motor_command = "{\"m1\":"+average_m1+",\"m2\":"+average_m2+"}";
		sendHardwareMSG(motor_command);
		motor_command = motor_command.replace("}", ", \"collective_stats\":true, \"numberOfClients\":\""+clients.length+"\"}");
		io.sockets.emit("brokeass-message-client", motor_command);
}
setInterval(initPowerup, 60000);
setTimeout(function(){sendHardwareMSG("{\"i_t\":}");}, 1000);
function initPowerup(){
	
	if (Math.random() < 0.5){
		return;	
	}
	console.log("initPowerup" + config.length_powerup);
	config.active_powerup = true;
	io.sockets.emit("brokeass-message-client", "{\"powerupMode\":"+true+"}");
	setTimeout(killPowerup, config.length_powerup);
}
function killPowerup(){
	console.log("killPowerup");
	if (config.active_powerup == false){
		return;	
	}
	config.active_powerup = false;	
	io.sockets.emit("brokeass-message-client", "{\"killPowerupMode\":"+true+"}");
	setTimeout(function(){io.sockets.emit("brokeass-message-client", "{\"end_invincibility\":"+true+"}");}, 500);
}
function killInvincibility(){
	config.invincibleClientId = "";
	config.active_invincibilty = false;	
	io.sockets.emit("brokeass-message-client", "{\"end_invincibility\":"+true+"}");
	setTimeout(function(){io.sockets.emit("brokeass-message-client", "{\"end_invincibility\":"+true+"}");}, 500);
	sendHardwareMSG("{\"i_f\":}");
}
/**/