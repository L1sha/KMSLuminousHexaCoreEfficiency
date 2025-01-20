#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <utility>



constexpr int SKILL_NUM = 11, REAL_SKILL_NUM = 9;
const long double THRESHOLD = -0.00001;

std::array<std::string, SKILL_NUM> skillNameSet
{
    "앱솔루트 킬 VI    ", "라이트 리플렉션 VI", "아포칼립스 VI     ", "트와일라잇 노바 VI",
    "진리의 문         ", "퍼니싱 리소네이터 ", "빛과 어둠의 세례  ", "리버레이션 오브   ",
    "하모닉 패러독스   ", "엔드리스 다크니스 ", "이터널 라이트니스 "
};

std::array<std::array<std::array<long double, 31>, 3>, 2> materialRequirements
{ {
        /*
        materialRequirements[MATERIAL TYPE][CORE TYPE][TARGET LEVEL]

        MATERIAL TYPE: 솔 에르다 = 0, 솔 에르다 조각 = 1
        CORE TYPE: 마스터리 코어 = 0, 강화 코어 = 1, 스킬 코어 = 2
        TARGET LEVEL: 1 ~ 30 (i번째 값: (i-1)레벨 -> i레벨에 필요한 재화량)
        */

        // 솔 에르다
        {{
                // 마스터리 코어
                {
                    0, 3, 1, 1, 1, 1, 1, 1, 2, 2,
                    5, 2, 2, 2, 2, 2, 2, 2, 2, 3,
                    8, 3, 3, 3, 3, 3, 3, 3, 3, 4, 10
                },

    // 강화 코어
    {
        0, 4, 1, 1, 1, 2, 2, 2, 3, 3,
        8, 3, 3, 3, 3, 3, 3, 3, 3, 4,
        12, 4, 4, 4, 4, 4, 5, 5, 5, 6, 15
    },

    // 스킬 코어
    {
        0, 5, 1, 1, 1, 2, 2, 2, 3, 3,
        10, 3, 3, 4, 4, 4, 4, 4, 4, 5,
        15, 5, 5, 5, 5, 5, 6, 6, 6, 7, 20
    }
}},

// 솔 에르다 조각
{{
        // 마스터리 코어
        {
            0, 50, 15, 18, 20, 23, 25, 28, 30, 33,
            100, 40, 45, 50, 55, 60, 65, 70, 75, 80,
            175, 85, 90, 95, 100, 105, 110, 115, 120, 125, 250
        },

    // 강화 코어
    {
        0, 75, 23, 27, 30, 34, 38, 42, 45, 49,
        150, 60, 68, 75, 83, 90, 98, 105, 113, 120,
        263, 128, 135, 143, 150, 158, 165, 173, 180, 188, 375
    },

    // 스킬 코어
    {
        0, 100, 30, 35, 40, 45, 50, 55, 60, 65,
        200, 80, 90, 100, 110, 120, 130, 140, 150, 160,
        350, 170, 180, 190, 200, 210, 220, 230, 240, 250, 500
    }
}}
} };



long double damageP = 100, bossDamageP = 500, ignoreEnemyDefense = 95, enemyDefense = 380, extraSkillDealShare;
int materialType = 0;

std::array<long double, SKILL_NUM> currentDealShare, defaultDealShare;
std::array<int, REAL_SKILL_NUM> currentHexaLevel;
std::array<std::array<long double, 31>, SKILL_NUM> dealIncrementP;
std::array<std::array<long double, 31>, REAL_SKILL_NUM> dealIncrement;

std::vector<std::pair<std::string, std::pair<int, int>>> reinforceHistory;
std::vector<std::array<int, REAL_SKILL_NUM>> levelHistory;
std::vector<long double> totalDealHistory, dealIncrementHistory, finalDealHistory, finalDealIncrementHistory, reinforceEfficiencyHistory;
std::vector<int> materialHistory, materialSumHistory;



