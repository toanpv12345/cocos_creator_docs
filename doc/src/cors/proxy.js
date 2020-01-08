'use strict';

const https = require('https');
const http = require('http');
const url = require('url');
const port = process.env.PORT || 2000;

let onRequest = async (request, response) => {
    const headers = {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'OPTIONS, POST, GET',
        'Access-Control-Max-Age': 2592000, // 30 days
    };

    if (request.method == 'OPTIONS') {
        response.writeHead(204, headers);
        response.end();
        return;
    }

    if (['GET', 'POST'].indexOf(request.method) > -1) {
        console.log(`Request to: ${request.url}`);
        if (request.url == '/favicon.ico') {
            return;
        }
        
        let query = url.parse(request.url, true).query;

        let oURL = new url.URL(query.url);
        const options = {
            hostname: oURL.hostname,
            port: 443,
            path: oURL.pathname,
            method: request.method
        };

        const proxy = https.request(options, (res) => {
            response.writeHead(res.statusCode, headers);
            res.pipe(response, {
                end: true
            });
        });

        request.pipe(proxy, {
            end: true
        });
        return;
    }

    response.writeHead(405, headers);
    response.end(`${request.method} is not allowed for the request.`);
}

http.createServer(onRequest).listen(port);