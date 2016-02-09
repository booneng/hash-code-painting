#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

struct command {
  virtual void print_command() = 0;
};

struct square:command {
  int r, c, S;
  vector <int[2]> blanks;
  square(int r, int c, int S, char **painting) : r(r), c(c), S(S) {
        for(int i=r; i < 2*S + r; i++)
        {
            for(int j=c; j < 2*S + c; j++)
            {
                if(painting[i][j] == '.')
                {
                    int temp[2];
                    temp[0] = i;
                    temp[1] = j;
                    blanks.push_back(temp);
                }
            }
        }
  }
  
  
  
  void print_command() {
    cout << "PAINT_SQUARE " << r << ' ' << c << ' ' << S << '\n';
  for(unsigned int i=0; i < blanks.size(); i++)
    cout << "ERASE_CELL " << blanks[i][0] << ' ' << blanks[i][1] << '\n';
  }
};

struct line:command {
  int start_r, start_c, last_r, last_c, cells;
  line(int sr, int sc, int lr, int lc) : start_r(sr), start_c(sc), last_r(lr), last_c(lc) {
      cells = (sr == last_r) ? (lc - sc):(lr - sr);
  }
  void print_command() {
    cout << "PAINT_LINE " << start_r << ' ' << start_c << ' ' << last_r << ' ' << last_c << '\n';
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


int main() {
  int N, M;
  cin >> N >> M;
  char **painting = new char *[N];
  for(int i=0; i < N; i++)
    painting[i] = new char[M + 1];

  for(int i=0; i < N; i++)
    {
      cin >> painting[i];
      painting[i][M] = '\0';
    }

  vector <command*> commands;
  for(int i=0; i < N; i++)
    {
      for(int j=0; j < M; j++)
  	{
  	  if(painting[i][j] == '#')
  	    {
  	      int end_line[2];
  	      check_line(i, j, painting, end_line, N);
  	      line *temp = new line(i, j, end_line[0], end_line[1]);
  	      commands.push_back(temp);
  	    }
  	}
    }
  cout << commands.size() << '\n';
  for(unsigned int i=0; i < commands.size(); i++)
    {
      commands[i]->print_command();
      delete commands[i];
      commands[i] = NULL;
    }
    
  return 0;
}