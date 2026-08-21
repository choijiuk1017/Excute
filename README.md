# Excute

> Unreal Engine 5 기반 3D 액션 RPG 팀 프로젝트

C++과 Blueprint를 사용해 플레이어 전투, 적 AI, 보스 패턴, 공격 판정, 패링, 전투 UI 및 레벨 진행 시스템을 구현한 액션 RPG 프로젝트입니다.

프로그래머와 그래픽 디자이너가 협업한 2인 팀 프로젝트로, 클라이언트 프로그래밍과 전투 시스템 구현을 담당했습니다.

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
| 팀 구성 | 클라이언트 프로그래머 1명, 그래픽 디자이너 1명 |
| 개발 기간 | 2026.02 ~ 2026.05 |
| 담당 역할 | 클라이언트 프로그래밍 및 전투 시스템 |
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
- Attack Trace
- Collision
- Parry System
- Trigger Actor

### AI

- AI Controller
- Behavior Tree
- Blackboard
- Behavior Tree Service
- Behavior Tree Task

### UI & Effect

- UMG
- Niagara
- Niagara UI Renderer

---

# 핵심 구현 내용

- 플레이어 전투와 공격 애니메이션을 구현했습니다.
- 공격 애니메이션의 실제 타격 구간에 맞춰 공격 Trace를 실행했습니다.
- 한 번의 공격에서 같은 대상에게 피해가 중복 적용되지 않도록 처리했습니다.
- 플레이어의 방어 타이밍에 따라 적의 공격을 패링할 수 있도록 구성했습니다.
- 패링 성공 시 적의 공격을 중단하고 경직 상태를 적용했습니다.
- Behavior Tree와 Blackboard 기반 적 탐지 및 전투 AI를 구현했습니다.
- 일반 적의 연속 공격과 공격 후 후퇴 행동을 구현했습니다.
- 보스 전용 공격 패턴과 범위 공격을 구현했습니다.
- 보스 범위 공격 전에 공격 범위를 표시하는 AOE Indicator를 구현했습니다.
- 플레이어와 보스 사이의 거리에 따라 보스 HP Bar를 표시했습니다.
- 보스 체력이 절반 이하로 감소하면 전용 패턴이 실행되도록 구성했습니다.
- 보스 전투 Trigger와 다음 레벨 이동 Trigger를 구현했습니다.
- AI를 일시정지하고 재개할 수 있는 기능을 구현해 연출과 게임 진행에 연결했습니다.

---

# 주요 시스템

## 1. 적 캐릭터 구조

`AEnemyCharacter`는 일반 적에게 필요한 능력치, 공격, 피격, 패링, 사망 및 UI 기능을 관리합니다.

### 주요 능력치

```cpp
int32 MaxHP;
int32 CurrentHP;
int32 AttackPower;
int32 Defense;
```

### 주요 전투 상태

```cpp
int32 CurrentAttackStep = 0;

bool bHasRetreatedThisCombo = false;
bool bAttackTraceActive = false;
bool bIsParried = false;
bool bIsDead = false;
```

### 주요 함수

```cpp
void PlayAttackMontageByIndex(int32 Index);

void OnAttackMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
);

void StartAttackTrace();
void EndAttackTrace();

void HandleParried();

void OnParriedMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
);

bool TryCallPlayerParry(AActor* HitActor);

void TakeDamage(float Damage);
void PerformAttackTrace();
```

---

## 2. 공격 Animation Montage

적 캐릭터는 여러 공격 Animation Montage를 배열로 관리합니다.

```cpp
TArray<UAnimMontage*> AttackMontages;
```

Behavior Tree Task 또는 전투 로직에서 공격 인덱스를 전달하면 해당 Montage를 재생합니다.

### 처리 흐름

```text
공격 패턴 결정
    ↓
공격 Montage Index 선택
    ↓
PlayAttackMontageByIndex()
    ↓
Animation Montage 재생
    ↓
Animation Notify에서 공격 Trace 시작
    ↓
대상 충돌 검사 및 피해 적용
    ↓
Animation Notify에서 공격 Trace 종료
    ↓
Montage 종료 Callback
    ↓
다음 공격 또는 후퇴 상태 결정
```

