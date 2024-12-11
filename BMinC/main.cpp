#include "Graph.h"
#include "Branch.h"

string file_path;
string KCore_path = "./KCorees.out";
Graph g;
int paramK;
int paramQ;
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("2 params are required: $./enumdp graph_path k !!! \n");
        exit(1);
    }
    if (argc == 3)
    {
        file_path = string(argv[1]);
        paramK = atoi(argv[2]);
        paramQ = 1e9;
    }
    else
    {
        file_path = string(argv[1]);
        paramK = atoi(argv[2]);
        paramQ = atoi(argv[3]);
    }
    g.readFromFile(file_path);


    Timer tot_time;

    Output out(KCore_path);

    puts("------------{BK}------------");
    Branch bk(g, out, paramK, paramQ);
    bk.run();

    printf("Done: #KCore= %lld use-time= %.4lf s dfs_cnt= %lld \n", out.counter, tot_time.get_time_seconds(), bk.dfs_cnt);

    return 0;
}