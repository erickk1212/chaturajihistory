#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#if __has_include("fmt/format.h")
#include "fmt/format.h"
namespace std { using namespace fmt; }
#else
#include <format>
#endif
using namespace std;
//RBYG
#define RED "\033[31m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define PURPLE "\033[35m"
#define RESET "\033[0m"
#define EMPTYSCREEN "\033[2J\033[H"
class chaturaji
{
public:
    enum color { X, red, blue, yellow, green };
    enum type { x, ship, knight, bishop, king, pawn };
    struct piece { color c; type t; };
    piece board[8][8];
private:
    int score[4];
    int turn;
    bool alive[4];
public:
    chaturaji()
    {
        score[0] = score[1] = score[2] = score[3] = 0;
        turn = 1;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                board[i][j] = { X, x };
            }
        }
        alive[0] = alive[1] = alive[2] = alive[3] = true;
        //chaturaji board initialization
        //blue
        board[0][0] = { blue,ship };
        board[1][0] = { blue,knight };
        board[2][0] = { blue,bishop };
        board[3][0] = { blue,king };
        board[0][1] = { blue,pawn };
        board[1][1] = { blue,pawn };
        board[2][1] = { blue,pawn };
        board[3][1] = { blue,pawn };
        //red
        board[7][0] = { red,ship };
        board[7][1] = { red,knight };
        board[7][2] = { red,bishop };
        board[7][3] = { red,king };
        board[6][0] = { red,pawn };
        board[6][1] = { red,pawn };
        board[6][2] = { red,pawn };
        board[6][3] = { red,pawn };
        //green
        board[7][7] = { green,ship };
        board[6][7] = { green,knight };
        board[5][7] = { green,bishop };
        board[4][7] = { green,king };
        board[7][6] = { green,pawn };
        board[6][6] = { green,pawn };
        board[5][6] = { green,pawn };
        board[4][6] = { green,pawn };
        //yellow
        board[0][7] = { yellow,ship };
        board[0][6] = { yellow,knight };
        board[0][5] = { yellow,bishop };
        board[0][4] = { yellow,king };
        board[1][7] = { yellow,pawn };
        board[1][6] = { yellow,pawn };
        board[1][5] = { yellow,pawn };
        board[1][4] = { yellow,pawn };
    }

    chaturaji(const piece(&boardstate)[8][8], const int s[4])
    {
        score[0] = score[1] = score[2] = score[3] = 0;
        alive[0] = alive[1] = alive[2] = alive[3] = 0;
        turn = 1;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                board[i][j] = boardstate[i][j];
                if (board[i][j].t == king) alive[board[i][j].c - 1] = true;
            }
            if (i < 4) score[i] = s[i];
        }
    }

    chaturaji& operator=(const piece(&boardstate)[8][8])
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                board[i][j] = boardstate[i][j];
            }
        }
        return *this;
    }

    void update_score(color c, int delta)
    {
        if (c == X) return;
        score[c - 1] += delta;
    }

    color cur_turn()
    {
        return (color)((turn - 1) % 4 + 1);
    }

    bool is_valid_move(int x1, int y1, int x2, int y2)
    {
        if (x1 < 0 || x1 >= 8 || y1 < 0 || y1 >= 8 ||
            x2 < 0 || x2 >= 8 || y2 < 0 || y2 >= 8) return false;
        piece p = board[x1][y1];
        if (p.c != cur_turn()) return false;
        vector<pair<int, int>> moves = get_legal_moves(x1, y1);
        return find(moves.begin(), moves.end(), make_pair(x2, y2)) != moves.end();
    }

    bool pawn_promotion(color c, int x, int y)
    {
        if (board[x][y].t != pawn) return false;
        switch (c)
        {
        case red: return x == 0;
        case blue: return y == 7;
        case yellow: return x == 7;
        case green: return y == 0;
        default: return false;
        }
    }

    bool move(int x1, int y1, int x2, int y2)
    {
        if (!is_valid_move(x1, y1, x2, y2)) return false;
        piece& from = board[x1][y1];
        piece& to = board[x2][y2];
        if (to.c != X && to.c != from.c && alive[to.c - 1])
        {
            if (to.t == king)
            {
                alive[to.c - 1] = false; // 킹 잡히면 탈락
            }
            update_score(from.c, get_score_of_piece(to.t));
        }
        to = from;
        from = { X, x };
        if (pawn_promotion(to.c, x2, y2))
        {
            to.t = ship;
        }
        do
        {
            turn++;
        } while (!alive[(turn - 1) % 4]);
        return true;
    }

    color is_game_over()
    {
        int alive_count = 0;
        for (int i = 0; i < 4; i++) if (alive[i]) alive_count++;
        if (alive_count <= 1)
        {
            int max_score = -1, winner = -1;
            for (int i = 0; i < 4; i++)
            {
                if (score[i] > max_score)
                {
                    max_score = score[i];
                    winner = i + 1;
                }
            }
            return (color)winner;
        }
        return X;
    }

    vector<pair<int, int>> get_legal_moves(int x, int y)
    {
        if (board[x][y].c != cur_turn()) return vector<pair<int, int>>();
        vector<pair<int, int>> moves;
        switch (board[x][y].t)
        {
        case ship:
            for (int i = x + 1; i < 8; i++)
            {
                if (board[i][y].c == cur_turn()) break;
                if (board[i][y].c != X) { moves.push_back(make_pair(i, y)); break; }
                moves.push_back(make_pair(i, y));
            }
            for (int i = x - 1; i >= 0; i--)
            {
                if (board[i][y].c == cur_turn()) break;
                if (board[i][y].c != X) { moves.push_back(make_pair(i, y)); break; }
                moves.push_back(make_pair(i, y));
            }
            for (int i = y + 1; i < 8; i++)
            {
                if (board[x][i].c == cur_turn()) break;
                if (board[x][i].c != X) { moves.push_back(make_pair(x, i)); break; }
                moves.push_back(make_pair(x, i));
            }
            for (int i = y - 1; i >= 0; i--)
            {
                if (board[x][i].c == cur_turn()) break;
                if (board[x][i].c != X) { moves.push_back(make_pair(x, i)); break; }
                moves.push_back(make_pair(x, i));
            }
            break;
        case knight:
            if (x + 2 < 8 && y + 1 < 8 && board[x + 2][y + 1].c != cur_turn()) moves.push_back(make_pair(x + 2, y + 1));
            if (x + 2 < 8 && y - 1 >= 0 && board[x + 2][y - 1].c != cur_turn()) moves.push_back(make_pair(x + 2, y - 1));
            if (x - 2 >= 0 && y + 1 < 8 && board[x - 2][y + 1].c != cur_turn()) moves.push_back(make_pair(x - 2, y + 1));
            if (x - 2 >= 0 && y - 1 >= 0 && board[x - 2][y - 1].c != cur_turn()) moves.push_back(make_pair(x - 2, y - 1));
            if (x + 1 < 8 && y + 2 < 8 && board[x + 1][y + 2].c != cur_turn()) moves.push_back(make_pair(x + 1, y + 2));
            if (x + 1 < 8 && y - 2 >= 0 && board[x + 1][y - 2].c != cur_turn()) moves.push_back(make_pair(x + 1, y - 2));
            if (x - 1 >= 0 && y + 2 < 8 && board[x - 1][y + 2].c != cur_turn()) moves.push_back(make_pair(x - 1, y + 2));
            if (x - 1 >= 0 && y - 2 >= 0 && board[x - 1][y - 2].c != cur_turn()) moves.push_back(make_pair(x - 1, y - 2));
            break;
        case bishop:
            for (int i = 1; x + i < 8 && y + i < 8; i++)
            {
                if (board[x + i][y + i].c == cur_turn()) break;
                if (board[x + i][y + i].c != X) { moves.push_back(make_pair(x + i, y + i)); break; }
                moves.push_back(make_pair(x + i, y + i));
            }
            for (int i = 1; x - i >= 0 && y - i >= 0; i++)
            {
                if (board[x - i][y - i].c == cur_turn()) break;
                if (board[x - i][y - i].c != X) { moves.push_back(make_pair(x - i, y - i)); break; }
                moves.push_back(make_pair(x - i, y - i));
            }
            for (int i = 1; x + i < 8 && y - i >= 0; i++)
            {
                if (board[x + i][y - i].c == cur_turn()) break;
                if (board[x + i][y - i].c != X) { moves.push_back(make_pair(x + i, y - i)); break; }
                moves.push_back(make_pair(x + i, y - i));
            }
            for (int i = 1; x - i >= 0 && y + i < 8; i++)
            {
                if (board[x - i][y + i].c == cur_turn()) break;
                if (board[x - i][y + i].c != X) { moves.push_back(make_pair(x - i, y + i)); break; }
                moves.push_back(make_pair(x - i, y + i));
            }
            break;
        case king:
            for (int i = -1; i <= 1; i++)
                for (int j = -1; j <= 1; j++)
                    if (i != 0 || j != 0)
                        if (x + i >= 0 && x + i < 8 && y + j >= 0 && y + j < 8)
                            if (board[x + i][y + j].c != cur_turn())
                                moves.push_back(make_pair(x + i, y + j));
            break;
        case pawn:
            switch (cur_turn())
            {
            case red:
                if (x - 1 >= 0 && board[x - 1][y].c == X) moves.push_back(make_pair(x - 1, y));
                if (x - 1 >= 0 && y - 1 >= 0 && board[x - 1][y - 1].c != X && board[x - 1][y - 1].c != cur_turn()) moves.push_back(make_pair(x - 1, y - 1));
                if (x - 1 >= 0 && y + 1 < 8 && board[x - 1][y + 1].c != X && board[x - 1][y + 1].c != cur_turn()) moves.push_back(make_pair(x - 1, y + 1));
                break;
            case blue:
                if (y + 1 < 8 && board[x][y + 1].c == X) moves.push_back(make_pair(x, y + 1));
                if (x - 1 >= 0 && y + 1 < 8 && board[x - 1][y + 1].c != X && board[x - 1][y + 1].c != cur_turn()) moves.push_back(make_pair(x - 1, y + 1));
                if (x + 1 < 8 && y + 1 < 8 && board[x + 1][y + 1].c != X && board[x + 1][y + 1].c != cur_turn()) moves.push_back(make_pair(x + 1, y + 1));
                break;
            case yellow:
                if (x + 1 < 8 && board[x + 1][y].c == X) moves.push_back(make_pair(x + 1, y));
                if (x + 1 < 8 && y - 1 >= 0 && board[x + 1][y - 1].c != X && board[x + 1][y - 1].c != cur_turn()) moves.push_back(make_pair(x + 1, y - 1));
                if (x + 1 < 8 && y + 1 < 8 && board[x + 1][y + 1].c != X && board[x + 1][y + 1].c != cur_turn()) moves.push_back(make_pair(x + 1, y + 1));
                break;
            case green:
                if (y - 1 >= 0 && board[x][y - 1].c == X) moves.push_back(make_pair(x, y - 1));
                if (x - 1 >= 0 && y - 1 >= 0 && board[x - 1][y - 1].c != X && board[x - 1][y - 1].c != cur_turn()) moves.push_back(make_pair(x - 1, y - 1));
                if (x + 1 < 8 && y - 1 >= 0 && board[x + 1][y - 1].c != X && board[x + 1][y - 1].c != cur_turn()) moves.push_back(make_pair(x + 1, y - 1));
                break;
            default: return vector<pair<int, int>>();
            }
            break;
        default: return vector<pair<int, int>>();
        }
        return moves;
    }

    void print_board()
    {
        cout << EMPTYSCREEN;
        cout << CYAN << "==" << PURPLE << " CLASSIC "
            << RED << 'C' << BLUE << 'H' << YELLOW << 'A' << GREEN << 'T' << RED << 'U'
            << BLUE << 'R' << YELLOW << 'A' << GREEN << 'J' << RED << 'I'
            << PURPLE << " GAME " << CYAN << "==" << RESET << '\n';
        cout << "    A  B  C  D  E  F  G  H\n";
        cout << "   +--+--+--+--+--+--+--+--+\n";
        for (int i = 0; i < 8; i++)
        {
            cout << i + 1 << "  ";
            for (int j = 0; j < 8; j++)
            {
                cout << RESET << "|";
                switch (board[i][j].c)
                {
                case blue:   cout << BLUE << 'B'; break;
                case yellow: cout << YELLOW << 'Y'; break;
                case red:    cout << RED << 'R'; break;
                case green:  cout << GREEN << 'G'; break;
                default: cout << " ";
                }
                switch (board[i][j].t)
                {
                case king:   cout << "K"; break;
                case bishop: cout << "B"; break;
                case ship:   cout << "S"; break;
                case knight: cout << "N"; break;
                case pawn:   cout << "P"; break;
                default: cout << " ";
                }
            }
            cout << RESET << "|\n   +--+--+--+--+--+--+--+--+\n";
        }
        cout << "   +-----+-----+-----+-----+\n";
        printf("   |%sR%s:%3d|%sB%s:%3d|%sY%s:%3d|%sG%s:%3d|\n", RED, RESET, score[0], BLUE, RESET, score[1], YELLOW, RESET, score[2], GREEN, RESET, score[3]);
        cout << "   +-----+-----+-----+-----+\n";
    }

    int get_score_of_piece(type t)
    {
        switch (t)
        {
        case ship: return 5;
        case knight: return 3;
        case bishop: return 5;
        case king: return 3;
        case pawn: return 1;
        default: return 0;
        }
    }

    vector<int> get_score()
    {
        return vector<int>(score, score + 4);
    }

    bool is_king_dead(int player)
    {
        return !alive[player - 1];
    }

    int count_alive()
    {
        int cnt = 0;
        for (int i = 0; i < 4; i++) if (alive[i]) cnt++;
        return cnt;
    }

    void set_turn(int t)
    {
        turn = t;
    }

    ~chaturaji() = default;
};

