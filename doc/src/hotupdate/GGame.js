// Test load resource from gate
window.gateBar = function () {
    cc.loader.loadRes('prefab/GBtnBack', (err, prefab) => {
        if (err) {
            console.log(err);
            return;
        }

        let node = cc.instantiate(prefab);
        let sprite = node.getComponent(cc.Sprite).node;
        let size = cc.winSize;

        node.setPosition(size.width - sprite.width, size.height - sprite.height);
        cc.director.getScene().addChild(node);
    });
};

window.bootSubGame = function (settings, bundledScript) {
    //var settings = window._CCSettings;
    //window._CCSettings = undefined;

    if (!settings.debug) {
        var uuids = settings.uuids;

        var rawAssets = settings.rawAssets;
        var assetTypes = settings.assetTypes;
        var realRawAssets = settings.rawAssets = {};
        for (var mount in rawAssets) {
            var entries = rawAssets[mount];
            var realEntries = realRawAssets[mount] = {};
            for (var id in entries) {
                var entry = entries[id];
                var type = entry[1];
                // retrieve minified raw asset
                if (typeof type === 'number') {
                    entry[1] = assetTypes[type];
                }
                // retrieve uuid
                realEntries[uuids[id] || id] = entry;
            }
        }

        var scenes = settings.scenes;
        for (var i = 0; i < scenes.length; ++i) {
            var scene = scenes[i];
            if (typeof scene.uuid === 'number') {
                scene.uuid = uuids[scene.uuid];
            }
        }

        var packedAssets = settings.packedAssets;
        for (var packId in packedAssets) {
            var packedIds = packedAssets[packId];
            for (var j = 0; j < packedIds.length; ++j) {
                if (typeof packedIds[j] === 'number') {
                    packedIds[j] = uuids[packedIds[j]];
                }
            }
        }

        var subpackages = settings.subpackages;
        for (var subId in subpackages) {
            var uuidArray = subpackages[subId].uuids;
            if (uuidArray) {
                for (var k = 0, l = uuidArray.length; k < l; k++) {
                    if (typeof uuidArray[k] === 'number') {
                        uuidArray[k] = uuids[uuidArray[k]];
                    }
                }
            }
        }
    }

    var onStart = function () {
        cc.loader.downloader._subpackages = settings.subpackages;

        cc.view.enableRetina(true);
        cc.view.resizeWithBrowserSize(true);

        if (cc.sys.isMobile) {
            if (settings.orientation === 'landscape') {
                cc.view.setOrientation(cc.macro.ORIENTATION_LANDSCAPE);
            }
            else if (settings.orientation === 'portrait') {
                cc.view.setOrientation(cc.macro.ORIENTATION_PORTRAIT);
            }
            cc.view.enableAutoFullScreen([
                cc.sys.BROWSER_TYPE_BAIDU,
                cc.sys.BROWSER_TYPE_WECHAT,
                cc.sys.BROWSER_TYPE_MOBILE_QQ,
                cc.sys.BROWSER_TYPE_MIUI,
            ].indexOf(cc.sys.browserType) < 0);
        }

        // Limit downloading max concurrent task to 2,
        // more tasks simultaneously may cause performance draw back on some android system / browsers.
        // You can adjust the number based on your own test result, you have to set it before any loading process to take effect.
        if (cc.sys.isBrowser && cc.sys.os === cc.sys.OS_ANDROID) {
            cc.macro.DOWNLOAD_MAX_CONCURRENT = 2;
        }

        // https://github.com/cocos-creator/engine/blob/master/cocos2d/core/platform/CCAssetLibrary.js
        // init assets
        cc.AssetLibrary.init({
            libraryPath: 'res/import',
            rawAssetsBase: 'res/raw-',
            rawAssets: settings.rawAssets,
            packedAssets: settings.packedAssets,
            md5AssetsMap: settings.md5AssetsMap
        });

        const launchScene = settings.launchScene;
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
    };

    // jsList
    var jsList = settings.jsList;
    if (jsList) {
        jsList = jsList.map(function (x) {
            return 'src/' + x;
        });
        jsList.push(bundledScript);
    }
    else {
        jsList = [bundledScript];
    }

    var option = {
        id: 'GameCanvas',
        scenes: settings.scenes,
        debugMode: settings.debug ? cc.debug.DebugMode.INFO : cc.debug.DebugMode.ERROR,
        showFPS: !false && settings.debug,
        frameRate: 60,
        jsList: jsList,
        groupList: settings.groupList,
        collisionMatrix: settings.collisionMatrix,
    }

    cc.loader.load(jsList, function (err) {
        if (err) {
            console.log(JSON.stringify(err));
            return;
        }

        cc.game._initConfig(option);
        onStart();
    });
    // https://github.com/cocos-creator/engine/blob/master/cocos2d/core/CCGame.js
    // Hack
    //cc.game._prepared = false;
    //cc.game.run(option, onStart);
};

window.runSubGame = function (path, settingScript, bundledScript, cache) {
    // Fix previous version.
    window['settings'] = window['settings'] || window['src/settings.js'];
    
    if (cache && window[settingScript]) {
        window.bootSubGame(window[settingScript], 'src/' + bundledScript);
        /*
        const launchScene = window[settingScript].launchScene;
        if (launchScene != '') {
            cc.director.loadScene(launchScene, null, function () {
                cc.loader.onProgress = null;
                console.log('Success to load scene: ' + launchScene);
            });
        } else {
            cc.director.emit('hotupdate', {
                code: 1,
                bundledScript: 'src/' + bundledScript,
                message: ''
            });
        }
        */
        return;
    }

    var searchPaths = jsb.fileUtils.getSearchPaths();
    searchPaths.unshift(path);
    jsb.fileUtils.setSearchPaths(searchPaths);

    var pathSetting = path + '/src/' + settingScript;

    cc.loader.load(pathSetting, function (err) {
        if (err) {
            console.log(JSON.stringify(err));
            return;
        }

        if (window.game && (path != window.game)) {
            cc.loader.release(window.game);
            // Hack
            delete cc.loader._cache[window.game];
            // for (var property in cc.loader._cache) {
            //     console.log(property);
            // }
        }

        window.game = 'src/' + bundledScript;

        if (window._CCSettings.launchScene.indexOf('/test') > 0)
            window._CCSettings.launchScene = '';

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
        if (window['settings'].uuids && window._CCSettings.uuids)
            window._CCSettings.uuids.push(...window['settings'].uuids);

        window[settingScript] = window._CCSettings;

        window.bootSubGame(window[settingScript], 'src/' + bundledScript);
    });
}