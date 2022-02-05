#pragma once
#include "../global.h"


struct VertexBufferAttribute{
    unsigned int offsetSize;
    unsigned int type;
    unsigned int count;
    unsigned int stride;
    bool normalized;
    void* data;
};

class VertexArray;
class VertexBuffer {
public:
    VertexBuffer(VertexArray& master);
    
    void CreateNew(int numberOfElements);

    template<typename T>
    VertexBuffer& AddAttribute(std::vector<T>& vec,bool normalized=false) {
        static_assert(false);
        return *this;
    };

    template<>
    VertexBuffer& AddAttribute(std::vector<float>& vec,bool normalized){
        unsigned int count = vec.size()/m_AttributeSize;

        VertexBufferAttribute& attrib = m_AttributeTempMemory.emplace_back();
        attrib.count =count;
        attrib.type = GL_FLOAT;
        attrib.offsetSize = vec.size() * sizeof(float);
        attrib.normalized = normalized;
        attrib.data = (void*)vec.data();
        return *this;
    };

    template<>
    VertexBuffer& AddAttribute(std::vector<int>& vec,bool normalized){
        unsigned int count = vec.size()/m_AttributeSize;

        VertexBufferAttribute& attrib = m_AttributeTempMemory.emplace_back();
        attrib.count =count;
        attrib.type = GL_INT;
        attrib.offsetSize = vec.size() * sizeof(int);
        attrib.normalized = normalized;
        attrib.data = (void*)vec.data();
        return *this;
    };
    
    template<>
    VertexBuffer& AddAttribute(std::vector<unsigned int>& vec,bool normalized){
        unsigned int count = vec.size()/m_AttributeSize;

        VertexBufferAttribute& attrib = m_AttributeTempMemory.emplace_back();
        attrib.count =count;
        attrib.type = GL_UNSIGNED_INT;
        attrib.offsetSize = vec.size() * sizeof(int);
        attrib.normalized = normalized;
        attrib.data = (void*)vec.data();
        return *this;
    };
    
    void Generate();

    void Bind();
    void Unbind();

private:


    VertexArray& m_Master;
    int m_AttributeSize=-1;
    std::vector<VertexBufferAttribute> m_AttributeTempMemory;
    shared_ptr<unsigned int> m_ID;

    friend class VertexArray;

};