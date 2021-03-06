/****************************************************************************
 *  Genesis Plus 1.2a
 *
 *  Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003  Charles Mac Donald
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Nintendo Gamecube Menus
 * Please put any user menus here! - softdev March 12 2006
 ***************************************************************************/

#include "shared.h"
#include "dvd.h"
#include "font.h"
#include "history.h"

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#include <di/di.h>
#endif

/***************************************************************************
 * drawmenu
 *
 * As it says, simply draws the menu with a highlight on the currently
 * selected item :)
 ***************************************************************************/
char menutitle[60] = { "" };
int menu = 0;

void drawmenu (char items[][25], int maxitems, int selected)
{
  int i;
  int ypos;

  ypos = (310 - (fheight * maxitems)) >> 1;
  ypos += 130;

  ClearScreen ();
  WriteCentre (134, menutitle);

  for (i = 0; i < maxitems; i++)
  {
      if (i == selected) WriteCentre_HL (i * fheight + ypos, (char *) items[i]);
      else WriteCentre (i * fheight + ypos, (char *) items[i]);
  }

  SetScreen ();
}

/****************************************************************************
 * domenu
 *
 * Returns index into menu array when A is pressed, -1 for B
 ****************************************************************************/
int domenu (char items[][25], int maxitems, u8 fastmove)
{
  int redraw = 1;
  int quit = 0;
  short p;
  int ret = 0;

  while (quit == 0)
  {
    if (redraw)
    {
      drawmenu (&items[0], maxitems, menu);
      redraw = 0;
    }
    
    p = ogc_input__getMenuButtons();
    
    if (p & PAD_BUTTON_UP)
    {
      redraw = 1;
      menu--;
      if (menu < 0) menu = maxitems - 1;
    }
    else if (p & PAD_BUTTON_DOWN)
    {
      redraw = 1;
      menu++;
      if (menu == maxitems) menu = 0;
    }

    if (p & PAD_BUTTON_A)
    {
      quit = 1;
      ret = menu;
    }
    else if (p & PAD_BUTTON_B)
    {
      quit = 1;
      ret = -1;
    }

    if (fastmove)
    {
      if (p & PAD_BUTTON_RIGHT)
      {
        quit = 1;
        ret = menu;
      }
      else if (p & PAD_BUTTON_LEFT)
      {
        quit = 1;
        ret = 0 - 2 - menu;
      }
    }
  }

  return ret;
}

/****************************************************************************
 * Sound Option menu
 *
 ****************************************************************************/
extern struct ym2612__ YM2612;
void soundmenu ()
{
	int ret;
	int quit = 0;
	int prevmenu = menu;
	int count = 6;
	char items[6][25];

	strcpy (menutitle, "Press B to return");

	menu = 0;
	while (quit == 0)
	{
		sprintf (items[0], "PSG Volume: %1.2f", config.psg_preamp);
		sprintf (items[1], "FM Volume: %1.2f", config.fm_preamp);
		sprintf (items[2], "Volume Boost: %dX", config.boost);
		sprintf (items[3], "LowPass Filter: %s", config.filter ? " ON":"OFF");
		sprintf (items[4], "HQ YM2612: %s", config.hq_fm ? " ON":"OFF");
		sprintf (items[5], "FM core: %s", config.fm_core ? "GENS" : "MAME");

		ret = domenu (&items[0], count, 1);
		switch (ret)
		{
			case 0:
			case -2:
				if (ret<0) config.psg_preamp -= 0.01;
				else config.psg_preamp += 0.01;
				if (config.psg_preamp < 0.0) config.psg_preamp = 5.0;
				if (config.psg_preamp > 5.0) config.psg_preamp = 0.0;
				break;

			case 1:
			case -3:
				if (ret<0) config.fm_preamp -= 0.01;
				else config.fm_preamp += 0.01;
				if (config.fm_preamp < 0.0) config.fm_preamp = 5.0;
				if (config.fm_preamp > 5.0) config.fm_preamp = 0.0;
				break;

			case 2:
				config.boost ++;
				if (config.boost > 4) config.boost = 0;
				break;
			
			case 3:
				config.filter ^= 1;
				break;

      case 4:
				config.hq_fm ^= 1;
        if (genromsize) 
				{
					audio_init(48000);
					fm_restore();
				}
				break;

			case 5:
				config.fm_core ^= 1;
				config.psg_preamp = config.fm_core ? 2.5 : 1.5;
				config.fm_preamp  = 1.0;
				if (genromsize) 
				{
					if (!config.fm_core) memcpy(fm_reg,YM2612.REG,sizeof(fm_reg));
					audio_init(48000);
					fm_restore();
				}
				break;

      case -1:
				quit = 1;
				break;
		}
	}
	menu = prevmenu;
}

