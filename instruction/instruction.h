#pragma once

#include <cstdint>
#include <utility>

namespace miniplc0 {

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
		PFI			//.F1:    .F0:     .F17:	...
	};
	
	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		// Instruction(Operation opr, int32_t x) : _opr(opr), _x(x) {}
		// Instruction(Operation opr, int32_t x, int32_t y) : _opr(opr), _x(x), _y(y) {} 
		Instruction(int32_t index, Operation opr, int32_t x, int32_t y) : _index(index), _opr(opr), _x(x), _y(y) {} 
		Instruction() : Instruction(0, Operation::NOP, 0, 0){}
		Instruction(const Instruction& i) { _index = i._index; _opr = i._opr; _x = i._x; _y = i._y;}
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _index == i._index && _opr == i._opr && _x == i._x && _y == i._y; }

		void SetX(int32_t x){ _x = x; }
		void SetY(int32_t y){ _y = y; }
		int32_t GetIndex() const { return _index; }
		Operation GetOperation() const { return _opr; }
		int32_t GetX() const { return _x; }
		int32_t GetY() const { return _y; }
	private:
		int32_t _index;
		Operation _opr;
		int32_t _x;
		int32_t _y;
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._index, rhs._index);
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
		swap(lhs._y, rhs._y);
	}
}