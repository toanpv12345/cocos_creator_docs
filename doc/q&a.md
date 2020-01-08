
1. **Q**: Làm thế nào để chạy thư viện `signalR` trên native?

    **A**: Copy thư viện `signalR` vào project như các file `js` bình thường, tích vào lựa chọn `Import As Plugin`, `Load In Web` và `Load In Native`.

2. **Q**: Làm thế nào để lấy các `sprite` trong 1 file `atlas`?

    **A**:  
    ```js
    cc.loader.loadRes(path, cc.SpriteAtlas, function (err, atlas) {
        var frame = atlas.getSpriteFrame(name);
        sprite.spriteFrame = frame;
    });
    ```

3. **Q**: Làm thế nào thiết lập lại độ phân giải màn hình?

    **A**: Dùng hàm `cc.view.setDesignResolutionSize`

    Ví dụ: 
    ```js
    cc.view.setDesignResolutionSize(width, height, cc.ResolutionPolicy.FIXED_HEIGHT);
    ```

4. **Q**: Làm thế nào để hiển thị toàn màn hình mà vẫn giữ được tỷ lệ `width`/`height` như thiết kế?

    **A**: Thiết lập `Canvas` ở chế độ cố định chiều cao để giữ tỷ lệ `width`/`height`, đối với ảnh nền thêm thành phần `Widget` và đặt `anchor` 4 phía (trên, dưới, phải, trái) để giãn toàn màn hình.

5. **Q**: Làm thế nào tạo được các file template?

    **A**: Tạo các thư mục con chứa các file template trong thư mục build-templates
    ```
    + assets
    + build
    + build-templates
    | + jsb-link (native)
    | + jsb-default (native)
    | + web-desktop
    | + fb-instant-games (facebook instant game)
    ```

6. **Q**: Làm thế nào biết được tiến độ tải scene (làm thế nào để tạo trang loading trước khi vào ứng dụng)?

    **A**: Thay vì vào thẳng scene chính ta tạo thêm 1 scene Loading làm nhiệm vụ tải scene chính. Tại scene Loading gọi `cc.director.preloadScene` để biết được tiến độ tải scene, sau khi hoàn tất 100% mới gọi `cc.director.loadScene` để chạy scene chính.

    Ví dụ: 

    ```js
    cc.director.preloadScene('MainScene', (completedCount, totalCount) => {
        if (totalCount > 0) {
            let percent = 100 * completedCount / totalCount;
            this.lblProgress.string = Math.round(percent) + '%';
        }
    }, (err) => {
        if (err) {
            // error handling
            return;
        }

        this.lblProgress.string = '100%';
        cc.director.loadScene('MainScene');
    });
    ```

7. **Q**: Làm thế xem log của thiết bị iPhone (xem log của ứng dụng cài trực tiếp từ AppStore)?  

    **A**: Kết nối iPhone với Mac, bật Xcode tìm menu `Windows` / `Devices and Simulators`. Chọn thiết bị đã kết nối ở bên trái, click `Open Console` để xem log các ứng dụng đang chạy.

8. **Q**: Làm thế nào sửa lỗi thiếu tài nguyên trong bản build Android (Android Studio 3.4, Cocos Creator 2.1.1)?

    **A**: Mặc định build gradle khi Cocos Creator 2.1.1 tạo bản build Android là `3.2` nếu nâng lên >= `3.4` sẽ bị lỗi sai đường dẫn khi copy assets, có thể giữ nguyên phiên bản `3.2` hoặc chỉ nâng lên `3.3` để Android copy đúng assets.

9. **Q**: Làm thế nào xử lý lỗi `Terminating app due to uncaught exception 'com.firebase.core', reason: 'Default app has already been configured.`?

    **A**:
    
    Tránh khởi tạo firebase nhiều lần.

10. **Q**: Làm thế nào từ js gọi được c++?

    **A**: Viết code binding theo chuẩn jsb 2.0

    Ví dụ js gọi hàm testFunc lấy 1 đối số kiểu chuỗi của c++:

    code c++:
    ```cpp
    #include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"

    static bool test_func(se::State& s)
    {
        const auto& args = s.args();
        int argc = (int)args.size();
        if (argc >= 1) {
            bool ok = false;
            std::string str;
            ok = seval_to_std_string(args[0], &str);
            SE_PRECONDITION2(ok, false, "args[0] isn't a string.");
            
            return true;
        }

        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting >=1", (int)argc);
        return false;
    }

    SE_BIND_FUNC(test_func)

    bool jsb_register_test_func(se::Object* global)
    {
        global->defineFunction("testFunc", _SE(test_func));
        return true;
    }

    ...

    bool AppDelegate::applicationDidFinishLaunching()
    {
        ...
        se->addRegisterCallback(jsb_register_test_func);

        se->start();
        ...
    }
    ```

    code js gọi c++:
    ```js
    testFunc('hello world!');
    ```

11. **Q**: Làm thế nào test post http trên console Mac?

    **A**:
    ```sh
    curl -d '<json string>' -H "Content-Type: application/json"  url
    ```