Animation과 공격 판정 시점을 분리하지 않고 Animation Notify를 기준으로 연결해 실제 무기가 휘둘러지는 구간에서만 판정이 활성화되도록 구성했습니다.

---

## 3. 공격 Trace

근접 공격은 공격 Animation의 유효 구간에만 Trace를 실행합니다.

### 공격 판정 흐름

1. 공격 Animation이 시작됩니다.
2. 무기가 타격 구간에 진입하면 `StartAttackTrace()`를 호출합니다.
3. `bAttackTraceActive`를 활성화합니다.
4. 공격 구간 동안 `PerformAttackTrace()`를 실행합니다.
5. Trace에 감지된 Actor가 이미 타격한 대상인지 확인합니다.
6. 처음 감지된 대상에게만 피해 또는 패링 검사를 적용합니다.
7. 공격 구간이 종료되면 `EndAttackTrace()`를 호출합니다.
8. 다음 공격을 위해 타격 대상 목록을 초기화합니다.

### 중복 타격 방지

```cpp
TArray<AActor*> HitActorsThisSwing;
```

같은 공격 구간에서 이미 충돌한 Actor를 배열에 기록해 하나의 휘두르기에서 동일한 대상에게 피해가 여러 번 적용되지 않도록 구성했습니다.

---

## 4. 패링 시스템

플레이어가 적의 공격 타이밍에 맞춰 패링하면 일반 피해 처리를 중단하고 적에게 경직을 적용합니다.

### 패링 처리 흐름

```text
적 공격 Trace
    ↓
플레이어 충돌 감지
    ↓
플레이어 패링 가능 상태 확인
    ↓
패링 성공 여부 반환
    ↓
일반 피해 처리 중단
    ↓
적 공격 Montage 중단
    ↓
적에게 패링 경직 상태 적용
    ↓
패링 Montage 재생
    ↓
일정 시간 또는 Montage 종료 후 상태 복구
```

### 패링 관련 변수

```cpp
bool bIsParried = false;
float ParryStunDuration = 0.8f;
```

### 주요 함수

```cpp
void HandleParried();
bool TryCallPlayerParry(AActor* HitActor);

void OnParriedMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
);
```

패링 중에는 일반 공격과 AI 행동이 중복 실행되지 않도록 상태를 구분하고 패링 Animation이 끝난 뒤 정상 전투 상태로 복귀시켰습니다.

---

## 5. 연속 공격과 후퇴

일반 적은 여러 공격 Montage를 순서대로 실행하며 공격이 끝난 뒤 일정 확률로 후퇴할 수 있습니다.

```cpp
int32 CurrentAttackStep = 0;
bool bHasRetreatedThisCombo = false;
float RetreatChancePercent = 40.0f;
```

### 처리 방식

1. 현재 공격 단계를 확인합니다.
2. 해당 공격 Montage를 재생합니다.
3. Montage가 끝나면 다음 공격 단계 또는 후퇴 여부를 결정합니다.
4. 후퇴를 선택하면 해당 연속 공격에서 중복 후퇴하지 않도록 상태를 기록합니다.
5. 연속 공격 종료 후 공격 단계와 후퇴 상태를 초기화합니다.

---

## 6. AI Controller

`AEnemyAIController`는 적 캐릭터의 Behavior Tree를 실행하고 AI의 일시정지 및 재개 기능을 제공합니다.

### 주요 함수

```cpp
void PauseAI();
void ResumeAI();

virtual void BeginPlay() override;
virtual void OnPossess(APawn* InPawn) override;
```

### Behavior Tree Component

```cpp
UBehaviorTreeComponent* CachedBehaviorTree;
```

Controller가 Pawn을 Possess할 때 Behavior Tree를 실행하고 사용하는 Behavior Tree Component를 저장합니다.

이를 통해 컷신, 보스 등장 연출, 사망 처리 등 AI 행동을 멈춰야 하는 상황에서 Tree를 일시정지하고 필요한 시점에 다시 실행할 수 있도록 했습니다.

