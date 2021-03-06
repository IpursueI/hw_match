#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <vector>
#include <hash_map>
#include <hash_set>
#include <string>
#include <climits>
#include <cstdlib>
#include <memory.h>
using namespace std;
using namespace __gnu_cxx;


//将需要经过的节点数据，从字符数组读入到set当中
void recordDemand(hash_set<int> &demandPos, char sdemandPos[]){
    string s(sdemandPos);
    s += '|';
    int start = 0;
    for(unsigned int i=0; i<s.size(); ++i){
        if(s[i] == '|'){
            demandPos.insert(atoi(s.substr(start, i-start).c_str()));
            start = i+1;
        }
    }
}

//判断当前访问结束后的路径，是否经过要求的节点
bool judgeContain(hash_set<int> &demandPos, bool visited[]) {
    for(hash_set<int>::iterator i =demandPos.begin(); i != demandPos.end(); ++i){
        if(visited[*i] == false)
            return false;
    }
    return true;
}

//对当前路径和花费进行打印
void printResult(int finalCost, vector<int> &path){
    printf("-------results---------\n");
    printf("total cost: %d\n", finalCost);
    printf("path:");
    for(vector<int>::iterator i = path.begin(); i != path.end(); ++i){
        printf("%d ", *i);
    }
    printf("\n");
    printf("-------results---------\n\n\n");
}

//将路径结果写入到文件
void writeResult(vector<int> &path){
    for(vector<int>::iterator i=path.begin(); i != path.end(); ++i)
        record_result(*i);
}

//判断是否超时
bool timeLimit(double time){

    if((double)clock()/CLOCKS_PER_SEC >= time)
        return true;
    return false;
}

//递归主函数
bool dfs(hash_map<int, hash_map<int, pair<int,int> > > &graph, hash_set<int> & demandPos, vector<int> &path,
            bool visited[], int curPos, const int &endPos, int &curCost, vector<int> &bestPath, int &finalCost, bool &tle){

    if(!tle && bestPath.size() != 0 && timeLimit(9.0)){
        tle = true;
        writeResult(bestPath);
    }

    if(curPos == endPos && judgeContain(demandPos, visited)){
        if(curCost < finalCost){
            finalCost = curCost;
            bestPath.clear();
            bestPath = path;
        }
        return true;

    }else {
        for(hash_map<int, pair<int,int> >::iterator item= graph[curPos].begin(); item != graph[curPos].end(); ++item){
            if(visited[item->first] == false){
                visited[item->first] = true;
                path.push_back(item->second.second);
                curCost += item->second.first;
                if(curCost > finalCost){   //关键
                    path.pop_back();
                    visited[item->first] = false;
                    curCost -= item->second.first;
                    return false;   //剪枝
                }

                if(dfs(graph, demandPos, path, visited, item->first, endPos, curCost, bestPath, finalCost, tle)){
                    //结果打印测试
                    //printResult(finalCost, path);
                }
                path.pop_back();
                visited[item->first] = false;
                curCost -= item->second.first;
            }
        }
    }
    return false;
}

void search_route(char *topo[5000], int edge_num, char *demand)
{

    /*文件数据打印测试
    printf("edge_num:%d\n", edge_num);
    for(int i=0; i<edge_num; ++i)
        printf("%s\n", topo[i]);
    printf("demands:%s\n", demand);
    */

    hash_map<int, hash_map<int, pair<int, int> > > graph;  //邻接矩阵保存图的结构
    int startPos=0,endPos=0;
    char sdemanPos[300];
    hash_set<int> demandPos;


    sscanf(demand,"%d,%d,%s", &startPos, &endPos, sdemanPos);
    //printf("%d %d %s\n", startPos, endPos, sdemanPos);
    recordDemand(demandPos, sdemanPos);


    for(int i=0; i<edge_num; ++i){
        int node[4];
        sscanf(topo[i], "%d,%d,%d,%d", &node[0],&node[1],&node[2],&node[3]);
        //printf("%d %d %d %d\n",node[0],node[1],node[2],node[3]);
        if(graph[node[1]].find(node[2]) == graph[node[1]].end()){    //两个顶点之间可能存在多条边，保存最小值
            graph[node[1]][node[2]] = make_pair(node[3],node[0]);
        }else {
            if(graph[node[1]][node[2]].first > node[3]);{
                graph[node[1]][node[2]] = make_pair(node[3],node[0]);
            }
        }

    }

    //hash_set<int> visited;
    bool visited[601];
    memset(visited, 0, 601*sizeof(bool));
    vector<int> path;
    vector<int> bestPath;
    int curCost = 0;
    int finalCost = INT_MAX;
    bool tle = false;
    visited[startPos] = true;
    dfs(graph, demandPos, path, visited, startPos, endPos, curCost, bestPath, finalCost, tle);

    if(!tle && bestPath.size() != 0){
        writeResult(bestPath);
    }
}
