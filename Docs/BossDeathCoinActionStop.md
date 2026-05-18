# Boss Death Coin Action Stop

## 변경 목적

보스 HP가 0이 되어 사망 연출이 시작되면, 진행 중인 코인 행동과 코인 행동 연출을 즉시 중단한다.

## 변경 내용

- `UBattleManagerWSubsystem::BossDeathStarted()`에서 보스 사망 시작 시점에 코인 행동 종료 처리를 호출한다.
- `UCoinActionManagementWSubsystem::StopActionSequenceForStageEnd()`를 추가해 코인 행동 상태, 입력 상태, 반복 실행, VFX 타이머를 정리한다.
- `UBattleLevelActingWSubsystem::StopCoinActionAct()`를 추가해 코인 상승/흔들림/하강 연출 타이머와 완료 콜백을 즉시 해제한다.
- 아이템 턴도 `ItemManager->SetTurn(false)`로 같이 닫는다.

## 최종 흐름

```text
Boss HP 0
-> OnBossDeathStarted
-> CoinActionManager action sequence stop
-> ItemManager turn off
-> ActingManager coin action act stop
-> Boss death camera move
-> BossClearAnim / DeathEffect / StageEnd UI flow
```

## 주의 사항

- 코인 연출은 즉시 중단되며, 코인을 원위치로 보정하지 않는다.
- 현재 목적은 보스 사망 후 추가 코인 행동과 추가 데미지 실행을 막는 것이다.
