# DiagramPche :: Dear ImGui

## TODO

* General:
  * [x] View → Jump to canvas origin
  * Saving/loading:
    * [ ] New, Open, Save, Save as
    * [ ] Confirm unsaved exit
    * [ ] Confirm unsaved open, including loading a debug diagram or an example
    * [ ] Change window name according to opened document
  * [x] Code editor syntax highlight
  * [ ] Klávesové zkratky (<kbd>Alt</kbd> pro navigaci v MainMenuBar, <kbd>Ctrl</kbd>+<kbd>S</kbd> pro uložení atd.)
* Canvas features:
  * [x] Canvas zooming: anchor under mouse
  * [ ] Change the cursor type while dragging the canvas
  * [ ] Ctrl+Click to jump to node definition in editor
* Settings window:
  * [x] Switch between vanilla and 3rd party text editor
  * [ ] Remember settings

## Problems

* InputTextMultiline problems:
  * Loses focus on AltGR
  * Horizontal scrolling is problematic [...](https://github.com/hoffstadt/DearPyGui/issues/2381#issuecomment-2303922790)
  * Scrollbar flashing while typing
* ImGuiColorTextEdit problems:
  * Scrolling by holding up/down arrow key weird "jumpy" behavior
  * No Ctrl + Backspace for removing the whole word (same for Delete)
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
* [simple-svg](https://github.com/adishavit/simple-svg), ([BSD-3-Clause](https://github.com/adishavit/simple-svg/blob/master/LICENSE))
* [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended) ([zlib](https://github.com/btzy/nativefiledialog-extended/blob/master/LICENSE))
* [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit) ([MIT](https://github.com/BalazsJako/ImGuiColorTextEdit/blob/master/LICENSE))

<br>

* [Inconsolata](https://levien.com/type/myfonts/inconsolata.html) ([SIL Open Font](https://github.com/google/fonts/blob/main/ofl/inconsolata/OFL.txt))

<br>

* Changes done to _simple-svg_:
  * Modified to support transparency (dirty)
* Changes done to _ImGuiColorTextEdit_:
  * Added support for NSOLD (the diagram language)
  * Added a custom color palette
  * Disabled line numbers
  * Disabled error and identifier tooltips
  * Removed `ImGuiWindowFlags_AlwaysHorizontalScrollbar` flag
  * Added `GetErrorMarkersRef` function

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
