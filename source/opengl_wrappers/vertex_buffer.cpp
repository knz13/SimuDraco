#include "vertex_buffer.h"
#include "../general/registry.h"
#include "vertex_array.h"


void VertexBuffer::CreateNew(int numberOfElements) {
    m_AttributeSize = numberOfElements;
    m_AttributeTempMemory.clear();
}

void VertexBuffer::Bind() {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER,*m_ID.get()));
}

void VertexBuffer::Unbind() {
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER,0));
}

VertexBuffer::VertexBuffer(VertexArray& master) : m_Master(master) {
    auto destructor = [](unsigned int* id){
        GL_CALL(glDeleteBuffers(1,id));
        delete id;
    };
    m_ID = std::shared_ptr<unsigned int>(new unsigned int(0),destructor);
    GL_CALL(glGenBuffers(1,m_ID.get()));
}





void VertexBuffer::Generate() {
    if(m_AttributeSize == -1){
        DEBUG_LOG("Calling 'VertexBuffer::Generate' without the proper creation order, check if calling 'VertexArray::SetVertexBuffer' or 'VertexBuffer::CreateNew'...");
        return;
    }
    float size;
    for(auto& attribute : m_AttributeTempMemory){
        size += attribute.offsetSize;
    }
    this->Bind();
    m_Master.Bind();
    GL_CALL(glBufferData(GL_ARRAY_BUFFER,size,NULL,GL_STATIC_DRAW));

    unsigned int offset = 0;
    int index = 0;



    for(auto& attribute : m_AttributeTempMemory){
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER,offset,attribute.offsetSize,attribute.data));
        GL_CALL(glVertexAttribPointer(index,attribute.count,attribute.type,attribute.normalized? GL_TRUE:GL_FALSE,attribute.count*GL_SIZEOF(attribute.type),(const void*)offset));
        GL_CALL(glEnableVertexAttribArray(index));
        offset += attribute.offsetSize;
    }

    m_AttributeTempMemory.clear();
    m_AttributeSize = -1;
    this->Unbind();
}
