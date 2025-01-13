#pragma once
#include <vector>

#include "Object.hpp"
#include "Model.hpp"
#include "LightSource.hpp"


struct ObjectContainer {
	void addModel(const Model& model);
	void addLightSource(LightSource lightSource);

	void removeObject(Object& object);

	const std::vector<std::unique_ptr<Object>>& get() const;
	void clear();

	size_t getModelCount() const;
	size_t getLightCount() const;
private:
	std::vector<std::unique_ptr<Object>> objects;
	size_t lightCount = 0;
	size_t modelCount = 0;
};