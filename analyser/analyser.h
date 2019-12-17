#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"

#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <cstddef> // for std::size_t

namespace majc0 {

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}), _current_pos(0, 0),
			_uninitialized_vars({}), _vars({}), _consts({}), _nextTokenIndex(0) {}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
		std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyse();
	private:
		// 所有的递归子程序

		// <程序> ,c0 use
		std::optional<CompilationError> analyseProgram();
		// <主过程>
		std::optional<CompilationError> analyseMain();
		// <常量声明>
		std::optional<CompilationError> analyseConstantDeclaration();
		
		// // <语句序列>
		// std::optional<CompilationError> analyseStatementSequence();
		// <常表达式>
		// 这里的 out 是常表达式的值
		std::optional<CompilationError> analyseConstantExpression(int32_t& out);
		// <表达式> ,c0 use
		std::optional<CompilationError> analyseExpression();
		// <赋值语句>
		std::optional<CompilationError> analyseAssignmentStatement();
		// <输出语句>
		std::optional<CompilationError> analyseOutputStatement();
		// <项>
		std::optional<CompilationError> analyseItem();
		// <因子>
		std::optional<CompilationError> analyseFactor();

		/// c0 add
		// <多重变量声明>::={<变量声明>}
		std::optional<CompilationError> analyseVariableDeclarationMulti();
		// <变量声明> ,c0 use
		std::optional<CompilationError> analyseVariableDeclaration();
		// <多重函数定义>::={<函数定义>}
		std::optional<CompilationError> analyseFunctionDefinitionMulti();
		// <函数定义>
		std::optional<CompilationError> analyseFunctionDefinition();
		// <类型修饰符>
		std::optional<CompilationError> analyseTypeSpecifier();
		// <初始化声明符表><init-declarator-list>
		std::optional<CompilationError> analyseInitDeclaratorList(int32_t isConst);
		// <初始化声明符><init-declarator>
		std::optional<CompilationError> analyseInitDeclarator(int32_t isConst);
		// <标识符>
		std::optional<CompilationError> analyseIdentifier();
		// // <初始器><initializer>
		// std::optional<CompilationError> analyseInitializer();
		// <加法类表达式><additive-expression>
		std::optional<CompilationError> analyseAdditiveExpression();
		// <乘法类表达式><multiplicative-expression>
		std::optional<CompilationError> analyseMultiplicativeExpression();
		// <强制类型转换表达式><cast-expression>
		std::optional<CompilationError> analyseCastExpression();
		// <一元表达式><unary-expression>
		std::optional<CompilationError> analyseUnaryExpression();
		// <基本表达式><primary-expression>
		std::optional<CompilationError> analysePrimaryExpression();
		// <函数调用><function-call>
		std::optional<CompilationError> analyseFunctionCall();
		// <表达式列表><expression-list>
		std::optional<CompilationError> analyseExpressionList();
		// <参数从句><parameter-clause>
		std::optional<CompilationError> analyseParameterClause();
		// <参数声明表><parameter-declaration-list>
		std::optional<CompilationError> analyseParameterDeclarationList();
		// <参数声明><parameter-declaration>
		std::optional<CompilationError> analyseParameterDeclaration();
		// <函数体><compound-statement>
		std::optional<CompilationError> analyseCompoundStatement();
		// <句子序列><statement-seq>
		std::optional<CompilationError> analyseStatementSeq();
		// <print-statement>
		std::optional<CompilationError> analysePrintStatement();
		// <scan-statement>
		std::optional<CompilationError> analyseScanStatement();

		// Token 缓冲区相关操作

		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();

		// 下面是符号表相关操作

		// helper function
		void _add(const Token&, std::map<std::string, int32_t>&);
		// 添加变量、常量、未初始化的变量
		void addVariable(const Token&);
		void addConstant(const Token&);
		void addUninitializedVariable(const Token&);
		// 是否被声明过
		bool isDeclared(const std::string&);
		// 是否是未初始化的变量
		bool isUninitializedVariable(const std::string&);
		// 是否是已初始化的变量
		bool isInitializedVariable(const std::string&);
		// 是否是常量
		bool isConstant(const std::string&);
		// 获得 {变量，常量} 在栈上的偏移
		int32_t getIndex(const std::string&);
	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _instructions;
		std::pair<uint64_t, uint64_t> _current_pos;

		// 为了简单处理，我们直接把符号表耦合在语法分析里
		// 变量                   示例
		// _uninitialized_vars    var a;
		// _vars                  var a=1;
		// _consts                const a=1;
		std::map<std::string, int32_t> _uninitialized_vars;
		std::map<std::string, int32_t> _vars;
		std::map<std::string, int32_t> _consts;
		// 下一个 token 在栈的偏移
		int32_t _nextTokenIndex;
	};
}
