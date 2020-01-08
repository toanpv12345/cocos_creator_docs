(function () {
    'use strict';

    const fs = require('fs');
    const path = require('path');

    let files = [];

    function copyFileSync(source, target) {

        var targetFile = target;

        //if target is a directory a new file with the same name will be created
        if (fs.existsSync(target)) {
            if (fs.lstatSync(target).isDirectory()) {
                targetFile = path.join(target, path.basename(source));
            }
        }

        //Editor.log(targetFile);
        fs.writeFileSync(targetFile, fs.readFileSync(source));
        files.push(targetFile);
    }

    function copyFolderRecursive(source, target) {
        var files = [];

        //check if folder needs to be created or integrated
        var targetFolder = path.join(target, path.basename(source));
        if (!fs.existsSync(targetFolder)) {
            fs.mkdirSync(targetFolder);
        }

        //copy
        if (fs.lstatSync(source).isDirectory()) {
            files = fs.readdirSync(source);
            files.forEach((file) => {
                var curSource = path.join(source, file);
                if (fs.lstatSync(curSource).isDirectory()) {
                    copyFolderRecursive(curSource, targetFolder);
                } else {
                    copyFileSync(curSource, targetFolder);
                }
            });
        }
    }

    function removeFiles(target) {
        if (!fs.existsSync(target))
            return;

        const data = fs.readFileSync(target);
        const files = JSON.parse(data);

        for (let i = 0; i < files.length; i++) {
            try {
                fs.unlinkSync(files[i]);
                //Editor.log('delete file: ', files[i]);
            } catch (err) {
            }
        }
    }

    function saveFilesCopy(target, files) {
        try {
            const data = JSON.stringify(files);
            fs.writeFileSync(target, data);
        } catch (err) {
            Editor.log(err);
        }
    }

    function folder(src, dest, filename) {
        const tmpPath = Editor.url('packages://sbweb/temp', 'utf8');
        if (filename)
            removeFiles(`${tmpPath}/${filename}.json`);

        files = [];
        copyFolderRecursive(src, dest);

        if (filename)
            saveFilesCopy(`${tmpPath}/${filename}.json`, files);
    }

    module.exports = {
        folder,
        saveFilesCopy,
        removeFiles
    };
})();
