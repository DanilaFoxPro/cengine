workspace "cengine"
  configurations {"Debug", "Release"}

  language "C"

  filter "configurations:Debug"
    defines {"DEBUG"}
    symbols "On"

  filter "configurations:Release"
    defines {"NDEBUG"}
    optimize "Speed"

  filter {}

  cdialect "gnu99"

  include "modules"

  newoption {
    ["trigger"] = "copy-res",
    ["description"] = "Copy resource files to output directory"
  }

  newaction {
    ["trigger"] = "clean",
    ["description"] = "Delete generated project and build files",
    ["onStart"] =
      function()
	      print "Cleaning files..."
      end,
    ["execute"] =
      function()
        if _TARGET_OS == "windows" then
          os.execute("IF EXIST bin ( RMDIR /S /Q bin )")
          os.execute("IF EXIST obj ( RMDIR /S /Q obj )")
          os.execute("for /f %i in ('dir /a:d /b modules') do rmdir /S /Q modules\\%i\\bin")
          os.execute("for /f %i in ('dir /a:d /b modules') do rmdir /S /Q modules\\%i\\obj")
          os.execute("del Makefile *.make modules\\Makefile")
        else
          os.execute("rm -rf bin obj Makefile *.make modules/Makefile modules/*/bin modules/*/obj")
        end
      end,
    ["onEnd"] =
      function()
	      print "Done."
      end
  }

  newaction {
    ["trigger"] = "build",
    ["description"] = "Build",
    ["execute"] =
      function()
        os.execute("premake5 gmake2")
        if _TARGET_OS == "windows" then
          os.execute("mingw32-make")
        else
          os.execute("make")
        end
      end
  }

  newaction {
    ["trigger"] = "dev",
    ["description"] = "Build and run",
    ["execute"] =
      function()
        os.execute("premake5 build")
        if _TARGET_OS == "windows" then
          os.execute("bin\\cengine.exe")
        else
          os.execute("./bin/cengine")
        end
      end
  }

project "cengine"
  targetdir "bin"

  files {"src/**.c", "modules/**.c"}

  includedirs {"include/", "modules/*"}
  links {"pthread", "noise"}

  disablewarnings {"trigraphs"}

  staticruntime "On"
  flags {"LinkTimeOptimization"}

  filter "configurations:Debug"
    kind "ConsoleApp"

  filter "configurations:Release"
    kind "WindowedApp"

  filter {"system:windows"}
    libdirs "lib/"
    links {"glew32s", "glfw3", "gdi32", "opengl32"}
		
  filter {"system:not windows"}
    links {"GLEW", "glfw", "rt", "m", "dl", "GL"}

  filter {}

  if _OPTIONS["copy-res"] then
    print "Copying resources"
    if _TARGET_OS == "windows" then
      os.execute("xcopy /Q /E /Y /I res bin\\res")
    else
      os.execute("mkdir -p bin/res")
      os.execute("cp -rf res bin/res")
    end
  end
