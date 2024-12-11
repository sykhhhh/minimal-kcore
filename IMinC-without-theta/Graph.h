#ifndef GRAPH_H
#define GRAPH_H

#include "LinearHeap.h"

class Vertex
{
public:
	int id;
	int origin_id;
	int d;
	vector<int> neighbors;
	Vertex() : d(0)
	{
	}
	~Vertex()
	{
	}
	Vertex &operator=(const Vertex &other)
	{
		id = other.id;
		origin_id = other.origin_id;
		d = other.d;
		neighbors = other.neighbors;
		return *this;
	}
	bool operator==(const Vertex &other) const
	{
		return id == other.id;
	}
	bool operator<(const Vertex &other) const
	{
		return id < other.id;
	}
};

class Graph
{
public:
	int n, m;
	vector<Vertex> vertices;
	double T;
	Graph() : n(0), m(0)
	{
	}
	Graph(vector<int> &S, vector<int> &C, vector<int> &vis, const Graph &origin_g)
	{
		vector<int> V = C;
		V.insert(V.end(), S.begin(), S.end());
		n = V.size();
		unique_vector(V);
		assert( n== V.size());
		vertices.resize(n);
		int idx = 0;
		for (int v : V)
		{
			assert(vis[v] == -1);
			vis[v] = idx++;
		}
		for (int v_origin : V)
		{
			int v = vis[v_origin];
			auto &vertex = vertices[v];
			auto &origin_vertex = origin_g.vertices[v_origin];
			vertex.id = v;
			vertex.origin_id = origin_vertex.origin_id;
			for (int u_origin : origin_vertex.neighbors)
			{
				int u = vis[u_origin];
				if (u == -1)
					continue;
				assert(u < idx);
				vertex.neighbors.push_back(u);
			}
		}
		m = 0;
		for (auto &vertex : vertices)
		{
			sort(vertex.neighbors.begin(), vertex.neighbors.end());
			vertex.d = vertex.neighbors.size();
			m += vertex.d;
		}
		m/=2;
		for (int &v : S)
		{
			int new_id = vis[v];
			vis[v] = -1;
			v = new_id;
		}
		for (int &v : C)
		{
			int new_id = vis[v];
			vis[v] = -1;
			v = new_id;
		}
	}
	Graph(vector<int> &C, vector<int> &vis, const Graph &origin_g)
	{
		vector<int> V = C;
		n = V.size();
		unique_vector(V);
		assert( n== V.size());
		vertices.resize(n);
		int idx = 0;
		for (int v : V)
		{
			assert(vis[v] == -1);
			vis[v] = idx++;
		}
		for (int v_origin : V)
		{
			int v = vis[v_origin];
			auto &vertex = vertices[v];
			auto &origin_vertex = origin_g.vertices[v_origin];
			vertex.id = v;
			vertex.origin_id = origin_vertex.origin_id;
			for (int u_origin : origin_vertex.neighbors)
			{
				int u = vis[u_origin];
				if (u == -1)
					continue;
				assert(u < idx);
				vertex.neighbors.push_back(u);
			}
		}
		m = 0;
		for (auto &vertex : vertices)
		{
			sort(vertex.neighbors.begin(), vertex.neighbors.end());
			vertex.d = vertex.neighbors.size();
			m += vertex.d;
		}
		m/=2;
		for (int &v : C)
		{
			int new_id = vis[v];
			vis[v] = -1;
			v = new_id;
		}
	}
	~Graph()
	{
	}
	Graph &operator=(const Graph &other)
	{
		n = other.n;
		m = other.m;
		vertices = other.vertices;
		return *this;
	}
	void readFromFile(string file_path)
	{
		string suffix = get_file_name_suffix(file_path);
		if (suffix == "bin")
		{
			FILE *in = fopen(file_path.c_str(), "rb");
			if (in == nullptr)
			{
				printf("Failed to open %s \n", file_path.c_str());
				exit(1);
			}
			int size_int;
			fread(&size_int, sizeof(int), 1, in);
			if (size_int != sizeof(int))
			{
				printf("sizeof int is different: graph_file(%u), machine(%u)\n", size_int, (int)sizeof(int));
				exit(1);
			}
			fread(&n, sizeof(int), 1, in);
			fread(&m, sizeof(int), 1, in);
			cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << endl;
			int *d = new int[n];
			int *pstart = new int[n + 1];
			int *edge_to = new int[m*2];
			fread(d, sizeof(int), n, in);
			fread(edge_to, sizeof(int), m*2, in);
			pstart[0] = 0;
			for (int i = 1; i <= n; i++)
				pstart[i] = pstart[i - 1] + d[i - 1];
			vertices.resize(n);
			for (int u = 0; u < n; u++)
			{
				auto &vertex = vertices[u];
				vertex.neighbors.resize(d[u]);
				for (int i = pstart[u], p = 0; i < pstart[u + 1]; i++, p++)
				{
					int j = edge_to[i];
					vertex.neighbors[p] = j;
				}
			}
			for (int u = 0; u < n; u++)
			{
				auto &vertex = vertices[u];
				vertex.id = vertex.origin_id = u;
				vertex.d = vertex.neighbors.size();
			}
			delete[] d;
			delete[] pstart;
			delete[] edge_to;
		}
		else // default graph file format: n m \n edges
		{
			ifstream in(file_path);
			if (!in.is_open())
			{
				printf("Failed to open %s \n", file_path.c_str());
				fflush(stdout);
				exit(1);
			}
			in >> n >> m;
			cout << "File: " << get_file_name_without_suffix(file_path) << " n= " << n << " m= " << m << endl;
			vector<pii> edges(m);
			int idx = 0;
			for (int i = 0; i < m; i++)
			{
				int a, b;
				in >> a >> b;
				if (a == b)
					continue;
				assert(a < n && b < n);
				edges[idx++] = {a, b};
			}
			edges.resize(idx);
			unique_pii(edges, n);
			m = edges.size();
			vertices.resize(n);
			for (auto &h : edges)
			{
				int a = h.x, b = h.y;
				vertices[a].neighbors.push_back(b);
				vertices[b].neighbors.push_back(a);
			}
			for (int u = 0; u < n; u++)
			{
				auto &vertex = vertices[u];
				vertex.id = vertex.origin_id = u;
				vertex.d = vertex.neighbors.size();
				vertex.neighbors.shrink_to_fit();
			}
			in.close();
		}
		printf("Graph init ok\n");
		fflush(stdout);
	}
	int compute_degeneracy_order(vector<int> &vertex_id)
	{
		vector<int> d(n);
		for (int i = 0; i < n; i++)
			d[i] = vertices[i].d;
		LinearHeap heap(n, n, d);
		vertex_id.clear();
		vector<bool> popped(n);
		int ret = 0;
		while (heap.sz)
		{
			int u = heap.get_min_node();
			heap.delete_node(u);
			vertex_id.push_back(u);
			popped[u] = 1;
			ret = max(ret, d[u]);
			for (int v : vertices[u].neighbors)
			{
				if (popped[v])
					continue;
				d[v]--;
				heap.decrease(d[v], v);
			}
		}
		return ret;
	}
	void re_sort(vector<int> &vertex_id)
	{
		assert(vertex_id.size() == n);
		vector<int> id_in_new(n);
		for (int i = 0; i < n; i++)
		{
			id_in_new[vertex_id[i]] = i;
		}
		for (auto &s : vertices)
		{
			s.id = id_in_new[s.id];
			for (auto &v : s.neighbors)
			{
				v = id_in_new[v];
			}
			sort(s.neighbors.begin(), s.neighbors.end());
		}
		sort(vertices.begin(), vertices.end());
	}
	void strong_reduce(int k)
	{
		int m_before = m;
		do
		{
			vertex_reduction(k);
			m_before = m;
		} while (m_before > m);
	}
	void vertex_reduction(int k)
	{
		vector<int> in_queue(n);
		queue<int>Q;
		for (auto &vertex : vertices)
		{
			if(vertex.d<k)
			{
				Q.push(vertex.id);
				in_queue[vertex.id]=1;
			}
		}
		while(!Q.empty())
		{
			int u=Q.front();
			Q.pop();
			for(int w: vertices[u].neighbors)
			{
				if(in_queue[w]) continue;
				vertices[w].d--;
				if(vertices[w].d<k)
				{
					Q.push(w);
					in_queue[w]=1;
				}
			}
		}
		remove_given_vertices(in_queue);
	}
	void two_hop_reduction(int k, int q, int u)
	{
		vector<bool> vis(n,0);
		vector<int> vertex_removed(n, 0);
		auto &u_vertex=vertices[u];
		for (ui v : u_vertex.neighbors)
			vis[v] = 1;
		ui new_size = 0;
		for (auto &vertex : vertices)
		{
			ui v = vertex.id;
			if (vertex.id == u)
				continue;
			int lb=vis[v]?2*k-q:2*k-q+2;
			int same_neighbor=0;
			for (int w : vertices[v].neighbors)
			{
				if (vis[w]) same_neighbor++;
			}
			if (same_neighbor<lb)
				vertex_removed[v] = 1;
		}
		for (ui v : u_vertex.neighbors)
			vis[v] = 0;
		remove_given_vertices(vertex_removed);
	}
	void remove_given_vertices(vector<int> &vertex_removed)
	{
		int idx = 0;
		auto &vertex_map = vertex_removed;
		for (int i = 0; i < n; i++)
		{
			if (vertex_removed[i])
				vertex_map[i] = -1;
			else
				vertex_map[i] = idx++;
		}
		vector<Vertex> new_vertices(idx);
		for (int u_origin = 0; u_origin < n; u_origin++)
		{
			if (vertex_map[u_origin] == -1)
				continue;
			int u = vertex_map[u_origin];
			auto &vertex = new_vertices[u];
			vertex.id = u;
			auto &origin_vertex = vertices[u_origin];
			vertex.origin_id = origin_vertex.origin_id;
			for (int v_origin : origin_vertex.neighbors)
			{
				int v = vertex_map[v_origin];
				if (v == -1)
					continue;
				vertex.neighbors.push_back(v);
			}
		}
		swap(new_vertices, vertices);
		n = idx;
		m = 0;
		for (auto &vertex : vertices)
		{
			vertex.neighbors.shrink_to_fit();
			vertex.d = vertex.neighbors.size();
			m += vertex.d;
		}
		m/=2;
	}

};

#endif