---

## 7. Behavior Tree

### 주요 Service

| 클래스 | 역할 |
| --- | --- |
| `BTService_DetectPlayer` | 플레이어 탐지 및 Blackboard 갱신 |
| `BTService_ChecakPattern` | 현재 전투 상황에 맞는 공격 패턴 조건 검사 |

### 주요 Task

| 클래스 | 역할 |
| --- | --- |
| `BTTask_PlayAttackMontage` | 일반 적 공격 Montage 실행 |
| `BTTask_PlayBossPattern` | 보스 전용 공격 패턴 실행 |

### AI 처리 흐름

```text
Behavior Tree 실행
    ↓
플레이어 탐지
    ↓
Blackboard에 대상 저장
    ↓
플레이어에게 이동
    ↓
공격 거리 검사
    ↓
일반 공격 또는 보스 패턴 선택
    ↓
Attack Montage 재생
    ↓
공격 종료 확인
    ↓
다음 행동 결정
```

---

## 8. 보스 시스템

`ABossCharacter`는 `AEnemyCharacter`를 상속하고 보스 전용 패턴, UI, 범위 공격 및 페이즈 기능을 추가합니다.

### 보스 패턴

```cpp
TArray<UAnimMontage*> PatternMontages;

int32 CurrentPatternIndex = -1;
bool bIsPatternPlaying = false;
```

### 주요 함수

```cpp
void PlayPatternMontage(int32 Index);

void OnPatternMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
);

bool IsPatternPlaying() const;
```

보스 패턴 실행 중 다른 패턴이 중복 재생되지 않도록 `bIsPatternPlaying` 상태를 관리하고 Montage 종료 시 상태를 복구합니다.

---

## 9. 보스 AOE 공격

보스의 범위 공격이 즉시 적용되면 플레이어가 공격에 대응하기 어렵기 때문에 실제 공격 전에 범위를 표시하도록 구성했습니다.

### AOE 기본 설정

```cpp
float AOERadius = 400.0f;
float AOEDamage = 30.0f;
float AOEChargeDuration = 1.5f;
```

### 주요 함수

```cpp
void StartAOEIndicatorCharge();
void ExecuteAOEAttack();
```

### 처리 흐름

```text
Behavior Tree에서 AOE 패턴 선택
    ↓
AOE Indicator 생성
    ↓
공격 위치와 범위 표시
    ↓
1.5초 동안 충전
    ↓
범위 내 플레이어 검사
    ↓
AOE 피해 적용
    ↓
Indicator 제거
```

### 관련 클래스

- `ABossAOEIndicator`
- `ABossCharacter`
- `BTTask_PlayBossPattern`

---

## 10. 보스 페이즈

보스 체력이 절반 이하가 되었을 때 새로운 패턴을 실행할 수 있도록 상태를 관리합니다.

```cpp
bool bHalfHPPatternTriggered = false;
```

체력이 절반 이하로 감소했는지 확인하고 해당 패턴이 아직 실행되지 않았다면 전용 패턴을 한 번 실행합니다.

동일한 조건에서 패턴이 반복 실행되지 않도록 `bHalfHPPatternTriggered`에 실행 여부를 기록합니다.

---

## 11. 보스 UI

보스 HP Bar는 항상 표시하지 않고 플레이어와 보스 사이의 거리를 기준으로 표시합니다.

```cpp
float BossHPBarVisibleDistance = 1500.0f;
```

### 관련 함수

```cpp
void UpdateBossHPBarVisibility();
void UpdateBossHPBarPercent();
```

플레이어가 보스 전투 범위에 진입하면 HP Bar를 표시하고 보스의 현재 체력이 변경될 때 진행률을 갱신합니다.

보스 사망 시에는 보스 사망 Widget을 표시하고 중복 표시되지 않도록 상태를 관리합니다.

---

## 12. 일반 적 HP Bar

일반 적의 HP Bar 역시 플레이어와의 거리에 따라 표시합니다.

```cpp
float HPBarVisibleDistance = 1000.0f;
```

