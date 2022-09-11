#include "asm.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

void init_asm_writer(ASMWriter* writer, size_t bufferSize) {
	writer->buffer = mmap(NULL, bufferSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	writer->offset = 0;
	writer->allocated = bufferSize;
}

void asm_finalise(ASMWriter* writer) {
	mprotect(writer->buffer, writer->allocated, PROT_READ | PROT_EXEC);
}

void asm_write1(ASMWriter* writer, uint8_t byte1) {
	writer->buffer[writer->offset] = byte1;
	writer->offset += 1;
}

void asm_write2(ASMWriter* writer, uint8_t byte1, uint8_t byte2) {
	writer->buffer[writer->offset] = byte1;
	writer->buffer[writer->offset + 1] = byte2;
	writer->offset += 2;
}

void asm_write3(ASMWriter* writer, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
	writer->buffer[writer->offset] = byte1;
	writer->buffer[writer->offset + 1] = byte2;
	writer->buffer[writer->offset + 2] = byte3;
	writer->offset += 3;
}

void asm_write4(ASMWriter* writer, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) {
	writer->buffer[writer->offset] = byte1;
	writer->buffer[writer->offset + 1] = byte2;
	writer->buffer[writer->offset + 2] = byte3;
	writer->buffer[writer->offset + 3] = byte4;
	writer->offset += 4;
}

void asm_write2_i32(ASMWriter* writer, uint8_t byte1, uint8_t byte2, uint32_t imm1) {
	writer->buffer[writer->offset] = byte1;
	writer->buffer[writer->offset + 1] = byte2;
	writer->buffer[writer->offset + 2] = (imm1 >> 0)  & 0xff;
	writer->buffer[writer->offset + 3] = (imm1 >> 8)  & 0xff;
	writer->buffer[writer->offset + 4] = (imm1 >> 16) & 0xff;
	writer->buffer[writer->offset + 5] = (imm1 >> 24) & 0xff;
	writer->offset += 6;
}

void asm_patch_i32(ASMWriter* writer, uint64_t offset, uint32_t imm1) {
	writer->buffer[offset] = (imm1 >> 0)  & 0xff;
	writer->buffer[offset + 1] = (imm1 >> 8)  & 0xff;
	writer->buffer[offset + 2] = (imm1 >> 16) & 0xff;
	writer->buffer[offset + 3] = (imm1 >> 24) & 0xff;
}
