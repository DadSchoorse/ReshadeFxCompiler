#include "../reshade/source/effect_parser.hpp"
#include "../reshade/source/effect_codegen.hpp"
#include "../reshade/source/effect_preprocessor.hpp"
#include <climits>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
/*
    Usage:
    ReshadeFxCompiler <shader> -o <outputfile> -I <includepath> -w <widht> -h <height>
*/
int main(int argc, char *argv[])
{
    reshadefx::preprocessor preprocessor;
    preprocessor.add_macro_definition("__RESHADE__", std::to_string(INT_MAX));
    preprocessor.add_macro_definition("__RESHADE_PERFORMANCE_MODE__", "1");
    
    int width = 1920;
    int height = 1080;
    std::string shaderFile = std::string(argv[1]);
    std::string outFile;
    std::string includePath;
    
    for(int i = 2; i < argc; i++)
    {
       if(std::string(argv[i]) == "-o")
       {
            outFile = std::string(argv[++i]);
       }
       if(std::string(argv[i]) == "-I")
       {
            includePath = std::string(argv[++i]);
       }
       if(std::string(argv[i]) == "-w")
       {
            width = std::stoi(std::string(argv[++i]));
       }
       if(std::string(argv[i]) == "-h")
       {
            height = std::stoi(std::string(argv[++i]));
       }
    }
    preprocessor.add_macro_definition("BUFFER_WIDTH", std::to_string(width));
    preprocessor.add_macro_definition("BUFFER_HEIGHT", std::to_string(height));
    preprocessor.add_macro_definition("BUFFER_RCP_WIDTH", "(1.0 / BUFFER_WIDTH)");
    preprocessor.add_macro_definition("BUFFER_RCP_HEIGHT", "(1.0 / BUFFER_HEIGHT)");
    preprocessor.add_include_path(includePath);
    preprocessor.append_file(shaderFile);

    reshadefx::parser parser;

    std::string errors = preprocessor.errors();
    if(errors != "")
    {
        std::cout << errors << std::endl;
    }

    std::unique_ptr<reshadefx::codegen> codegen(
        reshadefx::create_codegen_spirv(true /* vulkan semantics */, false /* debug info */, true /* uniforms to spec constants */));
    if (!parser.parse(std::move(preprocessor.output()), codegen.get()))
    {
        return 1;
    }
    
    errors = parser.errors();
    if(errors != "")
    {
        std::cout << errors << std::endl;
    }
    
    reshadefx::module module;
    
    codegen->write_result(module);
    
    for(size_t i = 0; i < module.entry_points.size(); i++)
    {
        std::ofstream(outFile + std::to_string(i), std::ios::binary).write(reinterpret_cast<const char *>(module.spirv_modules[module.entry_points[i].name].data()), module.spirv_modules[module.entry_points[i].name].size() * sizeof(uint32_t));
    }
	
    return 0;
}
