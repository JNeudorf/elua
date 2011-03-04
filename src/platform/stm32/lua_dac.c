// Module to use STM32 DAC on board

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "platform.h"
#include "auxmods.h"
#include "modcommon.h"
#include "lrotable.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "platform_conf.h"
#include "stm32f10x.h"

static int dac_init(lua_State * L)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DAC_InitTypeDef  DAC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  GPIO_InitStructure = (const GPIO_InitTypeDef) {
    .GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5,
    .GPIO_Mode  = GPIO_Mode_AIN,
    .GPIO_Speed = GPIO_Speed_2MHz,
  };
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  DAC_InitStructure = (const DAC_InitTypeDef) {
     .DAC_Trigger = DAC_Trigger_None ,
     .DAC_WaveGeneration = DAC_WaveGeneration_None, 
     .DAC_LFSRUnmask_TriangleAmplitude     = DAC_TriangleAmplitude_1,
     .DAC_OutputBuffer = DAC_OutputBuffer_Enable,
  };

  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);

	return 0;
}

//set 2 channels at once.
//dac0, dac 1 should be integers from 0 to 4095
static int dac_set(lua_State * L)
{
    u16  dac0 = luaL_checkint(L, 1);
    u16  dac1 = luaL_checkint(L, 2);

    DAC_SetDualChannelData(DAC_Align_12b_R ,dac0, dac1 );

    return 0;
}

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE dac_map[] =
{
	{ LSTRKEY( "init" ), LFUNCVAL( dac_init ) },
	{ LSTRKEY( "set" ),  LFUNCVAL( dac_set  ) },
#if LUA_OPTIMIZE_MEMORY > 0
        { LSTRKEY( "__metatable" ), LROVAL( dac_map ) },
#endif
	{ LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_dac(lua_State * L)
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_DAC, dac_map );
  
  // Set this table as its own metatable
  lua_pushvalue( L, -1 );
  lua_setmetatable( L, -2 );
  
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0  
}

