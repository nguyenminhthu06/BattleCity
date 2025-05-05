#include "ScoreManager.h"
#include <fstream>

ScoreManager::ScoreManager() {}

void ScoreManager::enemyKilled(int& score) {
    score += 100;
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
