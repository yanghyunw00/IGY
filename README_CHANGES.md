# IGY Codebase Changes Summary

This document summarizes the key additions and edits recently made across the project.

## Map Variants & Terrain
- Added map generation profiles for Balanced, WaterHeavy, PlainsHeavy, and MountainHeavy with configurable thresholds/noise (`source/Game/GridMap.cpp`, `source/Game/GridMap.hpp`, `source/Game/GridCell.hpp`).
- WaterHeavy maps now guarantee at least ~50% water by raising the water cutoff to the median height during generation (`ResetIsoGridMapArray` in `GridMap.cpp`).
- Terrain rendering now uses the active profile thresholds (height cutoffs) for both map draw and mouse atlas.

## Water Animation
- Water tiles now animate by oscillating their rendered height to simulate waves traveling from top-right to bottom-left (`GetWaterWaveOffset` and usage in `IsometricDrawMap` in `GridMap.cpp`).

## Rotation Pivot (Caesar 3 style)
- Map rotation now pivots around a focal tile so that tile stays fixed on screen when rotating. Defaults to grid center; uses mouse selection tile when valid (`ComputePivotOrigin`, pivot usage in `IsometricDrawMap` and `IsometricDrawMouseAtlas` in `GridMap.cpp`; selection accessors in `DKMouseSelection.hpp`).

## Map Selection & Menu
- Main menu includes a Map option to cycle variants and apply selection when starting gameplay (`source/Game/MainMenu.cpp`, `source/Game/MainMenu.hpp`).
- MapManager carries the selected variant into map generation (`source/Dokev/DKMapManager.cpp`, `source/Dokev/DKMapManager.hpp`).

## Demos
- Base DEMO now accepts a map variant and regenerates accordingly (`source/Game/DEMO.cpp`, `source/Game/DEMO.hpp`).
- Added terrain-specific demos: PlainsDemo, WaterDemo, MountainDemo (`source/Game/PlainsDemo.*`, `WaterDemo.*`, `MountainDemo.*`).

## UI Navigation
- Added ImGui “Back to Main Menu” buttons in DEMO and GamePlayState to quickly return to the menu (`source/Game/DEMO.cpp`, `source/Game/GamePlayState.cpp`).

## Miscellaneous
- Mouse selection exposes current grid position/validity for pivot-based rotation (`source/Dokev/DKMouseSelection.hpp`).
