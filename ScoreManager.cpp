#include "ScoreManager.h"
#include <fstream>
#include <chrono>

ScoreManager::ScoreManager() {}

void ScoreManager::enemyKilled(int& score, int basePoints) {
    auto now = std::chrono::steady_clock::now();
    float timeSinceLastKill = std::chrono::duration<float>(now - lastKillTime).count();

    // Reset combo nếu quá thời gian
    if (timeSinceLastKill > COMBO_TIME_WINDOW) {
        comboCount = 0;
    }

    comboCount++;
    lastKillTime = now;

    // Áp dụng hệ số combo
    float multiplier = 1.0f;
    for (const auto& [kills, bonus] : COMBO_MULTIPLIERS) {
        if (comboCount >= kills) {
            multiplier = bonus;
        }
    }

    score += static_cast<int>(basePoints * multiplier);
}

void ScoreManager::calculateTimeBonus(int& score, float levelTime, float targetTime) {
    if (levelTime < targetTime) {
        int bonus = static_cast<int>((targetTime - levelTime) * 10); // +10 điểm/giây
        score += bonus;
    }
}

void ScoreManager::loadHighScores(std::map<int, int>& highScores) {
    std::ifstream in("highscores.txt");
    if (!in) return;
    int level, hs;
    while (in >> level >> hs) {
        highScores[level] = hs;
    }
    in.close();
}

void ScoreManager::saveHighScores(const std::map<int, int>& highScores) {
    std::ofstream out("highscores.txt");
    for (const auto& [level, hs] : highScores) {
        out << level << " " << hs << "\n";
    }
    out.close();
}

void ScoreManager::updateHighScore(int level, int score, std::map<int, int>& highScores) {
    if (score > highScores[level]) {
        highScores[level] = score;
        saveHighScores(highScores);
    }
}
