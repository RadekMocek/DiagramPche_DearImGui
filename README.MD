# DiagramPche :: Dear ImGui

## TODO

* ~~Replace 'unknown' enum values with std::optional~~
* Add app icon
* Horizontal scrolling problem (Shift + mousewheel)
  * Can scroll like this in textedit even when all text is visible
  * Long error → can scroll the whole app
* ResizeX textedit not ideal, changing window size does not keep textedit:canvas ratio
* Check \n vs \r\n
* Error highlight breaks on tab

## Dependencies

* [Dear ImGui](https://github.com/ocornut/imgui) ([MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
* [GLFW](https://www.glfw.org/) ([zlib](https://www.glfw.org/license))
* [glad](https://github.com/Dav1dde/glad) ([MIT](https://github.com/Dav1dde/glad/blob/glad2/LICENSE))
* [FreeType](https://gitlab.freedesktop.org/freetype/freetype) ([Custom](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/FTL.TXT))
* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))
* [toml++](https://github.com/marzer/tomlplusplus) ([MIT](https://github.com/marzer/tomlplusplus/blob/master/LICENSE))

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
