#ifndef GRAPH_H
#define GRAPH_H

#include "Utility.h"

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
	Graph() : n(0), m(0)
	{
	}
	Graph(vector<int> &S, vector<int> &vis, const Graph &origin_g)
	{
		sort(S.begin(), S.end());
		n = S.size();
		assert(vis.size() >= n);
		// next, vis[u] is the new id of u in subgraph
		for (int i = 0; i < n; i++)
		{
			assert(vis[S[i]] == -1);
			assert(S[i] < vis.size());
			vis[S[i]] = i;
		}
		vertices.resize(n);
		for (int u_origin : S)
		{
			int u = vis[u_origin];
			auto &vertex = vertices[u];
			vertex.id = u;
			auto &origin_vertex = origin_g.vertices[u_origin];
			vertex.origin_id = origin_vertex.origin_id;
			for (int v_origin : origin_vertex.neighbors)
			{
				int v = vis[v_origin];
				if (v == -1)
					continue;
				// assert(v < S.size());
				// assert(u != v);
				vertex.neighbors.push_back(v);
			}
		}
		m = 0;
		for (auto &vertex : vertices)
		{
			vertex.neighbors.shrink_to_fit();
			vertex.d = vertex.neighbors.size();
			m += vertex.d;
			for (int u : vertex.neighbors)
				assert(u != vertex.id);
		}
		m/=2;
		// clear the tool-array
		for (int i = 0; i < n; i++)
		{
			vis[S[i]] = -1;
		}
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
		// re-map vertices to {0,...,n-1}
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
		// re-map and clear the tool-array
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
	/**
	 * @brief flip each edge, i.e., E' = {(u,v) | (v,u) \in E}
	 */
	/**
	 * @return whether (a, b) is in E
	 */
	bool exist_edge(int a, int b)
	{
		return has(vertices[a].neighbors, b);
	}
	/**
	 * @brief read edges from file where the file format can be ".txt" ".bin" ".out"
	 *
	 * ".txt" ".out" are text format
	 * ".bin"  is binary format
	 */
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

	/**
	 * @brief given a permutation of {0,...,n-1}, resort V
	 * @param vertex_id v in new graph corresponds to vertex_id[v] in old graph
	 */
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
};

#endif