멀리 있는 모든 적의 UI를 항상 표시하지 않고 전투와 관련된 거리에서만 표시해 화면의 불필요한 정보를 줄였습니다.

---

## 13. 레벨 및 보스 전투 Trigger

### 주요 클래스

| 클래스 | 역할 |
| --- | --- |
| `ACrossBossTrigger` | 보스 전투 구역 진입 및 이벤트 실행 |
| `ANextLevelTrigger` | 다음 레벨 이동 |
| `AInGameGameMode` | 인게임 규칙과 진행 상태 |
| `UMyGameInstance` | 레벨 전환 후 유지할 게임 데이터 |
| `ATitlePlayerController` | 타이틀 화면 입력 처리 |

Trigger Actor를 사용해 플레이어가 특정 위치에 진입했을 때 보스 전투를 시작하거나 다음 레벨로 이동하도록 구성했습니다.

---

# 트러블 슈팅

## 공격 Animation과 타격 판정 시점이 일치하지 않는 문제

### 문제

공격 입력 직후 피해를 적용하면 화면에서 무기가 아직 대상에게 닿지 않았는데 피해가 먼저 적용되거나 무기가 대상을 통과한 뒤 피해가 적용되는 문제가 발생했습니다.

### 해결

공격 Animation Montage에 공격 시작과 종료 Notify를 배치하고 해당 구간에서만 공격 Trace를 실행하도록 변경했습니다.

### 결과

실제 무기가 휘둘러지는 구간과 타격 판정 시점을 일치시켜 전투 피드백을 개선했습니다.

### 배운 점

액션 게임의 공격 판정은 입력 시점이 아니라 플레이어에게 보이는 Animation의 실제 타격 구간을 기준으로 설계해야 한다는 점을 배웠습니다.

---

## 하나의 공격에서 피해가 여러 번 적용되는 문제

### 문제

공격 Trace를 여러 Frame 동안 실행하면서 동일한 대상이 매 Frame 감지되어 한 번의 공격으로 여러 번 피해가 적용되는 문제가 발생했습니다.

### 해결

공격 한 번에 감지한 Actor를 `HitActorsThisSwing`에 저장하고 이미 목록에 존재하는 대상은 피해 처리에서 제외했습니다.

공격이 끝날 때 목록을 초기화해 다음 공격에서는 다시 타격할 수 있도록 구성했습니다.

### 결과

한 번의 휘두르기에서 대상마다 한 번만 피해가 적용되도록 개선했습니다.

### 배운 점

연속적으로 실행되는 충돌 검사에서는 충돌 여부뿐만 아니라 해당 공격 단위에서 이미 처리한 대상인지 관리해야 한다는 점을 배웠습니다.

---

## 패링 중 AI 공격이 계속 실행되는 문제

### 문제

패링에 성공한 뒤 적의 경직 Animation을 재생해도 Behavior Tree 또는 기존 공격 상태가 유지되어 다른 공격이 중복 실행될 수 있었습니다.

### 해결

패링 성공 시 `bIsParried` 상태를 적용하고 기존 공격 Montage와 공격 Trace를 종료했습니다.

필요한 동안 AI를 정지하고 패링 Montage 종료 Callback에서 패링 상태와 AI를 복구하도록 구성했습니다.

### 결과

패링 중 다른 공격이 중복 실행되는 문제를 줄이고 경직 이후 정상적인 전투 상태로 복귀할 수 있게 되었습니다.

### 배운 점

상태를 강제로 변경하는 전투 기능에서는 Animation만 변경하는 것이 아니라 기존 판정, AI 실행과 상태 변수까지 함께 정리해야 한다는 점을 배웠습니다.

---

## 보스 범위 공격을 피하기 어려운 문제

### 문제

보스의 범위 공격이 예고 없이 즉시 실행되어 플레이어가 공격 범위와 타이밍을 파악하기 어려웠습니다.

### 해결

`ABossAOEIndicator`를 이용해 실제 공격 전에 범위와 충전 시간을 표시했습니다.

