#pragma once
#include <map>

class ScoreManager {
public:
    ScoreManager();
    void enemyKilled(int& score);
    void loadHighScores(std::map<int, int>& highScores);
    void saveHighScores(const std::map<int, int>& highScores);
    void updateHighScore(int level, int score, std::map<int, int>& highScores);
};
