#pragma once

#include <vector>
#include <stdexcept>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferAttribute {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int getSizeOfType(unsigned int type) noexcept {
		switch (type) {
		case GL_FLOAT:			return 4;	
		case GL_UNSIGNED_INT :	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		default:
			ASSERT(false);
		}
		return 0; 
	}
};

class VertexBufferLayout {
private:
	std::vector<VertexBufferAttribute> mElements;
	unsigned int mStride;
public:
	VertexBufferLayout() noexcept : mStride(0) {};

	template<typename T>
	void push(unsigned int count) {
		if constexpr (std::is_same_v<T, float>) {
			mElements.emplace_back(GL_FLOAT, count, GL_FALSE);
			mStride += count * VertexBufferAttribute::getSizeOfType(GL_FLOAT);
		}
		else if constexpr (std::is_same_v<T, unsigned int>) {
			mElements.emplace_back(GL_UNSIGNED_INT, count, GL_FALSE);
			mStride += count * VertexBufferAttribute::getSizeOfType(GL_UNSIGNED_INT);
		}
		else if constexpr (std::is_same_v<T, unsigned char>) {
			mElements.emplace_back(GL_UNSIGNED_BYTE, count, GL_TRUE);
			mStride += count * VertexBufferAttribute::getSizeOfType(GL_UNSIGNED_BYTE);
		}
		else {
			throw std::logic_error("Unsupported type parameter");
		}
	}

	inline const std::vector<VertexBufferAttribute>& getElements() const noexcept { return mElements; }
	inline unsigned int getStride() const noexcept { return mStride; }
};