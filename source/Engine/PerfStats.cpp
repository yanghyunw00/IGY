/**
 * \file PerfStats.cpp
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "PerfStats.hpp"

#include "Engine.hpp"
#include "Logger.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <numeric>

namespace Perf
{
    FrameStats g_frame{};
    Settings   g_settings{};

    namespace
    {
        constexpr int kHistory = 300;

        std::vector<FrameStats> history;
        int                     historyHead = 0;
        bool                    historyFull = false;

        std::vector<FrameStats> captureBuffer;
        std::string             captureLabel;
        int                     captureTarget = 0;
        bool                    capturing     = false;
        std::string             lastCaptureSummary;

        // scratch buffers for ImGui::PlotLines
        std::array<float, kHistory> plotFrameMs{};
        std::array<float, kHistory> plotPathUs{};

        int HistoryCount()
        {
            return historyFull ? kHistory : historyHead;
        }

        const FrameStats& HistoryAt(int i)
        {
            const int count = HistoryCount();
            const int start = historyFull ? historyHead : 0;
            return history[static_cast<std::size_t>((start + i) % (count == 0 ? 1 : kHistory))];
        }

        std::string TimeStamp()
        {
            std::time_t t  = std::time(nullptr);
            std::tm     tm = {};
#if defined(_WIN32)
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tm);
            return buf;
        }

        void WriteCaptureCsv()
        {
            std::filesystem::create_directories("perf");
            const std::string path = "perf/perf_" + captureLabel + "_" + TimeStamp() + ".csv";

            std::ofstream out(path);
            if (!out)
            {
                Engine::GetLogger().LogError("PerfStats: could not open " + path);
                return;
            }

            out << "frame,frame_ms,fps,instanced_draw_calls,immediate_draw_calls,quads,flushes,"
                   "texture_binds,uniform_uploads,instance_bytes,path_queries,path_us,"
                   "path_nodes_reset,path_nodes_expanded,path_nodes_pushed,path_heap_allocs,path_steps\n";

            FrameStats sum{};
            int        frame = 0;
            for (const auto& f : captureBuffer)
            {
                out << frame++ << ',' << f.frameMilliseconds << ',' << (f.frameMilliseconds > 0.0 ? 1000.0 / f.frameMilliseconds : 0.0) << ',' << f.instancedDrawCalls << ',' << f.immediateDrawCalls
                    << ',' << f.quadsSubmitted << ',' << f.instancedFlushes << ',' << f.textureBinds << ',' << f.uniformUploads << ',' << f.instanceBytesUploaded << ',' << f.pathQueries << ','
                    << f.pathMicroseconds << ',' << f.pathNodesReset << ',' << f.pathNodesExpanded << ',' << f.pathNodesPushed << ',' << f.pathHeapAllocs << ',' << f.pathSteps << '\n';

                sum.frameMilliseconds += f.frameMilliseconds;
                sum.instancedDrawCalls += f.instancedDrawCalls;
                sum.immediateDrawCalls += f.immediateDrawCalls;
                sum.quadsSubmitted += f.quadsSubmitted;
                sum.instancedFlushes += f.instancedFlushes;
                sum.textureBinds += f.textureBinds;
                sum.uniformUploads += f.uniformUploads;
                sum.instanceBytesUploaded += f.instanceBytesUploaded;
                sum.pathQueries += f.pathQueries;
                sum.pathMicroseconds += f.pathMicroseconds;
                sum.pathNodesReset += f.pathNodesReset;
                sum.pathNodesExpanded += f.pathNodesExpanded;
                sum.pathNodesPushed += f.pathNodesPushed;
                sum.pathHeapAllocs += f.pathHeapAllocs;
                sum.pathSteps += f.pathSteps;
            }

            const double n       = static_cast<double>(std::max<std::size_t>(1, captureBuffer.size()));
            const double avgMs   = sum.frameMilliseconds / n;
            const double queries = static_cast<double>(std::max(1u, sum.pathQueries));

            out << "\nsummary,value\n";
            out << "frames," << captureBuffer.size() << '\n';
            out << "renderer," << (g_settings.forceImmediateRenderer ? "forced_immediate" : "game_default") << '\n';
            out << "pathfinding," << (g_settings.pathfindingOptimized ? "optimized" : "legacy") << '\n';
            out << "avg_frame_ms," << avgMs << '\n';
            out << "avg_fps," << (avgMs > 0.0 ? 1000.0 / avgMs : 0.0) << '\n';
            out << "avg_draw_calls_per_frame," << static_cast<double>(sum.instancedDrawCalls + sum.immediateDrawCalls) / n << '\n';
            out << "avg_quads_per_frame," << static_cast<double>(sum.quadsSubmitted) / n << '\n';
            out << "avg_quads_per_draw_call," << static_cast<double>(sum.quadsSubmitted) / std::max(1.0, static_cast<double>(sum.instancedDrawCalls + sum.immediateDrawCalls)) << '\n';
            out << "avg_uniform_uploads_per_frame," << static_cast<double>(sum.uniformUploads) / n << '\n';
            out << "total_path_queries," << sum.pathQueries << '\n';
            out << "avg_us_per_path_query," << sum.pathMicroseconds / queries << '\n';
            out << "avg_nodes_reset_per_query," << static_cast<double>(sum.pathNodesReset) / queries << '\n';
            out << "avg_nodes_expanded_per_query," << static_cast<double>(sum.pathNodesExpanded) / queries << '\n';
            out << "avg_heap_allocs_per_query," << static_cast<double>(sum.pathHeapAllocs) / queries << '\n';

            lastCaptureSummary = "saved " + path + "  |  avg " + std::to_string(avgMs) + " ms, " + std::to_string(sum.pathMicroseconds / queries) + " us/query";
            Engine::GetLogger().LogEvent("PerfStats: " + lastCaptureSummary);
        }
    }

    void NewFrame(double delta_seconds)
    {
        if (history.empty())
            history.resize(kHistory);

        g_frame.frameMilliseconds = delta_seconds * 1000.0;

        history[static_cast<std::size_t>(historyHead)] = g_frame;
        historyHead                                    = (historyHead + 1) % kHistory;
        if (historyHead == 0)
            historyFull = true;

        if (capturing)
        {
            captureBuffer.push_back(g_frame);
            if (static_cast<int>(captureBuffer.size()) >= captureTarget)
            {
                capturing = false;
                WriteCaptureCsv();
            }
        }

        g_frame = FrameStats{};
    }

    void BeginCapture(const std::string& label, int frames)
    {
        captureLabel  = label;
        captureTarget = std::max(1, frames);
        captureBuffer.clear();
        captureBuffer.reserve(static_cast<std::size_t>(captureTarget));
        capturing = true;
    }

    bool IsCapturing()
    {
        return capturing;
    }

    FrameStats WindowAverage()
    {
        FrameStats avg{};
        const int  count = HistoryCount();
        if (count == 0)
            return avg;

        FrameStats sum{};
        for (int i = 0; i < count; ++i)
        {
            const FrameStats& f = HistoryAt(i);
            sum.frameMilliseconds += f.frameMilliseconds;
            sum.instancedDrawCalls += f.instancedDrawCalls;
            sum.immediateDrawCalls += f.immediateDrawCalls;
            sum.quadsSubmitted += f.quadsSubmitted;
            sum.instancedFlushes += f.instancedFlushes;
            sum.textureBinds += f.textureBinds;
            sum.uniformUploads += f.uniformUploads;
            sum.instanceBytesUploaded += f.instanceBytesUploaded;
            sum.pathQueries += f.pathQueries;
            sum.pathMicroseconds += f.pathMicroseconds;
            sum.pathNodesReset += f.pathNodesReset;
            sum.pathNodesExpanded += f.pathNodesExpanded;
            sum.pathNodesPushed += f.pathNodesPushed;
            sum.pathHeapAllocs += f.pathHeapAllocs;
            sum.pathSteps += f.pathSteps;
        }

        const double n            = static_cast<double>(count);
        avg.frameMilliseconds     = sum.frameMilliseconds / n;
        avg.instancedDrawCalls    = static_cast<unsigned>(sum.instancedDrawCalls / count);
        avg.immediateDrawCalls    = static_cast<unsigned>(sum.immediateDrawCalls / count);
        avg.quadsSubmitted        = static_cast<unsigned>(sum.quadsSubmitted / count);
        avg.instancedFlushes      = static_cast<unsigned>(sum.instancedFlushes / count);
        avg.textureBinds          = static_cast<unsigned>(sum.textureBinds / count);
        avg.uniformUploads        = static_cast<unsigned>(sum.uniformUploads / count);
        avg.instanceBytesUploaded = sum.instanceBytesUploaded / static_cast<std::size_t>(count);
        avg.pathQueries           = sum.pathQueries; // totals are more useful than a per-frame mean here
        avg.pathMicroseconds      = sum.pathMicroseconds;
        avg.pathNodesReset        = sum.pathNodesReset;
        avg.pathNodesExpanded     = sum.pathNodesExpanded;
        avg.pathNodesPushed       = sum.pathNodesPushed;
        avg.pathHeapAllocs        = sum.pathHeapAllocs;
        avg.pathSteps             = sum.pathSteps;
        return avg;
    }

    void DrawImGui()
    {
        if (!g_settings.showWindow)
            return;

        ImGui::SetNextWindowSize(ImVec2(430, 560), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Perf (F1)", &g_settings.showWindow))
        {
            ImGui::End();
            return;
        }

        const int        count = HistoryCount();
        const FrameStats avg   = WindowAverage();

        double worstMs = 0.0;
        double bestMs  = 1e9;
        for (int i = 0; i < count; ++i)
        {
            const double ms                          = HistoryAt(i).frameMilliseconds;
            worstMs                                  = std::max(worstMs, ms);
            bestMs                                   = std::min(bestMs, ms);
            plotFrameMs[static_cast<std::size_t>(i)] = static_cast<float>(ms);
            plotPathUs[static_cast<std::size_t>(i)]  = static_cast<float>(HistoryAt(i).pathMicroseconds);
        }
        if (count == 0)
            bestMs = 0.0;

        ImGui::Text("frame  %6.2f ms  (%5.1f fps)   window=%d", avg.frameMilliseconds, avg.frameMilliseconds > 0.0 ? 1000.0 / avg.frameMilliseconds : 0.0, count);
        ImGui::Text("worst  %6.2f ms   best %6.2f ms", worstMs, bestMs);
        if (count > 1)
            ImGui::PlotLines("##frame_ms", plotFrameMs.data(), count, 0, "frame ms", 0.0f, FLT_MAX, ImVec2(0, 50));

        ImGui::Separator();

        // ------------------------------------------------------------------ renderer
        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("force ImmediateRenderer2D (A/B)", &g_settings.forceImmediateRenderer);
            ImGui::TextDisabled("off = game default (instanced). on = one draw call per quad.");

            const FrameStats& f         = g_frame;
            const unsigned    drawCalls = f.instancedDrawCalls + f.immediateDrawCalls;

            ImGui::Text("this frame");
            ImGui::Text("  quads submitted     %8u", f.quadsSubmitted);
            ImGui::Text("  draw calls          %8u  (inst %u / imm %u)", drawCalls, f.instancedDrawCalls, f.immediateDrawCalls);
            ImGui::Text("  quads / draw call   %8.1f", drawCalls ? static_cast<double>(f.quadsSubmitted) / drawCalls : 0.0);
            ImGui::Text("  batch flushes       %8u", f.instancedFlushes);
            ImGui::Text("  texture binds       %8u", f.textureBinds);
            ImGui::Text("  uniform uploads     %8u", f.uniformUploads);
            ImGui::Text("  instance bytes      %8.1f KB", static_cast<double>(f.instanceBytesUploaded) / 1024.0);

            const unsigned avgDrawCalls = avg.instancedDrawCalls + avg.immediateDrawCalls;
            ImGui::Text("avg over window");
            ImGui::Text("  quads %u   draw calls %u", avg.quadsSubmitted, avgDrawCalls);
            if (avgDrawCalls > 0)
                ImGui::Text("  draw call reduction vs 1-per-quad: %.0fx", static_cast<double>(avg.quadsSubmitted) / avgDrawCalls);
        }

        // --------------------------------------------------------------- pathfinding
        if (ImGui::CollapsingHeader("A* pathfinding", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("optimized A* (stamp reset + array neighbors)", &g_settings.pathfindingOptimized);
            ImGui::TextDisabled("off = original: full-grid reset + vector<Node*> per expansion.");

            const double queries = static_cast<double>(std::max(1u, avg.pathQueries));
            ImGui::Text("window totals (%d frames)", count);
            ImGui::Text("  queries             %8u", avg.pathQueries);
            ImGui::Text("  total time          %8.2f ms", avg.pathMicroseconds / 1000.0);
            ImGui::Separator();
            ImGui::Text("per query");
            ImGui::Text("  time                %8.1f us", avg.pathMicroseconds / queries);
            ImGui::Text("  nodes reset         %8.1f", static_cast<double>(avg.pathNodesReset) / queries);
            ImGui::Text("  nodes expanded      %8.1f", static_cast<double>(avg.pathNodesExpanded) / queries);
            ImGui::Text("  queue pushes        %8.1f", static_cast<double>(avg.pathNodesPushed) / queries);
            ImGui::Text("  heap allocations    %8.1f", static_cast<double>(avg.pathHeapAllocs) / queries);
            ImGui::Text("  path length         %8.1f tiles", static_cast<double>(avg.pathSteps) / queries);

            if (count > 1)
                ImGui::PlotLines("##path_us", plotPathUs.data(), count, 0, "pathfinding us / frame", 0.0f, FLT_MAX, ImVec2(0, 50));
        }

        ImGui::Separator();

        // ------------------------------------------------------------------- capture
        static int  captureFrames = 300;
        static char label[64]     = "run";
        ImGui::InputText("label", label, sizeof(label));
        ImGui::SliderInt("frames", &captureFrames, 60, 1800);

        if (capturing)
        {
            ImGui::ProgressBar(static_cast<float>(captureBuffer.size()) / static_cast<float>(std::max(1, captureTarget)));
            ImGui::TextDisabled("capturing...");
        }
        else if (ImGui::Button("capture -> perf/*.csv", ImVec2(-1, 0)))
        {
            const std::string tag = std::string(label) + (g_settings.forceImmediateRenderer ? "_immediate" : "_instanced") + (g_settings.pathfindingOptimized ? "_astarOpt" : "_astarLegacy");
            BeginCapture(tag, captureFrames);
        }

        if (!lastCaptureSummary.empty())
            ImGui::TextWrapped("%s", lastCaptureSummary.c_str());

        ImGui::End();
    }
}
