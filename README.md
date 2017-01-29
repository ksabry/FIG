# FIG
Fonts In Gl!

## TODO
- Fix project dependency issues in Release

- Cache font faces upon load (Freetype may do this already internally)
- Cache font renderers with equivalent options
- Possibly cache results of GetPositions of frequently requested strings (the overhead of this may make it not worth the effort at all)

- `Font::Create` should take `FontOptions` struct 
- `Font::CreateRenderer` should take `FontRendererOptions` struct
- `FontRenderer::Draw` should take `FontDrawOptions` struct

- Add support for more direct control over `FontRenderer::Draw`; allow a glsl snippet to be inserted into fragment shader?

- Create `FontRenderer::DrawDirect` for drawing 1 to 1 screen pixels which takes `FontDrawDirectOptions`
- Implement hinting and subpixels (only enablable from `FontDrawDirectOptions`)

## Building

TODO

## Usage

####This almost certainly going to have many breaking changes in the near future

Create a `Font` object with 
 ~~~~
 Font font = new Font("/Windows/Fonts/times.ttf", 0);
 ~~~~
 The first argument is a path to a `.ttf` or `.otf` file. The second, optional argument is the font face index in said file (this is usually 0).
 
 To render a font you must first create a `FontRenderer` object:
 ~~~~
 FontRenderer* renderer = font.CreateRenderer(24);
 ~~~~
 The `24` is the size (height) that the font will be rendered. Avoid creating a lot of renderers for different sizes, 
 and definitely avoid creating renderers every frame. If you need many different sizes, just create one renderer at as large a size as
 you will need, and scale your the transformation matrix you pass into `FontRenderer::Draw`.
 
 To actually draw to the current opengl context, simply call
 ~~~~
 renderer->Draw(matrix, fgColor, bgColor, "Hello, World!");
 ~~~~
 The first three parameters `matrix`, `fgColor`, and `bgColor` are all optional. 
 If only one color is specified, it is assumed that it is the fgColor.
 
 `matrix` is a float array of length 16. it represents The transformation matrix applied to
 the font in the shader, and in general will be something like `projectionMatrix * viewMatrix * modelMatrix`; 
 the default value is the identity matrix. 
 
 `fgColor` is a float array of length 4, representing the RGBA foreground color of the font, the color which the characters will be filled with; 
 the default value is solid white.
 
 `bgColor` is also a float array of length 4, the RGBA background color, which will fill the bounding boxes of the characters; 
 the default value is fully transparent.
 
 These colors will will be interpolated between for any character pixel not fully opaque or transparent. 
 **Note that partial transparency can have undesirable effects due to depth buffering issues.**
 
 The required argument for text can be a format specifier, with any further arguments being the format arguments. For example
 ~~~~
 renderer->Draw(matrix, fgColor, bgColor, "The meaning of life, the universe, and everything: %d", 42);
 ~~~~
