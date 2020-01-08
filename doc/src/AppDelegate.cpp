/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.
 
 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"

#include "cocos2d.h"

#include "cocos/scripting/js-bindings/manual/jsb_module_register.hpp"
#include "cocos/scripting/js-bindings/manual/jsb_global.h"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/event/EventDispatcher.h"
#include "cocos/scripting/js-bindings/manual/jsb_classtype.hpp"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/network/HttpClient.h"
using namespace cocos2d::network;

static std::string getFileNameForPath(const std::string& filePath)
{
    std::string fileName = filePath;
    const size_t lastSlashIdx = fileName.find_last_of("\\/");
    if (std::string::npos != lastSlashIdx)
    {
        fileName.erase(0, lastSlashIdx + 1);
    }
    return fileName;
}

static bool install_cert(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc >= 1) {
        bool ok = false;
        std::string caFilePath;
        ok = seval_to_std_string(args[0], &caFilePath);
        SE_PRECONDITION2(ok, false, "args[0] isn't a string.");

        auto fileUtils = cocos2d::FileUtils::getInstance();

        std::string writablePath = fileUtils->getWritablePath();
        std::string caFileName = getFileNameForPath(caFilePath);
        std::string newCaFilePath = writablePath + caFileName;

        if (fileUtils->isFileExist(newCaFilePath))
        {
            HttpClient::getInstance()->setSSLVerification(newCaFilePath);
        }
        else
        {
            if (fileUtils->isFileExist(caFilePath))
            {
                std::string fullPath = fileUtils->fullPathForFilename(caFilePath);
                if (fullPath[0] != '/')
                {
                    auto caData = fileUtils->getDataFromFile(fullPath);
                    if (!caData.isNull())
                    {
                        FILE* fp = fopen(newCaFilePath.c_str(), "wb");
                        if (fp != nullptr)
                        {
                            fwrite(caData.getBytes(), caData.getSize(), 1, fp);
                            fclose(fp);
                            
                            HttpClient::getInstance()->setSSLVerification(newCaFilePath);
                        }
                    }
                }
            }
        }

        return true;
    }
    
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting >=1", (int)argc);
    return false;
}
SE_BIND_FUNC(install_cert)

bool jsb_register_install_cert(se::Object* global)
{
    global->defineFunction("installCert", _SE(install_cert));
    return true;
}

#endif


AppDelegate::AppDelegate(int width, int height) : Application("Cocos Game", width, height)
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    se::ScriptEngine *se = se::ScriptEngine::getInstance();
    
    jsb_set_xxtea_key("c456dcfd-9c3f-4b");
    jsb_init_file_operation_delegate();
    
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
    // Enable debugger here
    jsb_enable_debugger("0.0.0.0", 6086, false);
#endif
    
    se->setExceptionCallback([](const char *location, const char *message, const char *stack) {
        // Send exception information to server like Tencent Bugly.
    });
    
    jsb_register_all_modules();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    se->addRegisterCallback(jsb_register_install_cert);
#endif

    se->start();
    
    se::AutoHandleScope hs;
    jsb_run_script("jsb-adapter/jsb-builtin.js");
    jsb_run_script("main.js");
    
    se->addAfterCleanupHook([]() {
        JSBClassType::destroy();
    });
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    EventDispatcher::dispatchEnterBackgroundEvent();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    EventDispatcher::dispatchEnterForegroundEvent();
}
