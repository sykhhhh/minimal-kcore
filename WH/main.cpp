#include "Graph.h"
#include "Branch.h"

string file_path;
string KCore_path = "./KCorees.out";
Graph g;
int paramK;
// int main()
// {
//     freopen("1.in","w",stdout);
//     g.readFromFile("Wiki-Vote.bin");
//     return 0;
// }
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("2 params are required: $./main graph_path k !!! \n");
        exit(1);
    }
    file_path = string(argv[1]);
    paramK = atoi(argv[2]);
    g.readFromFile(file_path);

    Timer tot_time;

    Output out(KCore_path);

    puts("------------{WH}------------");
    Branch bk(g, out, paramK);
    bk.run();

    printf("Done: #KCore= %lld use-time= %.4lf s dfs_cnt= %lld \n", out.counter, tot_time.get_time_seconds(), bk.dfs_cnt);

    return 0;
}