// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

const http = require('http');
const fs = require('fs').promises;
const port = 3000;
const version = 0.6;

const room = {
    "default": "95, PERROQUET",
    "rrf": "96, RRF",
    "fon": "97, FON",
    "tec": "98, TECHNIQUE",
    "int": "99, INTERNATIONAL",
    "bav": "100, BAVARDAGE",
    "loc": "101, LOCAL",
    "exp": "102, EXPERIMENTAL",
    "idf": "104, IDF",
};

const qsy = {
    95: "/etc/spotnik/restart.default",
    96: "/etc/spotnik/restart.rrf",
    97: "/etc/spotnik/restart.fon",
    98: "/etc/spotnik/restart.tec",
    99: "/etc/spotnik/restart.int",
    100: "/etc/spotnik/restart.bav",
    101: "/etc/spotnik/restart.loc",
    102: "/etc/spotnik/restart.exp",
    104: "/etc/spotnik/restart.idf",
};

let cachedTemp = null;
let cachedRaptorStatus = null;
let cacheTimestamp = 0;

const CACHE_DURATION = 5000; // Cache data for 5 seconds

// Function to get temperature and Raptor status with caching
async function getStatus() {
    const now = Date.now();
    if (now - cacheTimestamp < CACHE_DURATION && cachedTemp && cachedRaptorStatus) {
        return { temp: cachedTemp, raptor: cachedRaptorStatus };
    }

    try {
        const temp = (await fs.readFile('/sys/class/thermal/thermal_zone0/temp', 'utf8')).trim();
        const raptorStatus = await fs.readFile('/tmp/RRFRaptor_status.tcl', 'utf8');
        cachedTemp = temp;
        cachedRaptorStatus = raptorStatus.includes("ON") ? "ON" : "OFF";
        cacheTimestamp = now;
    } catch (error) {
        cachedTemp = "0";
        cachedRaptorStatus = "OFF";
    }

    return { temp: cachedTemp, raptor: cachedRaptorStatus };
}

const requestHandler = async (request, response) => {
    console.log(request.url);
    const { exec } = require('child_process');
    const input = new URL('https://localhost/' + request.url);
    const cmd = parseInt(input.searchParams.get('cmd'));

    if (cmd === 2000) {
        exec('reboot', handleExecResponse(response, 'Action A done'));
    } else if (cmd === 2001) {
        exec('echo "93#" > /tmp/dtmf_uhf', handleExecResponse(response, 'Action B done'));
    } else if (cmd === 2002) {
        exec('echo "201#" > /tmp/dtmf_uhf', handleExecResponse(response, 'Action C done'));
    } else if (cmd === 2003) {
        response.writeHead(200);
        response.end('Action D done');
    } else if (cmd > 0 && cmd < 1000) {
        const action = qsy[cmd] || `echo "${cmd}#" > /tmp/dtmf_uhf`;
        exec(action);
        response.writeHead(200);
        response.end('QSY done');
    } else {
        const { temp, raptor } = await getStatus();
        try {
            const network = (await fs.readFile('/etc/spotnik/network', 'utf8')).trim();
            const roomStatus = room[network] || "0, UNDEFINED";
            response.writeHead(200);
            response.end(`${roomStatus}, ${raptor}, ${temp}`);
        } catch (error) {
            response.writeHead(500);
            response.end("Error reading network configuration");
        }
    }
};

// Helper to handle exec responses
function handleExecResponse(response, successMessage) {
    return (error, stdout, stderr) => {
        if (error || stderr) {
            response.writeHead(500);
            response.end("Command execution failed");
        } else {
            response.writeHead(200);
            response.end(successMessage);
        }
    };
}

const server = http.createServer(requestHandler);

server.listen(port, (error) => {
    if (error) {
        return console.log('Something bad happened', error);
    }
    console.log(`Server is listening on port ${port}`);
});