/****************************************************************************
 * Misc Option menu
 *
 ****************************************************************************/
void miscmenu ()
{
	int ret;
	int quit = 0;
	int prevmenu = menu;
	int count = 6;
	char items[6][25];
	strcpy (menutitle, "Press B to return");
	menu = 0;
	
	while (quit == 0)
	{
		if (config.region_detect == 0)	     sprintf (items[0], "Region: AUTO");
		else if (config.region_detect == 1) sprintf (items[0], "Region:  USA");
		else if (config.region_detect == 2) sprintf (items[0], "Region:  EUR");
		else if (config.region_detect == 3) sprintf (items[0], "Region:  JAP");
		sprintf (items[1], "Force DTACK: %s", config.force_dtack ? "Y" : "N");
		if (config.bios_enabled & 1) sprintf (items[2], "Use BIOS: ON");
		else sprintf (items[2], "Use BIOS: OFF");
		sprintf (items[3], "SVP Cycles: %d", SVP_cycles);
		if (config.sram_auto == 0) sprintf (items[4], "Auto SRAM: SDCARD");
		else if (config.sram_auto == 1) sprintf (items[4], "Auto SRAM: MCARD A");
		else if (config.sram_auto == 2) sprintf (items[4], "Auto SRAM: MCARD B");
		else sprintf (items[4], "Auto SRAM: OFF");
		if (config.freeze_auto == 0) sprintf (items[5], "Auto FREEZE: SDCARD");
		else if (config.freeze_auto == 1) sprintf (items[5], "Auto FREEZE: MCARD A");
		else if (config.freeze_auto == 2) sprintf (items[5], "Auto FREEZE: MCARD B");
		else sprintf (items[5], "Auto FREEZE: OFF");

		ret = domenu (&items[0], count, 1);
		switch (ret)
		{
			case 0:	/*** Region Force ***/
				config.region_detect = (config.region_detect + 1) % 4;
				if (genromsize)
				{
					/* force region & cpu mode */
					set_region();
					
					/* reinitialize timings */
					system_init ();
					audio_init(48000);
					fm_restore();
									
					/* reinitialize HVC tables */
					vctab = (vdp_pal) ? ((reg[1] & 8) ? vc_pal_240 : vc_pal_224) : vc_ntsc_224;
					hctab = (reg[12] & 1) ? cycle2hc40 : cycle2hc32;

					/* reinitialize overscan area */
					bitmap.viewport.x = config.overscan ? ((reg[12] & 1) ? 16 : 12) : 0;
					bitmap.viewport.y = config.overscan ? (((reg[1] & 8) ? 0 : 8) + (vdp_pal ? 24 : 0)) : 0;
					bitmap.viewport.changed = 1;
				}
				break;

			case 1:	/*** force DTACK ***/
				config.force_dtack ^= 1;
				break;

			case 2:	/*** BIOS support ***/
        config.bios_enabled ^= 1;
        system_reset ();
				break;

			case 3:	/*** SVP emulation ***/
			case -5:
				if (ret<0) SVP_cycles = SVP_cycles ? (SVP_cycles-1) : 1500;
				else SVP_cycles++;
				if (SVP_cycles > 1500) SVP_cycles = 0;
				break;

      case 4:	/*** SRAM autoload/autosave ***/
				config.sram_auto ++;
        if (config.sram_auto > 2) config.sram_auto = -1;
        break;

      case 5:	/*** FreezeState autoload/autosave ***/
        config.freeze_auto ++;
        if (config.freeze_auto > 2) config.freeze_auto = -1;
        break;

      case -1:
				quit = 1;
				break;
		}
	}

	menu = prevmenu;
}

