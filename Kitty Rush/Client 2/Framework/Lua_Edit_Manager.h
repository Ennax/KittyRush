#pragma once
#include <tuple>
#include <iostream>
#include <lua.hpp>
#include <LuaBridge\LuaBridge.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

using namespace luabridge;
using namespace std;

class Lua_Edit_Manager
{
public:
	Lua_Edit_Manager() {};
	~Lua_Edit_Manager() {};
	tuple<string, int, int> window_Data();
	tuple<string> texture_Data();
	tuple<string> texture_Background();
	vector<string> getElements(const string& table, lua_State* L);
};

