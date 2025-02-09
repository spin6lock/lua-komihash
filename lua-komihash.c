#include <inttypes.h>
#include <lua.h>
#include <lauxlib.h>
#include "komihash/komihash.h"

static int
lhash64(lua_State *L) {
    size_t len;
    const char *str = luaL_checklstring(L, 1, &len);
    uint64_t ret = komihash(str, len, luaL_optinteger(L, 2, 0));
    lua_pushlstring(L, (const char *)&ret, sizeof(uint64_t));
    return 1;
}

struct rand {
    uint64_t seed1;
    uint64_t seed2;
};

#define MT_NAME ("_komihash_metatable")

static int
lrand(lua_State *L) {
    struct rand *r = luaL_checkudata(L, 1, MT_NAME);
    uint64_t ret = komirand(&r->seed1, &r->seed2);
    lua_pushlstring(L, (const char *)&ret, sizeof(uint64_t));
    return 1;
}

static int
lmetatable(lua_State *L) {
    if (luaL_newmetatable(L, MT_NAME)) {
        luaL_Reg l[] = {
            {"rand", lrand},
            { NULL, NULL }
        };
        luaL_newlib(L, l);
        lua_setfield(L, -2, "__index");
    }
    return 1;
}

static int
lnew(lua_State *L) {
    struct rand *r = lua_newuserdatauv(L, sizeof(struct rand), 0);
    r->seed1 = luaL_checkinteger(L, 1);
    r->seed2 = luaL_checkinteger(L, 2);
    lmetatable(L);
    lua_setmetatable(L, -2);
    return 1;
}

LUAMOD_API int
luaopen_lkomihash(lua_State *L) {
    luaL_checkversion(L);
    luaL_Reg l[] = {
        { "hash64", lhash64 },
        { "new", lnew },
        { NULL, NULL },
    };
    luaL_newlib(L, l);
    return 1;
}
