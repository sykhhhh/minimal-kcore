#pragma once

#include "Graph.h"
class Core
{
public:
	Graph *g;
	vector<int> vertices_id;
	vector<int> vis;
	vector<int> cnt_neighbor;
	int k;
	int sz;
	Core(Graph &_g, vector<int> &S, vector<int> &C,
		 int paramK) : sz(0), g(&_g), k(paramK)
	{
		int n = g->n;
		vis.resize(n);
		cnt_neighbor.resize(n);
		vertices_id = S;
		sz=S.size();
		for (int i = 0; i < S.size(); i++)
		{
			int v = S[i];
			assert(v < n);
			vis[v]=1;
			cnt_neighbor[v]=g->vertices[v].neighbors.size();
			if(cnt_neighbor[v]<k)
			{
				vertices_id.clear();
				S.clear();
				C.clear();
				return;
			}
		}
		queue<int>Q;
		for (int i = 0; i < C.size(); i++)
		{
			int v = C[i];
			assert(v < n);
			vis[v]=2;
			cnt_neighbor[v]=g->vertices[v].neighbors.size();
			if(cnt_neighbor[v]<k)
			{
				Q.push(v);
				vis[v]=0;
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
					if(vis[w]==1)
					{
						vertices_id.clear();
						S.clear();
						C.clear();
						return;
					}
					vis[w]=0;
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
		S=vertices_id;
	}
	~Core() {}
	Core &operator=(const Core &other)
	{
		g = other.g;
		vertices_id = other.vertices_id;
		vis=other.vis;
		cnt_neighbor = other.cnt_neighbor;
		k = other.k;
		sz=other.sz;
		return *this;
	}
	inline int reduce_vertex(int v, vector<int> &C)
	{
		queue<int>Q;
		Q.push(v);
		vis[v]=0;
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
					if(vis[w]==1) return 1;
					vis[w]=0;
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
		return 0;
	}
	inline void push_vertex(int v)
	{
		vertices_id.push_back(v);
		vis[v]=1;
		sz++;
	}
	int size()
	{
		return sz;
	}
	bool is_minimal()
	{
		if(sz==0) return 0;
		return 1;
		for(int v:vertices_id)
		{
			int cnt=0;
			auto neighbor=cnt_neighbor;
			auto vis1=vis;
			queue<int>Q;
			Q.push(v);
			vis1[v]=0;
			while(!Q.empty())
			{
				int u=Q.front();
				Q.pop();
				cnt++;
				for(int w: g->vertices[u].neighbors)
				{
					if(vis1[w]!=1) continue;
					neighbor[w]--;
					if(neighbor[w]<k)
					{
						vis1[w]=0;
						Q.push(w);
					}
				}
			}
			if(cnt!=sz) return 0;
		}
		return 1;
	}
};
class Branch
{
	Graph &g;
	Output &out;
	int k,q;
	int max_Core_size;

public:
	ll dfs_cnt;
	Branch(Graph &_g, Output &_o, int paramK, int paramQ) : g(_g), out(_o), k(paramK), q(paramQ)
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
		vector<int> vis(g.n, -1);
		Core SCC(g, S, C, k);
		Graph subg(S, C, vis, g);
		vector<int> vertex_map(subg.n);
		for (int i = 0; i < subg.n; i++)
			vertex_map[i] = subg.vertices[i].origin_id;
		Core SC(subg, S, C, k);
		bk(SC, C);
	}

	void bk(Core &SC, vector<int> &C)
	{
		dfs_cnt++;
		if (SC.size()>q) return;
		if (!C.size())
		{
			if(SC.is_minimal())
			{
				out.dump(SC.vertices_id, SC.size());
			}
			return;
		}
		int u = *C.rbegin();
		C.pop_back();
		{
			auto SC_new = SC;
			auto C_new = C;
			int flag=SC_new.reduce_vertex(u, C_new);
			if(!flag) bk(SC_new, C_new);
		}
		SC.push_vertex(u);
		bk(SC,C);
	}
};