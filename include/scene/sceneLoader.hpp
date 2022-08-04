#pragma once
#include <vector>

class window;
class scene;

class sceneLoader
{
	private:
		struct sceneNode
		{
			scene *data;
			std::vector<sceneNode> dependencies;

		};
		
		struct windowNode
		{
			window *data;
			std::vector<sceneNode> dependencies;
		};

		
		std::vector<windowNode> windowNodes;
		
	public:
		sceneLoader();
		sceneLoader(const sceneLoader&) = default;
		sceneLoader(sceneLoader&&) = default;
		
		sceneLoader& operator=(const sceneLoader&);
		sceneLoader& operator=(sceneLoader&&);

		~sceneLoader();
};
