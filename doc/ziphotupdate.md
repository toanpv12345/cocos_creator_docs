# **Cải thiện quá trình cập nhật**

Trong tình huống cần tải về ứng dụng đầy đủ lần đầu thì cách nén tài nguyên lại sẽ giúp quá trình nhanh hơn do giảm được số lần request và bước kiểm tra md5. Tuy nhiên zip file lại có hạn chế không phân biệt được các thay đổi để biết file nào cần cập nhật, giải pháp duy trì cả 2 phiên bản zip và unzip trên server sẽ đáp ứng được vấn đề trên.

## **Thứ tự cập nhật**

**Cập nhật lần đầu - Tải từ phiên bản zip (version = 1.1)**: 
* Ứng dụng khởi tạo giả sử có version nhỏ hơn `1.1`, gate sẽ kiểm tra tại thư mục zip trên server và tải về các file cần thiết cùng manifest có version `1.1`. 
* Ứng dụng khởi động lại và lại chạy vào gate để kiểm tra phiên bản. Lúc này gate trỏ đến thư mục unzip trên server. 
* Ứng dụng kiểm tra nội dung manifest từ storage nếu version là `1.1` thì thay bằng manifest có version `1.2` (nếu khác `1.1` thì bỏ qua), do 2 phiên bản client và trên server tại thư mục unzip bằng nhau nên gate không tải về mà chạy luôn.

**Cập nhật lần sau - Tải từ phiên bản unzip (version > 1.2)**: 
* Các file sẽ được đưa vào thư mục unzip.

## **Cấu trúc thư mục**
Thư mục zip `gate`:
```
+ gate
|   + assets
|   |   + res
|   |   |   - import.zip
|   |   |   - raw-assets.zip
|   |   + src
|   |   |   - project.jsc (chứa project.manifest v 1.2)
|   |   |   - settings.jsc
|   |   - project.manifest (v 1.1)
|   |   - version.manifest (v 1.1)
```

Thư mục không zip `gateunzip`:
```
+ gateunzip
|   + assets
|   |   + res
|   |   |   + import
|   |   |   |   ...
|   |   |   + raw-assets
|   |   |   |   ...
|   |   + src
|   |   |   - project.jsc
|   |   |   - settings.jsc
|   |   - project.manifest (v 1.2)
|   |   - version.manifest (v 1.2)
```

## **Thực hiện**

* Thực hiện build bản đầy đủ, copy vào thư mục `gateunzip`
* Chạy file sinh phiên bản trên server [version_generator.js](./src/version_generator.js) với version `1.2`
* Lấy nội dung của file `project.manifest` copy vào code dưới đây:

```js
//...
const MAIN_APP_ID = 'gateunzip'; // thư mục unzip
//...

hotUpdate() {
    if (this._am && !this._updating) {
        this._am.setEventCallback(this.updateCb.bind(this));

        if (this._am.getState() == jsb.AssetsManager.State.UNINITED) {
            let projectManifest = jsb.fileUtils.getStringFromFile(`${this._storagePath}${this.id}/project.manifest`);

            if (!projectManifest)
                projectManifest = JSON.stringify({
                    'packageUrl': `${HOT_UPDATE_URL}/${this.id}/assets/`,
                    'remoteManifestUrl': `${HOT_UPDATE_URL}/${this.id}/assets/project.manifest`,
                    'remoteVersionUrl': `${HOT_UPDATE_URL}/${this.id}/assets/version.manifest`,
                    version: '1.0.0',
                    assets: {},
                    searchPaths: []
                });
            else {
                var objpm = JSON.parse(projectManifest);
                if (this.id == MAIN_APP_ID && objpm.version == '1.1') {
                    projectManifest = '<content của project.manifest v 1.2 thư mục gateunzip>';                    
                }        
            }

            const manifest = new jsb.Manifest(projectManifest, `${this._storagePath}${this.id}`);
            this._am.loadLocalManifest(manifest, `${this._storagePath}${this.id}`);
        }

        this._failCount = 0;
        this._am.update();
        this._updating = true;
        this._download = false;
    }
}
```

* Build lại bản đầy đủ, copy vào thư mục `gate`
* Zip các thư mục `res/import` và `res/raw-assets`, xoá các file thừa
* Chạy file sinh phiên bản trên server [version_generator.js](./src/version_generator.js) với version `1.1`
