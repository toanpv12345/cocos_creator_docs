'use strict';

const fs = require('fs');
const path = require('path');
const Config = require(Editor.url('packages://sbweb/utils/config.js'));

Editor.Panel.extend({
    style: fs.readFileSync(Editor.url('packages://sbweb/panel/style/index.css', 'utf8')),
    template: fs.readFileSync(Editor.url('packages://sbweb/panel/template/index.html', 'utf8')),

    $: {
        txtDirNative: '#txtDirNative',
        btnDirNative: '#btnDirNative',
        txtHostNative: '#txtHostNative',

        txtDirWeb: '#txtDirWeb',
        btnDirWeb: '#btnDirWeb',
        txtHostWeb: '#txtHostWeb',

        txtSubpackage: '#txtSubpackage',
        txtVersion: '#txtVersion',
        chkAuto: '#chkAuto',
        btnClose: '#btnClose'
    },

    openDirectory(callback) {
        const DEFAULT_PATH = path.join(Editor.Project.path, '.');

        let res = Editor.Dialog.openFile({
            title: 'Select folder',
            defaultPath: DEFAULT_PATH,
            properties: ['openDirectory'],
        });

        if (!res) {
            Editor.Dialog.messageBox({
                type: 'error',
                buttons: [],
                message: 'Please choose a folder!',
            });
            callback('');
            return;
        }

        callback(res[0]);
    },

    saveConfig() {
        const config = {
            dirNative: this.$txtDirNative.value,
            hostNative: this.$txtHostNative.value,
            dirWeb: this.$txtDirWeb.value,
            hostWeb: this.$txtHostWeb.value,

            subpackage: this.$txtSubpackage.value,
            version: this.$txtVersion.value,
            auto: this.$chkAuto.value
        };

        Config.save(config);
    },

    run(argv) {

    },

    ready() {
        let config = Config.load();

        this.$txtDirNative.value = config.dirNative;
        this.$txtHostNative.value = config.hostNative;
        this.$txtDirWeb.value = config.dirWeb;
        this.$txtHostWeb.value = config.hostWeb;

        this.$txtSubpackage.value = config.subpackage;
        this.$txtVersion.value = config.version;
        this.$chkAuto.value = config.auto;

        this.$btnDirNative.addEventListener('confirm', () => {
            this.openDirectory((dir) => {
                this.$txtDirNative.value = dir;
            });
        });
        this.$btnDirWeb.addEventListener('confirm', () => {
            this.openDirectory((dir) => {
                this.$txtDirWeb.value = dir;
            });
        });

        this.$btnClose.addEventListener('confirm', () => {
            this.saveConfig();
            Editor.Ipc.sendToMain('sbweb:close', '', (error, answer) => {
                if (error && error.code === 'ETIMEOUT') {
                    Editor.error('Timeout for ipc message sbweb:close');
                    return;
                }
            });
        });
    },

    close() {
        this.saveConfig();
    },

    listeners: {
        mousedown(event) {
            event.stopPropagation();
            //Editor.log('on mousedown');
        },

        'panel-hide'(event) {
            event.stopPropagation();
            this.saveConfig();
        }
    }
});