/****************************************************************************
 * Display Option menu
 *
 ****************************************************************************/
uint8 old_overscan = 1;

void dispmenu ()
{
	int ret;
	int quit = 0;
	int prevmenu = menu;
	int count = config.aspect ? 8 : 10;
	char items[10][25];

	strcpy (menutitle, "Press B to return");
	menu = 0;

	while (quit == 0)
	{
		ogc_video__scale();

    sprintf (items[0], "Aspect: %s", config.aspect ? "ORIGINAL" : "STRETCH");
		if (config.render == 1) sprintf (items[1], "Render: INTERLACED");
		else if (config.render == 2) sprintf (items[1], "Render: PROGRESSIVE");
		else sprintf (items[1], "Render: ORIGINAL");
		if (config.tv_mode == 0) sprintf (items[2], "TV Mode: 60HZ");
		else if (config.tv_mode == 1) sprintf (items[2], "TV Mode: 50HZ");
		else sprintf (items[2], "TV Mode: 50/60HZ");
    sprintf (items[3], "Texture Filter: %s", config.filtering ? " ON" : "OFF");
		if (config.ntsc == 1) sprintf (items[4], "NTSC Filter: COMPOSITE");
		else if (config.ntsc == 2) sprintf (items[4], "NTSC Filter: S-VIDEO");
		else if (config.ntsc == 3) sprintf (items[4], "NTSC Filter: RGB");
		else sprintf (items[4], "NTSC Filter: OFF");
		sprintf (items[5], "Borders: %s", config.overscan ? " ON" : "OFF");
		sprintf (items[6], "Center X: %s%02d", config.xshift < 0 ? "-":"+", abs(config.xshift));
		sprintf (items[7], "Center Y: %s%02d", config.yshift < 0 ? "-":"+", abs(config.yshift));
		sprintf (items[8], "Scale  X: %02d", xscale*2);
		sprintf (items[9], "Scale  Y: %02d", yscale*2);

		ret = domenu (&items[0], count, 1);

		switch (ret)
		{
			case 0: /*** config.aspect ratio ***/
				config.aspect ^= 1;
        count = config.aspect ? 8 : 10;
				bitmap.viewport.changed = 1;
				break;

			case 1:	/*** rendering ***/
				config.render = (config.render + 1) % 3;
				if (config.render == 2)
				{
					if (VIDEO_HaveComponentCable())
					{
            /* progressive mode (60hz only) */
            config.tv_mode = 0;
            gc_pal = 0;
          }
          else
          {
            /* do nothing if component cable is not detected */
            config.render = 0;
          }
				}
				bitmap.viewport.changed = 1;
				break;

			case 2: /*** tv mode ***/
				if (config.render == 2) break; /* 60hz progressive only */
				config.tv_mode = (config.tv_mode + 1) % 3;
				break;
		
			case 3: /*** texture filtering ***/
				config.filtering ^= 1;
				bitmap.viewport.changed = 1;
				break;

			case 4: /*** NTSC filter ***/
				config.ntsc ++;
        if (config.ntsc > 3) config.ntsc = 0;
				bitmap.viewport.changed = 1;
				break;

      case 5: /*** overscan emulation ***/
				config.overscan ^= 1;
				bitmap.viewport.x = config.overscan ? ((reg[12] & 1) ? 16 : 12) : 0;
				bitmap.viewport.y = config.overscan ? (((reg[1] & 8) ? 0 : 8) + (vdp_pal ? 24 : 0)) : 0;
				bitmap.viewport.changed = 1;
				break;

			case 6:	/*** Center X ***/
			case -8:
				if (ret<0) config.xshift --;
				else config.xshift ++;
				break;

			case 7:	/*** Center Y ***/
			case -9:
				if (ret<0) config.yshift --;
				else config.yshift ++;
				break;
			
			case 8:	/*** Scale X ***/
			case -10:
				if (ret<0) config.xscale --;
				else config.xscale ++;
				break;

			case 9:	/*** Scale Y ***/
			case -11:
				if (ret<0) config.yscale --;
				else config.yscale ++;
				break;

			case -1:
				quit = 1;
				break;
		}
	}
	menu = prevmenu;
}

