/**
 * \file PerfStats.hpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 *
 * \brief Lightweight runtime profiler for A/B measuring the instanced renderer
 *        and the A* pathfinder.
 *
 * Hot paths only touch a plain global struct (no singleton guard, no locks), so
 * the counters can sit inside the per-tile draw loop and the A* inner loop
 * without distorting what they measure.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Perf
{
    /**
     * \brief Everything accumulated during a single frame.
     */
    struct FrameStats
    {
        // --- rendering ---
        unsigned    instancedDrawCalls    = 0; ///< glDrawElementsInstanced calls (one per flush)
        unsigned    immediateDrawCalls    = 0; ///< glDrawElements calls (one per quad/shape)
        unsigned    quadsSubmitted        = 0; ///< quads+shapes handed to whichever renderer is active
        unsigned    instancedFlushes      = 0; ///< batch flushes (full buffer, texture slot overflow, EndScene)
        unsigned    textureBinds          = 0; ///< glBindTexture calls issued by the renderers
        unsigned    uniformUploads        = 0; ///< per-draw uniform uploads (immediate mode pays these per quad)
        std::size_t instanceBytesUploaded = 0; ///< bytes pushed through glBufferSubData

        // --- pathfinding ---
        unsigned pathQueries       = 0;   ///< A* searches started this frame
        unsigned pathNodesReset    = 0;   ///< nodes cleared before/while searching
        unsigned pathNodesExpanded = 0;   ///< nodes popped off the open set and closed
        unsigned pathNodesPushed   = 0;   ///< pushes into the priority queue
        unsigned pathHeapAllocs    = 0;   ///< heap allocations caused by neighbor lookup
        unsigned pathSteps         = 0;   ///< waypoints in the returned paths
        double   pathMicroseconds  = 0.0; ///< wall clock spent inside FindPath

        // --- frame ---
        double frameMilliseconds = 0.0;
    };

    /**
     * \brief Knobs the HUD flips so both implementations can be compared in one build.
     */
    struct Settings
    {
        bool pathfindingOptimized   = true;  ///< false = original A* (full reset + vector neighbors)
        bool forceImmediateRenderer = false; ///< true = every draw goes through ImmediateRenderer2D
        bool showWindow             = true;
    };

    /// Written from hot paths. Plain global on purpose.
    extern FrameStats g_frame;
    extern Settings   g_settings;

    inline FrameStats& Counters() noexcept
    {
        return g_frame;
    }

    inline const Settings& Config() noexcept
    {
        return g_settings;
    }

    /**
     * \brief Close the previous frame, archive it, and zero the counters.
     * \param delta_seconds frame time reported by the engine timer
     */
    void NewFrame(double delta_seconds);

    /**
     * \brief Draw the profiler window. Call between ImGuiHelper::Begin/End.
     */
    void DrawImGui();

    /**
     * \brief Start recording `frames` frames and write them to perf_<label>_<timestamp>.csv.
     */
    void BeginCapture(const std::string& label, int frames);

    bool IsCapturing();

    /// Averages over the rolling window (last kHistory frames).
    FrameStats WindowAverage();
}
