'use strict';

//node copy -s ./testupdate3/build/web-desktop/res -d ./DemoGate/build/web-desktop

const crypto = require('crypto');
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

    fs.writeFileSync(targetFile, fs.readFileSync(source));
    console.log(targetFile);
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
        files.forEach(function (file) {
            var curSource = path.join(source, file);
            if (fs.lstatSync(curSource).isDirectory()) {
                copyFolderRecursive(curSource, targetFolder);
            } else {
                copyFileSync(curSource, targetFolder);
            }
        });
    }
}

let src = '';
let dest = '';
let i = 2;

while (i < process.argv.length) {
    const arg = process.argv[i];

    switch (arg) {
        case '--src':
        case '-s':
            src = process.argv[i + 1];
            i += 2;
            break;
        case '--dest':
        case '-d':
            dest = process.argv[i + 1];
            i += 2;
            break;
        default:
            i++;
            break;
    }
}

function removeFiles(target) {
    if (!fs.existsSync(target)) {
        return;
    }

    try {
        const data = fs.readFileSync(target);
        const files = JSON.parse(data);

        for (let i = 0; i < files.length; i++) {
            fs.unlinkSync(files[i]);
            console.log('delete: ' + files[i]);
        }
    } catch (err) {
        console.log(err);
    }
}

function saveFilesCopy(target) {
    const data = JSON.stringify(files);
    fs.writeFileSync(target, data);
}

const destMd5 = crypto.createHash('md5').update(dest).digest('hex');

removeFiles(`${destMd5}.json`);
copyFolderRecursive(src, dest);
saveFilesCopy(`${destMd5}.json`);