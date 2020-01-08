## **Fix FPS drops**

Trong quá trình chạy subpackage tải về từ hotupdate ứng dụng giảm fps chạy chậm dần, máy nóng. Sau khi loại trừ các khả năng do cơ chế download, unzip, ... của Cocos Creator thì nguyên nhân nằm ở cơ chế chạy subpackage của bản customize (chạy bản update không cần restart ứng dụng). 

Cơ chế chạy subpackage trong file `GGame.js` bao gồm việc ghép assets và chạy khởi tạo tài nguyên cũng như chạy launch scene. Loại trừ nguyên nhân do ghép assets và khởi tạo tài nguyên tại hàm `cc.AssetLibrary.init` còn lại hàm `cc.game.run()`.

Review hàm `cc.game.run()` tại [CCGame.js](https://github.com/cocos-creator/engine/blob/v2.1.1-release/cocos2d/core/CCGame.js) nhận thấy `cc.game.run() `sẽ gọi `this._runMainLoop()` như vậy mỗi lần chạy subpackage đều gọi `_runMainLoop()` trong khi ứng dụng chính đã gọi trước đó khi khởi tạo khiến tình trạng fps drops xảy ra. 

Cách khắc phục là tránh gọi lại `_runMainLoop()` bằng cách không sử dụng `cc.game.run()` mà thay bằng việc tải `project.js` sau đó khởi tạo tài nguyên từ hàm `onStart()`.

```js
cc.loader.load(jsList, function (err) {
    if (err) {
        console.log(JSON.stringify(err));
        return;
    }

    cc.game._initConfig(option);
    onStart();
});
//cc.game._prepared = false;
//cc.game.run(option, onStart);
```

Kết quả chạy thử nghiệm đã cho thấy giải pháp đáp ứng được yêu cầu.

## **Fix cache hotupdate**

### **Thảo luận**

Sau khi thực hiện hotupdate lần 1 subpackage chạy đúng yêu cầu. Lúc này ứng dụng vẫn đang hoạt động tiến hành update subpackage trên server. Khi ứng dụng thoát khỏi subpackage và chạy lại, nó sẽ thực hiện cập nhật lần nữa tuy nhiên khi chạy thì code sửa đổi không được thực thi mà vẫn là bản update lần đầu. Nếu kill app chạy lại thì lại đúng.

Do quá trình download bản cập nhật diễn ra bình thường nên khả năng creator đã cache tài nguyên. Có vài khả năng xảy ra ở đây: quá trình đọc file đã bị cache, hoặc bộ compiler js đã thực hiện cache, hoặc một phần chưa biết của creator đã thực hiện cache.

### **Phân tích**

Review source `js-bindings/jswrapper/jsc/ScriptEngine.mm` (thực hiện trên bản iOS simulator) ta thấy có 2 hàm sau:

```c++
bool ScriptEngine::runScript(const std::string& path, Value* ret/* = nullptr */)
```
và
```c++
bool ScriptEngine::evalString(
    const char* script, 
    ssize_t length/* = -1 */, 
    Value* ret/* = nullptr */, 
    const char* fileName/* = nullptr */)
```

Hàm `runScript` sẽ chạy khi `cc.loader.load` thực hiện load file js. Tuy nhiên tại lần update thứ 2 nếu đặt break point vào hàm `runScript` sẽ thấy không chạy. Lý do là creator đã cache nội dung file js. Lệnh xoá cache như sau:

```js
cc.loader.removeItem(path + '/src/' + bundledScript);
cc.loader.removeItem(path + '/src/' + settingScript);
```

Sau khi code vào `runScript`, tiến hành kiểm tra xem liệu source code có bị cache không bằng việc in nội dung của file `project.js` đã cập nhật:
```c++
if (!scriptBuffer.empty())
{
    SE_LOGE("ScriptEngine::runScript %s\n", scriptBuffer.c_str());
    return evalString(scriptBuffer.c_str(), scriptBuffer.length(), ret, path.c_str());
}
```
Kiểm tra thấy nội dung in ra là của bản cập nhật mới, do vậy ta loại trừ được khả năng lỗi do cache nội dung file.

Để kiểm tra quá trình compile của hàm `evalString` có bị cache không ta thêm vào `project.js` trên server 1 dòng log phiên bản ngay ở đầu (thêm vào file đã build, sau đó chạy lại generation version)
```js
console.log('version: 1.0.2');window.__require=...
```

Sau khi chạy update ta thấy log in ra `version: 1.0.2`, do đó compiler js không bị cache.

Theo dõi log kỹ hơn ta thấy lỗi sau xảy ra với class của nội dung cập nhật:
```
JS: [ERROR]: A Class already exists with the same __classname__ :
JS: [ERROR]: A Class already exists with the same __cid__ :
```

Như vậy quá trình thêm các class với creator đã bị lỗi trùng `name` và `id`. Ở đây các class đều được gán một giá trị `id` và khi đã được thêm nó sẽ được cache trong core creator, có thể tránh được nếu ta đổi `name` và `id` khi cập nhật (cách này quá rắc rối cho việc bảo trì) hoặc đơn giản hơn là xoá class trong bộ nhớ trước khi thêm lại.

### **Giải pháp**

Tìm kiếm trên mạng lỗi `A Class already exists with the same` ta thấy mã nguồn liên quan ở đây [cocos2d/core/platform/js.js](https://github.com/cocos-creator/engine/blob/v2.1.1-release/cocos2d/core/platform/js.js) tại hàm `function setup (key, publicName, table)`. 

Từ file `cocos2d/core/platform/js.js` ta thấy có 2 lệnh cài đặt `name` và `id` cho mỗi class:  `setup('__cid__', '_registeredClassIds', _idToClass);` và  `setup('__classname__', '_registeredClassNames', _nameToClass);`. Ta cũng thấy hàm `js.unregisterClass`. 

Ý tưởng là tìm kiếm các class đã được thêm vào lần đầu, tiếp đến lần sau ta sẽ thực hiện gỡ đăng ký các class nếu có trước khi cài đặt các class bằng hàm `js.unregisterClass`.

Review kỹ hơn ta thấy danh sách các class `id` được lưu ở biến `cc.js._registeredClassIds`, cũng có thể tiếp cận bằng cách sử dụng `name` (trong biến `cc.js._registeredClassNames`) thay cho `id` nhưng dự kiến creator sắp tới có thể cho phép trùng tên nên tiếp cận theo hướng `id` sẽ an toàn hơn. Từ danh sách này có thể so sánh trước và sau khi gọi `cc.loader.load` để tách ra những class nào đã được thêm.

Với biến `cc.js._registeredClassIds` và hàm `js.unregisterClass` ta đã có đủ điều kiện để thực hiện ý tưởng ở trên như sau:

* Tìm kiếm các class đã được đăng ký
    ```js
    window.bootSubGame = function (path, settings, bundledScript, clsIds) {
        //...
        var oldKeys = Object.keys(cc.js._registeredClassIds);
        cc.loader.load(jsList, function (err) {
            if (err) {
                console.log(JSON.stringify(err));
                return;
            }

            var newKeys = Object.keys(cc.js._registeredClassIds);
            for (var i = 0; i < newKeys.length; i++) {
                if (oldKeys.indexOf(newKeys[i]) == - 1) {
                    clsIds.push(newKeys[i]);
                }
            }

            cc.game._initConfig(option);
            onStart();
        });
    }
    ```
* Gỡ đăng ký các class cũ
    ```js
    window.runSubGame = function (path, settingScript, bundledScript, cache) {
        //...
        cc.loader.removeItem(path + '/src/' + bundledScript);
        cc.loader.removeItem(path + '/src/' + settingScript);

        window[bundledScript] = window[bundledScript] || [];
        for (var i = 0; i <= window[bundledScript].length; i++) {
            var cls = cc.js._getClassById(window[bundledScript][i]);
            if (cls)
                cc.js.unregisterClass(cls);
        }
        window[bundledScript] = [];
        
        var pathSetting = path + '/src/' + settingScript;
        //...
    ```

Ở đây biến `window[bundledScript]` sẽ lưu giữ mảng các `id` class.

### **Source code**

* File chạy bản cập nhật không yêu cầu restart ứng dụng đã được fix lỗi fps drops và cache [GGame.js](./src/fixhotupdate/GGame.js)
