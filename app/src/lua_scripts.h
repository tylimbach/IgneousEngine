#pragma once

#include <string>
#include <lua.hpp>
#include <engine.h>

namespace demo {
	static bool checkLua(lua_State* L, int r) {
		if (r != LUA_OK) {
			std::string errormsg = lua_tostring(L, -1);
			LOG_ERROR(errormsg);
			return false;
		}
		return true;
	}

	static int lua_HostFunction(lua_State* L) {
		float a = (float)lua_tonumber(L, 1);
		float b = (float)lua_tonumber(L, 2);
		LOG_INFO("[CPP S4] HostFunction({},{}) called from Lua", a, b);
		float c = a * b;
		lua_pushnumber(L, c);
		return 1;
	}

	static void demo_script() {
		lua_State* L = luaL_newstate();

		luaL_openlibs(L);

		lua_register(L, "HostFunction", lua_HostFunction);

		/*
		if (checkLua(L, luaL_dofile(L, "demo.lua"))) {
			// Stage 1: Just read simple variables
			lua_getglobal(L, "entities");
			if (lua_istable(L, -1)) {
				while (lua_next(L, -2) != 0) {
					// Here, 'key' is at index -2 and 'value' is at index -1
					// Note: lua_tostring on a key could mess up lua_next iteration if the key is not a string.
					// Use lua_typename to safely get the type name without modifying the key
					const char* key = lua_tostring(L, -2);  // Get the key as a string
					const char* valueTypeName = lua_typename(L, lua_type(L, -1));  // Get type of value

					// Do something with the key-value pair
					std::cout << "Key: " << key << ". Value type: " << valueTypeName << std::endl;

					if (strcmp(valueTypeName, "string") == 0) {
						const char* value = lua_tostring(L, -1);
						std::cout << "Value: " << value << std::endl;
					}
					else if (strcmp(valueTypeName, "number") == 0) {
						double value = lua_tonumber(L, -1);
						std::cout << "Value: " << value << std::endl;
					}
					// Handle other types as needed (boolean, table, etc.)

					lua_pop(L, 1);  // Remove 'value'; keep 'key' for next iteration
				}
			}

			// Stage 2: Read Table Object
			lua_getglobal(L, "player");
			if (lua_istable(L, -1)) {
				lua_pushstring(L, "Name");
				lua_gettable(L, -2);
				player.name = lua_tostring(L, -1);
				lua_pop(L, 1);

				lua_pushstring(L, "Family");
				lua_gettable(L, -2);
				player.family = lua_tostring(L, -1);
				lua_pop(L, 1);

				lua_pushstring(L, "Title");
				lua_gettable(L, -2);
				player.title = lua_tostring(L, -1);
				lua_pop(L, 1);

				lua_pushstring(L, "Level");
				lua_gettable(L, -2);
				player.level = (int)lua_tointeger(L, -1);
				lua_pop(L, 1);
			}
			std::cout << "[CPP S2] " << player.title << " " << player.name << " of " << player.family << " [Lvl: " << player.level << "]" << std::endl << std::endl;

			// Stage 3: Call Lua Function
			std::cout << "[CPP] Stage 3 - Call Lua Function" << std::endl;
			lua_getglobal(L, "CalledFromCPP1");
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, 5.0f);
				lua_pushnumber(L, 6.0f);
				lua_pushstring(L, "Bwa ha ha!");
				std::cout << "[CPP S3] Calling 'CalledFromCPP1' in lua script" << std::endl;
				if (CheckLua(L, lua_pcall(L, 3, 1, 0)))
				{
					std::cout << "[CPP S3] 'CalledFromCPP1' returned " << (float)lua_tonumber(L, -1) << std::endl << std::endl;
				}
			}

			// Stage 4: Call Lua Function, which calls C++ Function
			std::cout << "[CPP] Stage 4 - Call Lua Function, whcih in turn calls C++ Function" << std::endl;
			lua_getglobal(L, "CalledFromCPP2");
			if (lua_isfunction(L, -1))
			{
				lua_pushnumber(L, 5.0f);
				lua_pushnumber(L, 6.0f);
				std::cout << "[CPP S4] Calling 'CalledFromCPP2' in lua script" << std::endl;
				if (CheckLua(L, lua_pcall(L, 2, 1, 0)))
				{
					std::cout << "[CPP S4] 'CalledFromCPP2' returned " << (float)lua_tonumber(L, -1) << std::endl << std::endl;
				}
			}
		}
		*/
		system("pause");
		lua_close(L);
	}
}