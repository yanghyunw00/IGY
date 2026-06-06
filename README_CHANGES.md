# IGY Codebase Changes Summary

This document summarizes the key additions and edits recently made across the project.

---

## 해피니스(Happiness) 시스템

### 개요

시민들의 도시 만족도를 수치화한 시스템. 0~100% 범위로 관리되며, 이민자 유입 확률과 게임 오버 조건에 직접 연결됨.

**관련 파일**
- `source/Game/Managers/HappinessManager.hpp` — 지표 계산 및 가중치
- `source/Game/Managers/DKCityMetrics.cpp` — 건물/인구 집계 및 갱신 호출
- `source/Game/UI/DKUserInterfaceEditor.cpp` — City Metrics UI 패널 출력

---

### 전체 행복도 계산 공식

```
총 행복도 = (주거 점수 × 40%) + (고용 점수 × 40%) + (야간 안전 점수 × 20%)
```

---

### 지표 1 — 주거(Housing) `40%`

**계산 기준** : `주거 용량 / 시민 수` 비율

| 비율 | 점수 |
|------|------|
| ≥ 1.5 (인구 대비 여유 있음) | 100 |
| ≥ 1.0 | 80 |
| ≥ 0.7 | 50 |
| ≥ 0.5 | 30 |
| < 0.5 (심각한 주거 부족) | 10 |

**활용 아이디어**
- **이주 거부** : 주거 점수가 30 이하일 때 이민자 이벤트에서 "도시가 꽉 찼습니다" 메시지를 띄우고 유입 차단.
- **노숙자 이벤트** : 주거 비율이 0.5 미만이면 일부 Human이 "Homeless" 상태로 전환되어 Energy 회복이 안 되고 happiness가 빠르게 감소.
- **집값 상승 이벤트** : 주거가 부족한 상태가 일정 시간 지속되면 이민자 월급 요구치가 상승하는 랜덤 이벤트 트리거.

---

### 지표 2 — 고용(Employment) `40%`

**계산 기준** : `일자리 수 / 시민 수` 비율

| 비율 | 점수 |
|------|------|
| 0.8 ~ 1.2 (균형) | 100 |
| 0.5 ~ 1.5 | 70 |
| ≥ 0.3 | 40 |
| < 0.3 (심각한 실업) | 20 |

**활용 아이디어**
- **실업 패널티** : 고용 점수가 40 이하일 때 미취업 Human의 happiness가 평소보다 2배 빠르게 감소. 일하지 않으면 행복해지기 어렵다는 표현.
- **과잉 고용 디버프** : 일자리가 너무 많아도(비율 > 1.5) 인력 부족 상태로 건물 생산성이 감소하는 디버프 적용.
- **파업 이벤트** : 고용 점수가 일정 시간 이상 낮게 유지되면 WorkBuilding의 작동이 일시 중단되는 이벤트 발동.

---

### 지표 3 — 야간 안전(Night Safety) `20%`

**계산 기준** : `가로등 수(건설 완료) / 시민 수` 비율

| 비율 | 점수 |
|------|------|
| ≥ 0.2 (5명당 가로등 1개) | 100 |
| ≥ 0.1 | 70 |
| ≥ 0.05 | 40 |
| < 0.05 (가로등 없음 포함) | 20 |

※ 시민이 0명이면 100점(지표 불필요)

**활용 아이디어**
- **밤 이동 속도 패널티** : Night Safety가 40 이하일 때 밤 시간대(`DayCycleManager`의 Phase == Night) Human의 `walkSpeed`를 0.6배로 감소. 어두운 도시는 이동이 위험하다는 느낌 표현.
- **범죄 이벤트** : Night Safety < 40인 상태가 지속되면 `EventManager`에서 "Robbery" 이벤트 트리거 → 해당 타일 주변 Human의 happiness를 즉시 -15 감소.
- **이민자 야간 보너스** : `CalculateImmigrationChance`에 Night Safety 점수를 별도 가산점으로 추가 (예: +10% if nightSafetyScore >= 80). 안전한 도시에 이민이 더 빨리 옴.

---

### 행복도가 게임에 미치는 영향

| 조건 | 효과 |
|------|------|
| 행복도 ≥ 80% | 이민 확률 80% |
| 행복도 60~80% | 이민 확률 50% |
| 행복도 40~60% | 이민 확률 20% |
| 행복도 20~40% | 이민 확률 5% |
| 행복도 < 20% | 이민 확률 0% |
| 행복도 ≤ 10% | **게임 오버** |

---

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
