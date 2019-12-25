#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <iostream>

namespace cc0 {

	inline void DieAndPrint(std::string condition) {
		std::cout << "Exception: " <<  condition << "\n";
		std::cout << "The program should not reach here.\n";
		std::cout << "Please check your program carefully.\n";
		std::cout << "If you believe it's not your fault, please report this to TAs.\n";
		std::abort();
		// or *((int*)114514) = 19260817;
	}

	// To keep it simple, we don't create an error system.
	enum ErrorCode { 
		ErrNoError, // Should be only used internally.
		ErrStreamError, 
		ErrEOF,
		ErrUnknown,
		ErrInvalidInput,				//无效输入
		ErrInvalidPrint,				//无效print
		ErrInvalidIdentifier,			//无效标识符
		ErrNeedIdentifier,				//需要标识符
		ErrConstantNeedValue,			//常量无值
		ErrNoSemicolon,					// ;
		ErrInvalidVariableDeclaration,	//无效变量声明
		ErrIncompleteExpression,		//表达式不完整
		ErrNotDeclared,					//先声明后使用
		ErrAssignToConstant,			//给常量赋值
		ErrDuplicateDeclaration,		//未声明var
		ErrNotInitialized,				//未初始化var
		ErrInvalidAssignment,			//无效赋值语句
		ErrNeedLeftBracket,		//need '('
		ErrNeedRightBracket,	//need ')'
		ErrNeedLeftBrace,		//need '{'
		ErrNeedRightBrace,		//need '}'
		ErrNeedComma,			//need ','
		ErrVoidInVar,			//把void用在了变量声明
		ErrConstFunc,			//const修饰了函数
		ErrFrontZero,			//十进制整数不能有前导0
		ErrInvalidHexInteger,	//只有0x|0X，缺少后面的数值
		ErrNeedTypeSpecifier,	//const 后面需要类型修饰符
		ErrUnimplemented,			//未实现的功能
		ErrInvalidStatement,		//无效表达式
		ErrConstVoid,				// const void xxx
		ErrExpressionNeedValue,		// 表达式需要值
		ErrNeedMain					// 缺少main函数
	};

	class CompilationError final{
	private:
		using uint64_t = std::uint64_t;
	public:

		friend void swap(CompilationError& lhs, CompilationError& rhs);

		CompilationError(uint64_t line, uint64_t column, ErrorCode err) :_pos(line, column), _err(err) {}
		CompilationError(std::pair<uint64_t, uint64_t> pos, ErrorCode err) : CompilationError(pos.first, pos.second, err) {}
		CompilationError(const CompilationError& ce) { _pos = ce._pos; _err = ce._err; }
		CompilationError(CompilationError&& ce) :CompilationError(0, 0, ErrorCode::ErrNoError) { swap(*this, ce); }
		CompilationError& operator=(CompilationError ce) { swap(*this, ce); return *this; }
		bool operator==(const CompilationError& rhs) const { return _pos == rhs._pos && _err == rhs._err; }

		std::pair<uint64_t, uint64_t> GetPos()const { return _pos; }
		ErrorCode GetCode() const { return _err; }
	private:
		std::pair<uint64_t, uint64_t> _pos;
		ErrorCode _err;
	};

	inline void swap(CompilationError& lhs, CompilationError& rhs) {
		using std::swap;
		swap(lhs._pos, rhs._pos);
		swap(lhs._err, rhs._err);
	}
}