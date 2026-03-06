<#
Don't forget to:

* imconfig.h:
  * uncomment '//#define IMGUI_ENABLE_FREETYPE'
  * uncomment '//#define IMGUI_DEFINE_MATH_OPERATORS'
  * uncomment '//#define IMGUI_USE_WCHAR32'
* misc\cpp\imgui_stdlib.h:
  * add '#include "imgui.h"' above '#ifndef IMGUI_DISABLE' ?
#>

$dir = ".\imgui-1.92.6"

# Delete examples
Get-Item -LiteralPath "${dir}\examples" -ErrorAction SilentlyContinue | Remove-Item -Recurse

# Delete unneeded backends
$backend_whitelist = "imgui_impl_glfw.cpp", "imgui_impl_glfw.h", "imgui_impl_opengl3_loader.h", "imgui_impl_opengl3.cpp", "imgui_impl_opengl3.h"

Remove-Item -Path "${dir}\backends\*" -Exclude $backend_whitelist -Recurse