void play()
{
    chaturaji g;
    g.print_board();
    vector<long long> result = make_points(g);
    // 결과 출력
    cout << "각 팀의 게임 평가 계산 결과:\n";
    cout << RED << "red   " << RESET << ": " << result[0] << "\n";
    cout << BLUE << "blue  " << RESET << ": " << result[1] << "\n";
    cout << YELLOW << "yellow" << RESET << ": " << result[2] << "\n";
    cout << GREEN << "green " << RESET << ": " << result[3] << "\n";
    string t[4] =
    {
        (string)RED + "red" + RESET,
        (string)BLUE + "blue" + RESET,
        (string)YELLOW + "yellow" + RESET,
        (string)GREEN + "green" + RESET
    };
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            // 4명 순서대로
            if (g.cur_turn() != (chaturaji::color)(i + 1))continue;
            if (g.is_game_over() != chaturaji::X)
            {
                cout << "게임이 종료되었습니다. 승자는 " << t[g.is_game_over() - 1] << "입니다. 돌아가려면 아무거나 입력하고 엔터를 누르세요.\n";
                string s;
                cin >> s;
                return;
            }
            if (g.is_king_dead(i + 1)) continue;
            cout << t[i] << "의 차례입니다.\n";
            cout << "이동할 말의 좌표를 입력하세요.\n대문자로 입력하세요.\n";
            int x1, y1, x2, y2;
            string coord1;
            cin >> coord1;
            y1 = coord1[0] - 'A';
            x1 = coord1[1] - '1';
            cout << "이동할 좌표를 입력하세요.\n";
            string coord2;
            cin >> coord2;
            y2 = coord2[0] - 'A';
            x2 = coord2[1] - '1';
            if (!g.move(x1, y1, x2, y2))
            {
                cout << "잘못된 입력입니다.\n";
                i--;
            }
            cout << "아무거나 입력한 뒤 엔터를 누르세요.\n";
            string s;
            cin >> s;
            g.print_board();
            vector<long long> result = make_points(g);
            // 결과 출력
            cout << "각 팀의 게임 평가 계산 결과:\n";
            cout << RED << "red   " << RESET << ": " << result[0] << "\n";
            cout << BLUE << "blue  " << RESET << ": " << result[1] << "\n";
            cout << YELLOW << "yellow" << RESET << ": " << result[2] << "\n";
            cout << GREEN << "green " << RESET << ": " << result[3] << "\n";
        }
    }
}

