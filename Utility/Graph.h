/* Retrieved from: http://en.literateprograms.org/Dijkstra's_algorithm_(C_Plus_Plus)?oldid=13422 */

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <list>
#include <queue>
#include <set>

#undef min
#undef max
#include <limits>

template <typename VertexType = int, typename WeightType = float>
class Graph
{
private:
	typedef VertexType vertex_t;
	typedef WeightType weight_t;

	struct Edge {
		vertex_t target;
		weight_t weight;
		int index;

		Edge(vertex_t arg_target, weight_t arg_weight, int arg_index = -1)
			: target(arg_target), weight(arg_weight), index(arg_index) { }

		bool operator == (const Edge & rhs) const
		{
			return (target == rhs.target && weight == rhs.weight && index == rhs.index);
		}
	};

	typedef std::map<vertex_t, std::list<Edge> > adjacency_map_t;
	typedef std::set<vertex_t> vertices_set;

	template <typename T1, typename T2>
	struct pair_first_less
	{
		bool operator()(std::pair<T1,T2> p1, std::pair<T1,T2> p2) const {
			if(p1.first == p2.first) {
				return p1.second < p2.second;
			}
			return p1.first < p2.first;
		}
	};

	void DijkstraComputePaths(vertex_t source)
	{
		if(source == lastStart)
			return;
		else
			lastStart = source;

                for (typename adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
			vertex_iter != adjacency_map.end();
			vertex_iter++)
		{
			vertex_t v = vertex_iter->first;
			min_distance[v] = std::numeric_limits< WeightType >::infinity();
		}

		min_distance[source] = 0;
		std::set< std::pair<weight_t, vertex_t>,
			pair_first_less<weight_t, vertex_t> > vertex_queue;
                for (typename adjacency_map_t::iterator vertex_iter = adjacency_map.begin();
			vertex_iter != adjacency_map.end();
			vertex_iter++){
				vertex_t v = vertex_iter->first;
				vertex_queue.insert(std::pair<weight_t, vertex_t>(min_distance[v], v));
		}

		while (!vertex_queue.empty()) {
			vertex_t u = vertex_queue.begin()->second;
			vertex_queue.erase(vertex_queue.begin());

			// Visit each edge exiting u
                        for (typename std::list<Edge>::iterator edge_iter = adjacency_map[u].begin();
				edge_iter != adjacency_map[u].end();
				edge_iter++)
			{
				vertex_t v = edge_iter->target;
				weight_t weight = edge_iter->weight;
				weight_t distance_through_u = min_distance[u] + weight;
				if (distance_through_u < min_distance[v]) {
					vertex_queue.erase(std::pair<weight_t, vertex_t>(min_distance[v], v));

					min_distance[v] = distance_through_u;
					previous[v] = u;
					vertex_queue.insert(std::pair<weight_t, vertex_t>(min_distance[v], v));
				}

			}
		}
	}

	std::list<vertex_t> DijkstraGetShortestPathTo(vertex_t target)
	{
		std::list<vertex_t> path;
                typename std::map<vertex_t, vertex_t>::iterator prev;
		vertex_t vertex = target;

		if(previous.size())
			path.push_front(vertex);

		while((prev = previous.find(vertex)) != previous.end())
		{
			vertex = prev->second;
			path.push_front(vertex);
		}
		return path;
	}

	// Graph Variables:
	vertices_set vertices;
	adjacency_map_t adjacency_map;

	std::map<vertex_t, weight_t> min_distance;
	std::map<vertex_t, vertex_t> previous;

	int lastStart;

public:

	Graph()
	{
		lastStart = std::numeric_limits<int>::min();
	}

	Graph(const Graph& from)
	{
		this->adjacency_map = from.adjacency_map;
		this->vertices = from.vertices;
		this->min_distance = from.min_distance;
		this->previous = from.previous;
		this->lastStart = from.lastStart;
	}

	void AddEdge(vertex_t p1, vertex_t p2, weight_t weight, int index = -1)
	{
		adjacency_map[AddVertex(p1)].push_back(Edge(AddVertex(p2), weight, index));
		adjacency_map[AddVertex(p2)].push_back(Edge(AddVertex(p1), weight, index));
	}

	int AddVertex(vertex_t p)
	{
		vertices.insert(p);

		return p;
	}

	void removeDirectedEdge(vertex_t p1, vertex_t p2)
	{
		std::list<Edge> * adj = &adjacency_map[p1];

                for(typename std::list<Edge>::iterator i = adj->begin(); i != adj->end(); i++)
		{
			Edge * e = &(*i);

			if(e->target == p2)
			{
				adj->remove(*e);
				return;
			}
		}
	}

	void removeEdge(vertex_t p1, vertex_t p2)
	{
		removeDirectedEdge(p1, p2);
		removeDirectedEdge(p2, p1);
	}

