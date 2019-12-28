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

namespace cc0 {
	// 变量类型,"S","I","D","A","C"
	// for constants
	// enum varType{
	// 	S,
	// 	I,
	// 	C,
	// 	D,
	// 	A
	// };
	// 变量|函数参数
	class C0Var final{
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		friend void swap(C0Var& lhs, C0Var& rhs);

		// bool operator< (const C0Var&other) const{
		// 	return this->getLevel()<other.getLevel() || 
		// 			( this->getLevel()==other.getLevel() && this->getOffset()<other.getOffset())
		// 		;
		// }

		C0Var(const std::string& name, const std::string& type, const int32_t& level, const int32_t& offset)
			: _name(name), _type(type), _level(level), _offset(offset), _isInitialized(0), _isConst(0) {}
		C0Var& operator=(C0Var t) { swap(*this, t); return *this; }

		std::string getName() const { return _name; }
		std::string getType() const { return _type; }
		int32_t getLevel() const { return _level; }
		int32_t getOffset() const { return _offset; }
		int32_t isInitialized() const { return _isInitialized; }
		int32_t isConst() const { return _isConst; }
		// std::string getValueString() const { return _valueString; };
		
		void setName(const std::string& name) {this->_name = name; }
		void setType(const std::string& type) {this->_type = type; }
		void setLevel(int32_t level) {this->_level = level;}
		void setOffset(int32_t offset) {this->_offset = offset;}
		void setInitialized() {this->_isInitialized = 1;}
		void setConst() {this->_isConst = 1;}
		// void setValueString(const std::string& valuestr) {  _valueString = valuestr; }

		
	private:
		std::string _name;
		std::string _type;
		int32_t _level;	//所在层级
		int32_t _offset;	//在对应层级下的偏移
		int32_t _isInitialized = 0;
		int32_t _isConst = 0; // 1 == const
		// std::string _valueString;
	};

	inline void swap(C0Var& lhs, C0Var& rhs) {
		using std::swap;
		swap(lhs._name, rhs._name);
		swap(lhs._type, rhs._type);
		swap(lhs._level, rhs._level);
		swap(lhs._offset, rhs._offset);
		swap(lhs._isInitialized, rhs._isInitialized);
		swap(lhs._isConst, rhs._isConst);
		// swap(lhs._valueString, rhs._valueString);
	}

	// 函数
	class C0Function final{
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		friend void swap(C0Function& lhs, C0Function& rhs);

		C0Function(const std::string& name, const std::string& type, int32_t offset)
			: _funcName(name), _paramsList({}), _varList({}), _retType(type), _offset(offset) {}
		C0Function& operator=(C0Function t) { swap(*this, t); return *this; }

		std::string getFuncName() const { return _funcName; }
		std::string getRetType() const { return _retType; }
		std::int32_t getOffset() const { return _offset; }
		std::int32_t getParamsNum() {return _paramsList.size(); }
		std::vector<C0Var> * getParamsList() {return &_paramsList; }

		void setFuncName(const std::string& funcname) {this->_funcName = funcname; }
		void setRetType(const std::string& rettype) {this->_retType = rettype; }
		void setOffset(int32_t offset) {this->_offset = offset;}
	private:
		std::string _funcName;
		std::vector<C0Var> _paramsList;
		std::vector<C0Var> _varList;
		std::string _retType;
		int32_t _offset;	//from 0 ~ n-1
		//需要入口地址吗
	};

	inline void swap(C0Function& lhs, C0Function& rhs) {
		using std::swap;
		swap(lhs._funcName, rhs._funcName);
		swap(lhs._paramsList, rhs._paramsList);
		swap(lhs._varList, rhs._varList);
		swap(lhs._retType, rhs._retType);
		swap(lhs._offset, rhs._offset);
	}

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}), _current_pos(0, 0), 
			_functionsTable({}), _globalVariablesTable({}), _variablesTable({}), _returnTree(), _preReturnIndex(0){}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
		std::pair<std::vector<Instruction>, std::optional<CompilationError>> Analyse();
	private:
		// 所有的递归子程序
		// <程序>
		std::optional<CompilationError> analyseProgram();
		// <多重变量声明>::={<变量声明>}
		std::optional<CompilationError> analyseVariableDeclarationMulti();
		// <变量声明>
		std::optional<CompilationError> analyseVariableDeclaration();
		// <多重函数定义>::={<函数定义>}
		std::optional<CompilationError> analyseFunctionDefinitionMulti();
		// <函数定义>
		std::optional<CompilationError> analyseFunctionDefinition();
		// <类型修饰符>
		std::optional<CompilationError> analyseTypeSpecifier();
		// <初始化声明符表><init-declarator-list>
		std::optional<CompilationError> analyseInitDeclaratorList(int32_t);
		// <初始化声明符><init-declarator>
		std::optional<CompilationError> analyseInitDeclarator(int32_t);
		// <标识符>
		std::optional<CompilationError> analyseIdentifier();
		// // <初始器><initializer>
		// std::optional<CompilationError> analyseInitializer();
		// <表达式>
		std::optional<CompilationError> analyseExpression();
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
		std::optional<CompilationError> analyseExpressionList(int32_t);
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
		// <statement>
		std::optional<CompilationError> analyseStatement();
		// <condition-statement>
		std::optional<CompilationError> analyseConditionStatement();
		// <condition>
		std::optional<CompilationError> analyseCondition(Operation&);
		// <loop-statement>
		std::optional<CompilationError> analyseLoopStatement();
		// <return-statement>
		std::optional<CompilationError> analyseReturnStatement();
		// <print-statement>
		std::optional<CompilationError> analysePrintStatement();
		// <scan-statement>
		std::optional<CompilationError> analyseScanStatement();
		// <assignment-expression>
		std::optional<CompilationError> analyseAssignmentExpression();
		
		// Token 缓冲区相关操作

		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();

		// 下面是符号表相关操作

		// bool isDeclared(const std::string&,int32_t);
		bool isDeclaredSameLevel(const std::string&, int32_t);
		// bool isGlobalConstant(const std::string&);
		bool isGlobalVariable(const std::string&);
		bool isFunction(const std::string&);
		bool isVariable(const std::string&,int32_t);
		void addGlobalVariable(C0Var*);
		// void addGlobalConstant(C0Var&);
		void addVariable(C0Var*);
		void addFunction(C0Function*);

		// 删除level层级的所有变量
		void crushVar(int32_t);

		// 获取变量/常量
		C0Var* getVar(const std::string&, int32_t);
		C0Var* getGlobalVar(const std::string&);
		// 获取函数
		C0Function* getFunc(const std::string&);
		// 清除return树
		// void crushReturnTree();
		// // 设置returnIndex的节点为true,如果之前有元素未初始化,则初始化为false
		// void addReturnNode();
		// void inReturnLeaf();
		// void outReturnLeaf();
		// void moveReturnLeaf();
		// bool checkReturnTree(int32_t);

	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _instructions;
		std::pair<uint64_t, uint64_t> _current_pos;
		std::vector<cc0::C0Function> _functionsTable;
		std::vector<cc0::C0Var> _globalVariablesTable;
		// 不包含全局常量,全局变量和函数, level from 1 ~ n-1
		std::vector<cc0::C0Var> _variablesTable;

		std::vector<bool> _returnTree;
		int32_t _preReturnIndex;

		// 下一个 token 在栈的偏移
		int32_t _nextTokenIndex;	
	};
}
