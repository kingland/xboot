/*
 * framework/lua_helper.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <framework/framework.h>

const char * lua_helper_get_strfield(lua_State * L, const char * key, const char * def)
{
	const char * value;

	lua_getfield(L, -1, key);
	value = luaL_optstring(L, -1, def);
	lua_pop(L, 1);

	return value;
}

lua_Number lua_helper_get_numfield(lua_State * L, const char * key, lua_Number def)
{
	lua_Number value;

	lua_getfield(L, -1, key);
	value = luaL_optnumber(L, -1, def);
	lua_pop(L, 1);

	return value;
}

lua_Integer lua_helper_get_intfield(lua_State * L, const char * key, lua_Integer def)
{
	lua_Integer value;

	lua_getfield(L, -1, key);
	value = luaL_optinteger(L, -1, def);
	lua_pop(L, 1);

	return value;
}

lua_Unsigned lua_helper_get_uintfield(lua_State * L, const char * key, lua_Unsigned def)
{
	lua_Unsigned value;

	lua_getfield(L, -1, key);
	value = luaL_optinteger(L, -1, def);
	lua_pop(L, 1);

	return value;
}

void lua_helper_set_strfield(lua_State * L, const char * key, const char * value)
{
	lua_pushstring(L, value);
	lua_setfield(L, -2, key);
}

void lua_helper_set_numfield(lua_State * L, const char * key, lua_Number value)
{
	lua_pushnumber(L, value);
	lua_setfield(L, -2, key);
}

void lua_helper_set_intfield(lua_State * L, const char * key, lua_Integer value)
{
	lua_pushinteger(L, value);
	lua_setfield(L, -2, key);
}

void lua_helper_set_uintfield(lua_State * L, const char * key, lua_Unsigned value)
{
	lua_pushunsigned(L, value);
	lua_setfield(L, -2, key);
}
