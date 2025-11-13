#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
using namespace std;
//RBYG
class chaturaji
{
public:
	enum color { X, red, blue, yellow, green };
	enum type { x, ship, knight, bishop, king, pawn };
	struct piece { color c; type t; };
private:
	piece board[8][8];
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
		cout << "\033[2J\033[H";
		cout << "    A  B  C  D  E  F  G  H\n";
		cout << "   +--+--+--+--+--+--+--+--+\n";
		for (int i = 0; i < 8; i++)
		{
			cout << i + 1 << "  ";
			for (int j = 0; j < 8; j++)
			{
				cout << "\033[m|";
				switch (board[i][j].c)
				{
				case blue:   cout << "\033[34mB"; break;
				case yellow: cout << "\033[33mY"; break;
				case red:    cout << "\033[31mR"; break;
				case green:  cout << "\033[32mG"; break;
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
			cout << "\033[0m|\n   +--+--+--+--+--+--+--+--+\n";
		}
		cout << "   +-----+-----+-----+-----+\n";
		printf("   |\033[31mR\033[m:%3d|\033[34mB\033[m:%3d|\033[33mY\033[m:%3d|\033[32mG\033[m:%3d|\n", score[0], score[1], score[2], score[3]);
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

	~chaturaji() = default;
};

int main()
{
	chaturaji g;
	g.print_board();
	string t[4] = { "\033[31mred\033[m", "\033[34mblue\033[m", "\033[33myellow\033[m", "\033[32mgreen\033[m" };

	while (1)
	{
		for (int i = 0; i < 4; i++)
		{
			// 4명 순서대로
			if (g.cur_turn() != (chaturaji::color)(i + 1))continue;
			if (g.is_game_over() != chaturaji::X) { cout << "게임이 종료되었습니다. 승자는 " << t[g.is_game_over() - 1] << "입니다.\n"; return 0; }
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
		}
	}
	return 0;
}
