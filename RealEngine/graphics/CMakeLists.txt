add_subdirectory(texture)

target_sources(${PROJECT_NAME} PUBLIC
	Buffer.hpp				Buffer.cpp
	CursorDrawer.hpp		CursorDrawer.cpp
	default_shaders.hpp		
	Font.hpp				Font.cpp
	geometric_shapes.hpp	
	GeometryBatch.hpp		GeometryBatch.cpp
	ShaderProgram.hpp		ShaderProgram.cpp
	Sprite.hpp				Sprite.cpp
	SpriteBatch.hpp			SpriteBatch.cpp
	Surface.hpp				Surface.cpp
	UniformBuffer.hpp		UniformBuffer.cpp
	Vertex.hpp				
	VertexArray.hpp			VertexArray.cpp
	View.hpp				View.cpp
	Viewport.hpp			Viewport.cpp
)
