# Excute

> Unreal Engine 5 기반 3D 액션 RPG 팀 프로젝트

C++과 Blueprint를 사용해 플레이어 전투, 적 AI, 보스 패턴, 공격 판정, 패링, 전투 UI 및 레벨 진행 시스템을 구현한 액션 RPG 프로젝트입니다.

일반 적과 보스가 플레이어의 위치와 전투 상황에 따라 서로 다른 공격을 선택하도록 Behavior Tree 기반 AI를 구성했습니다.

---

## 플레이 영상

[![Excute 플레이 영상](https://img.youtube.com/vi/CM0YpxHINxk/0.jpg)](https://www.youtube.com/watch?v=CM0YpxHINxk)

- YouTube: https://www.youtube.com/watch?v=CM0YpxHINxk

---

## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 프로젝트명 | Excute |
| 장르 | 3D 액션 RPG |
| 개발 형태 | 2인 팀 프로젝트 |
| 개발 기간 | 2026.02 ~ 2026.05 |
| 팀 구성 | 클라이언트 프로그래머 1명, 그래픽 디자이너 1명 |
| 담당 역할 | 클라이언트 및 전투 시스템 |
| 엔진 | Unreal Engine 5.5 |
| 개발 언어 | C++, Blueprint |
| 플랫폼 | Windows |

---

## 기술 스택

### Engine & Language

- Unreal Engine 5.5
- C++
- Blueprint

### Gameplay

- Animation Montage
- Animation Notify
- Collision Trace
- Niagara
- UMG

### AI

- AI Controller
- Behavior Tree
- Blackboard
- Navigation System
- Gameplay Tasks

---

# 핵심 구현 내용

- 플레이어 이동, 공격, 회피 및 패링 기능을 구현했습니다.
- 공격 Animation Montage와 실제 공격 판정을 연결했습니다.
- 공격 구간에만 근접 공격 Trace가 활성화되도록 구성했습니다.
- 한 번의 공격에서 같은 대상에게 중복 피해가 적용되지 않도록 처리했습니다.
- 패링 성공 시 적 공격을 중단하고 일정 시간 경직 상태를 적용했습니다.
- Behavior Tree와 Blackboard 기반 적 AI를 구현했습니다.
- 플레이어 탐지, 추적, 공격, 후퇴 및 대기 흐름을 구성했습니다.
- 일반 적과 보스의 공격 패턴을 분리했습니다.
- 보스 체력과 플레이어 거리에 따라 HP Bar를 표시했습니다.
- 보스 범위 공격 전에 공격 범위를 표시하는 AOE Indicator를 구현했습니다.
- 보스의 체력에 따른 페이즈 전환 패턴을 구현했습니다.
- 보스 전투 진입, 전투 종료 및 다음 레벨 이동 Trigger를 구성했습니다.

---

# 주요 시스템

## 1. 적 캐릭터 구조

`AEnemyCharacter`는 일반 적과 보스가 공통으로 사용하는 전투 기능을 담당합니다.

### 주요 능력치

```cpp
int32 MaxHP = 0;
int32 CurrentHP = 0;

int32 AttackPower = 0;
int32 Defense = 0;
```

### 주요 전투 상태

```cpp
int32 Current
