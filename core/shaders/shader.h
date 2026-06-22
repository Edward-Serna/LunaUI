//
// Created by serna on 6/21/2026.
//
#pragma once

#include <string>

namespace sim {
    class shader {
    public:
        // the program ID
        unsigned int ID;
        // constructor reads and builds the shader
        shader(const char* vertexPath, const char* fragmentPath);
        // use/activate the shader
        void use();
        // utility uniform functions
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
    private:
        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(unsigned int shader, std::string type) const;

    };
}
