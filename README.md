# CS200 Computer Graphics I

## How to Build and Run

First Setup your [Development Environment](docs/DevEnvironment.md)

### Command Line Build

**Windows**

```sh
# configure / generate build files
cmake --preset windows-debug
cmake --preset windows-developer-release
cmake --preset windows-release
cmake --preset web-debug-on-windows

# Build exe
cmake --build --preset windows-debug
cmake --build --preset windows-developer-release
cmake --build --preset windows-release
cmake --build --preset web-debug-on-windows
```

**Linux**
```sh
# configure / generate build files
cmake --preset linux-debug
cmake --preset linux-developer-release
cmake --preset linux-release
cmake --preset web-debug
cmake --preset web-developer-release
cmake --preset web-release

# Build exe
cmake --build --preset linux-debug
cmake --build --preset linux-developer-release
cmake --build --preset linux-release
cmake --build --preset web-debug
cmake --build --preset web-developer-release
cmake --build --preset web-release
```

### Automated Build Script

For convenience, use the automated build script that handles all configurations:

```sh
python3 scripts/scan_build_project.py
```

See the script help for filtering options:
```sh
python3 scripts/scan_build_project.py --help
```

## Build Configurations

This project supports three distinct build configurations, each designed for different stages of development and deployment:

### **Debug Configuration**
- **Purpose**: Active development and debugging
- **Features**: 
  - Allows setting breakpoints to step through code line-by-line
  - No compiler optimizations (preserves variable values for debugging)
  - Console window for viewing debug output
  - All developer helper features enabled
- **Performance**: Slower execution, larger executable size
- **When to use**: When you need to debug issues or understand code flow

### **Developer-Release Configuration**  
- **Purpose**: Testing with optimized performance while keeping developer tools
- **Features**:
  - Compiler optimizations enabled (faster execution)
  - Cannot set breakpoints (optimizations interfere with debugging)
  - Console window for viewing output
  - Verbose logging and error checking enabled
  - Developer assertions active
- **Performance**: Fast execution, optimized code
- **When to use**: Performance testing while retaining diagnostic capabilities

### **Release Configuration**
- **Purpose**: Final builds for end users
- **Features**:
  - Full compiler optimizations
  - No console window (clean user experience)
  - Minimal logging (only critical errors)
  - No developer features or extra error checking
  - Smallest executable size
- **Performance**: Maximum speed, production-ready
- **When to use**: Final builds for distribution to users

## CMake Presets

CMake presets simplify the build process by pre-configuring complex settings. Instead of manually specifying:

- Compiler flags and optimization levels
- Output directories and build paths  
- Platform-specific configurations
- Dependency locations
- Debug/release-specific settings

The presets handle all of this automatically. For example, `cmake --preset linux-debug` is equivalent to running:

```sh
cmake -S . -B build/linux-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DDEVELOPER_VERSION=ON \
  [... many more flags ...]
```

This saves time and reduces configuration errors, especially when working across different platforms and build types.

## Class Diagram

