#ifndef JIT_ASM_H
#define JIT_ASM_H
#include <stdint.h>
#include <stddef.h>

// Instructions
#define PRE_0F 0x0f
#define ADD_RM8_I8 0x80
#define ADD_RM32_I8 0x83
#define JZ 0x84
#define JNZ 0x85
#define MOV_R8_RM8 0x8a
#define MOV_R32_I32 0xc7
#define MOV_RM32_R32 0x89
#define MOV_R32_RM32 0x8B
#define POP_RD 0x58
#define PUSH_RD 0x50
#define RET_NEAR 0xc3
#define SUB_RM32_I8 ADD_RM32_I8
#define SYSCALL 0x05
#define TEST_RM8_R8 0x84
#define TEST_RM32_R32 0x85

// REX
#define REX ((0b0100) << 4)
#define REX_W (1 << 3)

// MODR/M
#define R_DIRECT (0b11 << 6)
#define EAX 0
#define EAX_REG (EAX << 3)
#define EAX_RM EAX
#define ECX 1
#define ECX_REG (ECX << 3)
#define ECX_RM ECX
#define EDX 2
#define EDX_REG (EDX << 3)
#define EDX_RM EDX
#define ESI 6
#define ESI_REG (ESI << 3)
#define ESI_RM ESI
#define EDI 7
#define EDI_REG (EDI << 3)
#define EDI_RM EDI
#define REG_5 (5 << 3)
#define REG_6 (6 << 3)

typedef struct ASMWriter {
	uint8_t* buffer;
	size_t offset;
	size_t allocated;
} ASMWriter;

void init_asm_writer(ASMWriter* writer, size_t bufferSize);
void asm_finalise(ASMWriter* writer);
void asm_write1(ASMWriter* writer, uint8_t byte1);
void asm_write2(ASMWriter* writer, uint8_t byte1, uint8_t byte2);
void asm_write3(ASMWriter* writer, uint8_t byte1, uint8_t byte2, uint8_t byte3);
void asm_write4(ASMWriter* writer, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);
void asm_write2_i32(ASMWriter* writer, uint8_t byte1, uint8_t byte2, uint32_t imm1);
void asm_patch_i32(ASMWriter* writer, uint64_t offset, uint32_t imm1);

#endif