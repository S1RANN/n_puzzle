#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <chrono>

// represents generated patterns during searching
class Node
{
private:
	// find location of a value in the chessboard
	std::pair<int, int> findLocation(int n) const
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (chessboard[i][j] == n)
				{
					return std::pair<int, int>(i, j);
				}
			}
		}
		return std::pair<int, int>(0, 0);
	}

public:
	static constexpr int width = 4;
	int chessboard[width][width];
	int g = 0;
	int f = 0;
	const Node *p = nullptr;

	Node() : chessboard() {}

	Node(const int _chessboard[width][width])
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				chessboard[i][j] = _chessboard[i][j];
			}
		}
	}

	Node(const Node &node) : Node(node.chessboard)
	{
		g = node.g;
		f = node.f;
		p = node.p;
	}

	~Node() {}

	// calculate the sum of manhattan distances for each pair between 2 patterns
	int getManhattanDistance(const Node &node) const
	{
		int distance = 0;
		for (int i = 0; i < node.width; i++)
		{
			for (int j = 0; j < node.width; j++)
			{
				if (chessboard[i][j] == 0)
					continue;
				std::pair<int, int> location = node.findLocation(chessboard[i][j]);
				distance += abs(location.first - i) + abs(location.second - j);
			}
		}
		return distance;
	}

	// move a step along the direction
	bool move(std::pair<int, int> direction)
	{
		std::pair<int, int> zero_location = findLocation(0);
		std::pair<int, int> position(zero_location.first + direction.first, zero_location.second + direction.second);
		if (position.first < 0 || position.first >= width || position.second < 0 || position.second >= width)
		{
			return false;
		}
		chessboard[zero_location.first][zero_location.second] = chessboard[position.first][position.second];
		chessboard[position.first][position.second] = 0;
		return true;
	}

	bool operator==(const Node &node) const
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				if (chessboard[i][j] != node.chessboard[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}

	bool operator<(const Node &node) const
	{
		return f < node.f;
	}

	void print() const
	{
		std::cout << "\n";
		for (int i = 0; i < width; ++i)
		{
			std::cout << "	[ ";
			for (int j = 0; j < width; ++j)
			{
				if (chessboard[i][j] > 9)
				{
					std::cout << chessboard[i][j] << " ";
				}
				else
				{
					std::cout << " " << chessboard[i][j] << " ";
				}
			}
			std::cout << "]\n";
		}
		std::cout << "\n";
	}
};

namespace std
{
	template <>
	struct hash<Node>
	{
		int operator()(const Node &node) const
		{
			int hash_value = 0;
			for (int i = 0; i < node.width; ++i)
			{
				for (int j = 0; j < node.width; ++j)
				{
					hash_value = (hash_value + (324723947 + node.chessboard[i][j])) ^ 93485734985;
				}
			}
			return hash_value;
		}
	};
}

// used for comparison in the min heap according to the f value
struct NodeComparator
{
	bool operator()(const Node &a, const Node &b)
	{
		return a.f > b.f;
	}
};

std::vector<Node> aStarSearch(const Node &src, const Node &des)
{
	std::vector<Node> open_list;
	std::unordered_set<Node> closed_list;
	NodeComparator nodeComparator;

	open_list.push_back(src);
	Node picked;

	while (true)
	{
		if (open_list.size() == 0)
		{
			return std::vector<Node>();
		}

		picked = open_list.front();							 // pick the node with the lowest f
		auto actual_node = closed_list.insert(picked).first; // push into closed_list

		std::pop_heap(open_list.begin(), open_list.end(), nodeComparator);
		open_list.pop_back(); // remove it from open_list

		if (picked == des)
		{ // found solution
			break;
		}

		std::pair<int, int> directions[4]{// search along 4 directions
										  {-1, 0},
										  {1, 0},
										  {0, -1},
										  {0, 1}};

		for (auto direction : directions)
		{
			Node new_node(picked.chessboard);
			if (!new_node.move(direction))
			{ // this direction is invalid
				continue;
			}
			if (closed_list.find(new_node) != closed_list.end())
			{ // already visited
				continue;
			}
			auto location = std::find(open_list.begin(), open_list.end(), new_node);
			// compare f if already in open_list
			if (location != open_list.end())
			{
				int tmp_g = picked.g + 1;
				int tmp_f = tmp_g + new_node.getManhattanDistance(des);
				if (tmp_f < location->f)
				{
					// update if f is less
					location->g = tmp_g;
					location->f = tmp_f;
					location->p = &(*actual_node);
				}
				continue;
			}
			// push to the heap
			new_node.g = picked.g + 1;
			new_node.f = new_node.g + new_node.getManhattanDistance(des);
			new_node.p = &(*actual_node);
			open_list.push_back(new_node);
		}
		// update the heap
		std::make_heap(open_list.begin(), open_list.end(), nodeComparator);
	}

	// generate the solution path
	std::vector<Node> path{picked};
	const Node *current = picked.p;
	while (current)
	{
		path.push_back(*current);
		current = current->p;
	}
	std::reverse(path.begin(), path.end());
	return path;
}

int main()
{
	// int srcChessboard[4][4]{ {15,14,8,12},{10,11,9,13},{2,6,5,1},{3,7,4,0} };
	// int srcChessboard[4][4]{ {11,9,4,15},{1,3,0,12},{7,5,8,6},{13,2,10,14} };
	// int srcChessboard[4][4]{{6,5,2,4},{9,3,1,7},{13,12,15,10},{14,11,8,0}};
	int srcChessboard[4][4]{{12, 15, 6, 10}, {4, 9, 5, 8}, {14, 13, 0, 2}, {1, 7, 11, 3}};
	int desChessboard[4][4]{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 0}};

	Node src(srcChessboard);
	Node des(desChessboard);

	auto start = std::chrono::high_resolution_clock::now();

	auto path = aStarSearch(src, des);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	std::cout << "Solution:\n";
	for (int i = 0; i < path.size(); ++i)
	{
		path[i].print();
		if (i != path.size() - 1)
		{
			std::cout << "		|	\n";
			std::cout << "		↓	\n";
		}
	}
	std::cout << "moves: " << path.size() - 1 << "\n";
	std::cout << "execution time: " << float(duration.count()) / 1000 << " s";
}