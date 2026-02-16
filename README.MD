# DiagramPche :: Dear ImGui

## Problems

* InputTextMultiline problems:
  * Loses focus on AltGR
  * Shift + mousewheel allows you to do small amount of horizontal scroll (make an issue?)
    * `ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));` resolves it, but not ideal
  * Horizontal scrolling is problematic [...](https://github.com/hoffstadt/DearPyGui/issues/2381#issuecomment-2303922790)
* Error highlight breaks on tab
* All nodes disappear instead of showing last valid nodes when TOML parsing error occurs

## Dependencies

* [Dear ImGui](https://github.com/ocornut/imgui) ([MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
* [GLFW](https://www.glfw.org/) ([zlib](https://www.glfw.org/license))
* [glad](https://github.com/Dav1dde/glad) ([MIT](https://github.com/Dav1dde/glad/blob/glad2/LICENSE))
* [FreeType](https://gitlab.freedesktop.org/freetype/freetype) ([Custom](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/FTL.TXT))
* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))
* [toml++](https://github.com/marzer/tomlplusplus) ([MIT](https://github.com/marzer/tomlplusplus/blob/master/LICENSE))
* [stb](https://github.com/nothings/stb) ([MIT](https://github.com/nothings/stb/blob/master/LICENSE))

<!--
NODE:
*   Rich text? text=["abcd","red12b","defghi","green13i"]; nebo [[style]]
    that covers text color, size, [bold/italic/...]

*   rectangle round corners
*   rectangle edge color
*   rectangle edge style (dotted?)

*   other shapes than rectangle
*   custom image

PATH:     
     * path style: dotted/...
     * path style: thickness
     * path style: color
     * path ends (arrows etc.)
     * curves?
-->
