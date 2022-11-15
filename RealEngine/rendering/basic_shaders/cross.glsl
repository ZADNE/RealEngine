#ifdef VULKAN
#define VertexIndex (gl_VertexIndex)
#define InstanceIndex (gl_InstanceIndex)
#else
#define VertexIndex (gl_VertexID)
#define InstanceIndex (gl_InstanceID)
#endif
