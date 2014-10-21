var util = require('util');
var SerialPort = require("serialport").SerialPort;
var express = require('express');
var app = express();
var http = require('http');
var path = require('path');
var readline = require('readline');


var config = {};
//Whatever port the bluetooth is on
config.comPort = "COM10";
config.baudrate = 38400;
config.databits = 8;
config.MaxArduinoAttempts = 3;
config.active_powerup = false;
config.active_invincibilty = false;
config.length_powerup = 10000;
config.length_invincibility = 15000;
config.setSpeedInterval = 0;
config.invincibleClientId = -1;

var rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

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
		obj.m1 = map(obj.m1, -127,127);
		obj.m2 = map(obj.m2, -127,127);
		
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

var arduino;
var arduino_openattempts;
initArduino();
function initArduino(){
	arduino = new SerialPort(config.comPort, {
	  baudrate: config.baudrate,
	  databits: config.databits
	}, false); // this is the openImmediately flag [default is true]

	arduino_openattempts = 0;
	
	arduino.on('error', function(data){
		arduino_openattempts++
		if (arduino_openattempts<config.MaxArduinoAttempts){
			console.log("Arduino " + data);
		
			setTimeout(openArduino, 2000);
		}else{
			console.log("Unable to open connection. Please reset bluesmirf");	
			config.setSpeedInterval = setInterval(calculateAverage, 250);
			questionPromptCommands();
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
			questionPromptCommands();
		});
	}
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
				average_m1 = clients[i].motor_speed.m1;
				average_m2 = clients[i].motor_speed.m1;
				break;	
			}
			average_m1 += clients[i].motor_speed.m1;
			average_m2 += clients[i].motor_speed.m2;
		}
		
		if (config.active_invincibilty ==false){
			average_m1 /= -clients.length;
			average_m2 /= -clients.length;
		}
		if (isNaN(average_m1)){
			average_m1 = 0;	
		}
		if (isNaN(average_m2)){
			average_m2 = 0;	
		}
		var motor_command = "{\"m1\":"+average_m1+",\"m2\":"+average_m2+"}";
		sendHardwareMSG(motor_command);
//		console.log(motor_command);
		motor_command = motor_command.replace("}", ", \"collective_stats\":true, \"numberOfClients\":\""+clients.length+"\"}");
		io.sockets.emit("brokeass-message-client", motor_command);
//		console.log(motor_command);
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
	console.log("killInvincibility");
	config.invincibleClientId = "";
	config.active_invincibilty = false;	
	io.sockets.emit("brokeass-message-client", "{\"end_invincibility\":"+true+"}");
	setTimeout(function(){io.sockets.emit("brokeass-message-client", "{\"end_invincibility\":"+true+"}");}, 500);
	sendHardwareMSG("{\"i_f\":}");
}

function questionPromptCommands(){
	rl.question("Enter a command [u] [s] [i] [?]:", function(answer) {
		answer = answer.toLowerCase();
		command = answer.charAt(0);
		console.log(command);
		switch(command){
			case "u":
				var port = answer.split(" ");
				console.log(port[1]);
				if (port.length > 1){
					config.comPort = port[1];
					initArduino();
				}
				break;
			case "s":
				break;
			case "i":
				initPowerup();
				questionPromptCommands();
				break;
			case "?":
				console.log("Here are the commands:");
				console.log("u: updateComPort. Specify a port name. e.g. u COM10");
				console.log("s: stopAllMotors.");
				console.log("i: invincibleMode.");
				break;
			default:
				console.log("\n\nI don't know what that means\n\n");
				questionPromptCommands();
				break;
		}
	});
}