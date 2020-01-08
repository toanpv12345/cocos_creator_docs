
# **CORS**

Lỗi `Cross-Origin Resource Sharing` xuất hiện khi truy cập tài nguyên của trang web nằm ở một nơi khác với trang web yêu cầu. Thường xuất hiện khi chạy kiểm thử trên local mà máy chủ chưa cho phép (mặc định vì bảo mật thường chỉ yêu cầu được tài nguyên từ chính trang web đó). Có 2 hướng xử lý chính: hoặc cho phép thêm domain vào `Access-Control-Allow-Origin` để phục vụ quá trình kiểm thử, hoặc tạo 1 proxy để chuyển tiếp yêu cầu và dữ liệu giữa localhost và trang web. 

Giải pháp proxy có ưu điểm là giúp người kiểm thử chủ động hơn, không cần liên hệ với nhóm phát triển và triển khai web, mặc dù cũng có hạn chế là làm chậm quá trình tải dữ liệu hơn.

Proxy cần có khả năng sau:

* Cho phép mọi yêu cầu từ các địa chỉ gọi khác nhau để quá trình kiểm thử thuận lợi
* Chuyển trả dữ liệu từ trang web cần về cho địa chỉ đã yêu cầu.

# **NodeJs Proxy**

## **Tạo webserver cho phép gọi từ nơi bất kỳ**

```js
const http = require('http');
const port = 2000;

http.createServer((req, res) => {
  const headers = {
    'Access-Control-Allow-Origin': '*',
    'Access-Control-Allow-Methods': 'OPTIONS, POST, GET',
    'Access-Control-Max-Age': 2592000, // 30 days
    /** add other headers as per requirement */
  };

  if (req.method === 'OPTIONS') {
    res.writeHead(204, headers);
    res.end();
    return;
  }

  if (['GET', 'POST'].indexOf(req.method) > -1) {
    res.writeHead(200, headers);
    res.end('Hello World');
    return;
  }

  res.writeHead(405, headers);
  res.end(`${req.method} is not allowed for the request.`);
}).listen(port);
```

## **Chuyển dữ liệu từ đích đến nơi yêu cầu**

Ví dụ trang đích là `www.google.com`

```js
var http = require('http');

function onRequest(request, response) {

    console.log('serve: ' + request.url);

    var options = {
        hostname: 'www.google.com',
        port: 80,
        path: request.url,
        method: request.method,
        headers: request.headers
    };

    var proxy = http.request(options, function (res) {
        response.writeHead(res.statusCode, res.headers)
        res.pipe(response, {
            end: true
        });
    });

    request.pipe(proxy, {
        end: true
    });
}

http.createServer(onRequest).listen(3000);
```

## **Kết hợp 2 mã trên**

Dưới đây là mã nguồn proxy chạy giao thức http, cho phép truy vấn đến web đích bằng giao thức https.

```js
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
```

**Cách sử dụng**

Giả sử ta cần truy vấn tài nguyên ở đường dẫn https://golden-gate-b73b1.web.app/1/assets/src/settings.1.js tại `http://localhost:7456/build` (đường dẫn test web-desktop của Cocos Creator)

Ta chạy ứng dụng proxy ở trên. Lúc này proxy sẽ là 1 webserver lắng nghe ở cổng 2000. 

```sh
node proxy
```

Mã js thay vì gọi `https://golden-gate-b73b1.web.app/1/assets/src/settings.1.js` ta gọi `http://localhost:2000?url=https://golden-gate-b73b1.web.app/1/assets/src/settings.1.js`

Proxy sẽ đọc đường dẫn nằm tại tham số `url` của truy vấn và thực hiện `request` tới địa chỉ đích, sau đó trả lại kết quả thông qua cơ chế `pipe` của thư viện `https`.

[^] Source code [proxy](./src/cors/proxy.js)