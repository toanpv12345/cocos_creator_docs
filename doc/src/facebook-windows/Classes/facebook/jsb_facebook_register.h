#pragma once

#include <string>
#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "facebook//Facebook.h"

namespace cg {
	class FacebookJsb: public IBrowserEvent
	{
	public:
		FacebookJsb()
			: token("")
		{}

		void onLogin(std::string token)
		{
			this->token = token;
			_cb(token);
		}

		void setCallback(const std::function<void(std::string)>& cb) {
			_cb = cb;
		}

		std::string token;
		std::string appId;
		int width;
		int height;
	private:
		std::function<void(std::string)> _cb;
	};
}

bool js_register_ns_FacebookJsb(se::Object* global);