vector<long long> make_points(chaturaji g)
{
    int alive = g.count_alive();
    long long int result[4] = { 0, 0, 0, 0 };
    //weights
    int weight1_1_1, weight1_2_1,
        weight2_1, weight2_2,
        weight3,
        weight4_1, weight4_2, weight4_3, weight4_4, weight4_5,
        weight5;
    switch (alive)
    {
    case 4: // 4명 모두 생존
        weight1_1_1 = 35;
        weight1_2_1 = 14;
        weight2_1 = 360;
        weight2_2 = 120;
        weight3 = 15;
        weight4_1 = 200;
        weight4_2 = 200;
        weight4_3 = 200;
        weight4_4 = 50;
        weight4_5 = 200;
        weight5 = 80;
        break;
    case 3: // 3명 생존
        weight1_1_1 = 25;
        weight1_2_1 = 10;
        weight2_1 = 450;
        weight2_2 = 150;
        weight3 = 10;
        weight4_1 = 140;
        weight4_2 = 140;
        weight4_3 = 140;
        weight4_4 = 35;
        weight4_5 = 140;
        weight5 = 100;
        break;
    case 2: // 2명 생존
        weight1_1_1 = 15;
        weight1_2_1 = 8;
        weight2_1 = 600;
        weight2_2 = 200;
        weight3 = 7;
        weight4_1 = 100;
        weight4_2 = 100;
        weight4_3 = 100;
        weight4_4 = 25;
        weight4_5 = 80;
        weight5 = 150;
        break;
    default: // 1명 생존
        weight1_1_1 = 0;
        weight1_2_1 = 0;
        weight2_1 = 0;
        weight2_2 = 0;
        weight3 = 0;
        weight4_1 = 0;
        weight4_2 = 0;
        weight4_3 = 0;
        weight4_4 = 0;
        weight4_5 = 0;
        weight5 = 0;
        break;
    }
    // 1 to 4
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // 1
            // 1-1
            bool check_for_1 = false;
            if (g.board[i][j].c == chaturaji::X) continue;
            g.set_turn(g.board[i][j].c);
            vector<pair<int, int>> moves = g.get_legal_moves(i, j);
            if (moves.empty()) check_for_1 = true;
            if (!check_for_1)
            {
                int cnt = moves.size();
                int weight1_1_2 = 0;
                switch (g.board[i][j].t)
                {
                case chaturaji::ship: weight1_1_2 = 20; break;
                case chaturaji::knight: weight1_1_2 = 10; break;
                case chaturaji::bishop: weight1_1_2 = 15; break;
                case chaturaji::pawn: weight1_1_2 = 5; break;
                default: weight1_1_2 = 0; break;
                }
                for (auto& move : moves)
                {
                    //바로 잡히나?
                    bool chk = true;
                    chaturaji gchk = g;
                    for (int k = 0; k < 8; k++)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (g.board[k][l].c != chaturaji::X && g.board[k][l].c != g.board[i][j].c)
                            {
                                gchk.set_turn(g.board[k][l].c);
                                if (gchk.is_valid_move(k, l, move.first, move.second))
                                {
                                    cnt--;
                                    chk = false;
                                }
                            }
                        }
                    }
                    // 1-2
                    if (chk)
                    {
                        chaturaji g2 = g;
                        g2.set_turn(g.cur_turn());
                        g2.move(i, j, move.first, move.second);
                        vector<pair<int, int>> moves2 = g2.get_legal_moves(move.first, move.second);
                        int cnt2 = moves2.size();
                        int weight1_2_2 = weight1_1_2;
                        for (auto& move2 : moves2)
                        {
                            //바로 잡히나?
                            chaturaji gchk2 = g2;
                            for (int k = 0; k < 8; k++)
                            {
                                for (int l = 0; l < 8; l++)
                                {
                                    if (g2.board[k][l].c != chaturaji::X && g2.board[k][l].c != g2.board[i][j].c)
                                    {
                                        gchk2.set_turn(g2.board[k][l].c);
                                        if (gchk2.is_valid_move(k, l, move2.first, move2.second)) cnt2--;
                                    }
                                }
                            }
                        }
                        result[g.cur_turn() - 1] += cnt2 * weight1_2_1 * weight1_2_2;
                    }
                }
                result[g.cur_turn() - 1] += cnt * weight1_1_1 * weight1_1_2;
            }
            // 2
            if (g.board[i][j].t == chaturaji::king)
            {
                // 2-1
                g.set_turn(g.board[i][j].c);
                for (int k = 0; k < 8; k++)
                {
                    for (int l = 0; l < 8; l++)
                    {
                        if (g.board[k][l].c != chaturaji::X && g.board[k][l].c != g.board[i][j].c)
                        {
                            g.set_turn(g.board[k][l].c);
                            if (g.is_valid_move(k, l, i, j))
                                result[g.board[i][j].c - 1] -= weight2_1;
                            vector<pair<int, int>> moves = g.get_legal_moves(k, l);
                            // 2-2
                            for (auto& move : moves)
                            {
                                chaturaji g2 = g;
                                g2.set_turn(g.board[k][l].c);
                                g2.move(k, l, move.first, move.second);
                                if (g2.is_valid_move(move.first, move.second, i, j))
                                    result[g.board[i][j].c - 1] -= weight2_2;
                            }
                        }
                    }
                }
            }
            // 3
            else if (g.board[i][j].t == chaturaji::pawn)
            {
                // 3-1
                g.set_turn(g.board[i][j].c);
                vector<pair<int, int>> moves = g.get_legal_moves(i, j);
                for (auto& move : moves)
                {
                    chaturaji g2 = g;
                    g2.set_turn(g.board[i][j].c);
                    g2.move(i, j, move.first, move.second);
                    if (g2.pawn_promotion(g.board[i][j].c, move.first, move.second))
                    {
                        // 바로 잡히나?
                        bool chk = true;
                        for (int k = 0; k < 8; k++)
                        {
                            for (int l = 0; l < 8; l++)
                            {
                                if (g2.board[k][l].c != chaturaji::X && g2.board[k][l].c != g2.board[i][j].c)
                                {
                                    g2.set_turn(g2.board[k][l].c);
                                    if (g2.is_valid_move(k, l, move.first, move.second))
                                    {
                                        chk = false;
                                    }
                                }
                            }
                        }
                        if (chk)
                            result[g.board[i][j].c - 1] += weight3 * 100;
                    }
                    else
                    {
                        // 3-2
                        vector<pair<int, int>> moves2 = g2.get_legal_moves(move.first, move.second);
                        for (auto& move2 : moves2)
                        {
                            chaturaji g3 = g2;
                            g3.set_turn(g2.board[i][j].c);
                            g3.move(move.first, move.second, move2.first, move2.second);
                            if (g3.pawn_promotion(g2.board[i][j].c, move2.first, move2.second))
                            {
                                // 바로 잡히나?
                                bool chk = true;
                                for (int k = 0; k < 8; k++)
                                {
                                    for (int l = 0; l < 8; l++)
                                    {
                                        if (g3.board[k][l].c != chaturaji::X && g3.board[k][l].c != g3.board[i][j].c)
                                        {
                                            g3.set_turn(g3.board[k][l].c);
                                            if (g3.is_valid_move(k, l, move2.first, move2.second))
                                            {
                                                chk = false;
                                            }
                                        }
                                    }
                                }
                                if (chk)
                                    result[g.board[i][j].c - 1] += weight3 * 85;
                            }
                        }
                    }
                }
            }
            // 4
            if (g.board[i][j].c == chaturaji::X) continue;
            g.set_turn(g.board[i][j].c);
            vector<pair<int, int>> moves4 = g.get_legal_moves(i, j);
            for (auto& mv : moves4)
            {
                chaturaji g2 = g;
                g2.move(i, j, mv.first, mv.second);
                chaturaji::piece captured = g.board[mv.first][mv.second];
                auto value = [&](chaturaji::type t)
                    {
                        switch (t) {
                        case chaturaji::pawn: return 1;
                        case chaturaji::knight: return 3;
                        case chaturaji::bishop: return 5;
                        case chaturaji::ship: return 5;
                        case chaturaji::king: return 4;
                        default: return 0;
                        }
                    };
                int attacker = g.board[i][j].c - 1;
                int gain = 0;
                // 4-4
                if (captured.c != chaturaji::X && captured.c != g.board[i][j].c)
                {
                    bool defended = false;
                    for (int k = 0; k < 8; k++)
                        for (int l = 0; l < 8; l++)
                        {
                            if (g.board[k][l].c != chaturaji::X &&
                                g.board[k][l].c != g.board[i][j].c)
                            {
                                g.set_turn(g.board[k][l].c);
                                if (g.is_valid_move(k, l, mv.first, mv.second))
                                    defended = true;
                            }
                        }
                    if (!defended)
                        gain += value(captured.t) * weight4_4;
                }
                // 4-1
                int kingThreat = 0, otherThreat = 0;
                for (int k = 0; k < 8; k++)
                    for (int l = 0; l < 8; l++)
                    {
                        if (g2.board[k][l].c != chaturaji::X &&
                            g2.board[k][l].c != g2.board[mv.first][mv.second].c)
                        {
                            if (g2.is_valid_move(mv.first, mv.second, k, l))
                            {
                                if (g2.board[k][l].t == chaturaji::king)
                                    kingThreat++;
                                else
                                    otherThreat++;
                            }
                        }
                    }
                if (kingThreat && otherThreat)
                    gain += (value(g.board[i][j].t)) * weight4_1;
                // 4-2
                int multiThreat = 0;
                for (int k = 0; k < 8; k++)
                    for (int l = 0; l < 8; l++)
                        if (g2.board[k][l].c != chaturaji::X &&
                            g2.board[k][l].c != g2.board[mv.first][mv.second].c &&
                            g2.is_valid_move(mv.first, mv.second, k, l))
                            multiThreat++;
                if (multiThreat >= 2)
                    gain += value(g.board[i][j].t) * weight4_2;
                // 4-3
                for (int k = 0; k < 8; k++)
                    for (int l = 0; l < 8; l++)
                    {
                        if (g2.board[k][l].t == chaturaji::king &&
                            g2.board[k][l].c != g2.board[mv.first][mv.second].c &&
                            g2.is_valid_move(mv.first, mv.second, k, l))
                        {
                            // 킹이 공격받음
                            // 킹 뒤에 같은 팀 기물 있으면 스틸러 가능성
                            int di = k - mv.first, dj = l - mv.second;
                            int ni = k + di, nj = l + dj;
                            if (0 <= ni && ni < 8 && 0 <= nj && nj < 8)
                            {
                                if (g2.board[ni][nj].c == g2.board[k][l].c)
                                    gain += value(g2.board[ni][nj].t) * weight4_3;
                            }
                        }
                    }
                // 4-5
                if (captured.c != chaturaji::X)
                {
                    int v1 = value(g.board[i][j].t);
                    int v2 = value(captured.t);
                    gain += (v2 - v1) * weight4_5;
                }
                result[attacker] += gain;
            }
        }
    }
    // 5
    weight5 = 80;
    if (alive == 3) weight5 = 100;
    else if (alive == 2) weight5 = 150;
    for (int i = 0; i < 4; i++)
    {
        result[i] += g.get_score()[i] * weight5;
    }
    // 6, 7: 요구하는것이 처리되어있음

    return vector<long long>(result, result + 4);
    /*
    1.이동성
    1-1. 1수 이동가능 범위*가중치(35->25->15) * 각 기물별 가중치 차등
    1-2. 2수 이동가능 범위*가중치(14->10->8) * 각 기물별 가중치 차등
    이동 후 바로 잡히지 않는 경우만 산
    각 기물별 가중치 차등: 룩(20) > 비숍(15) > 나이트(10) > 폰(5)
    킹은 예외
    */
    /*
    2.킹 보안
    2-2. 2수 내 자신의 킹 포착 가능성*가중치(-120->-150->-200)
    2-1. 1수 내 자신의 킹 포착 가능성*가중치(-360->-450->-600)
    */
    /*
    3.폰
    (1수이내 승진가능: 100, 2수: 85)*가중치(15->10->7)
    승진후 바로먹힐 가능성이 1이라도 있으면 0으로 계산
    바로 먹히지 않고 승진되는게 하나라도 있으면 카운트됨
    */
    /*
    4.기물
    4-1.1수내 포크(한 상대의 킹과 다른기물 하나를 잡을수 있어서 그 기물 확정적으로 먹는것)*가중치(200->140->100)*기물의 가치
    4-2.1수내 핀(한 상대의 두 기물을 동시에 잡을수 있는것)*가중치(200->140->100)*기물의 가치
    4-3.1수내 스틸러(한 상대의 킹을 체크해서 같은 상대의 킹 뒤에 있는 다른 기물을 잡는것)*가중치(200->140->100)*기물의 가치
    4-4.1수내 거저기물잡기(상대가 기물을 무시하면 잡히는 경우)*가중치(50->35->25)*기물의 가치
    4-5.1수내 기물교환*가중치(200->140->80)*기물의 가치 차이(유리한 쪽한테 주기)
    중요:자신이 당하는것도 똑같이 계산해야함
    기물의 가치: 폰:1, 나이트:3, 비숍:5, 룩:5, 킹:4
    */
    /*
    5.현재 상황
    현재 포인트*(80->100->150)
    */
    /*
    6.비활성기물
    이 기물들은 0으로 처리
    킹이 잡히면 그 팀의 점수는 고정됨
    */
    /*
    7.기타
    (x->y->z)인 경우
    x->4명이 모두 생존한 경우 가중치
    y->3명이 생존한 경우 가중치
    z->2명이 생존한 경우 가중치
    */
}