void calculateExtraSkillDealShare() {

    // HEXA코어 강화 대상 스킬들을 제외한 나머지 스킬들의 딜 점유율 합을 계산

    extraSkillDealShare = 100;
    for (int idx = 0; idx < SKILL_NUM; ++idx) extraSkillDealShare -= currentDealShare[idx];
}

void calculateDealIncrementP() {

    // 초기 상태 대비 HEXA코어 강화 레벨에 따른 딜 증가량의 상댓값을 계산

    // 앱솔루트 킬 VI (0레벨 기준, 4차 스킬 대비)
    dealIncrementP[0][0] = 1;
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[0][lv] = (485 + 7 * lv) / 455;
    // 앱솔루트 킬 VI 코어 강화에 따른 몬스터 방어율 무시 수치 증가는 무시

    // 라이트 리플렉션 VI (0레벨 기준, 4차 스킬 대비)
    dealIncrementP[1][0] = 1;
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[1][lv] = (480 + 11 * lv) / 440;

    // 아포칼립스 VI (0레벨 기준, 4차 스킬 대비)
    dealIncrementP[2][0] = 1;
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[2][lv] = (408 + 12 * lv) / 375;

    // 트와일라잇 노바 VI (0레벨 기준, 4차 스킬 대비)
    dealIncrementP[3][0] = 1;
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[3][lv] = ((375 + 25 * lv) * 7 * 4) / (450 * 6 * 3);
    // 이퀄리브리엄 상태 기준 딜 증가량으로 계산 (임시)

    // 강화 코어 (0레벨 기준)
    dealIncrementP[4][0] = 1;
    for (long double lv = 1; lv <= 30; ++lv) {

        if (lv == 1) dealIncrementP[4][lv] = 1.11;
        else if (lv == 10) dealIncrementP[4][lv] = 1.25;
        else if (lv == 20) dealIncrementP[4][lv] = 1.4;
        else if (lv == 30) dealIncrementP[4][lv] = 1.6;
        else dealIncrementP[4][lv] = dealIncrementP[4][lv - 1] + 0.01;
    }
    dealIncrementP[5] = dealIncrementP[4];
    dealIncrementP[6] = dealIncrementP[4];
    dealIncrementP[7] = dealIncrementP[4];

    // 하모닉 패러독스 (1레벨 기준)
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[8][lv] = ((1500 + 50 * lv) * 7 * 17 + (758 + 26 * lv) * 7 * 39)
        / ((1500 + 50 * 1) * 7 * 17 + (758 + 26 * 1) * 7 * 39);

    // 10레벨 : 몬스터 방어율 무시 20% 증가
    for (long double lv = 10; lv <= 29; ++lv) dealIncrementP[8][lv] *=
        (100 - enemyDefense * (1 - (1 - ignoreEnemyDefense / 100) * (1 - 0.09) * (1 - 0.2)))
        / (100 - enemyDefense * (1 - (1 - ignoreEnemyDefense / 100) * (1 - 0.09)));

    // 20레벨 : 보스 몬스터 공격 시 데미지 20% 증가
    long double extraDamageP = 40 + 10 + 20 + 45 + 25 + 9 + 11 + 9 + 8;
    long double totalDamageP = 100 + damageP + bossDamageP + extraDamageP;
    /*
        extraDamageP : 오리진 스킬을 사용한 극딜 때 버프 스킬, 링크 스킬 등으로 추가되는 데미지%
        다크 크레센도(40%), 히어로즈 오쓰(10%), 메이플월드 여신의 축복(20%), 소울 컨트랙트(45%), 프리드의 가호(25%, 5중첩)
        링크 스킬: 모험가 마법사, 아크, 모험가 도적(가동률 고려 18% -> 9%), 아크(가동률 고려 17% -> 8%)
    */
    for (long double lv = 20; lv <= 29; ++lv) dealIncrementP[8][lv] *= (totalDamageP + 20) / totalDamageP;

    // 30레벨 : 몬스터 방어율 무시 30% 증가, 보스 몬스터 공격 시 데미지 30% 증가 (10레벨, 20레벨 추가 효과와 합적용)
    dealIncrementP[8][30] *= (100 - enemyDefense * (1 - (1 - ignoreEnemyDefense / 100) * (1 - 0.09) * (1 - 0.5)))
        / (100 - enemyDefense * (1 - (1 - ignoreEnemyDefense / 100) * (1 - 0.09)));
    dealIncrementP[8][30] *= (totalDamageP + 50) / totalDamageP;

    // 엔드리스 다크니스 (1레벨 기준)
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[9][lv] = (830 + 25 * lv) / (830 + 25 * 1);

    // 이터널 라이트니스 (1레벨 기준)
    for (long double lv = 1; lv <= 30; ++lv) dealIncrementP[10][lv] = (605 + 25 * lv) / (605 + 25 * 1);
}

