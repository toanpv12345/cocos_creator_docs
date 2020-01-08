# **Thiết lập SSL cho Http và Websocket**

## **Http**

Với các thiết bị Android có phiên bản SDK < 24 sẽ gặp lỗi kết nối đến https nếu không cài đặt chứng chỉ `ROOT CA`.

### **Thêm vào file AppDelegate.cpp**

```
xmlhttprequest js -> jsb_xmlhttprequest -> HttpClient-Android cpp -> Cocos2dxHttpURLConnection java
```

* Cài đặt hàm `installCert` cho js gọi C++.
* Sao chép file chứng chỉ vào storage (Do hàm `setVerifySSL` của class `Cocos2dxHttpURLConnection` bị thiếu ký tự `@` tại lệnh `String assetString = "assets/";` nên lấy thừa ký tự `/` và không đọc được file từ thư mục `assets`, có thể sửa thành `String assetString = "@assets/";` để tránh code sao chép chứng chỉ dài dòng, tuy nhiên cách này có nhược điểm là phải can thiệp vào core của Cocos Creator).
* Gọi hàm `setSSLVerification` của `HttpClient` để chỉ đường dẫn chứng chỉ trước khi thực hiện request.

```cpp
...
USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/network/HttpClient.h"
using namespace cocos2d::network;

static std::string getFileNameForPath(const std::string& filePath)
{
    std::string fileName = filePath;
    const size_t lastSlashIdx = fileName.find_last_of("\\/");
    if (std::string::npos != lastSlashIdx)
    {
        fileName.erase(0, lastSlashIdx + 1);
    }
    return fileName;
}

static bool install_cert(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc >= 1) {
        bool ok = false;
        std::string caFilePath;
        ok = seval_to_std_string(args[0], &caFilePath);
        SE_PRECONDITION2(ok, false, "args[0] isn't a string.");

        // Copy certificate
        auto fileUtils = cocos2d::FileUtils::getInstance();

        std::string writablePath = fileUtils->getWritablePath();
        std::string caFileName = getFileNameForPath(caFilePath);
        std::string newCaFilePath = writablePath + caFileName;

        if (fileUtils->isFileExist(newCaFilePath))
        {
            HttpClient::getInstance()->setSSLVerification(newCaFilePath);
        }
        else
        {
            if (fileUtils->isFileExist(caFilePath))
            {
                std::string fullPath = fileUtils->fullPathForFilename(caFilePath);
                if (fullPath[0] != '/')
                {
                    auto caData = fileUtils->getDataFromFile(fullPath);
                    if (!caData.isNull())
                    {
                        FILE* fp = fopen(newCaFilePath.c_str(), "wb");
                        if (fp != nullptr)
                        {
                            fwrite(caData.getBytes(), caData.getSize(), 1, fp);
                            fclose(fp);
                            
                            HttpClient::getInstance()->setSSLVerification(newCaFilePath);
                        }
                    }
                }
            }
        }

        return true;
    }
    
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting >=1", (int)argc);
    return false;
}
SE_BIND_FUNC(install_cert)

bool jsb_register_install_cert(se::Object* global)
{
    global->defineFunction("installCert", _SE(install_cert));
    return true;
}

#endif
...

bool AppDelegate::applicationDidFinishLaunching()
{
    ...
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    se->addRegisterCallback(jsb_register_install_cert);
#endif

    se->start();
}
```

### **Tại nơi gọi https request**

```js
...
var xhr = new XMLHttpRequest();

if (cc.sys.os == cc.sys.OS_ANDROID) {
    installCert(cc.url.raw('resources/ca.cer'));
}
...
```

*Lưu ý:*
* _Kết nối https trên Android đường dẫn không được chứa ký tự gạch dưới `_`_
* _Chứng chỉ `ROOT CA` phải là định dạng nhị phân (file `.cer`)._
* Giải pháp này chỉ có hiệu lực với các request khởi tạo từ `XMLHttpRequest`, không hoạt động với cơ chế hotupdate.

## **Websocket**

Trên Windows và Android bị lỗi kết nối websocket có ssl cần thiết lập chứng chỉ `ROOT CA` như sau:

```js
var webSocket = null;
if (cc.sys.isNative && cc.sys.os != cc.sys.OS_IOS) {
    var pemUrl = cc.url.raw('resources/cacert.pem');
    if (cc.loader.md5Pipe) {
        pemUrl = cc.loader.md5Pipe.transformURL(pemUrl)
    }
    console.log(pemUrl);
    webSocket = new WebSocket(url, null, pemUrl);
} else {
    webSocket = new WebSocket(url);
}
```

Ở đây file chứng chỉ `cacert.pem` đặt tại thư mục resources.

_Lưu ý: File chứng chỉ `ROOT CA` định dạng Base64 (file `.pem`)_

## **SignalR**

Sử dụng bản `signalr.js` chưa minified để sửa code. Tìm dòng `webSocket = new WebSocket(url)` để thay đổi như code trong phần Websocket đã mô tả.

_Lưu ý:_
* Để kết nối websocket ssl được http cũng cần có ssl.
* Do định dạng chứng chỉ cho https của Android là file `.cer`, của websocket cho Android và Windows là file `.pem` nên cần 2 file chứng chỉ cho project và phải đặt tên khác nhau để Cocos Creator phân biệt.

## **Tài nguyên**

* [AppDelegate.cpp](./src/AppDelegate.cpp)
* [signalr.js](./src/signalr.js)
* [cacert.pem](./file/cacert.pem)

