#include "stdafx.hpp"

#include "cvars.hpp"
#include "modules.hpp"
#include "hud_custom.hpp"

namespace CustomHud
{
	static SCREENINFO si;
	static float consoleColor[3] = { 1.0f, (180 / 255.0f), (30 / 255.0f) };
	static bool receivedAccurateInfo = false;
	static playerinfo player;

	static double length(double x, double y)
	{
		return std::hypot(x, y);
	}

	static double length(double x, double y, double z)
	{
		return std::sqrt((x * x) + (y * y) + (z * z));
	}

	static void UpdateScreenInfo()
	{
		si.iSize = sizeof(si);
		clientDLL.pEngfuncs->pfnGetScreenInfo(&si);
	}

	static void DrawString(int x, int y, const char* s, float r, float g, float b)
	{
		clientDLL.pEngfuncs->pfnDrawSetTextColor(r, g, b);
		clientDLL.pEngfuncs->pfnDrawConsoleString(x, y, const_cast<char*>(s));
	}

	static void DrawString(int x, int y, const char* s)
	{
		DrawString(x, y, s, consoleColor[0], consoleColor[1], consoleColor[2]);
	}

	static void DrawMultilineString(int x, int y, std::string s)
	{
		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()));
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};
	}

	static void UpdateConsoleColor()
	{
		if (con_color_.IsEmpty())
			return;

		unsigned r = 0, g = 0, b = 0;
		std::istringstream ss(con_color_.GetString());
		ss >> r >> g >> b;

		consoleColor[0] = r / 255.0f;
		consoleColor[1] = g / 255.0f;
		consoleColor[2] = b / 255.0f;
	}

	void Init()
	{
	}

	void VidInit()
	{
		UpdateScreenInfo();
	}

	void Draw(float flTime)
	{
		if (!y_bxt_hud.GetBool())
			return;

		int precision = y_bxt_hud_precision.GetInt();
		if (precision > 16)
			precision = 16;

		UpdateConsoleColor();
		
		if (y_bxt_hud_velocity.GetBool())
		{
			int x = 0, y = 0;
			if (y_bxt_hud_velocity_pos.IsEmpty())
			{
				x = -200;
			}
			else
			{
				std::istringstream pos_ss(y_bxt_hud_velocity_pos.GetString());
				pos_ss >> x >> y;
			}

			x += si.iWidth;
			
			if (receivedAccurateInfo)
				DrawString(x, y, "Velocity:");
			else
				DrawString(x, y, "Velocity:", 1.0f, 0.0f, 0.0f);

			y += si.iCharHeight;
			
			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "X: " << player.velocity[0] << "\n"
				<< "Y: " << player.velocity[1] << "\n"
				<< "Z: " << player.velocity[2] << "\n"
				<< "XY: " << length(player.velocity[0], player.velocity[1]) << "\n"
				<< "XYZ: " << length(player.velocity[0], player.velocity[1], player.velocity[2]);

			DrawMultilineString(x, y, out.str());
		}

		if (y_bxt_hud_origin.GetBool())
		{
			int x = 0, y = 0;
			if (y_bxt_hud_origin_pos.IsEmpty())
			{
				x = -200;
				y = (si.iCharHeight * 6) + 1;
			}
			else
			{
				std::istringstream pos_ss(y_bxt_hud_origin_pos.GetString());
				pos_ss >> x >> y;
			}

			x += si.iWidth;

			if (receivedAccurateInfo)
				DrawString(x, y, "Origin:");
			else
				DrawString(x, y, "Origin:", 1.0f, 0.0f, 0.0f);

			y += si.iCharHeight;

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "X: " << player.origin[0] << "\n"
				<< "Y: " << player.origin[1] << "\n"
				<< "Z: " << player.origin[2];

			DrawMultilineString(x, y, out.str());
		}

		receivedAccurateInfo = false;
	}

	void UpdatePlayerInfo(float vel[3], float org[3])
	{
		player.velocity[0] = vel[0];
		player.velocity[1] = vel[1];
		player.velocity[2] = vel[2];
		player.origin[0] = org[0];
		player.origin[1] = org[1];
		player.origin[2] = org[2];

		receivedAccurateInfo = true;
	}

	void UpdatePlayerInfoInaccurate(float vel[3], float org[3])
	{
		if (!receivedAccurateInfo)
		{
			player.velocity[0] = vel[0];
			player.velocity[1] = vel[1];
			player.velocity[2] = vel[2];
			player.origin[0] = org[0];
			player.origin[1] = org[1];
			player.origin[2] = org[2];
		}
	}
}

int CHudCustom_Wrapper::Init()
{
	CustomHud::Init();

	m_Initialized = true;
	m_iFlags = HUD_ACTIVE;
	clientDLL.AddHudElem(this);

	return 1;
}

int CHudCustom_Wrapper_NoVD::Init()
{
	CustomHud::Init();

	m_Initialized = true;
	m_iFlags = HUD_ACTIVE;
	clientDLL.AddHudElem(this);

	return 1;
}
