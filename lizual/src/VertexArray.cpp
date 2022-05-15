#include "VertexArray.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
  GLCall(glGenVertexArrays(1, &mRendererId));
}

VertexArray::~VertexArray()
{
  GLCall(glDeleteVertexArrays(1, &mRendererId));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
  bind();
  vb.bind();
  const auto& elements = layout.getElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto& element = elements[i];
    GLCall(glEnableVertexAttribArray(i));
    GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset));
    offset += element.count * VertexBufferAttribute::getSizeOfType(element.type);
  }
}

void VertexArray::bind() const
{
  GLCall(glBindVertexArray(mRendererId));
}

void VertexArray::unbind() const
{
  GLCall(glBindVertexArray(0));
}