```mermaid
classDiagram
    %% Core Engine System
    class Engine {
        -Impl* impl
        +Instance() Engine$
        +GetLogger() Logger$
        +GetWindow() Window$
        +GetInput() Input$
        +GetWindowEnvironment() WindowEnvironment$
        +GetGameStateManager() GameStateManager$
        +GetRenderer2D() IRenderer2D$
        +GetTextureManager() TextureManager$
        +Start(window_title)
        +Stop()
        +Update()
        +HasGameEnded() bool
        -updateEnvironment()
    }

    class WindowEnvironment {
        +int FPS
        +uint64_t FrameCount
        +double DeltaTime
        +double ElapsedTime
        +vec2 DisplaySize
    }

    %% Window System
    class Window {
        -SDL_Window* sdl_window
        -SDL_GLContext gl_context
        -bool closed
        -ivec2 size
        -WindowEventCallback eventCallback
        +Start(title)
        +Update()
        +IsClosed() bool
        +GetSize() ivec2
        +Clear(color)
        +ForceResize(w, h)
        +GetSDLWindow() SDL_Window*
        +GetGLContext() SDL_GLContext
        +SetEventCallback(callback)
        +SetBackgroundColor(r, g, b)$
        +GetWindowSize() ivec2
        -setupSDLWindow(title)
        -setupOpenGL()
    }

    %% Input System
    class Input {
        -keys_down : array~bool~
        -previous_keys_down : array~bool~
        +Update()
        +KeyDown(key) bool
        +KeyJustReleased(key) bool
        +KeyJustPressed(key) bool
        +convert_cs230_to_sdl(key)$ SDL_Scancode
        -SetKeyDown(key, value)
    }

    class InputKeys {
        <<enumeration>>
        A, B, C, D, E, F, G, H, I, J, K, L, M
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z
        Space, Enter, Left, Up, Right, Down
        Escape, Tab, Count
    }

    %% Game State System
    class GameState {
        <<interface>>
        +Load()*
        +Update()*
        +Unload()*
        +Draw()*
        +DrawImGui()*
        +GetName()* czstring
    }

    class GameStateManager {
        -mGameStateStack : vector~unique_ptr~GameState~~
        -mToClear : vector~unique_ptr~GameState~~
        +PushState~STATE~()
        +PopState()
        +Update()
        +Draw()
        +DrawImGui()
        +HasGameEnded() bool
        +Clear()
    }

    class GamePlayState {
        -testTexture : Texture2D
        +Load()
        +Update()
        +Unload()
        +Draw()
        +DrawImGui()
        +GetName() czstring
        -updateInput()
        -draw()
    }

    %% Rendering System
    class IRenderer2D {
        <<interface>>
        +Init()*
        +Shutdown()*
        +BeginScene(view_projection)*
        +EndScene()*
        +DrawQuad(transform, texture, ...)*
        +DrawCircle(transform, fill_color, ...)*
        +DrawRectangle(transform, fill_color, ...)*
        +DrawLine(transform, start, end, ...)*
        +DrawLine(start, end, ...)*
    }

    class ImmediateRenderer2D {
        -vao : VertexArrayHandle
        -vbo : BufferHandle
        -ibo : BufferHandle
        -uniformBlock : BufferHandle
        -indicesCount : GLsizei
        -sdf_vao : VertexArrayHandle
        -sdfVbo : BufferHandle
        -textureShader : CompiledShader
        -sdfShader : CompiledShader
        -uboCamera : BufferHandle
        -viewProjection : TransformationMatrix
        +Init()
        +Shutdown()
        +BeginScene(view_projection)
        +EndScene()
        +DrawQuad(transform, texture, ...)
        +DrawCircle(transform, fill_color, ...)
        +DrawRectangle(transform, fill_color, ...)
        +DrawLine(transform, start, end, ...)
        +DrawLine(start, end, ...)
        -DrawSDF(transform, fill_color, ...)
    }

    %% Texture System
    class TextureManager {
        -texture_cache : unordered_map~string, shared_ptr~Texture~~
        -curr : Currentstate
        -is_rendering : bool
        +Load(file_name) shared_ptr~Texture~
        +CreateFromPixels(size, pixels) shared_ptr~Texture~
        +Unload()
        +StartRenderTextureMode(width, height)
        +EndRenderTextureMode() shared_ptr~Texture~
    }

    class Texture2D {
        -texture : shared_ptr~Texture~
        +Texture2D(fileName)
        +IsValid() bool
        +GetWidth() int
        +GetHeight() int
        +Draw(posX, posY, tint)
        +Draw(position, tint)
        +DrawEx(posX, posY, rotation, scale, tint)
        +DrawEx(position, rotation, scale, tint)
        +DrawPro(sourceX, sourceY, ...)
        +DrawPro(position, rotation, ...)
    }

    %% Math System
    class TransformationMatrix {
        #matrix : double[3][3]
        +operator[](index) double*
        +operator*(m) TransformationMatrix
        +operator*=(m) TransformationMatrix&
        +operator*(v) vec2
        +Reset()
    }

    class TranslationMatrix {
        +TranslationMatrix(translate)
    }

    class RotationMatrix {
        +RotationMatrix(theta)
    }

    class ScaleMatrix {
        +ScaleMatrix(scale)
    }

    class vec2 {
        +double x
        +double y
        +operator+=(rhs) vec2&
        +operator-=(rhs) vec2&
        +operator*=(scalar) vec2&
        +operator/=(scalar) vec2&
        +Length() double
        +Normalize() vec2
    }

    class ivec2 {
        +int x
        +int y
        +operator+=(rhs) ivec2&
        +operator-=(rhs) ivec2&
        +operator*=(scalar) ivec2&
        +operator/=(scalar) ivec2&
    }

    %% Image System
    class Image {
        +int width
        +int height
        +pixels : vector~RGBA~
        +GetWidth() int
        +GetHeight() int
        +GetPixels() vector~RGBA~
        +IsValid() bool
        +GetPixel(x, y) RGBA
        +SetPixel(x, y, color)
    }

    %% Relationships
    GameState <|.. GamePlayState : implements
    IRenderer2D <|.. ImmediateRenderer2D : implements
    TransformationMatrix <|-- TranslationMatrix : extends
    TransformationMatrix <|-- RotationMatrix : extends
    TransformationMatrix <|-- ScaleMatrix : extends

    Engine o-- Window : manages
    Engine o-- Input : manages
    Engine o-- GameStateManager : manages
    Engine o-- IRenderer2D : manages
    Engine o-- TextureManager : manages
    Engine o-- WindowEnvironment : contains

    GameStateManager o-- GameState : manages
    GamePlayState o-- Texture2D : uses

    Input -- InputKeys : uses
    ImmediateRenderer2D -- TransformationMatrix : uses
    Texture2D -- Image : converts from
```

### Diagram Explanation

This class diagram represents the **Dokev Graphics Engine** architecture for CS200 Computer Graphics I.

#### Core Systems

- **Engine**: Singleton class managing all subsystems (Window, Input, Renderer, GameState, TextureManager)
- **WindowEnvironment**: Stores runtime information (FPS, frame count, delta time, display size)

#### Game State System

- **GameState**: Abstract interface for different application states (menu, gameplay, etc.)
- **GameStateManager**: Manages state stack with push/pop operations
- **GamePlayState**: Concrete implementation for game logic

#### Rendering System

- **IRenderer2D**: Abstract interface for 2D rendering operations
- **ImmediateRenderer2D**: OpenGL-based implementation with texture and SDF shape rendering

#### Mathematics

- **TransformationMatrix**: Base class for 3x3 transformation matrices
- **TranslationMatrix, RotationMatrix, ScaleMatrix**: Specialized transformation types
- **vec2, ivec2**: 2D vector types (double and int)

#### Graphics Resources

- **TextureManager**: Manages texture loading, caching, and render-to-texture functionality
- **Texture2D**: High-level texture wrapper with draw operations
- **Image**: Raw pixel data storage and manipulation

#### Relationships

- `+` indicates public members
- `-` indicates private members
- `#` indicates protected members
- `*` indicates abstract/pure virtual methods
- `$` indicates static methods
- `<|--` shows inheritance
- `<|..` shows interface implementation
- `o--` shows composition/aggregation

