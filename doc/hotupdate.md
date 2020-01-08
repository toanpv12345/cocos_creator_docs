# **Hotupdate (Cocos Creator 2.1.1)**

* [**Application**](#Application)
* [**Subpackage**](#Subpackage)
    * [**Thảo luận**](#Thảo-luận)
    * [**Phân tích**](#Phân-tích)
    * [**Giải pháp**](#Giải-pháp)
    * [**Các môi trường chạy**](#Các-môi-trường-chạy)
* [**Mở rộng**](#Mở-rộng)
* [**Source code**](#Source-code)

## **Application**

Trường hợp ứng dụng cần cập nhật trước khi bắt đầu. Tất cả các bước kiểm tra cập nhật thực hiện ngay khi khởi chạy ứng dụng. Theo tài liệu hướng dẫn của [Cocos Creator](https://docs.cocos2d-x.org/creator/manual/en/advanced-topics/hot-update.html) các bước làm như sau:

* Tạo ra template của file `main.js` nội dung như sau
    ```js
    ...
    else if (window.jsb) {
        if (jsb) {
            var hotUpdateSearchPaths = localStorage.getItem('HotUpdateSearchPaths');
            if (hotUpdateSearchPaths) {
                jsb.fileUtils.setSearchPaths(JSON.parse(hotUpdateSearchPaths));
            }
        }
    ...
    ```

    tại thư mục cấu trúc như sau:

    ```
    + assets
    + build
    + build-templates
    |   + jsb-default
    |   |   - main.js 
    |   + jsb-link
    |   |   - main.js
    + library
    + local
    + packages
    + sdkbox
    + settings
    + temp
    - jsconfig.json
    - project.json
    ```
* Build các file assets gồm code và resource của ứng dụng ra 2 thư mục `res`, `src` rồi lưu trữ trên server. _Lưu ý chọn Encrypt Js để ra file `project.jsc` và `setting.jsc` tại thư mục `src`_.
* Tạo các file `project.manifest` và `version.manifest` bằng file [version_generator.js](https://github.com/cocos-creator/tutorial-hot-update/blob/master/version_generator.js) cho asset ở thư mục `res` và `src` ở bước 1.
* Lưu trữ file `project.manifest` khởi tạo trong ứng dụng, có thể làm theo cách hướng dẫn lưu trong thư mục assets hoặc đơn giản hơn như sau: kiểm tra thư mục download về có chưa nếu chưa có thì lấy nội dung của `project.manifest` từ biến.
    ```js
    let projectManifest = jsb.fileUtils.getStringFromFile(this._storagePath + '/project.manifest');
    if (!projectManifest)
        projectManifest = JSON.stringify({
            'packageUrl': `${HOT_UPDATE_URL}/assets/`,
            'remoteManifestUrl': `${HOT_UPDATE_URL}/assets/project.manifest`,
            'remoteVersionUrl': `${HOT_UPDATE_URL}/assets/version.manifest`,
            version: '0.1.0',
            assets: {},
            searchPaths: []
        });
    ```
* Viết code download tại ứng dụng như [hướng dẫn](https://github.com/cocos-creator/tutorial-hot-update/blob/master/assets/scripts/module/HotUpdate.js).
* Sau khi download thành công thực hiện restart lại ứng dụng
    ```js
    cc.audioEngine.stopAll();
    cc.game.restart();
    ```

## **Subpackage**

### **Thảo luận**

Việc cập nhật toàn bộ ứng dụng đôi khi đòi hỏi nhiều thời gian, trong khi các nội dung tải về người dùng cũng chưa cần truy xuất ngay. Ta có thể chia nhỏ ứng dụng hơn nữa thành các subpackage, khi người dùng đã vào ứng dụng và sau đó cần truy cập các nội dung khác không sẵn có thì ứng dụng mới bắt đầu tải về.

Nếu áp dụng cách của use case 1 thì phải restart lại ứng dụng mỗi khi cập nhật nội dung. Khi bắt đầu chạy ứng dụng việc restart không ảnh hưởng đến luồng xử lý, nhưng trong trường hợp này thì việc restart là kém hiệu quả, cần tránh thao tác này. 

### **Phân tích**

Xem xét source code `main.js` ta thấy luồng xử lý của Cocos Creator khi bắt đầu 1 game thứ tự như sau:

* Load file `settings.js` khai báo các tài nguyên của ứng dụng trong thư mục `res` và các thư viện cần thiết khác.
* Khởi chạy bằng lệnh `cc.game.run`.
    * Load file `project.js` chứa code của ứng dụng.
    * Khởi tạo assets bằng lệnh `cc.AssetLibrary.init`, các thông số được lấy từ `window._CCSettings` sau khi load file `settings.js`.
    * Load scene bằng lệnh `cc.director.loadScene` từ launchScene khai báo trong `setting.js`

Có thể viết 1 file load game thực hiện các nhiệm vụ như `main.js` với 1 số sửa đổi như sau: 

* Chỉ định load file `setting.js` cho từng nội dung cần cập nhật (ví dụ `src/setting.1.js` cho game 1)
* Khởi tạo assets của nội dung cập nhật, nhưng vẫn giữ assets của ứng dụng chính để truy cập.
* Chỉ định load file `project.js` cho từng nội dung cần cập nhật (ví dụ `src/project.1.js` cho game 1)

Các khó khăn chính:

* `main.js` load `setting.js` bằng lệnh `require('src/settings.js');` tuy nhiên đối với file `settings.1.js` download từ server về thực hiện lệnh `require('src/settings.1.js');` bị lỗi `Error: Cannot find module 'settings.1.js'`.
* `setting.1.js` không khai báo các asset của ứng dụng chính, sau khi load nội dung cập nhật sẽ không có cách nào truy xuất được các asset của ứng dụng chính.
* `cc.game.run` không thực hiện việc load source code của file `project.1.js` khi đã load `project.js` trong `main.js`

### **Giải pháp**

* Để load `setting.1.js` có thể đơn giản gọi hàm load file của jsb như sau:
    ```js
    var pathSetting = path + '/src/' + settingScript;
    var setting = jsb.fileUtils.getStringFromFile(pathSetting);
    ```

    Vì `setting` chỉ chứa mã nguồn nên giá trị `window._CCSettings` của `settings.1.js` chưa lấy được, tuy nhiên js có sẵn hàm `eval` có thể thực thi mã ở dạng chuỗi. Do đó ta có:

    ```js
    eval(setting);

    // Xem giá trị window._CCSettings tính được
    console.log(window._CCSettings);
    ```

    Giải pháp này chấp nhận được đối với mã nguồn không dùng chế độ Encrypt Js, khi đã mã hoá thì `eval` không có cách nào lấy được giá trị của `setting`. Xem xét mã nguồn [CCGame.js](https://github.com/cocos-creator/engine/blob/master/cocos2d/core/CCGame.js) nhận thấy Cocos Creator dùng `cc.loader` để load `project.js` xử lý cho cả file mã hoá, do đó cũng có thể dùng để load `setting.1.js`, và lấy giá trị `window._CCSettings` của 1 file `settingScript` như sau:

    ```js
    var pathSetting = path + '/src/' + settingScript;
    cc.loader.load(pathSetting, function (err) {
        // Xem giá trị window._CCSettings lấy được
        console.log(window._CCSettings);
    });
    ```

    `CCLoader` sẽ giải mã nếu gặp file `.jsc` và thực hiện luôn code.
* Để giữ các asset của ứng dụng chính ta ghép tất cả các asset cần thiết của ứng dụng chính vào mỗi nội dung được load. Các lệnh ghép thực hiện sau khi load `settingScript` để đảm bảo giá trị `window._CCSettings` đã được tính.
    ```js
    window._CCSettings.rawAssets.assets = Object.assign(
        {},
        window['settings'].rawAssets.assets,
        window._CCSettings.rawAssets.assets
    );
    window._CCSettings.scenes.unshift(...window['settings'].scenes);
    window._CCSettings.packedAssets = Object.assign(
        {}, window['settings'].packedAssets,
        window._CCSettings.packedAssets
    );
    if (window['settings'].uuids)
        window._CCSettings.uuids.push(...window['settings'].uuids);

    // Lưu lại nội dung cấu hình của mỗi nội dung cập nhật theo key là file setting 
    // ('/src/settings.1.js', ...)
    window[settingScript] = window._CCSettings;
    ```

    Nội dung của file `main.js` trong thư mục build-templates bổ sung dòng sau: `window['settings'] = window._CCSettings;` ngay trước lệnh `window.boot();`
* Để load file `project.1.js` review source code `CCGame.js` nhận thấy cờ `cc.game._prepared` là true sau khi load `project.js` tại `main.js`, ta thiết lập là false như sau:

    ```js
    cc.game._prepared = false;
    cc.game.run(option, onStart);
    ```

### **Các môi trường chạy**

|               | **iOS** | **Android** | **Mac** | **Windows** |
|---------------|:-------:|:-----------:|:-------:|:-----------:|
| **Simulator** | Success | Success     | `Fail`  | `Fail`      |
| **Native**    | Success | Success     | Success | Success     |

_Lưu ý_:

_Các nội dung cập nhật và ứng dụng chính cần sử dụng chung key mã hoá._

_Từ phiên bản `1.9` api `cc.sys.cleanScript` bị remove nên không có cách nào xoá code đã load vào core js trừ khi restart toàn bộ ứng dụng. (Cập nhật: tham khảo tài liệu [Fix Hotupdate](./fixhotupdate.md#Fix-cache-hotupdate) cho vấn đề cache)_

## **Mở rộng**

Ở trên đã bàn đến cơ chế download các project riêng lẻ và sau khi download ứng dụng chính sẽ chạy scene mặc định của project đó. Phần này sẽ xem xét trường hợp chỉ cần download các tài nguyên mà không cần chạy scene. 

Nhận thấy các tài nguyên tải về vẫn phải tuân theo cấu trúc trong file setting.js nên về cơ bản cơ chế vẫn như cũ, chỉ khác ở điểm thay vì chạy scene mặc định thì ứng dụng chính cần bỏ qua. Ta quy ước scene mặc định cần bỏ qua là những scene nằm trong thư mục `test` của project.

Khi gặp scene này sẽ remove ra khỏi setting

```js
window.runSubGame = function (path, settingScript, bundledScript) {
    ...
    if (window._CCSettings.launchScene.indexOf('/test') > 0)
        window._CCSettings.launchScene = '';
    ...
}
```

Khi khởi tạo xong asset (kết thúc hàm `cc.AssetLibrary.init`) ứng dụng chính sẽ kiểm tra thông tin `launchScene` để quyết định chạy hay bỏ qua. Một sự kiện sẽ được gọi để ứng dụng biết khi nào việc khởi tạo tài nguyên kết thúc, thông tin `bundledScript` đi kèm giúp ứng dụng quyết định xem project nào vừa kết thúc khởi tạo tài nguyên.

```js
window.bootSubGame = function (settings, bundledScript) {
    ...
    var onStart = function () {

        ...
            var launchScene = settings.launchScene;
            if (launchScene != '') {
                cc.director.loadScene(launchScene, null, function () {
                    cc.loader.onProgress = null;
                    console.log('Success to load scene: ' + launchScene);
                });
            } else {
                cc.director.emit('hotupdate', {
                    code: 1,
                    bundledScript: bundledScript,
                    message: ''
                });
            }
        ...
    }
    ...
};
```

## **Source code**

* File sinh thông tin phiên bản cập nhật [version_generator.js](./src/version_generator.js)
* File build-templates [main.js](./src/build-templates/jsb-link/main.js)
* File kiểm tra và tải bản cập nhật [GLoadGame.js](./src/hotupdate/GLoadGame.js)
* File chạy bản cập nhật không yêu cầu restart ứng dụng [GGame.js](./src/hotupdate/GGame.js)

