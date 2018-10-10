
solution "LuaChinese"
	location ( "build" )
	targetdir("build")
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	characterset ("MBCS")
	
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols", "ExtraWarnings"}

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize", "ExtraWarnings"}			
	
	
   	project "lualib51"
		language "C++"
		kind "StaticLib"
		includedirs { "lua51","./" }
		files { "lua51/*.h","lua51/*.c","lua51/*.cpp","lconvert.*" }
		removefiles {"lua51/luac.c","lua51/lua.c"}
		
	project "lua51"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua51" }
		files { "lua51/lua.c" }
		links { "lualib51"}	
			
	project "luac51"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua51" }
		files { "lua51/luac.c" }
		links { "lualib51"}	

   	project "lualib52"
		language "C++"
		kind "StaticLib"
		includedirs { "lua52","./" }
		files { "lua52/*.h","lua52/*.c","lua52/*.cpp","lconvert.*" }
		removefiles {"lua52/luac.c","lua52/lua.c"}

	project "lua52"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua52" }
		files { "lua52/lua.c" }
		links { "lualib52"}	
			
	project "luac52"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua52" }
		files { "lua52/luac.c" }
		links { "lualib52"}	
		
		
   	project "lualib53"
		language "C++"
		kind "StaticLib"
		includedirs { "lua53","./" }
		files { "lua53/*.h","lua53/*.c","lua53/*.cpp","lconvert.*" }
		removefiles {"lua53/luac.c","lua53/lua.c"}

	project "lua53"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua53" }
		files { "lua53/lua.c" }
		links { "lualib53"}	
			
	project "luac53"
		kind "ConsoleApp"
		language "C++"
		includedirs { "lua53" }
		files { "lua53/luac.c" }
		links { "lualib53"}		
		
		
	project "convert"
		kind "ConsoleApp"
		language "C++"
		files { "convert.cpp","lconvert.*" }		
			