void calculateDefaultDealShare() {

    // HEXA코어 강화를 하지 않은 초기 상태의 딜점유율을 역산

    // 앱솔루트 킬 예외처리
    defaultDealShare[0] = currentDealShare[0] * 455 / (485 + 7 * currentHexaLevel[0] + 40 + 3 * currentHexaLevel[1]);

    for (int idx = 1; idx < REAL_SKILL_NUM; ++idx) defaultDealShare[idx] = currentDealShare[idx] / dealIncrementP[idx][currentHexaLevel[idx]];

    // 엔드리스 다크니스, 이터널 라이트니스 예외처리
    for (int idx = REAL_SKILL_NUM; idx < SKILL_NUM; ++idx) defaultDealShare[idx] = currentDealShare[idx] / dealIncrementP[idx][currentHexaLevel[idx - 8]];


    // 현재 딜점유율 초기화
    for (int idx = 0; idx < REAL_SKILL_NUM; ++idx) currentDealShare[idx] = defaultDealShare[idx];
    // 엔드리스 다크니스, 이터널 라이트니스 예외처리 (마스터리 코어를 개방하지 않은 초기 상태에서 존재하지 않음)
    currentDealShare[9] = 0;
    currentDealShare[10] = 0;

    // 현재 HEXA코어 강화 상태 초기화
    for (int idx = 0; idx < REAL_SKILL_NUM; ++idx) currentHexaLevel[idx] = 0;
    // 오리진 예외처리 (초기 상태에서 1레벨로 활성화)
    currentHexaLevel[8] = 1;
}

void calculateDealIncrement() {

    // 초기 상태 대비 HEXA코어 강화 레벨에 따른 딜 증가량의 절댓값을 계산

    for (int idx = 0; idx < REAL_SKILL_NUM; ++idx) {

        dealIncrement[idx][0] = 0;

        for (int lv = 1; lv <= 30; ++lv) {

            dealIncrement[idx][lv] = defaultDealShare[idx] * (dealIncrementP[idx][lv] - 1);
        }
    }

    // 라이트 리플렉션 VI 예외처리
    for (int lv = 1; lv <= 30; ++lv) {

        // 엔드리스 다크니스의 딜 상승량을 추가
        dealIncrement[1][lv] += defaultDealShare[9] * dealIncrementP[9][lv];

        // 엔드리스 다크니스의 패시브 효과로 인한 딜 상승량을 추가
        dealIncrement[1][lv] += defaultDealShare[0] * (40 + 3 * lv) / 455;
    }

    // 아포칼립스 VI 예외처리
    for (int lv = 1; lv <= 30; ++lv) {

        // 이터널 라이트니스의 딜 상승량을 추가
        dealIncrement[2][lv] += defaultDealShare[10] * dealIncrementP[10][lv];
    }
}

