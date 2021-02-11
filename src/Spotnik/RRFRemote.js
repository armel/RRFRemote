const http = require('http');
const port = 3000;

const requestHandler = (request, response) => {
  console.log(request.url);
  const input = new URL('https://localhost/' + request.url);
  const dtmf = input.searchParams.get('dtmf');
  const { exec } = require('child_process');

  var room = {"rrf":      "96, RRF", 
              "fon":      "97, FON", 
              "tec":      "98, TECHNIQUE",
              "int":      "99, INTERNATIONAL",
              "bav":      "100, BAVARDAGE",
              "loc":      "101, LOCAL",
              "exp":      "102, EXPERIMENTAL",
              "reg":      "103, REGIONAL",
              "default":  "95, PERROQUET"
            };

  if(dtmf > 0) {
    exec('echo "' + dtmf + '#" > /tmp/dtmf_uhf', (error, stdout, stderr) => {
      if (error || stderr) {
        //console.log(`error: ${error.message}`);
        //console.log(`stderr: ${stderr}`);
        response.writeHead(500);
      }
      else {
        //console.log(`stdout: ${stdout}`);
        response.writeHead(200);
        response.end('QSY done');
      }
    });
  } else {
    var raptor = "";

    exec('cat /tmp/RRFRaptor_status.tcl', (error, stdout, stderr) => {
      if (error || stderr) {
        raptor = "OFF";
      }
      else{
        if (stdout.indexOf("ON") !== -1 ) {
          raptor = "ON";
        }
        else {
          raptor = "OFF";
        }
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
        if(stdout.trim() in room) {
          response.end(room[stdout.trim()] + ", " + raptor);
        }
        else {
          response.end("0, UNDEFINED"  + ", " + raptor);
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