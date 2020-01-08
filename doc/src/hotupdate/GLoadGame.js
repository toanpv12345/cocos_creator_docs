import GConnection from 'GConnection';
require('./GGame');

const HOT_UPDATE_URL = 'https://golden-gate-b73b1.web.app';
const MAIN_APP_ID = 'jplay';

/**
 * Thư mục trên server
 * 
 * Mỗi game sẽ nằm trong 1 thư mục khác nhau có tên là id
 * 
 * ${HOT_UPDATE_URL}/${id}/assets/project.manifest
 * ${HOT_UPDATE_URL}/${id}/assets/version.manifest
 * ${HOT_UPDATE_URL}/${id}/assets/...
 * 
 * Ở đây id là 1 số nguyên hoặc chuỗi theo quy tắc sau:
 * 
 * id = MAIN_APP_ID: game chính
 * id != MAIN_APP_ID: các game con
 */
cc.Class({
    extends: cc.Component,

    properties: {
        lblTitle: cc.Label,
        sprGame: cc.Sprite,
        lblProgress: cc.Label,
        lblStatus: cc.Label,
        lblFile: cc.Label,
        lblDescription: cc.Label,
        _updating: false,
        _canRetry: false,
        _storagePath: ''
    },

    onLoad() {
    },

    start() {

    },

    createInfo(spriteFrame, info) {
        if (spriteFrame)
            this.sprGame.spriteFrame = spriteFrame;

        if (info.name)
            this.lblTitle.string = info.name;

        if (!info.id) {
            console.log('Miss id property for game.');
            return;
        }

        if (!cc.sys.isNative)
            return;

        this.updateGame(info.id);
    },

    closeClick(event) {
        this.node.removeFromParent(true);
    },

    updateGame(id) {
        if (!GConnection.isOnline()) {
            cc.director.emit('hotupdate', {
                code: -1,
                id: id,
                message: 'Network is not available!\nPlease checking and try again.',
                canRetry: false
            });
            return;
        }

        this.id = id;
        this._storagePath = jsb.fileUtils ? jsb.fileUtils.getWritablePath() : '/';
        console.log('Storage path for remote asset : ' + this._storagePath);

        // Setup your own version compare handler, versionA and B is versions in string
        // if the return value greater than 0, versionA is greater than B,
        // if the return value equals 0, versionA equals to B,
        // if the return value smaller than 0, versionA is smaller than B.
        this.versionCompareHandle = (versionA, versionB) => {
            console.log('JS Custom Version Compare: version A is ' + versionA + ', version B is ' + versionB);
            var vA = versionA.split('.');
            var vB = versionB.split('.');
            for (var i = 0; i < vA.length; ++i) {
                var a = parseInt(vA[i]);
                var b = parseInt(vB[i] || 0);
                if (a == b)
                    continue;

                return a - b;
            }

            if (vB.length > vA.length)
                return - 1;

            return 0;
        };

        // Init with empty manifest url for testing custom manifest
        this._am = new jsb.AssetsManager('', `${this._storagePath}${this.id}`, this.versionCompareHandle);

        // Setup the verification callback, but we don't have md5 check function yet, so only print some message
        // Return true if the verification passed, otherwise return false
        this._am.setVerifyCallback(function (path, asset) {
            // When asset is compressed, we don't need to check its md5, because zip file have been deleted.
            var compressed = asset.compressed;
            // Retrieve the correct md5 value.
            var expectedMD5 = asset.md5;
            // asset.path is relative path and path is absolute.
            var relativePath = asset.path;
            // The size of asset file, but this value could be absent.
            var size = asset.size;
            if (compressed) {
                console.log('Verification passed : ' + relativePath);
                return true;
            } else {
                console.log('Verification passed : ' + relativePath + ' (' + expectedMD5 + ')');
                return true;
            }
        });

        console.log('Hot update is ready, please check or directly update.');
        if (cc.sys.os == cc.sys.OS_ANDROID) {
            // Some Android device may slow down the download process when concurrent tasks is too much.
            // The value may not be accurate, please do more test and find what's most suitable for your game.
            this._am.setMaxConcurrentTask(2);
            console.log('Max concurrent tasks count have been limited to 2');
        }

        this.hotUpdate();
    },

    updateCb(event) {
        var needRestart = false;
        var failed = false;
        switch (event.getEventCode()) {
            case jsb.EventAssetsManager.ERROR_NO_LOCAL_MANIFEST:
                console.log('No local manifest file found, hot update skipped.');
                failed = true;
                break;
            case jsb.EventAssetsManager.UPDATE_PROGRESSION:
                if (isNaN(event.getPercent())) {

                } else {
                    if (!this._download) {
                        this._download = true;
                        this.lblStatus.string = 'download';
                    }

                    this.lblProgress.string = Math.round(event.getPercent() * 100) + '%';
                    this.lblFile.string = event.getDownloadedFiles() + ' / ' + event.getTotalFiles() + ' files';//event.getMessage();
                }

                //percentByFile: event.getPercentByFile()

                //console.log(event.getDownloadedFiles() + ' / ' + event.getTotalFiles());
                //console.log(event.getDownloadedBytes() + ' / ' + event.getTotalBytes());
                break;
            case jsb.EventAssetsManager.ERROR_DOWNLOAD_MANIFEST:
            case jsb.EventAssetsManager.ERROR_PARSE_MANIFEST:
                console.log('Fail to download manifest file, hot update skipped.');
                failed = true;
                break;
            case jsb.EventAssetsManager.ALREADY_UP_TO_DATE:
                console.log('Already up to date with the latest remote version.');
                failed = true;
                break;
            case jsb.EventAssetsManager.UPDATE_FINISHED:
                console.log('Update finished. ' + event.getMessage());
                needRestart = true;
                break;
            case jsb.EventAssetsManager.UPDATE_FAILED:
                console.log('Update failed. ' + event.getMessage());
                failed = true;
                this._updating = false;
                this._canRetry = true;
                break;
            case jsb.EventAssetsManager.ERROR_UPDATING:
                console.log('Asset update error: ' + event.getAssetId() + ', ' + event.getMessage());
                failed = true;
                this._updating = false;
                break;
            case jsb.EventAssetsManager.ERROR_DECOMPRESS:
                console.log(event.getMessage());
                failed = true;
                break;
            default:
                break;
        }

        if (failed) {
            this._am.setEventCallback(null);
            this._updateListener = null;
            this._updating = false;

            if (event.getEventCode() == jsb.EventAssetsManager.ALREADY_UP_TO_DATE) {            
                //this.nodDownloading.active = false;
                cc.director.emit('hotupdate', {
                    code: 0,
                    id: this.id,
                    message: 'Already up to date with the latest remote version'
                });

                if (this.id != MAIN_APP_ID) {
                    window.runSubGame(`${this._storagePath}${this.id}`, `settings.${this.id}.js`, `project.${this.id}.js`, true);
                }
            }
            else
                cc.director.emit('hotupdate', {
                    code: -2,
                    id: this.id,
                    message: 'Update failed!\nPlease try again.',
                    canRetry: this._canRetry
                });
            return;
        }

        if (needRestart) {
            if (this.id == MAIN_APP_ID) {
                this._am.setEventCallback(null);
                this._updateListener = null;

                // Prepend the manifest's search path
                var searchPaths = jsb.fileUtils.getSearchPaths();
                var newPaths = this._am.getLocalManifest().getSearchPaths();
                if (newPaths != '' && searchPaths.indexOf(newPaths) == -1) {
                    cc.sys.localStorage.setItem('HotUpdateSearchPaths', JSON.stringify(searchPaths));
                    console.log(JSON.stringify(newPaths));
                    Array.prototype.unshift.apply(searchPaths, newPaths);
                }
                // This value will be retrieved and appended to the default search path during game startup,
                // please refer to samples/js-tests/main.js for detailed usage.
                // !!! Re-add the search paths in main.js is very important, otherwise, new scripts won't take effect.
                cc.sys.localStorage.setItem('HotUpdateSearchPaths', JSON.stringify(searchPaths));
                jsb.fileUtils.setSearchPaths(searchPaths);

                cc.audioEngine.stopAll();
                cc.game.restart();
                return;
            }

            this._updating = false;
            window.runSubGame(`${this._storagePath}${this.id}`, `settings.${this.id}.js`, `project.${this.id}.js`, false);
        }
    },

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

                const manifest = new jsb.Manifest(projectManifest, `${this._storagePath}${this.id}`);
                this._am.loadLocalManifest(manifest, `${this._storagePath}${this.id}`);
            }

            this._failCount = 0;
            this._am.update();
            this._updating = true;
            this._download = false;
        }
    },

    retry() {
        if (!this._updating && this._canRetry) {
            this._canRetry = false;

            console.log('Retry failed Assets...');
            this._am.downloadFailedAssets();
        }
    },

    onDestroy() {
        if (this._updateListener) {
            this._am.setEventCallback(null);
            this._updateListener = null;
        }
    }
});