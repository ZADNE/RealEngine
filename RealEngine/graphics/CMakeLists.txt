add_subdirectory(buffers)
add_subdirectory(cameras)
add_subdirectory(textures)

target_sources(${PROJECT_NAME} PUBLIC
	default_shaders.hpp		
	geometric_shapes.hpp	
	GeometryBatch.hpp		GeometryBatch.cpp
	ShaderProgram.hpp		ShaderProgram.cpp
	Sprite.hpp				Sprite.cpp
	SpriteBatch.hpp			SpriteBatch.cpp
	Surface.hpp				Surface.cpp
	Vertex.hpp				
	VertexArray.hpp			VertexArray.cpp
	Viewport.hpp			Viewport.cpp
)
