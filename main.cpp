// #include "./src/vm.h"
// #include "./src/file.h"
// #include "./src/exception.h"
// #include "./src/util/print.hpp"
#include "argparse.hpp"
#include "fmt/core.h"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "fmts.hpp"

#include <iostream>
#include <fstream>

std::vector<cc0::Token> _tokenize(std::istream& input) {
	cc0::Tokenizer tkz(input);
	auto p = tkz.AllTokens();
	if (p.second.has_value()) {
		fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
		exit(2);
	}
	return p.first;
}

void Tokenize(std::istream& input, std::ostream& output) {
	auto v = _tokenize(input);
	for (auto& it : v)
		output << fmt::format("{}\n", it);
	return;
}

void Analyse(std::istream& input, std::ostream& output){
	auto tks = _tokenize(input);
	cc0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	if (p.second.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
		////////////////// exit(2);
	}
	auto v = p.first;
	for (auto& it : v)
		output << fmt::format("{}\n", it);
	return;
}

// void assemble_text(std::ifstream* in, std::ofstream* out ) {
//     try {
//         File f = File::parse_file_text(*in);
//         // f.output_text(std::cout);
//         f.output_binary(*out);
//     }
//     catch (const std::exception& e) {
//         println(std::cerr, e.what());
//     }
// }

int main(int argc, char** argv) {
	argparse::ArgumentParser program("cc0");
	program.add_argument("input")
		.help("speicify the file to be compiled.");
	program.add_argument("-t")
		.default_value(false)
		.implicit_value(true)
		.help("perform tokenization for the input file.");
	program.add_argument("-s")
		.default_value(false)
		.implicit_value(true)
		.help("assemble the text input file into the text file.");
	// program.add_argument("-c")
	// 	.default_value(false)
	// 	.implicit_value(true)
		// .help("assemble the text input file into the binary file.");
	program.add_argument("-o", "--output")
		.required()
		.default_value(std::string("-"))
		.help("specify the output file.");
	
	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		fmt::print(stderr, "{}\n\n", err.what());
		program.print_help();
		exit(2);
	}

	auto input_file = program.get<std::string>("input");
	auto output_file = program.get<std::string>("--output");
	std::ifstream* input;// std::istream* input;
	std::ofstream* output;// std::ostream* output;
	std::ifstream inf;
	std::ofstream outf;
	if (input_file != "-") {
		inf.open(input_file, std::ios::in);
		if (!inf) {
			fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
			exit(2);
		}
		input = &inf;
	}
	// 不允许通过标准输入来传递////////////////
	else{
		fmt::print(stderr, "Please give input file.\n");
		exit(2);
		// input = &std::cin;
	}
	// if (output_file != "-") {
		outf.open(output_file, std::ios::out | std::ios::trunc);
		if (!outf) {
			fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
			exit(2);
		}
		output = &outf;
	// }
	// else
	// 	output = &std::cout;

	// assemble -> binary
	// if (program["-c"]==true){
	// 	if(program["-t"]==true || program["-s"]==true)
	// 		exit(2);
	// 	std::ifstream mid_inf;
	// 	std::ofstream mid_outf;
	// 	std::ostream* mid_output; //记得关闭
	// 	std::ifstream* mid_input; //记得在关闭后打开
	// 	// 打开输出到的中间文件
	// 	std::string mid_file = input_file + ".s0";
	// 	if (mid_file == output_file)
	// 		mid_file += ".mid";
	// 	mid_outf.open(mid_file, std::ios::out | std::ios::trunc);
	// 	if (!mid_outf) {
	// 		fmt::print(stderr, "Fail to open {} for mid-writing.\n", mid_file);
	// 		exit(2);
	// 	}
	// 	mid_output = &mid_outf;
	// 	Analyse(*input, *mid_output);
	// 	// 关闭中间文件的输出流，打开中间文件的输入流
	// 	mid_outf.close();
	// 	mid_inf.open(mid_file, std::ios::in);
	// 	if (!mid_inf){
	// 		fmt::print(stderr, "Fail to open {} for mid-reading.\n", mid_file);
	// 		exit(2);
	// 	}
	// 	mid_input = &mid_inf;
		
	// 	// 打开输出文件
	// 	if (input_file == output_file)
	// 		output_file += ".o0";
	// 	outf.open(output_file, std::ios::binary | std::ios::out | std::ios::trunc);
	// 	if (!outf){
	// 		inf.close();
	// 		exit(2);
	// 	}
	// 	output = &outf;
	// 	assemble_text(mid_input, output);
	// }

	if (program["-t"] == true && program["-s"] == true) {
		fmt::print(stderr, "You can only perform tokenization or syntactic analysis at one time.");
		exit(2);
	}
	else if (program["-s"] == true) {
		Analyse(*input, *output);
	}
	else if (program["-t"] == true) {
		Tokenize(*input, *output);
	}
	else {
		fmt::print(stderr, "You must choose assemble_binay or assemble_text.");
		exit(2);
	}
	return 0;
}