/****************************************************************************
 * ConfigureJoypads
 ****************************************************************************/
extern int old_system[2];
void ConfigureJoypads ()
{
	int ret, max_players;
	int i = 0;
  int quit = 0;
	int prevmenu = menu;
  char padmenu[8][25];

  int player = 0;
#ifdef HW_RVL
  u32 exp;
#endif

	strcpy (menutitle, "Press B to return");

	menu = 0;
	while (quit == 0)
	{
    /* update max players */
    max_players = 0;
    if (input.system[0] == SYSTEM_GAMEPAD)
    {
      sprintf (padmenu[0], "Port 1: GAMEPAD");
      max_players ++;
    }
    else if (input.system[0] == SYSTEM_MOUSE)
    {
      sprintf (padmenu[0], "Port 1: MOUSE");
      max_players ++;
    }
    else if (input.system[0] == SYSTEM_WAYPLAY)
    {
      sprintf (padmenu[0], "Port 1: 4-WAYPLAY");
      max_players += 4;
    }
    else if (input.system[0] == SYSTEM_TEAMPLAYER)
    {
      sprintf (padmenu[0], "Port 1: TEAMPLAYER");
      max_players += 4;
    }
    else
      sprintf (padmenu[0], "Port 1: NONE");

    if (input.system[1] == SYSTEM_GAMEPAD)
    {
      sprintf (padmenu[1], "Port 2: GAMEPAD");
      max_players ++;
    }
    else if (input.system[1] == SYSTEM_MOUSE)
    {
      sprintf (padmenu[1], "Port 2: MOUSE");
      max_players ++;
    }
    else if (input.system[1] == SYSTEM_WAYPLAY)
    {
      sprintf (padmenu[1], "Port 2: 4-WAYPLAY");
    }
    else if (input.system[1] == SYSTEM_TEAMPLAYER)
    {
      sprintf (padmenu[1], "Port 2: TEAMPLAYER");
      max_players += 4;
    }
    else if (input.system[1] == SYSTEM_MENACER)
    {
      sprintf (padmenu[1], "Port 2: MENACER");
      max_players += 1;
    }
    else if (input.system[1] == SYSTEM_JUSTIFIER)
    {
      sprintf (padmenu[1], "Port 2: JUSTIFIERS");
      max_players += 2;
    }
    else
      sprintf (padmenu[1], "Port 2: NONE");

    /* JCART special case */
    if (j_cart) max_players +=2;

    /* reset current player nr */
    if (player >= max_players)
    {
      /* remove duplicate assigned inputs */
      if ((0!=player) && (config.input[0].device == config.input[player].device) && (config.input[0].port == config.input[player].port))
      {
          config.input[0].device = -1;
          config.input[0].port = i%4;
      }
      player = 0;
    }

    sprintf (padmenu[2], "Gun Cursor: %s", config.gun_cursor ? " ON":"OFF");
    sprintf (padmenu[3], "Invert Mouse: %s", config.invert_mouse ? " ON":"OFF");
    sprintf (padmenu[4], "Set Player: %d%s", player + 1, (j_cart && (player > 1)) ? "-JCART" : "");

    if (config.input[player].device == 0)
      sprintf (padmenu[5], "Device: GAMECUBE %d", config.input[player].port + 1);
#ifdef HW_RVL
    else if (config.input[player].device == 1)
      sprintf (padmenu[5], "Device: WIIMOTE %d", config.input[player].port + 1);
    else if (config.input[player].device == 2)
      sprintf (padmenu[5], "Device: NUNCHUK %d", config.input[player].port + 1);
    else if (config.input[player].device == 3)
      sprintf (padmenu[5], "Device: CLASSIC %d", config.input[player].port + 1);
#endif
    else
      sprintf (padmenu[5], "Device: NONE");

    /* when using wiimote, force to 3Buttons pad */
    if (config.input[player].device == 1) input.padtype[player] = DEVICE_3BUTTON;
    sprintf (padmenu[6], "%s", (input.padtype[player] == DEVICE_3BUTTON) ? "Type: 3BUTTONS":"Type: 6BUTTONS");

    sprintf (padmenu[7], "Configure Input");

    ret = domenu (&padmenu[0], 8,0);

		switch (ret)
		{
			case 0:
        if (j_cart)
        {
          WaitPrompt("JCART detected !");
          break;
        }
        input.system[0] ++;
        if (input.system[0] == SYSTEM_MENACER) input.system[0] ++;
        if (input.system[0] == SYSTEM_JUSTIFIER) input.system[0] ++;
        if ((input.system[0] == SYSTEM_MOUSE) && (input.system[1] == SYSTEM_MOUSE)) input.system[0] ++;
        if (input.system[0] == SYSTEM_WAYPLAY) input.system[1] = SYSTEM_WAYPLAY;
        if (input.system[0] > SYSTEM_WAYPLAY)
        {
          input.system[0] = NO_SYSTEM;
          input.system[1] = SYSTEM_GAMEPAD;
        }
        io_reset();
        old_system[0] = input.system[0];
        old_system[1] = input.system[1];
			  break;
		
			case 1:
        if (j_cart)
        {
          WaitPrompt("JCART detected !");
          break;
        }
        input.system[1] ++;
        if ((input.system[0] == SYSTEM_MOUSE) && (input.system[1] == SYSTEM_MOUSE)) input.system[1] ++;
        if (input.system[1] == SYSTEM_WAYPLAY) input.system[0] = SYSTEM_WAYPLAY;
        if (input.system[1] > SYSTEM_WAYPLAY)
        {
          input.system[1] = NO_SYSTEM;
          input.system[0] = SYSTEM_GAMEPAD;
        }
        io_reset();
        old_system[0] = input.system[0];
        old_system[1] = input.system[1];
			  break;

      case 2:
        config.gun_cursor ^= 1;
        break;

      case 3:
        config.invert_mouse ^= 1;
        break;

      case 4:
        /* remove duplicate assigned inputs */
        for (i=0; i<8; i++)
        {
          if ((i!=player) && (config.input[i].device == config.input[player].device) && (config.input[i].port == config.input[player].port))
          {
            config.input[i].device = -1;
            config.input[i].port = i%4;
          }
        }
        player = (player + 1) % max_players;
        break;

      case 5:
#ifdef HW_RVL
        if (config.input[player].device > 0)
        {
          config.input[player].port ++;
        }
        else
        {
          config.input[player].device ++;
          if (config.input[player].device == 1) config.input[player].port = 0;
        }

        if (config.input[player].device == 1)
        {
          exp = 4;
          if (config.input[player].port<4)
          {
            WPAD_Probe(config.input[player].port,&exp);
            if (exp == WPAD_EXP_NUNCHUK) exp = 4;
          }

          while ((config.input[player].port<4) && (exp == 4))
          {
            config.input[player].port ++;
            if (config.input[player].port<4)
            {
              exp = 4;
              WPAD_Probe(config.input[player].port,&exp);
              if (exp == WPAD_EXP_NUNCHUK) exp = 4;
            }
          }

          if (config.input[player].port >= 4)
          {
            config.input[player].port = 0;
            config.input[player].device = 2;
          }
        }

        if (config.input[player].device == 2)
        {
          exp = 4;
          if (config.input[player].port<4)
          {
            WPAD_Probe(config.input[player].port,&exp);
          }

          while ((config.input[player].port<4) && (exp != WPAD_EXP_NUNCHUK))
          {
            config.input[player].port ++;
            if (config.input[player].port<4)
            {
              exp = 4;
              WPAD_Probe(config.input[player].port,&exp);
            }
          }

          if (config.input[player].port >= 4)
          {
            config.input[player].port = 0;
            config.input[player].device = 3;
          }
        }

        if (config.input[player].device == 3)
        {
          exp = 4;
          if (config.input[player].port<4)
          {
            WPAD_Probe(config.input[player].port,&exp);
          }

          while ((config.input[player].port<4) && (exp != WPAD_EXP_CLASSIC))
          {
            config.input[player].port ++;
            if (config.input[player].port<4)
            {
              exp = 4;
              WPAD_Probe(config.input[player].port,&exp);
            }
          }

          if (config.input[player].port >= 4)
          {
            config.input[player].port = player % 4;
            config.input[player].device = 0;
          }
        }
#else
        config.input[player].device = 0;
#endif
        break;
    
			case 6:
        if (config.input[player].device == 1) break;
        input.padtype[player] ^= 1;
        io_reset();
        break;

      case 7:
        /* special case: wiimote controls lightgun */
        
        ogc_input__config(config.input[player].port, config.input[player].device, input.padtype[player]);
        break;

			case -1:
        /* remove duplicate assigned inputs */
        for (i=0; i<8; i++)
        {
          if ((i!=player) && (config.input[i].device == config.input[player].device) && (config.input[i].port == config.input[player].port))
          {
            config.input[i].device = -1;
            config.input[i].port = i%4;
          }
        }
			  quit = 1;
			  break;
		}
	}

	menu = prevmenu;
}

