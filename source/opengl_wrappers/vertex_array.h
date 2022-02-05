#pragma once
#include "vertex_buffer.h"
#include "index_buffer.h"

class VertexArray {
public:
    VertexArray();


    VertexBuffer& CreateVertexBuffer(unsigned int numberOfVertices);
    VertexBuffer& CreateVertexBuffer();
    IndexBuffer& CreateIndexBuffer();

    bool HasIndexBuffer();

    void Bind();
    void Unbind();


protected:
    

private:
    std::unique_ptr<IndexBuffer> m_IB;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::shared_ptr<unsigned int> m_ID;
;
};