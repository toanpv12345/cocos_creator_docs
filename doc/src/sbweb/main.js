'use strict';

const fs = require('fs');
const path = require('path');

const Config = require('./utils/config');
const Copy = require('./utils/copy');
const VG = require('./utils/version_generator');

function removeDir(dirPath) {
    if (!fs.existsSync(dirPath)) {
        return;
    }

    let list = fs.readdirSync(dirPath);
    for (let i = 0; i < list.length; i++) {
        let filename = path.join(dirPath, list[i]);
        let stat = fs.statSync(filename);

        if (filename == "." || filename == "..") {
            // do nothing for current and parent dir
        } else if (stat.isDirectory()) {
            removeDir(filename);
        } else {
            fs.unlinkSync(filename);
        }
    }

    fs.rmdirSync(dirPath);
}

function onBeforeChangeFiles(options, callback) {
    if (options.platform == 'web-desktop' || options.platform == 'web-mobile') {
        let mainJsPath = path.join(options.dest, 'main.js');  // get path of main.js in build folder
        let script = fs.readFileSync(mainJsPath, 'utf8');     // read main.js

        if (script.indexOf("window['settings'] = window._CCSettings;") == -1) {
            script += "\nelse {\n" +
                "   window['settings'] = window._CCSettings;\n" +
                "}";
        }

        // append any scripts as you need
        fs.writeFileSync(mainJsPath, script); // save main.js
    }

    callback();
}

function renameFileJs(folder, subpackage, files) {
    fs.readdirSync(folder).forEach(file => {
        if (files)
            files.push(file);

        const p1 = file.indexOf('project.');
        const extension = file.indexOf('.jsc') > -1 ? 'jsc' : 'js';
        if (p1 > -1) {
            const projectSub = path.join(folder, `project.${subpackage}.${extension}`);
            fs.renameSync(path.join(folder, file), projectSub);
        } else {
            const p2 = file.indexOf('settings.');
            if (p2 > -1) {
                const settingsSub = path.join(folder, `settings.${subpackage}.${extension}`);
                fs.renameSync(path.join(folder, file), settingsSub);
            }
        }
    });
}

function recoverFileJs(folder, subpackage, files) {
    for (let i = 0; i < files.length; i++) {
        const p1 = files[i].indexOf('project.');
        if (p1 > -1) {
            const projectSub = path.join(folder, `project.${subpackage}.js`);
            fs.renameSync(projectSub, path.join(folder, files[i]));
        } else {
            const p2 = files[i].indexOf('settings.');
            if (p2 > -1) {
                const settingsSub = path.join(folder, `settings.${subpackage}.js`);
                fs.renameSync(settingsSub, path.join(folder, files[i]));
            }
        }
    }
}

function native(config, options) {
    if (config.dirNative == '') {
        Editor.info('You need to select the deploy folder.');
        callback();
        return;
    }

    const srcBuildFolder = path.join(options.dest, 'src');
    const resBuildFolder = path.join(options.dest, 'res');

    const subpackageFolder = path.join(config.dirNative, config.subpackage);
    if (!fs.existsSync(subpackageFolder))
        fs.mkdirSync(subpackageFolder);

    const assets = path.join(subpackageFolder, 'assets');
    if (fs.existsSync(assets))
        removeDir(assets);

    fs.mkdirSync(assets);

    // Copy resource
    Copy.folder(resBuildFolder, assets);

    // Copy source code
    Copy.folder(srcBuildFolder, assets);

    // generate version for native 
    if (config.host != '') {
        // rename file
        if (config.subpackage != '')
            renameFileJs(path.join(assets, 'src'), config.subpackage);

        // generate version
        if (config.auto)
            config.version++;

        VG.execute(config.host, config.version, assets, assets);
        Config.save(config);
    }
}

function web(config, options) {
    if (config.dirWeb == '') {
        Editor.info('You need to select the deploy folder.');
        callback();
        return;
    }

    const srcBuildFolder = path.join(options.dest, 'src');
    const resBuildFolder = path.join(options.dest, 'res');
    
    let files = [];
    const subpackageFolder = path.join(config.dirWeb, config.subpackage);

    // rename file subpackage
    if (config.subpackage != '') {
        renameFileJs(srcBuildFolder, config.subpackage, files);
    }

    // subpackage
    if (config.subpackage != '') {
        if (fs.existsSync(subpackageFolder)) {
            removeDir(subpackageFolder);
        }

        let destRes = path.join(config.dirWeb, options.platform);
        if (fs.existsSync(destRes)) {
            Copy.folder(resBuildFolder, destRes, 'res');

            const assets = path.join(subpackageFolder, 'assets');

            if (!fs.existsSync(subpackageFolder))
                fs.mkdirSync(subpackageFolder);

            if (!fs.existsSync(assets))
                fs.mkdirSync(assets);

            Copy.folder(srcBuildFolder, assets, 'src');

            // recover file name
            recoverFileJs(srcBuildFolder, config.subpackage, files);
        } else {
            Editor.info(`This is subpackage "${config.subpackage}". You need to build the gate before building the subpackage.`);
        }
    }

    // gate
    if (config.subpackage == '') {
        let platform = path.join(config.dirWeb, options.platform);
        if (!fs.existsSync(platform))
            fs.mkdirSync(platform);

        Copy.folder(resBuildFolder, platform, 'res');

        // wait for md5 file
        setTimeout(() => {
            let srcSrc = path.join(options.dest, 'src');
            Copy.folder(srcSrc, platform, 'src');

            const tmpPath = Editor.url('packages://sbweb/temp', 'utf8');
            Copy.removeFiles(`${tmpPath}/common.json`);

            let files = [];
            fs.readdirSync(options.dest).forEach(file => {
                let src = path.join(options.dest, file);

                if (!fs.lstatSync(src).isDirectory()) {
                    let dest = path.join(platform, file);
                    fs.writeFileSync(dest, fs.readFileSync(src));

                    files.push(dest);
                }
            });

            Copy.saveFilesCopy(`${tmpPath}/common.json`, files);

            Editor.info('Copy finished!.');
        }, 5000);
    }
}

function onBuildFinished(options, callback) {
    let config = Config.load();

    if (options.platform == 'web-desktop' || options.platform == 'web-mobile') {
        web(config, options);
    } else {
        native(config, options);
    }

    callback();

    Editor.info('Copy finished!.');
}

module.exports = {
    load() {
        Editor.Builder.on('before-change-files', onBeforeChangeFiles);
        Editor.Builder.on('build-finished', onBuildFinished);
    },

    unload() {
        Editor.Builder.removeListener('before-change-files', onBeforeChangeFiles);
        Editor.Builder.removeListener('build-finished', onBuildFinished);
    },

    messages: {
        test(event, question) {
            if (event.reply) {
                //if no error, the first argument should be null
                event.reply(null, dir);
            }
        },
        close() {
            Editor.Panel.close('sbweb');
        },
        open() {
            Editor.Panel.open('sbweb');
        }
    },
};