충전 시간이 종료된 후 실제 범위 판정과 피해를 적용했습니다.

### 결과

플레이어가 공격 범위와 실행 타이밍을 확인하고 회피할 수 있도록 전투 가독성을 개선했습니다.

### 배운 점

강한 공격은 피해량만 설정하는 것이 아니라 공격 전조와 대응 시간을 함께 설계해야 공정한 전투 경험을 만들 수 있다는 점을 배웠습니다.

---

# 주요 클래스

| 클래스 | 역할 |
| --- | --- |
| `AEnemyCharacter` | 일반 적의 능력치, 공격, 피격, 패링 및 사망 |
| `ABossCharacter` | 보스 패턴, AOE, 페이즈 및 보스 UI |
| `AEnemyAIController` | Behavior Tree 실행 및 AI 일시정지 |
| `BTService_DetectPlayer` | 플레이어 탐지 |
| `BTService_ChecakPattern` | 공격 패턴 조건 확인 |
| `BTTask_PlayAttackMontage` | 일반 적 공격 실행 |
| `BTTask_PlayBossPattern` | 보스 패턴 실행 |
| `ABossAOEIndicator` | 보스 범위 공격 예고 |
| `ACrossBossTrigger` | 보스 전투 Trigger |
| `ANextLevelTrigger` | 다음 레벨 전환 |
| `UHPBar` | 적 및 보스 체력 표시 |
| `UMyGameInstance` | 레벨 간 데이터 유지 |
| `AInGameGameMode` | 인게임 진행 관리 |

---

# 프로젝트 구조

```text
Source/pixelate_project/
├─ Actor/
│  ├─ BossAOEIndicator.*
│  ├─ CrossBossTrigger.*
│  └─ NextLevelTrigger.*
│
├─ BT/
│  ├─ BTService_ChecakPattern.*
│  ├─ BTService_DetectPlayer.*
│  ├─ BTTask_PlayAttackMontage.*
│  └─ BTTask_PlayBossPattern.*
│
├─ Character/
│  ├─ BossCharacter.*
│  ├─ EnemyAIController.*
│  ├─ EnemyCharacter.*
│  └─ TitlePlayerController.*
│
├─ GameInstance/
│  └─ MyGameInstance.*
│
├─ GameMode/
│  └─ InGameGameMode.*
│
├─ UI/
│  └─ HPBar.*
│
├─ pixelate_project.Build.cs
├─ pixelate_project.cpp
└─ pixelate_project.h
```

---

# 실행 방법

## 요구 환경

- Unreal Engine 5.5
- Visual Studio 2022
- Windows 10/11
- Git LFS

## 프로젝트 실행

```bash
git lfs install
git clone https://github.com/choijiuk1017/Excute.git
cd Excute
```

1. `pixelate_project.uproject`를 우클릭합니다.
2. `Generate Visual Studio project files`를 실행합니다.
3. Visual Studio에서 `Development Editor / Win64`로 빌드합니다.
4. `pixelate_project.uproject`를 실행합니다.
5. 필요한 Unreal Plugin이 활성화되어 있는지 확인합니다.

### 사용 Plugin

- Niagara UI Renderer
- Gameplay Behaviors
- Water
- Modeling Tools Editor Mode

> 저장소에 게임 에셋이 포함되어 있어 최초 실행 시 Shader Compile에 시간이 걸릴 수 있습니다.

---

# 개발 과정에서 얻은 경험

- C++과 Blueprint를 함께 사용한 Unreal 팀 프로젝트
- Animation Notify 기반 근접 공격 Trace
- 공격 단위별 중복 타격 방지
- 패링 성공과 적 경직 상태 처리
- Behavior Tree와 Blackboard 기반 적 AI
- 일반 적과 보스의 상속 구조
- 보스 공격 패턴 및 페이즈 관리
- AOE Indicator를 이용한 공격 전조
- 거리 기반 적 및 보스 UI 표시
- Trigger를 활용한 전투와 레벨 진행
- 그래픽 디자이너와의 협업

---

# Repository

https://github.com/choijiuk1017/Excute
