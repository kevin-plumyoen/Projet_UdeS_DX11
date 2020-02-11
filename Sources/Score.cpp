#include "Score.h"
#include <filesystem>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <iterator>

CScore::CScore(std::string fichierTexte)
    :fichierScore(fichierTexte)
{
    format_file();
}

void CScore::format_file() {
    std::ifstream fichierscore{ fichierScore };
    std::string line;
    while (getline(fichierscore, line))
        /************************************************************************************************
        *                           LE BUG SE TROUVE SUR LA LIGNE SUIVANTE                              *
        ************************************************************************************************/
        scoreBoard.emplace_back(line.begin(),line.end());
    fichierscore.close();
}

void CScore::add_score(std::wstring score) {
    if ((scoreBoard.size()<10) || is_best_score(score))
    //{
        scoreBoard.emplace_back(score);
    /*    std::ofstream fichierscore{ fichierScore,std::ios::app };
        fichierscore.seekp(std::ios_base::end);
        fichierscore << std::string(score.begin(), score.end());
        fichierscore.close();
    }*/
    else if ((scoreBoard.size() > 10) && is_best_score(score))
        scoreBoard.at(score_place(score_get_line_to_del(score))) = score;
        //rewrite_file(score);
}

bool CScore::is_best_score(std::wstring pScore)
{
    /*assert(!pScore.empty());
    assert(!scoreBoard.at(current_sc).empty());
    int score_debut = std::stoi(pScore);
    int score_contenu = std::stoi(scoreBoard.at(current_sc));
    return (score_debut > score_contenu )? true :
                ((++current_sc)<10)?is_best_score(pScore, current_sc):false ;*/
    std::vector<std::wstring> best_sc = ten_best_scores();
    return std::find(best_sc.begin(),best_sc.end(),pScore) != best_sc.end();
}

std::vector<std::wstring> CScore::ten_best_scores()
{
    std::sort(scoreBoard.begin(), scoreBoard.end(), [](std::wstring beg_sc, std::wstring end_sc) {
        int score_debut = std::stoi(beg_sc);
        int score_contenu = std::stoi(end_sc);
        return score_debut < score_contenu;
    });
    std::vector<std::wstring> best_sc;
    if(scoreBoard.size()>10)
        std::copy(std::prev(scoreBoard.end(), 10), scoreBoard.end(), std::back_inserter(best_sc));
    else best_sc = scoreBoard;
    return best_sc;
}

int CScore::score_place(std::wstring score)
{
    int i = 0;
    for (i; (std::stoi(score) < std::stoi(scoreBoard.at(i))) || (i < scoreBoard.size()); ++i);
    return i>-1?i-1:i;
}

std::wstring CScore::score_get_line_to_del(std::wstring score)
{
    for (std::wstring current_score : scoreBoard)
        if (std::stoi(current_score) > std::stoi(score))
            return current_score;
    return scoreBoard.at(9);
}

/*
void CScore::rewrite_file(std::wstring score)
{
    scoreBoard.at(score_place(score_get_line_to_del(score))) = score;
    std::ifstream sup{ fichierScore };
    std::ofstream temp{ "temp.txt" };
    std::string deleteline = std::string(score_get_line_to_del(score).begin(), score_get_line_to_del(score).end());
    std::string line;
    while (std::getline(sup, line))
        if (line != deleteline)
            temp << line;
        else temp << std::string(score.begin(), score.end());
    temp.close();
    sup.close();
    std::filesystem::remove(fichierScore);
    std::filesystem::rename("temp.txt", fichierScore);
}*/

void CScore::save_to_disk()
{
    std::fstream fichierscore{ fichierScore };
    for (std::wstring current_score : scoreBoard)
        fichierscore << std::string(current_score.begin(),current_score.end());
    fichierscore.close();
}

std::vector<std::wstring> CScore::getText() { return scoreBoard; }