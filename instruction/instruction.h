#pragma once

#include <cstdint>
#include <utility>
#include <string>

namespace cc0 {

	enum Operation {

		NOP = 0,
		BIPUSH,
		IPUSH,
		POP,
		POP2,
		POPN,
		DUP,
		DUP2,
		LOADC,		// load constant
		LOADA,		// loada level_diff(2), offset(4)
		NEW,		// pop (int)count, new (count) slot
		SNEW,		// snew count(4)
		ILOAD,
		ALOAD,
		IALOAD,		// array
		AALOAD,
		ISTORE,
		ASTORE,
		IASTORE,	// array
		AASTORE,
		IADD,
		// DADD,
		ISUB,
		// DSUB,
		IMUL,
		// DMUL,
		IDIV,
		// DDIV,
		INEG,		// a = -a
		// DNEG,
		ICMP,		// pop r, pop l, push sign(l-r);  1|-1|0
		// DCMP,
		// I2D,
		// D2I,
		// I2C,
					// jxxx offset(2)
		JMP,		
		JE,
		JNE,
		JL,
		JGE,
		JG,
		JLE,
		CALL,		// call index(2)
		RET,
		IRET,
		// ARET,
		IPRINT,
		CPRINT,
		// APRINT,
		// SPRINT,
		PRINTL,
		ISCAN,

		PCONSTANTS,	//.constants:
		PSTART,		//.start:
		PFUNCTION,	//.functions:
		PFI,			//.F1:    .F0:     .F17:	...

		SAVECONST,
		SAVEFUNCTION
	};
	
	class Instruction final {
	private:
		using uint64_t = std::uint64_t;
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		// 常量池指令
		Instruction(int32_t index, Operation opr, std::string& constType, std::string& stringValue) : 
			_index(index), _opr(opr), _nameindex(-1), _x(0), _y(0), _constType(constType), _stringValue(stringValue)  {} 
		// 常规指令
		Instruction(int32_t index, Operation opr, int32_t x, int32_t y) : 
			_index(index), _opr(opr), _nameindex(-1), _x(x), _y(y), _constType("S")  {} 
		// 函数表指令
		// Instruction(int32_t index, int32_t nameindex, int32_t x, int32_t y) : 
		// 	_type((int32_t)2), _index(index), _opr(opr),_nameindex(nameindex), _x(x), _y(y), _stringValue(" "), _constType(" ")  {} 
		Instruction() : Instruction(0, Operation::NOP, 0, 0){}
		Instruction(const Instruction& i) { _index = i._index; _opr = i._opr; _x = i._x; _y = i._y;}
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _index == i._index && _opr == i._opr && _x == i._x && _y == i._y; }

		void SetX(int32_t x){ _x = x; }
		void SetY(int32_t y){ _y = y; }
		void SetConstType(std::string contsType){_constType = contsType;}
		void SetStringValue(std::string stringValue){ _stringValue = stringValue;}
		int32_t GetIndex() const { return _index; }
		Operation GetOperation() const { return _opr; }
		int32_t GetNameindex() const { return _nameindex; }
		int32_t GetX() const { return _x; }
		int32_t GetY() const { return _y; }
		std::string GetConstType() const { return _constType;}
		std::string GetStringValue() const { return _stringValue;}
		
	private:
		// _type=0:常规指令;=1:常量池;=2:函数表
		// int32_t _type ;
		int32_t _index ;
		Operation _opr;
		int32_t _nameindex ;
		int32_t _x ;
		int32_t _y ;
		std::string _constType ;//常量池中，常量的类型
		std::string _stringValue;//字符串的值
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._index, rhs._index);
		swap(lhs._opr, rhs._opr);
		swap(lhs._nameindex,rhs._nameindex);
		swap(lhs._x, rhs._x);
		swap(lhs._y, rhs._y);
		swap(lhs._constType, rhs._constType);
		swap(lhs._stringValue, rhs._stringValue);
	}
}