void calculateReinforceEfficiency() {

    // 히스토리 관리용 변수선언 및 초기 상태 기록

    long double currentTotalDeal = extraSkillDealShare;
    for (int idx = 0; idx < SKILL_NUM; ++idx) currentTotalDeal += currentDealShare[idx];

    int currentMaterialSum = 0;

    reinforceHistory.push_back(std::make_pair("초기 상태         ", std::make_pair(0, 0)));
    levelHistory.push_back(currentHexaLevel);
    totalDealHistory.push_back(currentTotalDeal);
    dealIncrementHistory.push_back(0);
    finalDealHistory.push_back(100);
    finalDealIncrementHistory.push_back(0);
    reinforceEfficiencyHistory.push_back(0);
    materialHistory.push_back(0);
    materialSumHistory.push_back(currentMaterialSum);

    // HEXA코어 강화 효율 계산에 사용할 변수 선언 및 초기화

    long double maxEfficiency, maxDealIncrement;
    int maxSkill, maxLevel, maxMaterialRequirement;

    std::array<long double, REAL_SKILL_NUM> nextBestEfficiency, nextDealIncrement;
    std::array<int, REAL_SKILL_NUM> nextLevel, nextMaterialRequirement;
    std::array<bool, REAL_SKILL_NUM> reinforceCompleted;

    for (int idx = 0; idx < REAL_SKILL_NUM; ++idx) {

        nextBestEfficiency[idx] = 0;
        reinforceCompleted[idx] = false;

        int materialSum = 0;

        // 강화 가능한 모든 스킬들에 대해 현재 레벨보다 높은 모든 레벨에 대해 확인하며 가장 강화 효율이 높은 레벨 탐색

        for (int lv = currentHexaLevel[idx] + 1; lv <= 30; ++lv) {

            materialSum += materialRequirements[materialType][idx / 4][lv];

            if (dealIncrement[idx][lv] / materialSum - nextBestEfficiency[idx] > THRESHOLD) {

                nextBestEfficiency[idx] = dealIncrement[idx][lv] / materialSum;
                nextLevel[idx] = lv;
                nextDealIncrement[idx] = dealIncrement[idx][lv];
                nextMaterialRequirement[idx] = materialSum;
            }
        }
    }

    // HEXA코어 강화 효율 계산

    while (true) {

        // 강화 효율을 비교하여 현재 스텝에서 강화할 스킬을 선택

        maxEfficiency = 0;

        for (int idx = 0; idx < REAL_SKILL_NUM; ++idx) {

            if (reinforceCompleted[idx]) continue;

            if (maxEfficiency < nextBestEfficiency[idx]) {

                maxEfficiency = nextBestEfficiency[idx];
                maxDealIncrement = nextDealIncrement[idx];
                maxSkill = idx;
                maxLevel = nextLevel[idx];
                maxMaterialRequirement = nextMaterialRequirement[idx];
            }
        }

        // 현재 스텝을 통해 강화한 스킬이 30레벨을 달성하였을 경우
        if (maxLevel == 30) reinforceCompleted[maxSkill] = true;

        // 현재 스탭에서 강화할 스킬이 없는 경우, 즉 모든 스킬이 30레벨이 달성된 경우
        if (maxEfficiency == 0) break;

        // 히스토리에 현재 스텝의 강화 결과 추가
        reinforceHistory.push_back(std::make_pair(skillNameSet[maxSkill], std::make_pair(currentHexaLevel[maxSkill], maxLevel)));

        currentHexaLevel[maxSkill] = maxLevel;
        currentTotalDeal += maxDealIncrement;
        currentMaterialSum += maxMaterialRequirement;

        levelHistory.push_back(currentHexaLevel);
        totalDealHistory.push_back(currentTotalDeal);
        dealIncrementHistory.push_back(maxDealIncrement);
        finalDealHistory.push_back(100 * currentTotalDeal / totalDealHistory.front());
        finalDealIncrementHistory.push_back(100 * maxDealIncrement / (totalDealHistory.back() - maxDealIncrement));
        reinforceEfficiencyHistory.push_back(finalDealIncrementHistory.back() * 100 / maxMaterialRequirement);
        materialHistory.push_back(maxMaterialRequirement);
        materialSumHistory.push_back(currentMaterialSum);

        // 현재 스텝에서 강화한 스킬에 대해서 다시 가장 강화 효율이 높은 레벨 탐색
        int materialSum = 0;
        nextBestEfficiency[maxSkill] = 0;

        for (int lv = currentHexaLevel[maxSkill] + 1; lv <= 30; ++lv) {

            materialSum += materialRequirements[materialType][maxSkill / 4][lv];

            if ((dealIncrement[maxSkill][lv] - dealIncrement[maxSkill][currentHexaLevel[maxSkill]]) / materialSum - nextBestEfficiency[maxSkill] > THRESHOLD) {

                nextBestEfficiency[maxSkill] = (dealIncrement[maxSkill][lv] - dealIncrement[maxSkill][currentHexaLevel[maxSkill]]) / materialSum;
                nextLevel[maxSkill] = lv;
                nextDealIncrement[maxSkill] = dealIncrement[maxSkill][lv] - dealIncrement[maxSkill][maxLevel];
                nextMaterialRequirement[maxSkill] = materialSum;
            }
        }
    }
}

