// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

const http = require('http');
const port = 3000;
const version = 0.4;

const requestHandler = (request, response) => {
  console.log(request.url);
  const { exec } = require('child_process');
  const input = new URL('https://localhost/' + request.url);
  const cmd = parseInt(input.searchParams.get('cmd'));

  var room = {
    "default": "95, PERROQUET",
    "rrf": "96, RRF",
    "fon": "97, FON",
    "tec": "98, TECHNIQUE",
    "int": "99, INTERNATIONAL",
    "bav": "100, BAVARDAGE",
    "loc": "101, LOCAL",
    "exp": "102, EXPERIMENTAL",
    "reg": "104, REGIONAL",
  };

  var qsy = {
    95: "/etc/spotnik/restart.default",
    96: "/etc/spotnik/restart.rrf",
    97: "/etc/spotnik/restart.fon",
    98: "/etc/spotnik/restart.tec",
    99: "/etc/spotnik/restart.int",
    100: "/etc/spotnik/restart.bav",
    101: "/etc/spotnik/restart.loc",
    102: "/etc/spotnik/restart.exp",
    104: "/etc/spotnik/restart.reg",
  };


  if (cmd == 2000) {
    exec('reboot', (error, stdout, stderr) => {
      if (error || stderr) {
        //console.log(`error: ${error.message}`);
        //console.log(`stderr: ${stderr}`);
        response.writeHead(500);
      }
      else {
        //console.log(`stdout: ${stdout}`);
        response.writeHead(200);
        response.end('Action A done');
      }
    });
  } else if (cmd == 2001) {
    exec('echo "93#" > /tmp/dtmf_uhf', (error, stdout, stderr) => {
      if (error || stderr) {
        //console.log(`error: ${error.message}`);
        //console.log(`stderr: ${stderr}`);
        response.writeHead(500);
      }
      else {
        //console.log(`stdout: ${stdout}`);
        response.writeHead(200);
        response.end('Action B done');
      }
    });
  } else if (cmd == 2002) {
    exec('echo "201#" > /tmp/dtmf_uhf', (error, stdout, stderr) => {
      if (error || stderr) {
        //console.log(`error: ${error.message}`);
        //console.log(`stderr: ${stderr}`);
        response.writeHead(500);
      }
      else {
        //console.log(`stdout: ${stdout}`);
        response.writeHead(200);
        response.end('Action C done');
      }
    });
  } else if (cmd == 2003) {
    exec('true', (error, stdout, stderr) => {
      if (error || stderr) {
        //console.log(`error: ${error.message}`);
        //console.log(`stderr: ${stderr}`);
        response.writeHead(500);
      }
      else {
        //console.log(`stdout: ${stdout}`);
        response.writeHead(200);
        response.end('Action D done');
      }
    });
  } else if (cmd > 0 && cmd < 1000) {
    var action = "";
    if (cmd in qsy) {
      action = qsy[cmd];
    }
    else {
      action = 'echo "' + cmd + '#" > /tmp/dtmf_uhf';
    }
    exec(action);
    response.writeHead(200);
    response.end('QSY done');
  } else {
    var raptor = "";
    var temp = "";

    exec('cat /tmp/RRFRaptor_status.tcl', (error, stdout, stderr) => {
      if (error || stderr) {
        raptor = "OFF";
      }
      else {
        if (stdout.indexOf("ON") !== -1) {
          raptor = "ON";
        }
        else {
          raptor = "OFF";
        }
      }
    });

    exec('cat /sys/class/thermal/thermal_zone0/temp', (error, stdout, stderr) => {
      if (error || stderr) {
        temp = "0";
      }
      else {
        temp = stdout.trim();
      }
    });

    exec('cat /etc/spotnik/network', (error, stdout, stderr) => {
      if (error || stderr) {
        //console.log(`error: ${error.message}`);
        //console.log(`stderr: ${stderr}`);
        response.writeHead(500);
      }
      else {
        //console.log(`stdout: ${stdout}`);
        response.writeHead(200);
        if (stdout.trim() in room) {
          response.end(room[stdout.trim()] + ", " + raptor + ", " + temp);
        }
        else {
          response.end("0, UNDEFINED" + ", " + raptor + ", " + temp);
        }
      }
    });
  }
}

const server = http.createServer(requestHandler);

server.listen(port, (error) => {
  if (error) {
    return console.log('something bad happened', error)
  }
  console.log(`server is listening on ${port}`)
});