	void SetDirectedEdgeWeight(vertex_t p1, vertex_t p2, weight_t newWeight)
	{
		std::list<Edge> * adj = &adjacency_map[p1];

                for(typename std::list<Edge>::iterator i = adj->begin(); i != adj->end(); i++)
		{
			Edge * e = &(*i);

			if(e->target == p2)
			{
				e->weight = newWeight;
				return;
			}
		}
	}

	void SetEdgeWeight(vertex_t p1, vertex_t p2, weight_t newWeight)
	{
		SetDirectedEdgeWeight(p1, p2, newWeight);
		SetDirectedEdgeWeight(p2, p1, newWeight);
	}

	vertex_t GetRandomNeighbour(vertex_t p)
	{
		return adjacency_map[p].front().target;
	}

	std::vector<vertex_t> GetNeighbours(vertex_t p)
	{
		std::vector<vertex_t> neighbours;

		std::list<Edge> * adj = &adjacency_map[p];

                for(typename std::list<Edge>::iterator i = adj->begin(); i != adj->end(); i++)
		{
			Edge * e = &(*i);

			neighbours.push_back(e->target);
		}

		return neighbours;
	}

	vertex_t GetOtherNeighbour(vertex_t p, vertex_t q)
	{
		vertex_t n = p;

		std::list<Edge> * adj = &adjacency_map[p];
                for(typename std::list<Edge>::iterator i = adj->begin(); i != adj->end(); i++)
		{
			Edge * e = &(*i);

			if(e->target != q)
			{
				n = e->target;
				break;
			}
		}

		return n;
	}

	bool isCircular(vertex_t p)
	{
		std::set<vertex_t> visited;

		visited.insert(p);

		bool hasMore = true;

		vertex_t curr = p;
		vertex_t prev = p;

		while(hasMore)
		{
			vertex_t next = GetOtherNeighbour(curr, prev);

			if(next == curr)
				return false;

			if(visited.find(next) != visited.end())
				return true;
			
			prev = curr;
			curr = next;
		}

		return false;
	}

	bool isConnected(vertex_t v1, vertex_t v2)
	{
		this->DijkstraComputePaths(v1);

		if(min_distance[v2] != std::numeric_limits< WeightType >::infinity())
			return true;

		return false;
	}

	std::vector<vertex_t> GetLeaves() const
	{
		std::vector<vertex_t> leaves;

                for(typename adjacency_map_t::const_iterator it = adjacency_map.begin(); it != adjacency_map.end(); it++)
		{
			if(it->second.size() < 2)
			{
				leaves.push_back(it->first);
			}
		}

		return leaves;
	}

	void explore(vertex_t seed, std::set<vertex_t> & explored)
	{
		std::queue<vertex_t> q;

		q.push(seed);
		explored.insert(seed);

		while(!q.empty())
		{
			vertex_t i = q.front();
			q.pop();

			std::list<Edge> * adj = &adjacency_map[i];

                        for(typename std::list<Edge>::const_iterator it = adj->begin(); it != adj->end(); it++)
			{
				int j = it->target;
				
				// Check: not visited
				if(explored.find(j) == explored.end()) 
				{
					explored.insert(j);
					q.push(j);
				}
			}
		}
	}
	
	std::list<vertex_t> DijkstraShortestPath(vertex_t start, vertex_t end)
	{
		this->DijkstraComputePaths(start);
		return this->DijkstraGetShortestPathTo(end);
	}

	vertex_t getNodeLargestConnected()
	{
		std::vector<std::set<vertex_t> > connectedComponents;
		std::set<vertex_t> unvisited;

		if(vertices.size() == 0)
			return -1;

		// fill unvisited set
                for(typename vertices_set::const_iterator it = vertices.begin(); it != vertices.end(); it++)
			unvisited.insert(*it);

		while(unvisited.size() > 1)
		{
			// Take first unvisited node
			vertex_t firstNode = *(unvisited.begin());

			// Explore its tree
			std::set<vertex_t> currVisit;
			currVisit.insert(firstNode);
			explore(firstNode, currVisit);

			// Add as a connected component
			connectedComponents.push_back(currVisit);

			// Remove from unvisited set
                        for(typename std::set<vertex_t>::iterator it = currVisit.begin(); it != currVisit.end(); it++)
				unvisited.erase(*it);
		}

		// Find set with maximum number of nodes
		int maxConnectSize = -1, max_i = 0;
                for(int i = 0; i < (int)connectedComponents.size(); i++)
		{
			int currSize = connectedComponents[i].size();

			if(currSize > maxConnectSize){
				maxConnectSize = currSize;
				max_i = i;
			}
		}

		// Return first node of that maximum set
		return *(connectedComponents[max_i].begin());
	}
};

#endif // GRAPH_H
