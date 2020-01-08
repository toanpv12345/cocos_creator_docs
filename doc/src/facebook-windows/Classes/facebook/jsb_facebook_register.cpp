#pragma once

#include <thread>
#include "facebook/jsb_facebook_register.h"
#include "facebook/Facebook.h"

static se::Object* __jsb_ns_FacebookJsb_proto = nullptr;
static se::Class* __jsb_ns_FacebookJsb_class = nullptr;

static bool js_FacebookJsb_finalize(se::State& s)
{
	cg::FacebookJsb* cobj = (cg::FacebookJsb*)s.nativeThisObject();
	delete cobj;
	return true;
}

SE_BIND_FINALIZE_FUNC(js_FacebookJsb_finalize)

static bool js_FacebookJsb_constructor(se::State& s)
{
	LoadFacebookDll();
	
	cg::FacebookJsb* cobj = new cg::FacebookJsb();
	s.thisObject()->setPrivateData(cobj);

	const auto& args = s.args();
	int argc = (int)args.size();
	if (argc > 0) 
	{
		bool ok = false;
		ok = seval_to_std_string(args[0], &cobj->appId);
	}

	if (argc > 1)
	{
		cobj->width = args[1].toInt32();
	}

	if (argc > 2)
	{
		cobj->height = args[2].toInt32();
	}

	return true;
}
SE_BIND_CTOR(js_FacebookJsb_constructor, __jsb_ns_FacebookJsb_class, js_FacebookJsb_finalize)

static bool js_FacebookJsb_logout(se::State& s)
{
	cg::FacebookJsb* cobj = (cg::FacebookJsb*)s.nativeThisObject();

	ClearData();

	return true;
}
SE_BIND_FUNC(js_FacebookJsb_logout)

static bool js_FacebookJsb_get_token(se::State& s)
{
	cg::FacebookJsb* cobj = (cg::FacebookJsb*)s.nativeThisObject();
	s.rval().setString(cobj->token);
	return true;
}
SE_BIND_PROP_GET(js_FacebookJsb_get_token)

static bool js_FacebookJsb_set_token(se::State& s)
{
	const auto& args = s.args();
	int argc = (int)args.size();
	if (argc > 0)
	{
		cg::FacebookJsb* cobj = (cg::FacebookJsb*)s.nativeThisObject();

		bool ok = false;
		ok = seval_to_std_string(args[0], &cobj->token);
		SE_PRECONDITION2(ok, false, "args[0] isn't a string.");
		return true;
	}

	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
SE_BIND_PROP_SET(js_FacebookJsb_set_token)

static bool js_FacebookJsb_login(se::State& s)
{
	const auto& args = s.args();
	int argc = (int)args.size();
	if (argc >= 1)
	{
		cg::FacebookJsb* cobj = (cg::FacebookJsb*)s.nativeThisObject();

		se::Value jsFunc = args[0];
		se::Value jsTarget = argc > 1 ? args[1] : se::Value::Undefined;

		assert(jsFunc.isObject() && jsFunc.toObject()->isFunction());

		// If the current SomeClass is a class that can be created by `new`, we use se::Object::attachObject to associate jsFunc with jsTarget to the current object.
		s.thisObject()->attachObject(jsFunc.toObject());

		// If the current SomeClass class is a singleton, or a class that always has only one instance, we can not associate it with se::Object::attachObject.
		// Instead, you must use se::Object::root, developers do not need to unroot since unroot operation will be triggered in the destruction of lambda which makes the se::Value jsFunc be destroyed, then se::Object destructor will do the unroot operation automatically.
		// The binding function `js_cocos2dx_EventDispatcher_addCustomEventListener` implements it in this way because `EventDispatcher` is always a singleton.
		// Using s.thisObject->attachObject(jsFunc.toObject); for binding addCustomEventListener will cause jsFunc and jsTarget varibales can't be released, which will result in memory leak.

		// jsFunc.toObject()->root();
		// jsTarget.toObject()->root();

		cobj->setCallback([jsFunc, jsTarget](std::string token) {

			// Add the following two lines of code in CPP callback function before passing data to the JS.
			se::ScriptEngine::getInstance()->clearException();
			se::AutoHandleScope hs;
			//

			se::ValueArray args;
			args.push_back(se::Value(token));

			se::Object* target = jsTarget.isObject() ? jsTarget.toObject() : nullptr;
			jsFunc.toObject()->call(args, target);
		});
		
		FacebookLogin(cobj->appId, cobj, cobj->width, cobj->height);

		return true;
	}

	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
SE_BIND_FUNC(js_FacebookJsb_login)

bool js_register_ns_FacebookJsb(se::Object* global)
{
	// Make sure the namespace exists
	se::Value nsVal;
	if (!global->getProperty("cg", &nsVal))
	{
		// If it doesn't exist, create one. Similar as `var ns = {};` in JS.
		se::HandleObject jsobj(se::Object::createPlainObject());
		nsVal.setObject(jsobj);

		// Set the object to the global object with the property name `ns`.
		global->setProperty("cg", nsVal);
	}
	se::Object* ns = nsVal.toObject();

	// Create a se::Class object, developers do not need to consider the release of the se::Class object, which is automatically handled by the ScriptEngine.
	auto cls = se::Class::create("Facebook", ns, nullptr, _SE(js_FacebookJsb_constructor)); // If the registered class doesn't need a  constructor, the last argument can be passed in with nullptr, it will make  `new SomeClass();` illegal.

	// Define member functions, member properties.
	cls->defineProperty("token", _SE(js_FacebookJsb_get_token), _SE(js_FacebookJsb_set_token));
	cls->defineFunction("login", _SE(js_FacebookJsb_login));
	cls->defineFunction("logout", _SE(js_FacebookJsb_logout));

	// Define finalize callback function
	cls->defineFinalizeFunction(_SE(js_FacebookJsb_finalize));

	// Install the class to JS virtual machine
	cls->install();

	// JSBClassType::registerClass is a helper function in the Cocos2D-X native binding code, which is not a part of the ScriptEngine.
	JSBClassType::registerClass<cg::FacebookJsb>(cls);

	// Save the result to global variable for easily use in other places, for example class inheritence.
	__jsb_ns_FacebookJsb_proto = cls->getProto();
	__jsb_ns_FacebookJsb_class = cls;

	// Clear JS exceptions
	se::ScriptEngine::getInstance()->clearException();
	return true;
}