/****************************************************************************
 * Main Option menu
 *
 ****************************************************************************/
void optionmenu ()
{
	int ret;
	int quit = 0;
	int prevmenu = menu;
	int count = 5;
	char items[5][25] =
  {
		"Video Options",
		"Sound Options",
		"System Options",
		"Controls Options",
		"Game Genie Codes"
	};

	menu = 0;
	while (quit == 0)
	{
		strcpy (menutitle, "Press B to return");
		ret = domenu (&items[0], count, 0);
		switch (ret)
		{
			case 0:
				dispmenu();
				break;
			case 1:
				soundmenu();
				break;
			case 2:
				miscmenu();
				break;
			case 3:
				ConfigureJoypads();
				break;
			case 4:
				GetGGEntries();
				break;
			case -1:
				quit = 1;
				break;
		}
	}

  config_save();
	menu = prevmenu;
}

/****************************************************************************
* Generic Load/Save menu
*
****************************************************************************/
static u8 device = 0;

int loadsavemenu (int which)
{
	int prevmenu = menu;
	int quit = 0;
	int ret;
	int count = 3;
	char items[3][25];

	strcpy (menutitle, "Press B to return");

	menu = 2;

  if (which == 1)
  {
    sprintf(items[1], "Save State");
    sprintf(items[2], "Load State");
  }
  else
  {
    sprintf(items[1], "Save SRAM");
    sprintf(items[2], "Load SRAM");
  }

	while (quit == 0)
	{
    if (device == 0) sprintf(items[0], "Device: SDCARD");
    else if (device == 1) sprintf(items[0], "Device: MCARD A");
    else if (device == 2) sprintf(items[0], "Device: MCARD B");

		ret = domenu (&items[0], count, 0);
		switch (ret)
		{
			case -1:
				quit = 1;
				break;

			case 0:
        device = (device + 1)%3;
				break;

			case 1:
			case 2:
				if (which == 1) quit = ManageState(ret-1,device);
				else if (which == 0) quit = ManageSRAM(ret-1,device);
				if (quit) return 1;
        break;
		}
	}

	menu = prevmenu;
	return 0;
}


