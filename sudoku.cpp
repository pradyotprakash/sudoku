// Copyright 2017 Pradyot Prakash

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <bitset>
#include <iostream>
#include <vector>
using namespace std;

void printBoard(vector<vector<char> >& vec){
    cout<<"+ ";
    for(int j=0;j<vec[0].size();j++){
       cout<<"- ";
        if(j%3 == 2) cout<<"+ ";
    }
    cout<<endl;
    for(int i=0;i<vec.size();i++){
        cout<<"| ";
        for(int j=0;j<vec[i].size();j++){
            cout<<vec[i][j]<<" ";
            if(j%3 == 2) cout<<"| ";
        }
        cout<<endl;
        if(i%3 == 2){
            cout<<"+ ";
            for(int j=0;j<vec[i].size();j++){
               cout<<"- ";
                if(j%3 == 2) cout<<"+ ";
            }
            cout<<endl;
        }
    }
}

bool updatePossibleOutcomes(vector<vector<int> >& poss, bool fixed[9][9], int i, int j, int index){
    int p = (i/3)*3 + 3;
    int q = (j/3)*3 + 3;

    int a = i, b = j;
    while(true){
        b++;
        if(b == q){
            b = q - 3;
            a++;
            if(a == p) break;
        }
        if(fixed[a][b]) continue;
        poss[a][b] &= ~(1<<index);
        if(!poss[a][b]) return false;
    }

    for(int b=j+1;b<9;b++){
        if(fixed[i][b]) continue;
        poss[i][b] &= ~(1<<index);
        if(!poss[i][b]) return false;
    }

    for(int a=i+1;a<9;a++){
        if(fixed[a][j]) continue;
        poss[a][j] &= ~(1<<index);
        if(!poss[a][j]) return false;
    }

    return true;
}

void createACopy(vector<vector<int> >& poss, int i, int j, vector<vector<int> > &copy){
    int p = (i/3)*3;
    int q = (j/3)*3;

    for(int a=p;a<p+3;a++){
        for(int b=q;b<q+3;b++){
            copy[0].push_back(poss[a][b]);
        }
    }

    for(int b=j+1;b<9;b++){
        copy[1].push_back(poss[i][b]);
    }

    for(int a=i+1;a<9;a++){
        copy[2].push_back(poss[a][j]);
    }
}

void undoPoss(vector<vector<int> >& poss, int i, int j, vector<vector<int> > &copy){
    int p = (i/3)*3;
    int q = (j/3)*3;
    int count = 0;

    for(int a=p;a<p+3;a++)
        for(int b=q;b<q+3;b++)
            poss[a][b] = copy[0][count++];

    count = 0;
    for(int b=j+1;b<9;b++){
        poss[i][b] = copy[1][count++];
    }

    count = 0;
    for(int a=i+1;a<9;a++)
        poss[a][j] = copy[2][count++];
}

bool dfs(vector<vector<char> >& board, vector<vector<int> >& poss, bool fixed[9][9], int i, int j){
    if(i > 8) return true;

    int newi = i, newj = j + 1;
    if(newj == 9){
        newi = i + 1; newj = 0;
    }

    if(fixed[i][j]){
        return dfs(board, poss, fixed, newi, newj);
    }
    else{
        // create a copy of poss
        vector<vector<int> > possCopy(3, vector<int>());
        createACopy(poss, i, j, possCopy);

        for(int t=0;t<9;t++){
            if((poss[i][j] & (1<<t)) != 0){

                // update others possibles
                if(!updatePossibleOutcomes(poss, fixed, i, j, t)){
                    undoPoss(poss, i, j, possCopy);
                    continue;
                }

                board[i][j] = t + 1 + 48;
                bool flag = dfs(board, poss, fixed, newi, newj);
                if(flag) return true;

                board[i][j] = '.';
                undoPoss(poss, i, j, possCopy);
            }
        }

        return false;
    }
}

bool solveSudoku(vector<vector<char> >& board){
    bool fixed[9][9];
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(board[i][j] == '.')
                fixed[i][j] = false;
            else fixed[i][j] = true;
        }
    }

    vector<vector<int> > poss(9, vector<int>(9, 511));
 
    // set initial possibilites
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(fixed[i][j]){
                poss[i][j] = 0;
                continue;
            }

            for(int a=0;a<9;a++){
                if(board[a][j] != '.'){
                    int shift = board[a][j] - '1';
                    poss[i][j] &= ~(1<<shift);
                }
            }

            for(int b=0;b<9;b++){
                if(board[i][b] != '.'){
                    int shift = board[i][b] - '1';
                    poss[i][j] &= ~(1<<shift);
                }
            }

            int p = (i/3)*3;
            int q = (j/3)*3;

            for(int a=p;a<p+3;a++){
                for(int b=q;b<q+3;b++){
                    if(board[a][b] != '.'){
                        int shift = board[a][b] - '1';
                        poss[i][j] &= ~(1<<shift);
                    }
                }
            }
        }
    }

    return dfs(board, poss, fixed, 0, 0);
}

bool verify(vector<vector<char> >& board){
    // row
    for(int i=0;i<9;i++){
        bool row[9] = {};
        for(int j=0;j<9;j++){
            if(row[board[i][j] - '1']) return false;
            row[board[i][j] - '1'] = true;
        }
    }

    // column
    for(int j=0;j<9;j++){
        bool col[9] = {};
        for(int i=0;i<9;i++){
            if(col[board[i][j] - '1']) return false;
            col[board[i][j] - '1'] = true;
        }
    }

    //boxes
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            bool box[9] = {};
            for(int p=3*i;p<3*i+3;++p){
                for(int q=3*j;q<3*j+3;q++){
                    if(box[board[p][q] - '1']) return false;
                    box[board[p][q] - '1'] = true;
                }
            }
        }
    }
    return true;
}

int main(){
    ifstream ifile("sudoku.txt");
    char c;

    int t = 145;
    while(t--){
        vector<vector<char> > board(9, vector<char>(9));

        // skip the initial Grid ...
        while(1){
            ifile.get(c);
            if(c == '\n') break;
        }

        for(int i=0;i<9;i++){
            for(int j=0;j<9;j++){
                ifile.get(c);
                if(c == '\n') ifile.get(c);
                board[i][j] = c;
            }
        }
        ifile.get(c); // read the trailing newline

        // printBoard(board);
        cout<<"Puzzle number: "<<t<<endl;
        cout<<"Solution found: "<<solveSudoku(board)<<endl;
        // printBoard(board);
        cout<<"Valid solution: "<<verify(board)<<endl;
    }
    ifile.close();
}
