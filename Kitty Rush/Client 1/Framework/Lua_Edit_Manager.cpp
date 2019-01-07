#include "Lua_Edit_Manager.h"

tuple<string, int, int> Lua_Edit_Manager::window_Data()
{
	lua_State* F = luaL_newstate();
	luaL_dofile(F, "Lua_Edit_Manager.lua");
	luaL_openlibs(F);
	lua_pcall(F, 0, 0, 0);

	LuaRef t = getGlobal(F, "Window");
	LuaRef title = t["Title"];
	
	LuaRef w = t["Width"];
	LuaRef h = t["Height"];

	string titleString = title.cast<string>();
	int width = w.cast<int>();
	int height = h.cast<int>();

	tuple<string, int, int> window(titleString, width, height);
	return window;
}
tuple<string> Lua_Edit_Manager::texture_Data()
{
	lua_State* G = luaL_newstate();
	luaL_dofile(G, "Lua_Edit_Manager.lua");
	luaL_openlibs(G);
	lua_pcall(G, 0, 0, 0);

	LuaRef t = getGlobal(G, "Player1");
	LuaRef texture = t["texture"];

	string texString = texture.cast<string>();
	tuple<string> setTexture(texString);
	return setTexture;
}
tuple<string> Lua_Edit_Manager::texture_Background()
{
	lua_State* B = luaL_newstate();
	luaL_dofile(B, "Lua_Edit_Manager.lua");
	luaL_openlibs(B);
	lua_pcall(B, 0, 0, 0);

	LuaRef t = getGlobal(B, "Background");
	LuaRef texturebg = t["background"];

	string texString = texturebg.cast<string>();
	tuple<string> setTexture(texString);
	return setTexture;
}