void displayResult() {

    // HEXA코어 강화 효율 계산 결과를 콘솔창에 표시

    int totalStep = reinforceHistory.size();

    std::cout << "강화한 스킬\t\tLv(전)\tLv(후)\t\t";
    std::cout << "앱킬VI\t라맆VI\t아포VI\t노바VI\t진문\t퍼니싱\t빛둠\t오브\t오리진\t\t";
    std::cout << "딜\t딜증가\t최종뎀\t최뎀증\t효율\t재화\t총재화\n";

    for (int idx = 0; idx < totalStep; ++idx) {

        std::cout << reinforceHistory[idx].first << '\t' << reinforceHistory[idx].second.first << '\t' << reinforceHistory[idx].second.second << "\t\t";

        for (int idx2 = 0; idx2 < REAL_SKILL_NUM; ++idx2) std::cout << levelHistory[idx][idx2] << '\t';
        std::cout << '\t';

        std::cout << totalDealHistory[idx] << '\t';
        std::cout << dealIncrementHistory[idx] << '\t';
        std::cout << finalDealHistory[idx] << '\t';
        std::cout << finalDealIncrementHistory[idx] << '\t';
        std::cout << reinforceEfficiencyHistory[idx] << '\t';
        std::cout << materialHistory[idx] << '\t';
        std::cout << materialSumHistory[idx] << '\n';
    }

    std::cout << '\n';
}

void displayResult2() {

    // HEXA코어 강화순서만을 콘솔창에 표시

    int totalStep = reinforceHistory.size();

    for (int idx = 1; idx < totalStep; ++idx) {

        std::cout << reinforceHistory[idx].first << '\t' << ": ";
        if (reinforceHistory[idx].second.first < 10) std::cout << ' ';
        std::cout << reinforceHistory[idx].second.first << " Lv -> ";
        if (reinforceHistory[idx].second.second < 10) std::cout << ' ';
        std::cout << reinforceHistory[idx].second.second << " Lv\n";
    }

    std::cout << '\n';
}

