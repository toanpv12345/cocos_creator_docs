//node version_generator.js -v 1.0.0 -u http://your-server-address/tutorial-hot-update/remote-assets/ -s native/package/ -d assets/
//node version_generator.js -v 1.0.1 -u http://golden-gate-b73b1.web.app/4/assets/ -s ./public/4/assets -d ./public/4/assets/
(function () {
    'use strict';

    var fs = require('fs');
    var path = require('path');
    var crypto = require('crypto');

    var SERVER = '';
    var GAME_FOLDER = '';

    var src = '';
    var dest = '';

    var manifest = {
        packageUrl: `https://${SERVER}/${GAME_FOLDER}assets/`,
        remoteManifestUrl: `https://${SERVER}/${GAME_FOLDER}assets/project.manifest`,
        remoteVersionUrl: `https://${SERVER}/${GAME_FOLDER}assets/version.manifest`,
        version: '1.0.0',
        assets: {},
        searchPaths: []
    };

    function readDir(dir, obj) {
        var stat = fs.statSync(dir);
        if (!stat.isDirectory()) {
            return;
        }
        var subpaths = fs.readdirSync(dir), subpath, size, md5, compressed, relative;
        for (var i = 0; i < subpaths.length; ++i) {
            if (subpaths[i][0] === '.') {
                continue;
            }
            subpath = path.join(dir, subpaths[i]);
            stat = fs.statSync(subpath);
            if (stat.isDirectory()) {
                readDir(subpath, obj);
            }
            else if (stat.isFile()) {
                // Size in Bytes
                size = stat['size'];
                md5 = crypto.createHash('md5').update(fs.readFileSync(subpath, 'binary')).digest('hex');
                compressed = path.extname(subpath).toLowerCase() === '.zip';

                relative = path.relative(src, subpath);
                relative = relative.replace(/\\/g, '/');
                relative = encodeURI(relative);
                obj[relative] = {
                    'size': size,
                    'md5': md5
                };
                if (compressed) {
                    obj[relative].compressed = true;
                }
            }
        }
    }

    var mkdirSync = function (path) {
        try {
            fs.mkdirSync(path);
        } catch (e) {
            if (e.code != 'EEXIST') throw e;
        }
    }

    function execute(url, version, asrc, adest) {
        src = asrc;
        dest = adest;

        manifest.packageUrl = url;
        manifest.remoteManifestUrl = url + 'project.manifest';
        manifest.remoteVersionUrl = url + 'version.manifest';
        manifest.version = version;

        // Iterate res and src folder
        readDir(path.join(src, 'src'), manifest.assets);
        readDir(path.join(src, 'res'), manifest.assets);

        var destManifest = path.join(dest, 'project.manifest');
        var destVersion = path.join(dest, 'version.manifest');

        mkdirSync(dest);

        fs.writeFile(destManifest, JSON.stringify(manifest), (err) => {
            if (err) throw err;
            console.log('Manifest successfully generated');
        });

        delete manifest.assets;
        delete manifest.searchPaths;
        fs.writeFile(destVersion, JSON.stringify(manifest), (err) => {
            if (err) throw err;
            console.log('Version successfully generated');
        });
    }

    module.exports = {
        execute
    };
})();