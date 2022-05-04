#pragma once

class VertexBuffer {
private:
	unsigned int mRendererId;
public:
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(VertexBuffer& other) = delete;
	~VertexBuffer();

	void bind() const;
	void unbind() const;
};