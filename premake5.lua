
solution "LuaChinese"
	location ( "build" )
	configurations { "Debug", "Release" }
	platforms {"x64", "x32"}
	
   	project "lualib"
		language "C++"
		kind "StaticLib"
		includedirs { "lua" }
		files { "lua/*.h","lua/*.c","lua/*.cpp" }
		removefiles {"lua/luac.c","lua/lua.c"}
		targetdir("build")
		defines { "_CRT_SECURE_NO_WARNINGS" }
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}		

	project "lua"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua" }
		files { "lua/lua.c" }
		targetdir("build")
		links { "lualib"}

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}			
			
	project "luac"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua" }
		files { "lua/luac.c" }
		targetdir("build")
		links { "lualib"}

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}				
			
			
	project "convert"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua" }
		files { "convert.cpp" }
		targetdir("build")
		links { "lualib"}

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}				
			



