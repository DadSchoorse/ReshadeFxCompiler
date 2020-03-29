#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>

#include <reshade/effect_parser.hpp>
#include <reshade/effect_codegen.hpp>
#include <reshade/effect_preprocessor.hpp>


auto main(const int argc, const char** argv) -> int {
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2) {
      std::cout << "Usage: " << args.at(0) << " <fxfile> [-I includepath] [-o output.conf]" << std::endl;
      return 1;
  }

  std::string fxFile(args.at(1));
  std::string incPath;
  std::string outFile;
  std::stringstream output;

  // TODO(): check better
  for (size_t i = 2; i < args.size(); i++) {
    auto next = i + 1;
    if (args.at(i) == "-o") {
      if (args.size() != next) { outFile = args.at(next); continue; }
      std::cout << "Error: wrong -o param" << std::endl;
      return 1;
    }
    if (args.at(i) == "-I") {
      if (args.size() != next) { incPath = args.at(next); continue; }
      std::cout << "Error: wrong -I param" << std::endl;
      return 1;
    }
  }

  reshadefx::preprocessor preprocessor;
  reshadefx::parser parser;
  reshadefx::module module;

  preprocessor.add_macro_definition("__RESHADE__", std::to_string(INT_MAX));
  preprocessor.add_macro_definition("BUFFER_WIDTH", "800");
  preprocessor.add_macro_definition("BUFFER_HEIGHT", "600");
  preprocessor.add_macro_definition("BUFFER_RCP_WIDTH", "(1.0 / BUFFER_WIDTH)");
  preprocessor.add_macro_definition("BUFFER_RCP_HEIGHT", "(1.0 / BUFFER_HEIGHT)");

  if (!incPath.empty()) {
    preprocessor.add_include_path(incPath);
  }
  preprocessor.append_file(fxFile);


  if (!preprocessor.errors().empty()) {
    std::cout
      << "Preprocessor errors:" << std::endl
      << preprocessor.errors() << std::endl;
    return 1;
  }

  std::unique_ptr<reshadefx::codegen> codegen(reshadefx::create_codegen_spirv(true, false, true));
  if (!parser.parse(std::move(preprocessor.output()), codegen.get())) {
    std::cout
      << "Parser errors:" << std::endl
      << parser.errors() << std::endl;
    return 1;
  }
  codegen->write_result(module);


  // Print value as string
  auto print_value = [](reshadefx::type& t, reshadefx::constant& c) {
    std::string s;
    switch(t.base) {
      case reshadefx::type::t_bool:
        s = c.as_int[0] ? "true" : "false";
        break;
      case reshadefx::type::t_float:
        s = std::to_string(c.as_float[0]);
        s.erase(s.find_last_not_of('0') + 1);
        if (s.ends_with('.')) {
          s.push_back('0');
        }
        break;
      case reshadefx::type::t_int:
        s = std::to_string(c.as_int[0]);
        break;
      case reshadefx::type::t_uint:
        s = std::to_string(c.as_uint[0]);
        break;
      default:
        s = c.string_data;
        // handle \n
        size_t cur_pos(0);
        while ((cur_pos = s.find('\n', cur_pos)) != std::string::npos) {
          s.replace(cur_pos, 1, "\n# ");
          cur_pos += 3;
        }
    }
    return s;
  };

  // Batch compare annotation names
  auto is_one_of = [](const std::string &a, const std::vector<std::string> &b) {
    for (auto &s : b) {
      if (std::equal(a.begin(), a.end(), s.begin(), s.end(),
                     [](char a, char s) { return tolower(a) == tolower(s); })) {
        return true;
      }
    }
    return false;
  };


  // Create config
  output << "# Spec constants" << std::endl;
  for (auto& spc : module.spec_constants) {
    output << std::endl;
    // Hints
    for (auto& hnt : spc.annotations) {
      output << "# ";
      if (!is_one_of(hnt.name, {"ui_label", "ui_tooltip"})) {
        output << hnt.name.substr(3 /* skip "ui_" */) << "\t= ";
      }
      output << print_value(hnt.type, hnt.value);
      output << std::endl;
    }

    // Default value
    output << spc.name << " = ";
    output << print_value(spc.type, spc.initializer_value);
    output << std::endl;
  }


  // Additional info
  output << std::endl << "# Directives" << std::endl;
  for (auto &ppd : preprocessor.used_macro_definitions()) {
    output << "#" << ppd.first << "=" << ppd.second << std::endl;
  }


  // Write to file or print to stdout
  if (!outFile.empty()) {
    std::ofstream(outFile, std::ios::binary).write(output.str().c_str(), output.str().size());
  } else {
    std::cout << output.str();
  }

  return 0;
}
