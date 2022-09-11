#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "asm.h"

typedef uint64_t (*program_t)(uint8_t* mem);

void compile_dot(ASMWriter* writer) {
	// push rcx
	asm_write1(writer, PUSH_RD | ECX);
	//mov eax, 1 (write)
	asm_write2_i32(writer, MOV_R32_I32, R_DIRECT | EAX_RM, 1);
	// mov edi, 1 (stdout)
	asm_write2_i32(writer, MOV_R32_I32, R_DIRECT | EDI_RM, 1);
	// mov rsi, rcx (buffer)
	asm_write3(writer, REX | REX_W, MOV_RM32_R32, R_DIRECT | ECX_REG | ESI_RM);
	// mov edx, 1 (count)
	asm_write2_i32(writer, MOV_R32_I32, R_DIRECT | EDX_RM, 1);
	// syscall
	asm_write2(writer, PRE_0F, SYSCALL);
	// pop rcx
	asm_write1(writer, POP_RD | ECX);
}

void compile_comma(ASMWriter* writer) {
	// push rcx
	asm_write1(writer, PUSH_RD | ECX);
	//mov eax, 0 (write)
	asm_write2_i32(writer, MOV_R32_I32, R_DIRECT | EAX_RM, 0);
	// mov edi, 0 (stdout)
	asm_write2_i32(writer, MOV_R32_I32, R_DIRECT | EDI_RM, 0);
	// mov rsi, rcx (buffer)
	asm_write3(writer, REX | REX_W, MOV_RM32_R32, R_DIRECT | ECX_REG | ESI_RM);
	// mov edx, 1 (count)
	asm_write2_i32(writer, MOV_R32_I32, R_DIRECT | EDX_RM, 1);
	// syscall
	asm_write2(writer, PRE_0F, SYSCALL);
	// pop rcx
	asm_write1(writer, POP_RD | ECX);
}

ASMWriter compile(const char* source) {
	ASMWriter writer;
	init_asm_writer(&writer, 1024);

	size_t sourceLength = strlen(source);

	int openBrackets = 0;
	uint64_t bracketOffsets[64];

	// ==== Header ====
	// mov rcx, rdi
	asm_write3(&writer, REX | REX_W, MOV_RM32_R32, R_DIRECT | EDI_REG | ECX_RM);

	for(size_t i = 0; i < sourceLength; ++i) {
		char c = source[i];

		switch(c) {
			case '>': {
				// add rcx, byte <count>
				int count = 0;
				while(source[i] == '>') {
					count++;
					i++;
				}
				i--;
				asm_write4(&writer, REX | REX_W, ADD_RM32_I8, R_DIRECT | ECX_RM, count);
			} break;

			case '<': {
				// sub rcx, byte <count>
				int count = 0;
				while(source[i] == '<') {
					count++;
					i++;
				}
				i--;
				asm_write4(&writer, REX | REX_W, SUB_RM32_I8, R_DIRECT | REG_5 | ECX_RM, count);
			} break;

			case '+': {
				// add byte [rcx], <count>
				int count = 0;
				while(source[i] == '+') {
					count++;
					i++;
				}
				i--;
				asm_write3(&writer, ADD_RM8_I8, ECX_RM, count);
			} break;

			case '-': {
				// sub byte [rcx], <count>
				int count = 0;
				while(source[i] == '-') {
					count++;
					i++;
				}
				i--;
				asm_write3(&writer, ADD_RM8_I8, REG_5 | ECX_RM, count);
			} break;

			case '.': {
				compile_dot(&writer);
			} break;

			case ',': {
				compile_comma(&writer);
			} break;

			case '[': {
				if(openBrackets >= 63) {
					fprintf(stderr, "Loop nesting limit reached - 64\n");
					exit(1);
				}

				asm_write2(&writer, MOV_R8_RM8, EAX_REG | ECX_RM);
				asm_write2(&writer, TEST_RM8_R8, R_DIRECT | EAX_REG | EAX_RM);

				bracketOffsets[openBrackets++] = writer.offset;

				asm_write2_i32(&writer, PRE_0F, JZ, 0);
			} break;

			case ']': {
				if(openBrackets == 0) {
					fprintf(stderr, "Bracket Mismatch\n");
					exit(1);
				}

				asm_write2(&writer, MOV_R8_RM8, EAX_REG | ECX_RM);
				asm_write2(&writer, TEST_RM8_R8, R_DIRECT | EAX_REG | EAX_RM);

				uint64_t openOffset = bracketOffsets[--openBrackets];
				uint64_t currentOffset = writer.offset;

				uint64_t offset = currentOffset - openOffset;
				offset = -offset;

				asm_write2_i32(&writer, PRE_0F, JNZ, offset);

				uint64_t openJumpDest = writer.offset;
				uint64_t openJumpOffset = openJumpDest - openOffset - 6; // the - 6 skips the JZ instruction
				asm_patch_i32(&writer, openOffset + 2, openJumpOffset); // the + 2 here skips the opcode
			} break;
		}
	}
	
	// ==== Footer ====
	// mov rax, qword [rcx]
	asm_write3(&writer, REX | REX_W, MOV_R32_RM32, ECX_RM | EAX_REG);
	// ret
	asm_write1(&writer, RET_NEAR);

	asm_finalise(&writer);

	return writer;
}

const char* read_file(const char* filename) {
	FILE* file = fopen(filename, "rb");

	if(file == NULL) {
		fprintf(stderr, "Failed to open file '%s': %s\n", filename, strerror(errno));
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	rewind(file);

	char* input = malloc(length + 1);

	fread(input, length, 1, file);

	fclose(file);

	input[length] = '\0';

	return input;
}

int main(int argc, const char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Expected at least 1 argument\n");
		return -1;
	}
	
	const char* source = read_file(argv[1]);

	ASMWriter writer = compile(source);

	program_t program = (program_t)writer.buffer;

	uint8_t* buffer = calloc(1, 30000);
	
	program(buffer);

	free(buffer);

	if(argc > 2) {
		FILE* dumpFile = fopen(argv[2], "wb");

		fwrite(writer.buffer, writer.offset, 1, dumpFile);

		fclose(dumpFile);
	}

	munmap(program, writer.allocated);

	return 0;
}