/****************************************************************************
 * File Manager menu
 *
 ****************************************************************************/
int filemenu ()
{
	int prevmenu = menu;
	int ret;
	int quit = 0;
	int count = 2;
	char items[2][25] = {
		{"SRAM Manager"},
		{"STATE Manager"}
	};

	menu = 0;

	while (quit == 0)
	{
		strcpy (menutitle, "Press B to return");
		ret = domenu (&items[0], count, 0);
		switch (ret)
		{
			case -1: /*** Button B ***/
				ret = 0;
				quit = 1;
				break;
			case 0:	 /*** SRAM Manager ***/
			case 1:  /*** SaveState Manager ***/
				if (loadsavemenu(ret)) return 1;
				break;
		}
	}

	menu = prevmenu;
	return 0;
}


/****************************************************************************
 * Load Rom menu
 *
 ****************************************************************************/
static u8 load_menu = 0;

void loadmenu ()
{
	int ret;
	int quit = 0;
  int count = 4;
  char item[4][25] = {
		{"Load Recent"},
		{"Load from SDCARD"},
		{"Load from DVD"},
    {"Stop DVD Motor"}
	};

	menu = load_menu;
	
	while (quit == 0)
	{
		strcpy (menutitle, "Press B to return");
		ret = domenu (&item[0], count, 0);
		switch (ret)
		{
			case -1: /*** Button B ***/
				quit = 1;
				break;

			case 0: /*** Load Recent ***/
				quit = OpenHistory();
				break;

			case 1:  /*** Load from SCDARD ***/
				quit = OpenSD();
				break;

      case 2:	 /*** Load from DVD ***/
  			quit = OpenDVD();
        break;
  
      case 3:  /*** Stop DVD Disc ***/
        dvd_motor_off();
				break;
    }
	}

	load_menu = menu;
}

