/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Engine/Texture.hpp"
#include "OpenGL/Framebuffer.hpp"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CS230
{
    class Texture;

    /**
     * \brief Centralized texture resource management system with caching and render-to-texture capabilities
     *
     * TextureManager serves as the central hub for all texture-related operations in the engine,
     * providing efficient resource management, automatic caching, and advanced render-to-texture
     * functionality. The manager prevents duplicate texture loading, handles memory cleanup,
     * and provides tools for creating procedural textures through render-to-texture operations.
     *
     * Core Responsibilities:
     * - Texture loading and caching to prevent duplicate GPU resources
     * - Memory management and cleanup of all texture resources
     * - Render-to-texture functionality for creating dynamic textures
     * - Resource lifecycle tracking and automatic cleanup
     *
     * Caching System:
     * The manager maintains a cache of loaded textures indexed by file path, ensuring that
     * the same image file is never loaded multiple times into GPU memory. This significantly
     * reduces memory usage and loading times when the same textures are referenced by
     * multiple game objects or UI elements.
     *
     * Render-to-Texture Features:
     * Provides a complete render-to-texture system that allows drawing operations to be
     * captured into texture objects rather than displayed on screen. This enables:
     * - Procedural texture generation through drawing operations
     * - Text rendering to textures for efficient text display
     * - Dynamic UI element creation and caching
     * - Off-screen rendering for special effects and post-processing
     *
     * Resource Management:
     * All textures created through the manager are automatically tracked and cleaned up
     * during shutdown, preventing memory leaks and ensuring proper resource disposal.
     * The manager handles both file-loaded textures and runtime-generated textures
     * through a unified interface.
     *
     * Integration with Engine:
     * The TextureManager integrates seamlessly with the 2D renderer and coordinate
     * system, automatically handling viewport management and coordinate transformations
     * during render-to-texture operations while preserving the normal rendering state.
     */
    class TextureManager
    {
    public:
        /**
         * \brief Load a texture from file with automatic caching
         * \param file_name Path to the image file to load
         * \return Pointer to the loaded texture (cached if previously loaded)
         *
         * Loads a texture from the specified image file, utilizing an internal cache
         * to ensure that the same file is never loaded multiple times. If the texture
         * has been previously loaded, the cached version is returned immediately
         * without performing file I/O or GPU operations.
         *
         * Supported Formats:
         * The function supports common image formats through the underlying image
         * loading system, including PNG, JPEG, BMP, and other formats supported
         * by the engine's image loader.
         *
         * Caching Behavior:
         * - First load: Reads file, creates GPU texture, stores in cache
         * - Subsequent loads: Returns cached texture immediately
         * - Cache key: Full file path (different paths to same file create separate entries)
         * - Memory efficiency: Prevents duplicate GPU resources for same image
         *
         * Resource Ownership:
         * The returned texture remains owned by the TextureManager and will be
         * automatically cleaned up when Unload() is called or the manager is
         * destroyed. Callers should not manually delete the returned texture.
         *
         * Error Handling:
         * If the file cannot be loaded (missing file, unsupported format, etc.),
         * the function behavior depends on the underlying image loading system.
         * The manager logs loading events for debugging and monitoring purposes.
         *
         * Performance Characteristics:
         * - First load: File I/O + GPU texture creation overhead
         * - Cached loads: Very fast hash table lookup with no I/O
         * - Memory usage: One GPU texture per unique file path
         */
        std::shared_ptr<Texture> Load(const std::filesystem::path& file_name);

        /**
         * \brief Create texture from raw pixel data
         * \param size Texture dimensions (width, height)
         * \param pixels Span of RGBA pixel data
         * \return Pointer to the created texture
         */
        std::shared_ptr<Texture> CreateFromPixels(Math::ivec2 size, std::span<const CS200::RGBA> pixels);

        /**
         * \brief Unload and clean up all managed textures
         *
         * Performs comprehensive cleanup of all textures managed by the TextureManager,
         * including both file-loaded textures and render-to-texture generated textures.
         * This function ensures proper GPU memory cleanup and prevents resource leaks.
         *
         * Cleanup Operations:
         * - Deletes all cached file-based textures and clears the cache
         * - Deletes all render-to-texture generated textures
         * - Releases associated GPU memory and OpenGL resources
         * - Logs cleanup operation for debugging purposes
         *
         * When to Call:
         * - Application shutdown to ensure clean resource cleanup
         * - Level transitions when textures are no longer needed
         * - Memory management when switching between different content sets
         * - Error recovery situations requiring resource reset
         *
         * Post-Cleanup State:
         * After calling Unload(), all previously returned texture pointers become
         * invalid and should not be used. The manager returns to its initial empty
         * state and is ready to load new textures.
         *
         */
        void Unload();

        /**
         * \brief Begin render-to-texture mode for creating dynamic textures
         * \param width Width of the render target texture in pixels
         * \param height Height of the render target texture in pixels
         *
         * Initiates render-to-texture mode by setting up a framebuffer and redirecting
         * all subsequent drawing operations to a texture instead of the screen. This
         * system integrates seamlessly with the engine's 2D renderer to capture
         * drawing operations into reusable texture objects.
         *
         * Implementation Details:
         * - Creates OpenGL framebuffer with color attachment of specified dimensions
         * - Saves current viewport, clear color, and rendering state for restoration
         * - Ends current 2D renderer scene to ensure clean state transition
         * - Sets up Y-flipped coordinate system for proper texture orientation
         * - Binds framebuffer as render target, replacing screen rendering
         * - Clears render target with transparent black (0,0,0,0) for clean start
         *
         * Coordinate System Management:
         * The function applies a Y-flip transformation to ensure that rendered content
         * appears correctly oriented in the final texture. This handles the difference
         * between OpenGL's bottom-left origin and texture's top-left convention:
         * - Screen rendering: Standard NDC with Y-up orientation
         * - Texture rendering: Flipped NDC with Y-down orientation for proper texture layout
         *
         * Renderer Integration:
         * - Properly ends current 2D renderer scene before framebuffer switch
         * - Begins new 2D renderer scene with texture-appropriate coordinate system
         * - Ensures transformation matrices work correctly for texture-space rendering
         * - Maintains compatibility with all standard 2D rendering operations
         *
         * State Management:
         * The function carefully preserves rendering state (viewport dimensions,
         * clear color values) in a global structure for accurate restoration during
         * EndRenderTextureMode(). This ensures seamless transitions between texture
         * and screen rendering without affecting the main rendering pipeline.
         *
         * Use Cases and Workflow:
         * 1. Text rendering: Font systems use this to create cached text textures
         * 2. UI element generation: Complex layouts rendered once and reused
         * 3. Procedural content: Algorithmic drawing captured as texture assets
         * 4. Dynamic graphics: Game state visualizations as reusable textures
         *
         * Critical Requirements:
         * - Must be paired with EndRenderTextureMode() to complete the operation
         * - All drawing operations between Start/End calls contribute to final texture
         * - Framebuffer remains active until EndRenderTextureMode() is called
         * - Screen rendering is suspended during render-to-texture mode
         */
        void StartRenderTextureMode(int width, int height);

        /**
         * \brief Complete render-to-texture mode and return the generated texture
         * \return Shared pointer to the newly created texture containing the rendered content
         *
         * Finalizes the render-to-texture operation by properly cleaning up the framebuffer,
         * restoring the previous rendering state, and creating a texture object from the
         * captured content. This function handles all necessary OpenGL state management
         * to ensure seamless transition back to normal screen rendering.
         *
         * Cleanup and Restoration Process:
         * - Ends current 2D renderer scene to flush any pending draw operations
         * - Unbinds framebuffer (returns to default screen framebuffer 0)
         * - Restores original viewport dimensions from saved state
         * - Restores original clear color values from saved state
         * - Begins new 2D renderer scene with screen-appropriate coordinate system
         * - Deletes temporary framebuffer to free GPU resources
         *
         * Texture Creation:
         * Creates a new Texture object by wrapping the framebuffer's color attachment:
         * - Transfers ownership of OpenGL texture ID from framebuffer to Texture object
         * - Preserves original dimensions specified in StartRenderTextureMode()
         * - Maintains RGBA format with alpha channel for transparency support
         * - Content includes all drawing operations performed during render-to-texture mode
         *
         * Memory Management Strategy:
         * Returns shared_ptr for flexible ownership and automatic cleanup.
         * The generated texture is returned directly to the caller and is not
         * cached by the TextureManager, allowing independent lifecycle management.
         *
         * State Restoration Details:
         * The function meticulously restores rendering state using values saved during
         * StartRenderTextureMode(), including:
         * - Viewport: X, Y, width, height coordinates for screen rendering
         * - Clear color: R, G, B, A values for background clearing
         * - Coordinate system: Returns to standard screen-space NDC matrix
         * - 2D renderer: Proper scene management for continued screen rendering
         *
         * Integration with Engine Systems:
         * - Seamlessly integrates with IRenderer2D for scene management
         * - Maintains compatibility with engine's coordinate system conventions
         * - Ensures proper cleanup of OpenGL resources to prevent GPU memory leaks
         * - Returns texture that works with all standard texture operations
         *
         * Error Handling and Robustness:
         * - Handles cleanup even if rendering operations failed during texture creation
         * - Ensures framebuffer is always deleted to prevent resource leaks
         * - Resets internal state variables to prevent corruption on subsequent calls
         * - Maintains engine stability even if texture creation encounters issues
         *
         * Performance Considerations:
         * - Framebuffer creation/destruction has GPU overhead
         * - State switching between texture and screen rendering has performance cost
         * - Generated textures are full GPU resources - manage lifecycle appropriately
         * - Consider caching at the application level if reusing the same generated content
         */
        std::shared_ptr<Texture> EndRenderTextureMode();

    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> texture_cache;

        struct Currentstate
        {
            OpenGL::FramebufferWithColor fbo{};
            GLint                        viewport[4]{};
            GLfloat                      clearColor[4];
            Math::ivec2                  size{ 0, 0 };
        };

        Currentstate curr;

        bool is_rendering = false;
    };
}
