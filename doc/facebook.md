# **Login Facebook trên windows (Cocos Creator 2.1.1)**

## **Thiết lập Facebook**

* Đăng nhập vào Developer Facebook.
* Vào mục `Facebook Login` (Đăng nhập Facebook), chọn phần `Settings` (Cài đặt). 
* Tại ô text `Valid OAuth Redirect URIs` (URI chuyển hướng OAuth hợp lệ) nhập vào chuỗi sau: `https://www.facebook.com/connect/login_success.html`

## **Thiết lập Project**
Copy toàn bộ thư mục project `facebook` và thư mục `facebook` trong `Classes` như bên dưới:
```
+ Classes
|   + facebook
|   |   - Facebook.cpp
|   |   - Facebook.h
|   |   - jsb_facebook_register.cpp
|   |   - jsb_facebook_register.h
|   - AppDelegate.cpp
|   - AppDelegate.h
+ facebook
|   - BrowserEvent.cpp
|   - BrowserEvent.h 
|   - cppwebpage.cpp
|   - cppwebpage.h
|   - Main.cpp
|   - WebBrowserHelper.cpp
|   - WebBrowserHelper.h
|   - facebook.vcxproj
|   - facebook.vcxproj.filters
+ proj.android-studio
+ proj.ios_mac
+ proj.win32
```
Thêm project `facebook` vào solution trên win32.

Chọn chế độ Unicode cho project.
```
Platform Toolset: Visual Studio 2015 - Windows XP(v140_xp)
Character Set: Use Unicode Character Set
```
## **Code C++**
Đăng ký facebook jsb trong `AppDelegate.cpp`

```C++
USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "facebook/jsb_facebook_register.h"
#endif

    // ...
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	se->addRegisterCallback(js_register_ns_FacebookJsb);
#endif

    se->start();
    // ...
```
## **Code Js**
Đăng nhập Facebook và lấy `accesstoken` như sau:
```js
if (cc.sys.os == cc.sys.OS_WINDOWS) {
    this._facebook = new cg.Facebook('<APP_ID>', 960, 640);

    this._facebook.login(function (token) {
        console.log('token: ' + token);
    });
}
```
|Khai báo|Kiểu|Tham số|Bắt buộc|Mô tả|
|--------|----|-------|--------|-----|
|`cg.Facebook`|Class||Có|Class `Facebook` thuộc domain name `cg`|Tạo đối tượng gọi đăng nhập facebook|
|`login`|Function|FacebookAppId|Có|ID của App Facebook|
|||Width|Không|Chiều rộng màn hình đăng nhập facebook (mặc định 800)|
|||Height|Không|Chiều cao màn hình đăng nhập facebook (mặc định 600)|

Đăng xuất
```js
if (cc.sys.os == cc.sys.OS_WINDOWS) {
    if (this._facebook)
        this._facebook.logout();
}
```

## **Source code**

[Windows Facebook](./src/facebook-windows/facebook-windows.zip)