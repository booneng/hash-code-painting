#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

struct cell {
  int r;
  int c;
  cell(int r, int c) : r(r), c(c) {}
};

struct command {
  int Ncommands;
  virtual void print_command() = 0;
  virtual ~command() {};
};

struct line:command {
  int start_r, start_c, last_r, last_c, cells;
  line(int sr, int sc, int lr, int lc) : start_r(sr), start_c(sc), last_r(lr), last_c(lc) {
      cells = (sr == last_r) ? (lc - sc):(lr - sr);
      Ncommands = 1;
  }
  void print_command() {
    cout << "PAINT_LINE " << start_r << ' ' << start_c << ' ' << last_r << ' ' << last_c << '\n';
  }
};

struct square:command {
  int r, c, S;
  vector <cell> blanks;
  vector <line*> lines_in_square;
  int cellsCoveredByLines;

  square(int r, int c, int S, char **painting, vector <line*> lines) : r(r), c(c), S(S) {
    for(int i=r; i < 2*S + r + 1; i++)
      {
	for(int j=c; j < 2*S + c + 1; j++)
	  {
	    if(painting[i][j] == '.')
	      {
		cell temp(i, j);
		blanks.push_back(temp);
	      }
	  }
      }
    Ncommands = blanks.size() + 1;
    cellsCoveredByLines = 0;

    for(unsigned int i=0; i < lines.size(); i++)
      {
	int start_row = lines[i]->start_r;
	int last_row = lines[i]->last_r;
	int start_column = lines[i]->start_c;
	int last_column = lines[i]->last_c;
	if(last_row == start_row && start_row < (r + 2*S + 1) && start_row > (r - 1))
	  {
	    if((start_column < c && last_column > (c - 1)) || (start_column > (c - 1) && start_column < (c + 2*S + 1))) {
	      lines_in_square.push_back(lines[i]);
	      cellsCoveredByLines += lines[i]->cells;
	    }
	  }

	else if(last_column == start_column && start_column < (c + 2*S + 1) && start_column > c - 1)
	  {
	    if((start_row < c && last_row > (r - 1)) || (start_row > (r - 1) && start_row < (r + 2*S + 1))) {
	      lines_in_square.push_back(lines[i]);
	      cellsCoveredByLines += lines[i]->cells;
	    }
	  }
      }
  }
  
  void print_command() {
    cout << "PAINT_SQUARE " << r << ' ' << c << ' ' << S << '\n';
    
    for(unsigned int i=0; i < blanks.size(); i++)
      cout << "ERASE_CELL " << blanks[i].r << ' ' << blanks[i].c << '\n';
  }
};

void check_line(int i, int j, char **painting, int *end_line, int N) {
  int column_length = 0;
  int row = i;
  while(row < N && painting[row][j]=='#')
    {
      column_length++;
      row++;
    }
  
  int row_length = 0;
  int column = j;
  
  while(painting[i][column] == '#'&& painting[i][column] != '\0')
    {
      row_length++;
      column++;
    }
  if(row_length > column_length)
    {
      for(int n=j; n < j + row_length; n++)
	painting[i][n] = '.';
      end_line[0] = i;
      end_line[1] = column - 1;
    }
  else
    {
      for(int n=i; n < i + column_length; n++)
  	painting[n][j] = '.';
      end_line[0] = row - 1;
      end_line[1] = j;
    }
  
}

bool check_squares(cell C, int &S, char **painting, vector <line*> &lines, int N, int M) {
  square sq(C.r, C.c, S, painting, lines);
  if(sq.blanks.size() + 1 < sq.lines_in_square.size()) {// && sq.cellsCoveredByLines < pow(2*S + 1, 3)) {
    int nextS = S + 1;
    if(C.r + 2*nextS < N && C.c + 2*nextS < M && check_squares(C, nextS, painting, lines, N, M)) {
      S = nextS;
    }
    return true;
  }
  return false;
}

void remove_square_from_painting(square *sq, char **painting) {
  int r = sq->r;
  int c = sq->c;
  int S = sq->S;
  for(int i=r; i < r + 2*S + 1; i++) {
    for(int j=c; j < c + 2*S + 1; j++) {
      if(painting[i][j] == '#')
	painting[i][j] = '.';
    } 
  }
}

void add_lines_to_painting(vector <line*> &lines, char **painting) {
  for(unsigned int i=0; i < lines.size(); i++) {
    int start_c = lines[i]->start_c;
    int last_c = lines[i]->last_c;
    int start_r = lines[i]->start_r;
    int last_r = lines[i]->last_r;
    if(start_r == last_r) {
      for(int j=start_c; j < last_c + 1; j++) {
	painting[start_r][j] = '#';
      }
    }
    else {
      for(int j=start_r; j < last_r + 1; j++) {
	painting[j][start_c] = '#';
      }
    }
  }

}

void remove_lines_from_vector(vector <line*> &sqlines, vector <line*> &lines) {
  for(unsigned int i=0; i < sqlines.size(); i++) {
    for(unsigned int j=0; j < lines.size(); j++) {
      if(sqlines[i] == lines[j]) {
	lines.erase(lines.begin() + j);
      }
    }
  }
}

void add_lines_to_vector(vector <line*> &lines, char **painting, int N, int M) {
  for(int i=0; i < N; i++)
    {
      for(int j=0; j < M; j++)
  	{
  	  if(painting[i][j] == '#')
  	    {
  	      int end_line[2];
  	      check_line(i, j, painting, end_line, N);
  	      line *temp = new line(i, j, end_line[0], end_line[1]);
  	      lines.push_back(temp);
  	    }
  	}
    }
}

void print_painting(char **painting, int M) {
  for(int i=0; i < M; i++)
    cout << painting[i] << endl;
}

int main() {
  int N, M;
  cin >> N >> M;
  char **painting = new char *[N];
  char **paintingcopy = new char *[N];
  for(int i=0; i < N; i++)
    {
      painting[i] = new char[M + 1];
      paintingcopy[i] = new char[M + 1];
      cin >> painting[i];
      painting[i][M] = '\0';
      strcpy(paintingcopy[i], painting[i]);
    }

  vector <command*> commands;
  vector <line*> lines;


  add_lines_to_vector(lines, paintingcopy, N, M);

  
  for(int i=0; i < N - 2; i++)
    {
      for(int j=0; j < M - 2; j++)
	{
	  int S = 1;
	  cell C(i, j);
	  if(painting[i][j] == '#' &&check_squares(C, S, painting, lines, N, M))
	    {
	      square *temp = new square(i, j, S, painting, lines);
	      remove_square_from_painting(temp, painting);
	      print_painting(painting, N);
	      commands.push_back(temp);
	      add_lines_to_painting(temp->lines_in_square, paintingcopy);
	      remove_lines_from_vector(temp->lines_in_square, lines);
	      remove_square_from_painting(temp, paintingcopy);
	      add_lines_to_vector(lines, paintingcopy, N, M);
	    }
	}
    }


  for(unsigned int i=0; i < lines.size(); i++)
    {
      commands.push_back(lines[i]);
    }
  int noOfCommands = 0;
  for(unsigned int i=0; i < commands.size(); i++)
    {
      noOfCommands += commands[i]->Ncommands;
    }
  cout << noOfCommands << '\n';
  for(unsigned int i=0; i < commands.size(); i++)
    {
      commands[i]->print_command();
      delete commands[i];
    }
  return 0;
}
