# FIG
Fonts In Gl!

## TODO
- Cache font faces upon load (Freetype may do this already internally)
- Possibly cache results of GetPositions of frequently requested strings (the overhead of this may make it not worth the effort at all)

- Specify bounding box options in `Draw` and wordwrapping option

- Add support for more direct control over `FontRenderer::Draw`; allow a glsl snippet to be inserted into fragment shader?

- Implement hinting and subpixels (only enablable from `FontDrawDirectOptions`)

## Building

TODO

## Usage

Create a `Font` object with 
```cpp
FIG::Font font = new FIG::Font("/Windows/Fonts/times.ttf");
```
The first argument is a path to a `.ttf` or `.otf` file. The second, optional argument is an `FontSettings` object. This object can be initialized with field, value pairs in any order inside a regular constructor or aggregate (using curly braces) constructor. For example:
 
```cpp
FIG::Font* font = new FIG::Font("/Windows/Fonts/times.ttf", {
    FACE_INDEX,          0,
    FREETYPE_LOAD_FLAGS, 0
});
``` 
 
To render a font, simply call `Font::Draw` like so

```cpp
float fgColor[4] = { 1.0, 0.0, 0.0, 1.0 };

font->Draw({
    FONT_SIZE, 40.0,
    ALIGNMENT, TextAlignment::Center,
    DIRECT,   true,
    DIRECT_X, 300,
    DIRECT_Y, 100,
    COLOR_FG, fgColor
}, "Hello, World!");
```
The first argument is a `FontRendererDrawSettings` object which is an object which takes any options from either a `FontRendererSettings` or a `FontDrawSettings` object. A comprehensive list of options can be found in the Documentation section.
 
This call is actually a shorthand for creating a `FontRenderer` object and calling `FontRenderer::Draw`. Note that `FontRenderer` instances with the same settings are cached and not recreated with each call, so do not be concerned with multiple calls with the same `FontRenderSettings`.

The more explicit syntax would be

```cpp
float fgColor[4] = { 1.0, 0.0, 0.0, 1.0 };

FIG::FontRenderer* renderer = font->CreateRenderer(
    FONT_SIZE, 50,
    ALIGNMENT, TextAlignment::Center
);

renderer->Draw({
    DIRECT, true,
    DIRECT_X, 300,
    DIRECT_Y, 200,
    COLOR_FG, fgColor
}, "Hello, World!");
```
For either of these methods of drawing, any further arguments after the string will be interpreted as string format arguments. For example

```cpp
float fgColor[4] = { 1.0, 0.0, 0.0, 1.0 };

font->Draw({
    FONT_SIZE, 40.0,
    ALIGNMENT, TextAlignment::Center,
    DIRECT,   true,
    DIRECT_X, 300,
    DIRECT_Y, 100,
    COLOR_FG, fgColor
}, "I have a level %d foo, and level %.2f bar", 5, 2.4);
```

## Documentation

### class Font

### Font::Font(const char\* const filename, FontSettings settings = FontSettings());
Constructs a `Font` instance using the `ttf` or `otf` file specified by `filename` and the optional second argument of type `FontSettings`. This will set the error if the specified font could not be loaded.

### Font::Font(const char\* const filename, TArgs... args);
Delegates the the above constructor, creating a `FontSettings` object using `args`.

### char\* Font::Error();
Returns a string representation of an error if one occured. Will be `nullptr` if no errors have occured.

### FontSettings Font::Settings();
Returns the `FontSettings` object that was used to intialize the `Font`.

### FontRenderer\* Font::CreateRenderer(FontRendererSettings settings);
Create a `FontRenderer` instance associated with the `Font`. This is just a shorthand for using the `FontRenderer` constructor with this instance and the settings. 

### FontRenderer\* Font::CreateRenderer(TArgs... args);
Delegates to the above method passing in `args` to the `FontRendererSettings` constructor.

### void Font::Draw(FontRendererDrawSettings settings, const char\* const format, TArgs... args)
Draw text to the current OpenGL context. The `format` argument will be have c-style string formatting applied using `args`. This method will create a `FontRenderer` instance internally or use a cached version with the same renderer settings. 

---

### class FontRenderer

### FontRenderer::FontRenderer(Font\* font, FontRendererSettings settings);
Constructs a `FontRenderer` instance from a `Font` and a `FontRendererSettings` object. This will set the error if any freetype2 or opengl initialization steps fail.

### FontRenderer::FontRenderer(Font\* font, TArgs... args);
Delegates the the above constructor, creating a `FontRendererSettings` object using `args`.

### FontRendererSettings FontRenderer::Settings();
Returns the `FontRendererSettings` object that was used to intialize the `FontRenderer`.

### void FontRendererSettings::Draw(FontDrawSettings drawSettings, const char\* const format, TArgs... args);
Draw text to the current OpenGL context. The `format` argument will be have c-style string formatting applied using `args`. 

