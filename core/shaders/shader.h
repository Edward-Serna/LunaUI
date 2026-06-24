//
// Created by serna on 6/21/2026.
//

#pragma once

#include <string>

namespace sim {
    class Shader {
    public:
        unsigned int ID; // Program ID

        // constructor reads and builds the shader
        Shader       (const char* vertexPath, const char* fragmentPath);
        void use     () const; // use/activate the shader
        void setBool (const std::string &name, bool value) const;
        void setInt  (const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
    private:
        // utility function for checking shader compilation/linking errors.
        static void checkCompileErrors(unsigned int shader, const std::string& type);
    };
}