void getUserInput() {

    // HEXA코어 강화 효율 계산을 위해 사용자로부터 1) 딜점유율 및 HEXA코어 강화 상태 2) 캐릭터 스펙 3) 강화 효율 계산 기준 (솔 에르다/솔 에르다 조각) 을 입력받음 

    std::cout << "스킬들의 딜점유율 및 HEXA코어 강화 레벨을 입력해 주세요\n\n";

    for (int idx = 0; idx < SKILL_NUM; ++idx) {

        const std::string& skillName = skillNameSet[idx];

        std::cout << "[" << skillName << "] 의 점유율(%) : ";
        std::cin >> currentDealShare[idx];

        // 엔드리스 다크니스와 이터널 라이트니스는 HEXA 레벨을 입력받지 않음
        if (idx < 9) {

            std::cout << "[" << skillName << "] 의 HEXA코어 강화 레벨 : ";
            std::cin >> currentHexaLevel[idx];

            std::cout << '\n';
        }/*
        else {

            // 엔드리스 다크니스는 라이트 리플렉션 VI와, 이터널 라이트니스는 아포칼립스 VI와 같은 레벨로 설정
            currentHexaLevel[idx] = currentHexaLevel[idx - 8];
        }*/
    }
    std::cout << "\n\n";

    std::cout << "보다 정확한 HEXA코어 강화 효율 계산을 위해 캐릭터 스펙의 입력이 필요합니다.\n";
    std::cout << "필요한 스펙 : 풀 도핑 상태에서의 데미지(%), 보스 몬스터 공격 시 데미지(%), 몬스터 방어율 무시(%)\n";
    std::cout << "입력하지 않을 경우 내부적으로 설정된 디폴트 캐릭터 스펙을 사용해 계산합니다.\n";
    std::cout << "입력하시겠습니까?(Y/N) : ";

    char userInputChar;
    std::cin >> userInputChar;

    switch (userInputChar) {

    case 'Y':

        std::cout << "데미지(%) : ";
        std::cin >> damageP;
        std::cout << '\n';

        std::cout << "보스 몬스터 공격 시 데미지(%) : ";
        std::cin >> bossDamageP;
        std::cout << '\n';

        std::cout << "몬스터 방어율 무시(%) : ";
        std::cin >> ignoreEnemyDefense;
        std::cout << '\n';

        break;

    case 'N':

        std::cout << "디폴트 캐릭터 스펙을 사용해 계산을 진행합니다.\n";

        break;

    default:

        std::cout << "부정확한 입력입니다.\n";
        std::cout << "디폴트 캐릭터 스펙을 사용해 계산을 진행합니다.\n";

        break;
    }
    std::cout << '\n';

    std::cout << "HEXA코어 강화 효율 계산을 솔 에르다 기준으로 할지, 솔 에르다 조각 기준으로 할지 결정해주세요.\n";
    std::cout << "솔 에르다 기준(Y), 솔 에르다 조각 기준(N) : ";
    std::cin >> userInputChar;

    switch (userInputChar) {

    case 'Y':

        std::cout << "솔 에르다 기준으로 계산을 진행합니다.\n";
        materialType = 0;

        break;

    case 'N':

        std::cout << "솔 에르다 조각 기준으로 계산을 진행합니다.\n";
        materialType = 1;

        break;

    default:

        std::cout << "부정확한 입력입니다.\n";
        std::cout << "솔 에르다 기준으로 계산을 진행합니다.\n";
        materialType = 0;

        break;
    }
    std::cout << '\n';
}



int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::cout << std::fixed;
    std::cout.precision(3);

    // freopen("input.txt", "r", stdin);

    getUserInput();

    /*
    std::cout << "currentDealShare\n";
    for (int idx = 0; idx < SKILL_NUM; ++idx) std::cout << currentDealShare[idx] << ' ';
    std::cout << "\n\n";
    */

    calculateExtraSkillDealShare();

    calculateDealIncrementP();

    /*
    std::cout << "dealIncrementP\n";
    for (int idx = 0; idx < SKILL_NUM; ++idx) {

        for (int lv = 0; lv <= 30; ++lv) std::cout << dealIncrementP[idx][lv] << ' ';
        std::cout << '\n';
    }
    std::cout << '\n';
    */

    calculateDefaultDealShare();

    /*
    std::cout << "defaultDealShare\n";
    for (int idx = 0; idx < SKILL_NUM; ++idx) std::cout << defaultDealShare[idx] << ' ';
    std::cout << "\n\n";
    */

    calculateDealIncrement();

    /*
    std::cout << "dealIncrement\n";
    for (int idx = 0; idx < REAL_SKILL_NUM; ++idx) {

        for (int lv = 0; lv <= 30; ++lv) std::cout << dealIncrement[idx][lv] << ' ';
        std::cout << '\n';
    }
    std::cout << '\n';
    */

    calculateReinforceEfficiency();

    displayResult();

    displayResult2();

    return 0;
}