### BoundingBox FontRendererSettings::Bounds(const char\* const format, TArgs... args);
Return the bounding box of the string if it was rendered. This bounding box is determined by the font metrics and should be used if you want to draw a selection box over text for example.

### BoundingBox FontRendererSettings::RenderBounds(const char\* const format, TArgs... args);
Return the bounding box of the string if it was rendered. This bounding box is the smallest box around all the rendered characters.

---

### struct BoundingBox

### float BoundingBox::left, BoundingBox::right, BoundingBox::top, BoundingBox::bottom;
The respective sides of the bounding box

### float BoundingBox::width()
The width of the bounding box

### float BoundingBox::height()
The height of the bounding box

---

### struct FontSettings

### FontSettings::FontSettings(TArgs... args)
Initialize the `FontSettings` struct with field, value pairs. The first argument must be an initializer field name, and the second is it's associated value, the third argument is another field with the forth being its value and so on. 

FontSettings instance member | Initializer field name   | Description
---------------------------- | ------------------------ | -------------
long faceIndex               | FIG::FACE_INDEX          | The index of the face in the font file (most font files only have one face)
int freetypeLoadFlags        | FIG::FREETYPE_LOAD_FLAGS | Flags passed to freetype2 upon font load

---

### struct FontRendererSettings

### FontRendererSettings::FontRendererSettings(TArgs... args)
Initialize the `FontRendererSettings` struct with field, value pairs. The first argument must be an initializer field name, and the second is it's associated value, the third argument is another field with the forth being its value and so on. 

FontSettings instance member | Initializer field name   | Description
---------------------------- | ------------------------ | ------------
float fontSize               | FIG::FONT_SIZE           | The font size in pixels; this can be a non-integral value though depending on the font all sizes may not be supported
int filterMode               | FIG::FILTER_MODE         | The opengl texture filter mode to use for drawing; defaults to `GL_LINEAR`
int glyphLoadFlags           | FIG::GLYPH_LOAD_FLAGS    | Flags passed to freetype2 when loading the glyphs
int glyphRenderMode          | FIG::GLYPH_RENDER_MODE   | Flags passed to freetype2 when rendering glyphs to bitmaps
bool useKerning              | FIG::USE_KERNING         | Enable or disable font kerning on fonts which support it; defaults to true
TextAlignment alignment      | FIG::ALIGNMENT           | Alignment of text; defaults to `TextAlignment::Left`
bool fullBackground          | FIG::FULL_BACKGROUND     | Fill in the bounds of the text (as determined by `FontRenderer::Bounds`) with the background color if true. If false, only draws backgrounds of individual glyphs, but is faster. 

---

### struct FontDrawSettings

### FontRendererSettings::FontRendererSettings(TArgs... args)
Initialize the `FontDrawSettings` struct with field, value pairs. The first argument must be an initializer field name, and the second is it's associated value, the third argument is another field with the forth being its value and so on. 

FontSettings instance member | Initializer field name   | Description
---------------------------- | ------------------------ | ------------
const float\* colorFg        | FIG::COLOR_FG            | The foreground color of the text; the float array must be of size 4, interpreted as RGBA; defaults to solid white
const float\* colorBg        | FIG::COLOR_BG            | The background color of the text; the float array must be of size 4, interpreted as RGBA; defaults to the foreground color with alpha set to 0. Any partial pixels of the font will have a color interpolated between this and the foreground
const float\* transform      | FIG::TRANSFORM           | A matrix transformation to be applied before drawing. the float array must be of size 16, and will be interpreted row-major; defaults to the identity matrix; this will be ignored if direct is true.
bool direct                  | FIG::DIRECT              | Enable or disable direct rendering; when disabled the font will be rendererd in opengl world space, with each font pixel being equivalent to one opengl unit; when enabled it will drawn in screen space, with each font pixel equivalent to one screen pixel
int directX                  | FIG::DIRECT_X            | The x position in pixels from the left of the screen to draw the text when direct is enabled; will be ignored otherwise
int directY                  | FIG::DIRECT_Y            | The y position in pixels from the top of the screen to draw the text when direct is enabled; will be ignored otherwise

---

### struct FontRendererDrawSettings

### FontRendererDrawSettings::FontRendererDrawSettings(TArgs... args)
Initialize a `FontRendererDrawSettings`, which is simply a combination of `FontRendererSettings` and `FontDrawSettings`. All fields valid in either of these can be used in this.

### FontRendererSettings FontRendererDrawSettings::RendererSettings();
Get a `FontRendererSettings` with the associated values

### FontDrawSettings FontRendererDrawSettings::DrawSettings();
Get a `FontDrawSettings` with the associated values

---

### enum class TextAlignment

### TextAlignment::Left
Text will all be drawn with the left bound of each line aligned to the x position of drawing.
### TextAlignment::Center
Text will all be drawn with the center of each line aligned to the x position of drawing.
### TextAlignment::Right
Text will all be drawn with the right bound of each line aligned to the x position of drawing.
