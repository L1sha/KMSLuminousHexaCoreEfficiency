KMS 루미너스의 HEXA 코어 강화 효율 계산기입니다.

(KMS ver. 1.2.399)

Features
- 사용자 맞춤 강화 효율 제공
  - 사용자의 전투분석 측정 결과(a.k.a. 스킬 별 딜점유율)를 바탕으로 강화 효율을 계산합니다.
    사용자가 원하는 상황 조건 하에서의 강화 효율을 계산할 수 있습니다.
    (특정 보스, 플레이 스타일, 컨티/리웨, 쿨n초뚝 등)
    강화 효율 계산을 위해 사용자의 HEXA 코어 강화 상태를 받습니다.
  - 강화 레벨에 따라 데미지%, 몬스터 방어율 무시가 추가되는 스킬의 효율을 정확하게 계산하기 위해 사용자의 스펙을 입력받습니다.
  - 솔 에르다 기준으로 강화 효율을 계산할지, 솔 에르다 조각 기준으로 강화 효율을 계산할지 선택할 수 있습니다.
- 강화 효율 계산 방식
  - 전투분석 측정 결과와 HEXA 코어 강화 상태로부터 강화를 진행하지 않은 default 상태에서의 초기 딜점유율을 역산합니다.
  - 강화 가능한 스킬들의 강화 레벨에 따른 딜 상승량을 계산하고, 필요 재화량으로 나누어 강화 효율을 계산합니다.
    이 때 특정 레벨 구간에서 강화 효율이 높아지는 스킬들이 존재하기 때문에, 현재 강화 레벨부터 30레벨까지 모든 레벨의 강화 효율을 계산하여 비교합니다.
  - 가장 강화 효율이 높은 스킬 & 가장 강화 효율이 높은 레벨을 선택하여 그리디하게 강화합니다. 모든 스킬의 강화가 끝날 때까지 반복합니다.
- 결과 출력
  - 출력 1) 강화 순서, 딜 증가량, 강화 효율, 소모 재화량, 총 소모 재화량 등 얻을 수 있는 모든 정보를 출력합니다.
  - 출력 2) 단순하게 강화 순서만 출력합니다.
 
Future Plan
- N/A

개발 기간
- 1차 (Matlab code) : 23년 여름 6차 전직 업데이트
- 2차 (C++ Migration) : 23년 겨울 마스터리 코어 추가 업데이트
- 3차 (C++ Refactoring) : 24년 겨울 마스터리 코어 추가 업데이트

Last Update : 25.01.06
