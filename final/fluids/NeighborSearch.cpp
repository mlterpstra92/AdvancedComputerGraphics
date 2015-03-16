#include "NeighborSearch.h"
#include <stdexcept>

Cell::Cell(int i, int j, int k)
  : i(i), j(j), k(k)
{

}

void NeighborSearch::initialise(double radius)
{
	this->radius = radius;
	this->radiusSq = radius * radius;
}


void NeighborSearch::clear()
{
	neighbors.clear();
}

void NeighborSearch::update(std::vector<Particle>& particles)
{
	for (std::vector<Particle>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		insert(*it);
	}
}

void NeighborSearch::insert(Particle& p)
{
	neighbors[cell(p.position)].push_back(&p);
}

NeighborList NeighborSearch::query(Particle& p)
{
	return query(p.position, &p);
}

NeighborList NeighborSearch::query(Vector3d position, Particle* self /* = NULL */)
{
	Cell cp = cell(position);
	NeighborList result;

	for (int i = -1; i <= 1; ++i)
	for (int j = -1; j <= 1; ++j)
	for (int k = -1; k <= 1; ++k)
	{
		Cell c(cp.i + i, cp.j + j, cp.k + k);

		std::unordered_map<Cell, NeighborList>::iterator cell = neighbors.find(c);

		if (cell != neighbors.end())
		{
			NeighborList& list = cell->second;

			for (NeighborList::iterator it = list.begin(); it != list.end(); ++it)
			{
				if (*it != self && ((*it)->position - position).lengthSq() < radiusSq)
				{
					result.push_back(*it);
				}
			}
		}
	}

	return result;
}

Cell NeighborSearch::cell(Vector3d const& position)
{
	return Cell(position.x / radius, position.y / radius, position.z / radius);
}
