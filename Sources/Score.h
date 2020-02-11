#pragma once
#include <string>
#include <vector>
#include <fstream>
class CScore
{
    std::string fichierScore;
    std::vector<std::wstring> scoreBoard;
public:
    CScore(std::string fichierTexte);
    void format_file();
    void add_score(std::wstring score);
    void rewrite_file(std::wstring score);
    void save_to_disk();
    bool is_best_score(std::wstring pScore);
    int score_place(std::wstring score);
    std::wstring score_get_line_to_del(std::wstring score);
    std::vector<std::wstring> getText();
    std::vector<std::wstring> ten_best_scores();
    ~CScore() = default;
};