void probability()
{
    cout << EMPTYSCREEN;
    cout << "게임판의 상태를 입력하세요.";
    cout << "말 예시: RN, BP, YS, GK, RB, GP, YK, XX\n";
    cout << "XX는 빈칸을 의미합니다.\n";
    chaturaji::piece board[8][8];
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            string s;
            cin >> s;
            switch (s[0])
            {
            case 'R': board[i][j].c = chaturaji::red; break;
            case 'B': board[i][j].c = chaturaji::blue; break;
            case 'Y': board[i][j].c = chaturaji::yellow; break;
            case 'G': board[i][j].c = chaturaji::green; break;
            default: board[i][j].c = chaturaji::X; break;
            }
            switch (s[1])
            {
            case 'K': board[i][j].t = chaturaji::king; break;
            case 'B': board[i][j].t = chaturaji::bishop; break;
            case 'S': board[i][j].t = chaturaji::ship; break;
            case 'N': board[i][j].t = chaturaji::knight; break;
            case 'P': board[i][j].t = chaturaji::pawn; break;
            default: board[i][j].t = chaturaji::x; break;
            }
        }
    }
    cout << "각 팀의 점수를 입력하세요.\n";
    int score[4];
    for (int i = 0; i < 4; i++) cin >> score[i];
    chaturaji g(board, score);
    g.print_board();
    vector<long long> result = make_points(g);
    // 결과 출력
    cout << "각 팀의 게임 평가 계산 결과:\n";
    cout << RED << "red   " << RESET << ": " << result[0] << "\n";
    cout << BLUE << "blue  " << RESET << ": " << result[1] << "\n";
    cout << YELLOW << "yellow" << RESET << ": " << result[2] << "\n";
    cout << GREEN << "green " << RESET << ": " << result[3] << "\n";
    cout << "아무거나 입력한 뒤 엔터를 누르세요.\n";
    string s;
    cin >> s;
}
/*
디버깅용도-복붙하셈
BS BP XX XX YK YB YN YS
BN BP XX XX YP YP YP YP
BB BP XX XX XX XX XX XX
BK BP XX XX XX XX XX XX
XX XX XX XX XX XX GP GK
XX XX XX XX XX XX GP GB
RP RP RP RP XX XX GP GN
RS RN RB RK XX XX GP GS
0 0 0 0
*/
int main()
{
    while (1)
    {
        string s;
        cout << EMPTYSCREEN << CYAN;
        cout << format("##################################################################\n");
        cout << format("#                                                                #\n");
        cout << format("#{}         ##### ##      ####   #####  ##### ######  #####        {}#\n", PURPLE, CYAN);
        cout << format("#{}        ##     ##     ##  ## ##     ##       ##   ##            {}#\n", PURPLE, CYAN);
        cout << format("#{}        ##     ##     ######  ####   ####    ##   ##            {}#\n", PURPLE, CYAN);
        cout << format("#{}        ##     ##     ##  ##     ##     ##   ##   ##            {}#\n", PURPLE, CYAN);
        cout << format("#{}         ##### ###### ##  ## #####  #####  ######  #####        {}#\n", PURPLE, CYAN);
        cout << format("#                                                                #\n");
        cout << format("# {} ##### {}##  ## {} ####  {}###### {}##  ## {}#####  {} ####  {}    ## {}###### {}#\n", RED, BLUE, YELLOW, GREEN, RED, BLUE, YELLOW, GREEN, RED, CYAN);
        cout << format("# {}##     {}##  ## {}##  ## {}  ##   {}##  ## {}##  ## {}##  ## {}    ## {}  ##   {}#\n", RED, BLUE, YELLOW, GREEN, RED, BLUE, YELLOW, GREEN, RED, CYAN);
        cout << format("# {}##     {}###### {}###### {}  ##   {}##  ## {}#####  {}###### {}    ## {}  ##   {}#\n", RED, BLUE, YELLOW, GREEN, RED, BLUE, YELLOW, GREEN, RED, CYAN);
        cout << format("# {}##     {}##  ## {}##  ## {}  ##   {}##  ## {}##  ## {}##  ## {}##  ## {}  ##   {}#\n", RED, BLUE, YELLOW, GREEN, RED, BLUE, YELLOW, GREEN, RED, CYAN);
        cout << format("# {} ##### {}##  ## {}##  ## {}  ##   {} ####  {}##  ## {}##  ## {} ####  {}###### {}#\n", RED, BLUE, YELLOW, GREEN, RED, BLUE, YELLOW, GREEN, RED, CYAN);
        cout << format("#                                                                #\n");
        cout << format("##################################################################\n");
        cout << format("{}차{}투{}라{}지 {}게{}임\n", RED, BLUE, YELLOW, GREEN, PURPLE, CYAN);
        cout << format("{}게{}임{}하{}기{}: {}G\n", RED, BLUE, YELLOW, GREEN, CYAN, PURPLE);
        cout << format("{}게{}임 {}평{}가{}하{}기{}: {}P\n", PURPLE, RED, YELLOW, BLUE, RED, GREEN, CYAN, RED);
        cout << format("{}종{}료{}: {}Q{}\n\n", CYAN, GREEN, BLUE, PURPLE, RESET);
        cin >> s;
        if (s == "G") play();
        else if (s == "P") probability();
        else if (s == "Q") break;
        else cout << "잘못된 입력입니다.\n";
    }
    return 0;
}
