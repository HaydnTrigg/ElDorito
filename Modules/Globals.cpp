#include "Globals.h"

#include "../ElDorito.h"
#include "../BlamTypes.h"

// STL
#include <inttypes.h>
#include <iostream>


Globals::Globals()
{
	setupPhysicsGlobals();
	setupGraphicsGlobals();
	setupMiscGlobals();
}

Globals::~Globals()
{

}

std::string Globals::Info()
{
	std::string info("Allows interaction with globals.  Available globals:\n");
	for(auto &i : commands)
	{
		info += "  -" + i.first + ":" + i.second.Info();
	}

	return info;
}

void Globals::Tick(const std::chrono::duration<double>& dt)
{

}

bool Globals::Run(const std::vector<std::string>& args)
{
	// Enable graphics overrides
	ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0](GameGlobals::Graphics::GraphicsOverrideIndex).Write<bool>(true);

	if(args.size() >= 3) // Only accept "global <commmand> <args>
	{
		auto &commandItr = commands.find(args[1]);
		if(commandItr != commands.end()) // If our command exists
		{
			CommandLineArgs commandArgs(args.begin() + 2, args.end());

			// Run our command 
			return commandItr->second.Run(commandArgs);
		}
	}
 
	return false;
}

void Globals::setupGraphicsGlobals()
{
	///
	// Color
	///
	commands["color"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer GraphicsPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &rPtr = GraphicsPtr(GameGlobals::Graphics::ColorIndex);
				Pointer &gPtr = GraphicsPtr(GameGlobals::Graphics::ColorIndex + 4);
				Pointer &bPtr = GraphicsPtr(GameGlobals::Graphics::ColorIndex + 8);

				if(args[0] == "reset")
				{
					float rgb[3] = { 1.f, 1.f, 1.f };

					rPtr.Write(1.f);
					gPtr.Write(1.f);
					bPtr.Write(1.f);

					std::cout << "Color reset to " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << "." << std::endl;
					return true;
				}
				else if(args.size() >= 3)
				{
					try
					{
						float rgb[3];
						for(unsigned i = 0; i < 3; ++i)
							rgb[i] = std::stof(args[i]);

						rPtr.Write(rgb[0]);
						gPtr.Write(rgb[1]);
						bPtr.Write(rgb[2]);

						std::cout << "Color set to " << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << "." << std::endl;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
					return true;
				}
			}

			return false;
		},
			[]() // Info
		{
			return  R"(
	Usage: color (r|'reset') (g) (b)
	Sets the color values of the screen to given
	RGB values (0.0 - 1.0).
	Pass "reset" to reset to the default color.
)";
		}
	};

	///
	// Saturation
	///
	commands["saturation"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer GraphicsPtr = ElDorito::GetMainTls(GameGlobals::Graphics::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &saturationPtr = GraphicsPtr(GameGlobals::Graphics::SaturationIndex);

				if(args[0] == "reset")
				{
					saturationPtr.Write(1.f);
					std::cout << "Saturation reset to 1." << std::endl;
					return true;
				}
				else
				{
					try
					{
						float newSaturation = std::stof(args[0]);
						saturationPtr.Write(newSaturation);
						std::cout << "Saturation reset to " << newSaturation << "." << std::endl;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
					return true;
				}
			}

			return false;
		},
			[]() // Info
		{
			return  R"(
	Usage: saturation (number|'reset')
	Sets the saturation value of the screen to given number (0.0 - 1.0).
	Pass "reset" to reset to the default saturation.
)";
		}
	};

	///
	// Letterbox
	///
	commands["letterbox"] = {
		[this](const CommandLineArgs &args) // Run
		{
			Pointer CinimaticPtr = ElDorito::GetMainTls(GameGlobals::Cinematic::TLSOffset)[0];
			if(args.size() >= 1)
			{
				Pointer &saturationPtr = CinimaticPtr(GameGlobals::Cinematic::LetterboxIndex);

				if(args[0] == "on" || args[0] == "true")
				{
					saturationPtr.Write(true);
					std::cout << "Letterboxing enabled." << std::endl;
					return true;
				}
				else if(args[0] == "off" || args[0] == "false")
				{
					saturationPtr.Write(false);
					std::cout << "Letterboxing enabled." << std::endl;
					return true;
				}
			}

			return false;
		},
			[]() // Info
		{
			return  R"(
	Usage: letterbox (on|off)
	Enables letterboxing.
)";
		}
	};

	///
	// Test
	///
	//commands["test"] = {
	//	[this](const CommandLineArgs &args) // Run
	//	{
	//    Pointer GraphicsPtr = ElDorito::GetMainTls(0x104)[0];
	//    if(args.size() > 1)
	//    {
	//      
	//      int index = std::atoi(args[0].c_str());
	//      
	//    }
	//    else
	//    {
	//      for(unsigned i = 0; i < 4; ++i)
	//      {
	//        std::cout << "Debug Value " << i << ": " << GraphicsPtr(i * 4).Read<float>() << std::endl;
	//      }
	//    }
	//    return true;
	//	},
	//		[]() // Info
	//	{
	//    return  R"(
	//Usage: you probably shouldn't use this
	//)";
	//	}
	//};
}

void Globals::setupPhysicsGlobals()
{
	///
	// Gravity
	///
	commands["gravity"] = {
		[this](const CommandLineArgs &args) // Run
		{
			if(args.size() >= 1)
			{
				Pointer &gravityPtr = ElDorito::GetMainTls(GameGlobals::Physics::TLSOffset)[0](GameGlobals::Physics::GravityIndex);

				if(args[0] == "reset")
				{
					gravityPtr.Write(GameGlobals::Physics::DefualtGravity);
					std::cout << "Gravity reset to " << GameGlobals::Physics::DefualtGravity << "." << std::endl;
				}
				else
				{
					try
					{
						float newGravity = std::stof(args[0]);
						gravityPtr.Write(newGravity);

						std::cout << "Gravity set to " << newGravity << "." << std::endl;
					}
					catch(std::invalid_argument&)
					{
						return false;
					}
				}
				return true;
			}

			return false;
		},
			[]() // Info
		{
			return  R"(
	Usage: gravity (number|'reset')
	Sets the gravity of the game to a given number.
	Pass "reset" to reset to the default gravity.
)";
		}
	};
}

void Globals::setupMiscGlobals()
{

}