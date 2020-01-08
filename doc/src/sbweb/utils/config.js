(function () {
    'use strict';

    const fs = require('fs');

    function load() {
        const cfgPath = Editor.url('packages://sbweb/config/config.json', 'utf8');
        let config = {
            dirNative: '',
            hostNative: '',
            dirWeb: '',
            hostWeb: '',

            subpackage: '',
            version: '0',
            auto: true
        };

        try {
            const cfgData = fs.readFileSync(cfgPath, { encoding: 'utf8' });
            config = JSON.parse(cfgData);
        } catch (err) {

        }

        return config;
    }

    function save(config) {
        const cfgPath = Editor.url('packages://sbweb/config/config.json', 'utf8');
        const cfgData = JSON.stringify(config)
        fs.writeFileSync(cfgPath, cfgData, { encoding: 'utf8' });
    }

    module.exports = {
        load,
        save
    }
})();