/***************************************************************************
  * Show rom info screen
 ***************************************************************************/
void showrominfo ()
{
  int ypos;
  u8 i,j,quit,redraw,max;
  char msg[128];
  short p;
  char pName[14][21];

  quit = 0;
  j = 0;
  redraw = 1;

  /*** Remove any still held buttons ***/
  while (PAD_ButtonsHeld(0))  PAD_ScanPads();
#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0)) WPAD_ScanPads();
#endif

  max = 14;
  for (i = 0; i < 14; i++)
  {
	  if (peripherals & (1 << i))
	  {
		  sprintf(pName[max-14],"%s", peripheralinfo[i].pName);
		  max ++;
	  }
  }

  while (quit == 0)
  {
      if (redraw)
      {
		  ClearScreen ();

		  ypos = 134;
		  WriteCentre(ypos, "ROM Header Information");
		  ypos += 2*fheight;

		  for (i=0; i<8; i++)
		  {
			  switch (i+j)
			  {
				case 0:
					sprintf (msg, "Console type: %s", rominfo.consoletype);
					break;
				case 1:
					sprintf (msg, "Copyright: %s", rominfo.copyright);
					break;
				case 2:
					sprintf (msg, "Company: %s", companyinfo[getcompany ()].company);
					break;
				case 3:
					sprintf (msg, "Game Domestic Name:");
					break;
				case 4:
					sprintf(msg, " %s",rominfo.domestic);
					break;
				case 5:
					sprintf (msg, "Game International Name:");
					break;
				case 6:
					sprintf(msg, " %s",rominfo.international);
					break;
				case 7:
					sprintf (msg, "Type - %s : %s", rominfo.ROMType, strcmp (rominfo.ROMType, "AI") ? "Game" : "Educational");
					break;
				case 8:
					sprintf (msg, "Product - %s", rominfo.product);
					break;
				case 9:
					sprintf (msg, "Checksum - %04x (%04x) (%s)", rominfo.checksum, realchecksum, (rominfo.checksum == realchecksum) ? "Good" : "Bad");
					break;
				case 10:
					sprintf (msg, "ROM end: $%06X", rominfo.romend);
					break;
				case 11:
					if (svp) sprintf (msg, "SVP Chip detected");
					else if (sram.custom) sprintf (msg, "EEPROM(%dK) - $%06X", ((eeprom.type.size_mask+1)* 8) /1024, (unsigned int)sram.start);
					else if (sram.detected) sprintf (msg, "SRAM Start  - $%06X", sram.start);
					else sprintf (msg, "External RAM undetected");
						 
					break;
				case 12:
					if (sram.custom) sprintf (msg, "EEPROM(%dK) - $%06X", ((eeprom.type.size_mask+1)* 8) /1024, (unsigned int)sram.end);
					else if (sram.detected) sprintf (msg, "SRAM End   - $%06X", sram.end);
					else if (sram.on) sprintf (msg, "Default SRAM activated ");
					else sprintf (msg, "SRAM is disactivated  ");
					break;
				case 13:
					if (region_code == REGION_USA) sprintf (msg, "Region - %s (USA)", rominfo.country);
					else if (region_code == REGION_EUROPE) sprintf (msg, "Region - %s (EUR)", rominfo.country);
					else if (region_code == REGION_JAPAN_NTSC) sprintf (msg, "Region - %s (JAP)", rominfo.country);
					else if (region_code == REGION_JAPAN_PAL) sprintf (msg, "Region - %s (JPAL)", rominfo.country);
					break;
				default:
					sprintf (msg, "Supports - %s", pName[i+j-14]);
					break;
			}

			write_font (100, ypos, msg);
			ypos += fheight;
		}

		ypos += fheight;
		WriteCentre (ypos, "Press A to Continue");
		SetScreen ();
	}

	p = ogc_input__getMenuButtons();
	redraw = 0;

	if ((j<(max-8)) && (p & PAD_BUTTON_DOWN)) {redraw = 1; j++;}
	if ((j>0) && (p & PAD_BUTTON_UP)) {redraw = 1; j--;}
	if (p & PAD_BUTTON_A) quit = 1;
	if (p & PAD_BUTTON_B) quit = 1;
  }
}

