# **Đa ngôn ngữ**

# **1. Cài đặt hỗ trợ đa ngôn ngữ**
Vào menu Extension/Extension Store... cài đặt Internationalization Label and Sprite (i18n)
Sau khi cài đặt sẽ xuất hiện thư mục `i18n-plugin` và tại thư mục `resources` sẽ có thêm thư mục con `i18n`
```
+ assets
+ resources
|   + i18n 
+ i18n-plugin
```
# **2. Tạo file ngôn ngữ**
Vào menu Extension/i18n (menu i18n xuất hiện sau khi cài đặt) 

* Ấn vào nút + để thêm ngôn ngữ, tại giao diện New Language nhập tên ngôn ngữ ví dụ `en` và `vi` ... (nên đặt theo chuẩn mã chung để thuận tiện khi dùng hàm `cc.sys.language`) 

* Sau khi tạo tại thư mục `i18n` có thêm file ngôn ngữ
    ```
    + assets
    + resources
    |   + i18n
    |   |   - en.js
    |   |   - vi.js
    + i18n-plugin
    ```
# **3. Thêm nội dung vào các file ngôn ngữ**

File `en.js`

```js
'use strict';

if (!window.i18n) {
    window.i18n = {};
}

if (!window.i18n.languages) {
    window.i18n.languages = {};
}

window.i18n.languages['en'] = {
    // write your key value pairs here
    loading: 'Loading',
    recent: 'Recent',
    seemore: 'See More'
};
```

File `vi.js`

```js
'use strict';

if (!window.i18n) {
    window.i18n = {};
}

if (!window.i18n.languages) {
    window.i18n.languages = {};
}

window.i18n.languages['vi'] = {
    // write your key value pairs here
    loading: 'Đang tải',
    recent: 'Đã chơi',
    seemore: 'Xem thêm'
};
```

# **4. Thiết lập cho các component**

Tại component cần thiết lập đa ngôn ngữ click Add Component chọn `i18n/LocalizedLabel` hoặc `i18n/LocalizedSprite` tuỳ theo mục đích.

`DataID` của component mới thêm `LocalizedLabel` hoặc `LocalizedSprite` đặt là key trong file ngôn ngữ, ví dụ ở đây là `loading` thì khi thay đổi ngôn ngữ các label có `DataID` là `loading` sẽ bị thay đổi sang ngôn ngữ tương ứng `en` là `Loading` còn `vi` là `Đang tải`.

# **5. Sử dụng trong code**

## **5.1. Tự động hiển thị theo ngôn ngữ mặc định của thiết bị**

```js
const i18n = require('LanguageData');

cc.Class({
    extends: cc.Component,

    ...

    onLoad() {
        if (['en', 'vi'].indexOf(cc.sys.langage) >= 0) {
            i18n.init(cc.sys.language);

            // Lưu vào storage
            GStorage.saveValue('lang', cc.sys.language);
            return;
        }

        i18n.init('en');

        // Lưu vào storage
        GStorage.saveValue('lang', 'en');
    }
    ...
});
```

## **5.2. Hiển thị theo lựa chọn**

Trường hợp muốn người dùng chọn ngôn ngữ hiển thị khi đang chơi game thực hiện như sau:

```js
changeLanguage(lang) {
    i18n.init(lang);
    i18n.updateSceneRenderers();
}   
```

ví dụ, lưu ngôn ngữ hiện tại vào storage để so sánh

```js
languageClick(event) {
    const lang = GStorage.loadValue('lang');
    if (lang == 'en') {
        this.changeLanguage('vi');
        GStorage.saveValue('lang', 'vi');
    } else {
        this.changeLanguage('en');
        GStorage.saveValue('lang', 'en');
    }
},
```

## **5.3. Lấy text từ file localized**

```js
const myLocalizedText = i18n.t('TEXT_KEY');
```

## **5.4. Lấy text từ file localized có tham số**

```js
module.exports = {
    "Hello_name": "Hello,% {name}"
}}

var greetingText = i18n.t('hello_name', {name: 'nantas'});
```
