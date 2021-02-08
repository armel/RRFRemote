// content of index.js
const http = require('http')
const port = 3000

const requestHandler = (request, response) => {
    console.log(request.url)
    const input = new URL("https://localhost/" + request.url);
    const dtmf = input.searchParams.get('dtmf');
    //response.end('Hello Node.js Server!')
    const { exec } = require("child_process");

    exec('echo "' + dtmf +'#" > /tmp/dtmf_uhf', (error, stdout, stderr) => {
        if (error) {
            console.log(`error: ${error.message}`);
            return;
        }
        if (stderr) {
            console.log(`stderr: ${stderr}`);
            return;
        }
        console.log(`stdout: ${stdout}`);
    });
}

const server = http.createServer(requestHandler)

server.listen(port, (err) => {
    if (err) {
    return console.log('something bad happened', err)
    }

    console.log(`server is listening on ${port}`)
})