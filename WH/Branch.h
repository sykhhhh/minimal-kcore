#pragma once
#ifndef BRANCH_H
#define BRANCH_H

#include "Graph.h"
class Core
{
public:
	Graph *g;
	vector<int> S;
	vector<int> vis,visN;
	vector<int> cnt_neighbor;
	int k;
	int sz;
	Core(Graph &_g, vector<int> &_S, vector<int> &C,
		 int paramK) : sz(0), g(&_g), k(paramK)
	{
		int n = g->n;
		S=C;
		vis.resize(n);
		visN.resize(n);
		cnt_neighbor.resize(n);
		sz=_S.size()+C.size();
		queue<int>Q;
		for (int i = 0; i < C.size(); i++)
		{
			int v = C[i];
			assert(v < n);
			vis[v]=1;
			cnt_neighbor[v]=g->vertices[v].neighbors.size();
			if(cnt_neighbor[v]<k)
			{
				Q.push(v);
				vis[v]=0;
				sz--;
			}
		}
		while(!Q.empty())
		{
			int u=Q.front();
			Q.pop();
			for(int w: g->vertices[u].neighbors)
			{
				if(!vis[w]) continue;
				cnt_neighbor[w]--;
				if(cnt_neighbor[w]<k)
				{
					vis[w]=0;
					sz--;
					Q.push(w);
				}
			}
		}
		int size_C = 0;
		for (int i = 0; i < C.size(); i++)
		{
			int w = C[i];
			if (!vis[w])
				continue;
			C[size_C++] = w;
		}
		C.resize(size_C);
	}
	~Core() {}
	Core &operator=(const Core &other)
	{
		g = other.g;
		S = other.S;
		vis=other.vis;
		visN=other.visN;
		cnt_neighbor = other.cnt_neighbor;
		k = other.k;
		sz=other.sz;
		return *this;
	}
	inline int find_minimum_degree(vector<int> &C)
	{
		int s=0;
		for(int i=0;i<C.size();i++)
		{
			if(cnt_neighbor[C[i]]<cnt_neighbor[C[s]]) s=i;
		}
		return s;
	}
	inline void resize_C(vector<int> &C)
	{
		int size_C=0;
		for(int i=0;i<C.size();i++)
		{
			int v=C[i];
			if(!vis[v]||visN[v]) continue;
			if(is_core(v)) C[size_C++]=v;
		}
		C.resize(size_C);
	}
	inline void reduce_vertex(int v)
	{
		vis[v]=0;
		sz--;
		for(int w: g->vertices[v].neighbors)
		{
			if(!vis[w]) continue;
			cnt_neighbor[w]--;
		}
	}
	int size()
	{
		return sz;
	}
	bool is_core(int v)
	{
		int cnt=0;
		auto neighbor=cnt_neighbor;
		auto vis1=vis;
		queue<int>Q;
		int size_S=0;
		for(int i=0;i<S.size();i++)
		{
			int u=S[i];
			if(!vis1[u]) continue;
			if(vis1[u]&&neighbor[u]<k)
			{
				Q.push(u);
				vis1[u]=0;
			}
			S[size_S++]=u;
		}
		S.resize(size_S);
		if(vis1[v])
		{
			Q.push(v);
			vis1[v]=0;
		}
		while(!Q.empty())
		{
			int u=Q.front();
			Q.pop();
			cnt++;
			for(int w: g->vertices[u].neighbors)
			{
				if(!vis1[w]) continue;
				neighbor[w]--;
				if(neighbor[w]<k)
				{
					vis1[w]=0;
					Q.push(w);
				}
			}
		}
		if(cnt!=sz) return 1;
		return 0;
	}
	bool is_minimal(vector<int> &N)
	{
		if(sz==0) return 0;
		for(int v:N)
		{
			assert(vis[v]);
			if(is_core(v)) return 0;
		}
		return 1;
	}
	void write()
	{
		for(int v:S)
		{
			cerr<<cnt_neighbor[v]<<endl;
		}
	}
};

/**
 * @brief conduct recursive WH algorithm
 */
class Branch
{
	Graph &g;
	Output &out;
	int k;
	int max_Core_size;

public:
	ll dfs_cnt;
	Branch(Graph &_g, Output &_o, int paramK) : g(_g), out(_o), k(paramK)
	{
		dfs_cnt = 0;
	}
	~Branch() {}


	void run()
	{
		int n = g.n;
		vector<int> C(n);
		for (int i = 0; i < n; i++)
			C[i] = i;
		vector<int> S;
		generate_set_for_BK_invocation(S, C);
	}

	void generate_set_for_BK_invocation(vector<int> &S, vector<int> &C)
	{
		// invoke BK
		// step-1: induce subgraph
		Timer induce_timer;
		vector<int> vis(g.n, -1);
		Core SCC(g, S, C, k);
		Graph subg(S, C, vis, g);
		vector<int> vertex_map(subg.n);
		for (int i = 0; i < subg.n; i++)
			vertex_map[i] = subg.vertices[i].origin_id;
		Core SC(subg, S, C, k);
	
		Timer t;
		vector<int> N;
		SC.resize_C(C);
		bk(SC, C, N);
	}


	void bk(Core &SC, vector<int> &C, vector<int> &N)
	{
		dfs_cnt++;
		if (!C.size())
		{
			if(SC.is_minimal(N))
			{
				out.dump(SC.S, SC.size());
			}
			return;
		}
		while(!C.empty())
		{
			int c=SC.find_minimum_degree(C);
			auto new_SC=SC;
			auto new_C=C;
			auto new_N=N;
			new_SC.reduce_vertex(C[c]);
			new_SC.resize_C(new_C);
			bk(new_SC, new_C, new_N);
			N.push_back(C[c]);
			SC.visN[C[c]]=1;
			swap(C[c],C[C.size()-1]);
			C.resize(C.size()-1);
		}
	}
};

#endif