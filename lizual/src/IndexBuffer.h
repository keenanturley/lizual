#pragma once

class IndexBuffer {
private:
	unsigned int mRendererId;
	unsigned int mCount;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	IndexBuffer(IndexBuffer& other) = delete;
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return mCount; }
};