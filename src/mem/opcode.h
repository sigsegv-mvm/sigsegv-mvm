#ifndef _INCLUDE_SIGSEGV_MEM_OPCODE_H_
#define _INCLUDE_SIGSEGV_MEM_OPCODE_H_


class JmpRelImm32
{
public:
	static constexpr size_t Size() { return 5; }
	
	/* is this atomic? no
	 * do we care?     no */
	static void Write(void *buf, uint32_t target)
	{
		uint32_t reltgt = target - ((uintptr_t)buf + Size());
		
		*(uint8_t  *)((uintptr_t)buf + 0x00) = 0xe9;
		*(uint32_t *)((uintptr_t)buf + 0x01) = reltgt;
		
	//	TRACE("[%08x -> %08x]: %02x %02x %02x %02x %02x",
	//		(uintptr_t)buf, target,
	//		((uint8_t *)buf)[0],
	//		((uint8_t *)buf)[1],
	//		((uint8_t *)buf)[2],
	//		((uint8_t *)buf)[3],
	//		((uint8_t *)buf)[4]);
	}
	
	/* pad out the empty space with NOP's to avoid confusing the disassembler */
	static void WritePadded(void *buf, uint32_t target, size_t len)
	{
		Write(buf, target);
		
		for (size_t i = Size(); i < len; ++i) {
			*(uint8_t *)((uintptr_t)buf + i) = 0x90;
		}
	}
};

class CallAbsMem32
{
public:
	static constexpr size_t Size() { return 6; }
	
	static void Write(void *buf, uint32_t target)
	{
		*(uint8_t  *)((uintptr_t)buf + 0x00) = 0xff;
		*(uint8_t  *)((uintptr_t)buf + 0x01) = 0x15;
		*(uint32_t *)((uintptr_t)buf + 0x02) = target;
		
	//	TRACE("[%08x -> %08x]: %02x %02x %02x %02x %02x %02x",
	//		(uintptr_t)buf, target,
	//		((uint8_t *)buf)[0],
	//		((uint8_t *)buf)[1],
	//		((uint8_t *)buf)[2],
	//		((uint8_t *)buf)[3],
	//		((uint8_t *)buf)[4],
	//		((uint8_t *)buf)[5]);
	}
};

class PushImm32
{
public:
	static constexpr size_t Size() { return 5; }
	
	static void Write(void *buf, uint32_t val)
	{
		*(uint8_t  *)((uintptr_t)buf + 0x00) = 0x68;
		*(uint32_t *)((uintptr_t)buf + 0x01) = val;
		
	//	TRACE("[%08x: %08x]: %02x %02x %02x %02x %02x",
	//		(uintptr_t)buf, val,
	//		((uint8_t *)buf)[0],
	//		((uint8_t *)buf)[1],
	//		((uint8_t *)buf)[2],
	//		((uint8_t *)buf)[3],
	//		((uint8_t *)buf)[4]);
	}
};


#endif