/****************************************************************************
 * Main Menu
 *
 ****************************************************************************/
void MainMenu ()
{
 	menu = 0;
	int ret;
	int quit = 0;
	uint32 crccheck;

	int count = 8;
	char items[8][25] =
	{
		{"Play Game"},
		{"Game Infos"},
		{"Hard Reset"},
		{"Load New Game"},
		{"File Management"},
		{"Emulator Options"},
		{"Return to Loader"},
		{"System Reboot"}
	};

	/* Switch to menu default rendering mode (60hz or 50hz, but always 480 lines) */
	VIDEO_Configure (vmode);
	VIDEO_ClearFrameBuffer(vmode, xfb[whichfb], COLOR_BLACK);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();

	while (quit == 0)
	{
    crccheck = crc32 (0, &sram.sram[0], 0x10000);
    if (genromsize && (crccheck != sram.crc)) strcpy (menutitle, "*** SRAM has been modified ***");
    else sprintf(menutitle, "%d FPS", FramesPerSecond);

		ret = domenu (&items[0], count, 0);
		switch (ret)
		{
			case -1: /*** Button B ***/
			case 0:  /*** Play Game ***/
				if (genromsize)
				{
				   quit = 1;
				}
				break;

			case 1:	 /*** ROM Information ***/
				showrominfo ();
				break;

			case 2:  /*** Emulator Reset ***/
				if (genromsize || (config.bios_enabled == 3))
				{
          system_init ();
          audio_init(48000);
          system_reset (); 
					quit = 1;
				}
				break;

			case 3:  /*** Load ROM Menu ***/
        loadmenu();
        menu = 0;
				break;

			case 4:  /*** Memory Manager ***/
				quit = filemenu ();
				break;

			case 5:  /*** Emulator Options */
				optionmenu ();
				break;

			case 6:  /*** SD/PSO/TP Reload ***/
        memfile_autosave();
        system_shutdown();
        audio_shutdown();
        VIDEO_ClearFrameBuffer(vmode, xfb[whichfb], COLOR_BLACK);
        VIDEO_Flush();
        VIDEO_WaitVSync();
#ifdef HW_RVL
        DI_Close();
#endif
        exit(0);
        break;

			case 7:  /*** Return to Wii System Menu ***/
        memfile_autosave();
        system_shutdown();
        audio_shutdown();
        VIDEO_ClearFrameBuffer(vmode, xfb[whichfb], COLOR_BLACK);
        VIDEO_Flush();
        VIDEO_WaitVSync();
#ifdef HW_RVL
        DI_Close();
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
#else
				SYS_ResetSystem(SYS_HOTRESET,0,0);
#endif
				break;
		}
	}

	/*** Remove any still held buttons ***/
  while (PAD_ButtonsHeld(0))  PAD_ScanPads();
#ifdef HW_RVL
  while (WPAD_ButtonsHeld(0)) WPAD_ScanPads();
#endif

	/*** Reinitialize GX ***/
  ogc_video__reset();

#ifndef HW_RVL
	/*** Stop the DVD from causing clicks while playing ***/
	uselessinquiry ();			
#endif
}
