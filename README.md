# ElementalPointer

캐릭터를 직접 조종하는 대신 커서 위치를 공격 범위로 쓰고, 보스는 스탯 벽이 아니라 기믹 이해로 풀도록 설계한 Unreal C++ 전투 프로토타입입니다.

[Repository](https://github.com/iamfreakin/ElementalPointer) | [Portfolio Hub](https://github.com/iamfreakin/GamePortfolio)

## Overview

| 항목 | 내용 |
| --- | --- |
| 장르 | 커서 기반 인크리멘탈 액션 로그라이크 프로토타입 |
| 엔진/언어 | Unreal Engine 5.8, C++ |
| 개발 형태 | 1인 프로토타입 |
| Repository | [iamfreakin/ElementalPointer](https://github.com/iamfreakin/ElementalPointer) |

## Problem

액션 로그라이크에서 이동, 조준, 공격을 모두 요구하면 조작 부담이 커질 수 있습니다. 이 프로젝트는 캐릭터 이동보다 커서 위치 판단을 전투의 중심으로 두는 실험입니다.

## Implementation

PlayerController가 마우스 화면 좌표를 world ray로 변환하고, 전투 평면과의 교차점을 `CursorState.WorldLocation`으로 저장합니다. `AEPSwordField`는 이 위치를 따라가며 범위 안 적을 공격합니다.

성장은 각성, 상점, 깨달음으로 나누고, 보스 기믹은 커서 위치 기반 조건 평가 구조로 설계했습니다.

## Key Features

- 화면 좌표를 전투 평면 월드 좌표로 변환
- 커서를 따라가는 공격 범위
- 범위 안 적에게 주기적으로 피해를 주는 auto attack 구조
- Head/Back 위치 보상 설계
- 성장 루프와 보스 기믹 설계 분리

## My Contribution

- `DeprojectMousePositionToWorld` 기반 커서 월드 좌표 계산
- 전투 평면 교차점 계산
- `AEPSwordField` 기반 커서 추종 공격 범위 구현
- 범위 오토어택 구조 설계
- Head/Back 위치 보상 설계
- 각성, 상점, 깨달음 성장 구조 설계
- 보스 기믹 설계와 구현 범위 분리

## Keywords

| 키워드 | 설명 |
| --- | --- |
| Cursor projection | 화면 좌표를 전투 평면 월드 좌표로 변환 |
| `AEPSwordField` | 커서를 따라가는 공격 범위 |
| Area auto attack | 범위 안 적에게 주기적 피해 |
| Head/Back multiplier | 위치 판단 보상 |
| Gimmick runner | 보스 기믹 설계 기반 |
