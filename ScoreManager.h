#pragma once
#include <map>
#include <chrono>

class ScoreManager {
private:
    int comboCount = 0;
    std::chrono::steady_clock::time_point lastKillTime;
    const float COMBO_TIME_WINDOW = 2.0f; // Combo hợp lệ trong 2 giây
    const std::map<int, float> COMBO_MULTIPLIERS = {
        {3, 1.5f},  // x1.5 điểm nếu 3 kills liên tiếp
        {5, 2.0f}    // x2.0 điểm nếu 5 kills liên tiếp
    };

public:
    ScoreManager();

    // Cập nhật điểm khi tiêu diệt kẻ địch (có combo)
    void enemyKilled(int& score, int basePoints = 100);

    // Thưởng điểm theo thời gian hoàn thành
    void calculateTimeBonus(int& score, float levelTime, float targetTime);

    // Xử lý điểm cao
    void loadHighScores(std::map<int, int>& highScores);
    void saveHighScores(const std::map<int, int>& highScores);
    void updateHighScore(int level, int score, std::map<int, int>& highScores);
};
