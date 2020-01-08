file [base64-binary.js](./src/base64-binary.js)

```js
//const Buffer = require('buffer').Buffer;
const Base64Binary = require('base64-binary');

cc.Class({
    statics: {
        // sprite is the cc.Sprite component you wish to display the image on 
        base64ToTexture(base64, sprite) {
            // store the base64 data into a buffer, you can use any method to do this
            // const buffer = new Buffer(base64, 'base64');
            // const len = buffer.length;
            // const bytes = new Uint8Array(len);
            // for (let i = 0; i < len; i++) {
            //     bytes[i] = buffer[i];
            // }

            let bytes = Base64Binary.decode(base64);

            const extName = 'png'; //depends on you file
            const fileName = `base64_img.${extName}`;

            // prepare the file path for writing 
            const path = `${jsb.fileUtils.getWritablePath()}${fileName}`;
            // written to a file 
            if (jsb.fileUtils.writeDataToFile(bytes, path)) {
                console.log(path);
                // load the file                
                cc.loader.load(path, (err, texture) => {
                    if (err) {
                        console.log(err);
                        cc.loader.release(path);
                    } else {
                        sprite.spriteFrame = new cc.SpriteFrame(texture);
                    }

                    jsb.fileUtils.removeFile(path);
                });
            }
        }
    }
});
```

