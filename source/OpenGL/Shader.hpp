/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Handle.hpp"
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace OpenGL
{
    /**
     * \brief Descriptive alias for OpenGL shader program handles
     *
     * ShaderHandle provides a more specific and readable name for the generic
     * OpenGL handle type when referring to compiled shader programs. This improves
     * code clarity without adding compile-time type safety.
     */
    using ShaderHandle = Handle;


    struct [[nodiscard]] CompiledShader
    {
        /** \brief Handle to the compiled and linked OpenGL shader program */
        ShaderHandle Shader;

        /** \brief Cache of uniform names mapped to their OpenGL locations for fast access */
        std::unordered_map<std::string, GLint> UniformLocations;
    };

    /**
     * \brief Create shader program from vertex and fragment shader files
     * \param vertex_filepath Path to the vertex shader source file (.vert)
     * \param fragment_filepath Path to the fragment shader source file (.frag)
     * \return Fully compiled shader program with cached uniform locations
     *
     * Loads, compiles, and links a complete shader program from separate vertex
     * and fragment shader files. This is the standard approach for shader
     * development, allowing separate editing and version control of vertex
     * and fragment shader code.
     *
     * The compilation process includes:
     * - Loading shader source code from the specified files
     * - Compiling vertex and fragment shaders separately
     * - Linking both shaders into a complete program
     * - Extracting and caching all uniform locations for fast access
     * - Cleaning up intermediate shader objects
     *
     * Error handling and debugging:
     * If compilation or linking fails, detailed error messages are logged with
     * line numbers to help identify the problematic shader code. The function
     * throws exceptions for compilation errors, making shader loading failures
     * immediately apparent.
     *
     * File organization patterns:
     * - Vertex shaders typically have .vert extension
     * - Fragment shaders typically have .frag extension
     * - Shaders are located through the asset system for proper path resolution
     *
     * This approach is ideal for production code where shaders are stored as
     * separate files and can be modified without recompiling the application.
     */
    CompiledShader CreateShader(std::filesystem::path vertex_filepath, std::filesystem::path fragment_filepath);


    CompiledShader CreateShader(std::string_view vertex_source, std::string_view fragment_source);

    /**
     * \brief Safely destroy shader program and release all associated resources
     * \param shader Compiled shader structure to destroy (will be reset to safe state)
     *
     * Properly cleans up the OpenGL shader program and clears the uniform location
     * cache, preventing resource leaks and ensuring the structure is in a safe
     * state for reuse or destruction.
     *
     * Resource cleanup includes:
     * - Deleting the OpenGL shader program object
     * - Clearing the uniform location cache
     * - Resetting the shader handle to zero for safety
     *
     * The function is designed to be safe for multiple calls on the same shader
     * structure, as it resets handles after deletion. This prevents double-deletion
     * errors and makes the function suitable for use in destructors or cleanup code.
     *
     * Usage patterns:
     * - Call when shader is no longer needed
     * - Include in application shutdown sequences
     * - Use in RAII wrapper destructors for automatic cleanup
     * - Call before reassigning shader variables
     *
     * After calling this function, the shader structure should not be used for
     * rendering operations until a new shader program is created and assigned.
     */
    void DestroyShader(CompiledShader& shader) noexcept;

    /**
     * \brief Bind uniform buffer to shader's uniform block for shared data access
     * \param shader_handle Handle to the shader program
     * \param binding_number Binding point index for the uniform buffer
     * \param uniform_bufer Handle to the uniform buffer object
     * \param uniform_block_name Name of the uniform block in the shader
     *
     * Establishes a connection between a uniform buffer object and a named uniform
     * block in the shader program. This enables efficient sharing of uniform data
     * across multiple shader programs and reduces the overhead of individual
     * uniform updates.
     *
     * Uniform buffer benefits:
     * - Share common data (matrices, lighting parameters) across multiple shaders
     * - Reduce driver overhead compared to individual uniform calls
     * - Enable more efficient uniform updates for large data sets
     * - Provide better organization of related uniform variables
     *
     * The binding process:
     * - Locates the named uniform block within the shader program
     * - Assigns the block to the specified binding point
     * - Binds the uniform buffer to the same binding point
     * - Logs errors if the uniform block is not found
     *
     * Common uniform block uses:
     * - Camera matrices (view, projection, view-projection)
     * - Lighting parameters (light positions, colors, attenuation)
     * - Material properties shared across multiple objects
     * - Time-based values for animations and effects
     *
     * The binding number should be consistent across all shaders that need to
     * access the same uniform buffer data, enabling true data sharing.
     */
    void BindUniformBufferToShader(ShaderHandle shader_handle, GLuint binding_number, Handle uniform_bufer, std::string_view uniform_block_name);
}
