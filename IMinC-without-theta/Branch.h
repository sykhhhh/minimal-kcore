#pragma once
#ifndef BRANCH_H
#define BRANCH_H
#include "Core.h"
#include "Graph.h"
class Branch
{
	Graph &g;
	Output &out;
	int k;
	vector<int> int_array_size_N_value_0;
	vector<int> int_array_size_N_value_0_another;
	vector<int> int_array_size_N_value_neg1;

public:
	ll dfs_cnt=0,cnt_small=0,cnt_large=0;
	Average subgraph_size;
	Branch(Graph &_g, Output &_o, int paramK) : g(_g), out(_o), k(paramK),
															subgraph_size(Average("avg-Graph-size"))
	{
		dfs_cnt = 0;
		int_array_size_N_value_0_another.resize(g.n);
		int_array_size_N_value_neg1.resize(g.n, -1);
		int_array_size_N_value_0.resize(g.n, 0);
	}
	~Branch() {}

	void run()
	{
		KCore();
	}
	void KCore()
	{
		g.strong_reduce(k);
		printf("now: n= %u m= %u\n", g.n, g.m);
		{
			vector<int> vertices;
			g.compute_degeneracy_order(vertices);
			g.re_sort(vertices);
		}
		auto &vis = int_array_size_N_value_neg1;
		for (int u = 0; u < g.n; u++)
		{
			double percentage = u * 1.0 / g.n;
			print_progress_bar(percentage);
			vector<int> C;
			queue<int>q;
			q.push(u);
			vis[u]=1;
			while(!q.empty())
			{
				int x=q.front();q.pop();
				for(auto i:g.vertices[x].neighbors)
				{
					if(vis[i]==-1) q.push(i),vis[i]=1;
				}
			}

			for(int i=u+1;i<g.n;i++) if(vis[i]==1&&i!=u) C.push_back(i);
			q.push(u);
			vis[u]=-1;
			while(!q.empty())
			{
				int x=q.front();q.pop();
				for(auto i:g.vertices[x].neighbors)
				{
					if(vis[i]==1) q.push(i),vis[i]=-1;
				}
			}
			vector<int> temp_C = C;
			temp_C.push_back(u);
			unordered_map<int, int> inverse_C;
			for (int v : C)
				inverse_C[g.vertices[v].origin_id] = v;
			Graph subg(temp_C, vis, g);
			int prev_n = subg.n;
			do // reduce C
			{
				prev_n = subg.n;
				int origin_id = g.vertices[u].origin_id;
				int idx = -1;
				for (auto &vertex : subg.vertices)
					if (vertex.origin_id == origin_id)
					{
						idx = vertex.id;
						break;
					}
				if (idx == -1)
				{
					subg.n = 0;
					break;
				}
				subg.strong_reduce(k);
			} while (prev_n > subg.n);
			{
				C.clear();
				int u_origin_id = g.vertices[u].origin_id;
				for (auto &vertex : subg.vertices)
				{
					if (vertex.origin_id == u_origin_id)
						continue;
					assert(inverse_C.count(vertex.origin_id));
					int v = inverse_C[vertex.origin_id];
					C.push_back(v);
				}
				generate_set_for_BK_invocation(u,C);
			}
		}
		print_progress_bar(1.0, true);
	}

	void generate_set_for_BK_invocation(int v, vector<int> &C)
	{
		vector<int> vis(g.n, -1);
		C.push_back(v);
		Graph subg(C, vis, g);
		vector<int> vertex_map(subg.n);
		for (int i = 0; i < subg.n; i++)
			vertex_map[i] = subg.vertices[i].origin_id;
		Core SC(subg, C[C.size()-1], C, k);
		Pivot(SC);
	}
	void Pivot(Core &SC)
	{
		dfs_cnt++;
		SC.resize_C();
		int pivot = SC.select_pivot();
		if (pivot==-1)
		{
			if(SC.is_minimal())
			{
				out.dump(SC.S);
			}
			return;
		}
		{
			auto SC_new = SC;
			if(SC_new.reduce_vertex(pivot))
				Pivot(SC_new);
		}
		if(SC.push_vertex(pivot))
			Pivot(SC);
	}
};

#endif