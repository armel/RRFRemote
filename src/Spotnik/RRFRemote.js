const http = require('http');
const port = 3000;

const requestHandler = (request, response) => {
  console.log(request.url);
  const input = new URL('https://localhost/' + request.url);
  const dtmf = input.searchParams.get('dtmf');
  const { exec } = require('child_process');

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
}

const server = http.createServer(requestHandler);

server.listen(port, (error) => {
  if (error) {
    return console.log('something bad happened', error)
  }
  console.log(`server is listening on ${port}`)
});