#include "Graph.h"
class Core
{
public:
	Graph *g;
	vector<int> S,C;
	vector<int> vis;
	vector<int> vis1;
	vector<int> d_SC;
	vector<int> d_S;
	int k,q;
	int sz;
	Core(Graph &_g, int v, vector<int> &_C,
		 int paramK,int paramQ) : sz(0), C(_C), g(&_g), k(paramK), q(paramQ)
	{
		int n = g->n;
		vis.resize(n);
		vis1.resize(n);
		d_SC.resize(n);
		d_S.resize(n);
		queue<int>Q;
		for (int i = 0; i < C.size(); i++)
		{
			int v = C[i];
			assert(v < n);
			vis[v]=2;
			d_SC[v]=g->vertices[v].neighbors.size();
			if(d_SC[v]<k)
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
				d_SC[w]--;
				if(d_SC[w]<k)
				{
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
		if(!vis[v])
		{
			sz=0;
			S.clear();
			C.clear();
			return;
		}
		C.pop_back();
		if(!push_vertex(v))
		{
			sz=0;
			S.clear();
			C.clear();
		}
	}
	~Core() {}
	Core &operator=(const Core &other)
	{
		g = other.g;
		S = other.S;
		C = other.C;
		vis=other.vis;
		vis1=other.vis1;
		d_SC = other.d_SC;
		d_S = other.d_S;
		k = other.k;
		q = other.q;
		sz=other.sz;
		return *this;
	}
	inline int select_pivot()
	{
		if(C.empty()) return -1;
		int pd=-1;
		for(int v:S)
		{
			if(d_S[v]>=k) continue;
			if(pd==-1 || d_SC[v]<d_SC[pd])
			{
				pd=v;
			}
		}
		if(pd==-1) return -1;
		for(int v:C)
		{
			if(d_S[v]>=k) continue;
			if(pd==-1 || d_SC[v]<d_SC[pd])
			{
				pd=v;
			}
		}
		if(vis[pd]==2) return pd;
		int pivot=-1,mx=0;
		for(int v:g->vertices[pd].neighbors)
		{
			if(vis[v]==2)
			{
				if(d_SC[v]>mx)
				{
					pivot=v;
					mx=d_SC[v];
				}
			}
		}
		return pivot;

	}
	inline int reduce_vertex(int v)
	{
		if(vis[v]==1) return 0;
		if(vis[v]==0) return 1;
		vis[v]=0;
		for(int w: g->vertices[v].neighbors)
		{
			if(!vis[w]) continue;
			d_SC[w]--;
			if(d_SC[w]<k)
			{
				if(!reduce_vertex(w)) return 0;
			}
			if(d_SC[w]==k&&vis[w]==1)
			{
				for(int u:g->vertices[w].neighbors)
				{
					if(vis[u]==2)
					{
						if(!push_vertex(u)) return 0;
					}
				}
			}
		}
		return 1;
	}
	inline int push_vertex(int v)
	{
		if(vis[v]==1) return 1;
		if(vis[v]==0) return 0;
		S.push_back(v);
		for(int u:g->vertices[v].neighbors)
		{
			d_S[u]++;
		}
		vis[v]=1;
		sz++;
		if(d_SC[v]==k)
		{
			for(int u:g->vertices[v].neighbors)
			{
				if(vis[u]==2)
				{
					if(!push_vertex(u)) return 0;
				}
			}
		}
		auto C1=C;
		for(int u:C1)
		{
			if(vis[u]!=2) continue;
			if(k-d_S[u]+sz>=q)
			{
				if(!reduce_vertex(u)) return 0;
			}
		}
		auto S1=S;
		for(int u:S1)
		{
			if(k-d_S[u]+sz>q) return 0;
			if(k-d_S[u]+sz==q)
			{
				vector<int> vis2(g->n);
				for(int w:g->vertices[u].neighbors)
				{
					vis2[w]=1;
				}
				auto C2=C;
				for(int w:C2)
				{
					if(vis[w]==2&&!vis2[w])
					{
						if(!reduce_vertex(w)) return 0;
					}
				}
			}
		}
		return 1;
	}
	int size()
	{
		return sz;
	}
	bool is_minimal()
	{
		if(!sz) return 0;
		return 1;
		for(int v:S)
		{
			int cnt=0;
			auto neighbor=d_S;
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
	void resize_C()
	{
		int size_C=0;
		for (int i = 0; i < C.size(); i++)
		{
			int w = C[i];
			if (vis[w]!=2)
				continue;
			C[size_C++] = w;
		}
		C.resize(size_C);
	}
};