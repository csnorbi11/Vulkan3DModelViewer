#include "ObjectContainer.hpp"

void ObjectContainer::addModel(const Model& model)
{
	objects.push_back(std::make_unique<Model>(model));
	modelCount++;
}

void ObjectContainer::addLightSource(LightSource lightSource)
{
	objects.push_back(std::make_unique<LightSource>(lightSource));
	lightCount++;
}


void ObjectContainer::removeObject(Object& object)
{
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it).get() == &object)
		{
			objects.erase(it);
			return;
		}
	}
}


const std::vector<std::unique_ptr<Object>>& ObjectContainer::get() const
{
	return objects;
}

void ObjectContainer::clear()
{
	objects.clear();
}

size_t ObjectContainer::getModelCount() const
{
	return modelCount;
}

size_t ObjectContainer::getLightCount() const
{
	return lightCount;
}
