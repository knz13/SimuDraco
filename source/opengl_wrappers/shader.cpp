#include "shader.h"

Shader::Shader() {
    


}



void Shader::Bind() {
    if(!m_ID){
        return;
    }
    GL_CALL(glUseProgram(*m_ID.get()));
}

void Shader::Unbind() {
    GL_CALL(glUseProgram(0));
}



void Shader::SetShaders(std::unordered_map<ShaderType,std::vector<std::string>> sources) {
    

    for(auto& shader : sources){
        unsigned int shaderID = -1;
        ShaderType type = shader.first;
        auto& sources = shader.second;
        

        switch(type){
        case ShaderType::Vertex:
            GL_CALL(shaderID = glCreateShader(GL_VERTEX_SHADER));
            break;
        case ShaderType::Fragment:
            GL_CALL(shaderID = glCreateShader(GL_FRAGMENT_SHADER)); 
            break;
        }

        std::vector<GLchar const*> files(sources.size());
        std::vector<GLint> lengths(sources.size());

        int index = 0;
        for(auto& source : sources){
            files[index] = source.c_str();
            lengths[index] = source.size();

            index++;
        }

        GL_CALL(glShaderSource(shaderID,index,files.data(),lengths.data()));

        if(!CompileShader(shaderID)){
            return;
        } 
        m_CompiledShadersCache.push_back(shaderID);
    }

    if(!LinkShader()){
        return;
    };


}
bool Shader::CompileShader(unsigned int shaderID) {
    GL_CALL(glCompileShader(shaderID));

    GLint success = 0;
    GL_CALL(glGetShaderiv(shaderID,GL_COMPILE_STATUS,&success));

    if(success == GL_FALSE){
        GLint logsize = 0;
        GL_CALL(glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&logsize));

        std::vector<GLchar> errorLog(logsize);
        GL_CALL(glGetShaderInfoLog(shaderID,logsize,&logsize,&errorLog[0]));

        std::string errorStr;
        for(auto& character : errorLog){
            errorStr += character;
        }

        LOG("Shader compilation error: " << errorStr);

        GL_CALL(glDeleteShader(shaderID));

        return false;

    }

    //TODO: test if changing the m_ID, it calls the destructor
    if(m_ID){
        GL_CALL(glDeleteProgram(*m_ID.get()));
        
    }
    if(!m_ID){
        auto deleter = [](unsigned int* id){
            GL_CALL(glDeleteProgram(*id));
        };

        m_ID = std::unique_ptr<unsigned int,decltype(deleter)>(new unsigned int(0),deleter);
    }

    GL_CALL(*m_ID.get() = glCreateProgram());

    GL_CALL(glAttachShader(*m_ID.get(),shaderID));

    return true;

}

bool Shader::LinkShader() {
    GL_CALL(glLinkProgram(*m_ID.get()));

    int isLinked = 0;
    GL_CALL(glGetProgramiv(*m_ID.get(),GL_LINK_STATUS,&isLinked));

    if(isLinked == GL_FALSE){
        GLint maxLength = 0;
        glGetProgramiv(*m_ID.get(), GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(*m_ID.get(), maxLength, &maxLength, &infoLog[0]);
        
        
        m_ID.reset();
        
        
        for(auto id : m_CompiledShadersCache){
            GL_CALL(glDetachShader(*m_ID.get(),id));
            GL_CALL(glDeleteShader(id));
        }
        
        return false;
        
    }

    return true;

}

ShaderCreationProperties& ShaderCreationProperties::AddShader(ShaderType type, std::string source) {
    
    if(m_Shaders.find(type) == m_Shaders.end()){
        m_Shaders[type] = {source};
        return *this;
    }

    m_Shaders[type].emplace_back(std::move(source));
    return *this;
}

ShaderCreationProperties Shader::CreateNew() {
    return ShaderCreationProperties(*this);
}   

void ShaderCreationProperties::Generate() {
    m_Master->SetShaders(m_Shaders);
}

ShaderCreationProperties::ShaderCreationProperties(Shader& master) {
    m_Master = &master;
}
