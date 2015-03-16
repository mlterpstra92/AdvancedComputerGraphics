#ifndef _H_NEIGHBORSEARCH_H_
#define _H_NEIGHBORSEARCH_H_

#include <unordered_map>
#include <list>
#include <vector>
#include <functional>
#include "Vector.h"
#include "Particle.h"

typedef std::list<Particle*> NeighborList;

class Cell
{
public:
	int i, j, k;

	Cell(int i, int j, int k);

	bool operator==(Cell const& c) const
	{
		return i == c.i && j == c.j && c.k == k;
	};
};

namespace std
{
	template <>
	struct hash<Cell>
	  : public unary_function<Cell, size_t>
	{
		size_t operator()(Cell c) const
		{
			const long magicX = 73856093;
			const long magicY = 19349663;
			const long magicZ = 83492791;

			return c.i * magicX ^ c.j * magicY ^ c.k * magicZ;
		}
	};
}

class NeighborSearch
{
	double radius;
	double radiusSq;
	std::unordered_map<Cell, NeighborList> neighbors;

public:
	void initialise(double radius);

	void clear();
	void update(std::vector<Particle>& particles);
	void insert(Particle& p);

	NeighborList query(Particle& p);
	NeighborList query(Vector3d position, Particle* self = NULL);

protected:
	Cell cell(Vector3d const& position);
};

#endif
