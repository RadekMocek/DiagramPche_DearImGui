# DiagramPche :: Dear ImGui

## TODO

* [x] SVG export ability (Nodes, Multiline text, Paths, Arrow tips)
* [x] SVG export proper dialog
* [ ] Try reseting the scrolling and zoom_level just before SVG export ⇒ cleaner code
* [ ] Reorder calls in canvas loop (first user interaction, then drawing) ⇒ 0 frame delay, smoother scrolling

## Problems

* InputTextMultiline problems:
  * Loses focus on AltGR
  * Shift + mousewheel allows you to do small amount of horizontal scroll ([9249](https://github.com/ocornut/imgui/issues/9249))
  * Horizontal scrolling is problematic [...](https://github.com/hoffstadt/DearPyGui/issues/2381#issuecomment-2303922790)
* Error highlight breaks on tab
* Hacky SVG edit
  * It seems that only modern web browsers support svg fill transparency via rgba(r,g,b,a) or #RRGGBBAA, so I'm using the style="..." option, which the svg library wasn't made for, hence the "hack"

## Dependencies

* [Dear ImGui](https://github.com/ocornut/imgui) ([MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
* [GLFW](https://www.glfw.org/) ([zlib](https://www.glfw.org/license))
* [glad](https://github.com/Dav1dde/glad) ([MIT](https://github.com/Dav1dde/glad/blob/glad2/LICENSE))
* [FreeType](https://gitlab.freedesktop.org/freetype/freetype) ([Custom](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/FTL.TXT))
* [toml++](https://github.com/marzer/tomlplusplus) ([MIT](https://github.com/marzer/tomlplusplus/blob/master/LICENSE))
* [stb](https://github.com/nothings/stb) ([MIT](https://github.com/nothings/stb/blob/master/LICENSE))
* [simple-svg](https://github.com/adishavit/simple-svg), modified ([BSD-3-Clause](https://github.com/adishavit/simple-svg/blob/master/LICENSE))
* [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended) ([zlib](https://github.com/btzy/nativefiledialog-extended/blob/master/LICENSE))

<br>

* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))

## Building locally

### Debian based distros

```bash
sudo apt install cmake ninja-build libdbus-1-dev

# Navigate to the root directory of this repository

cmake -S . -B build

cmake --build build
```

### Windows

* The most straightforward way is probably to use an IDE:
  * In CLion, go to _File_ → _Open..._ and select root directory of this repository
  * In Visual Studio's launcher, click on the _Open a local folder_ button and select root directory of this repository

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
