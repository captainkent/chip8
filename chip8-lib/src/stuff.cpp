#include "stuff.h"

void stuff::DissassembleChip8OpCode(uint8_t* codebuffer, int pc)
{
	uint8_t* code = &codebuffer[pc];
	uint8_t	firstnib = (code[0] >> 4);

	uint16_t opcode = code[0] << 8 | code[1];
	uint8_t codes[2];
	codes[0] = (opcode & 0xFF00) >> 8;
	codes[1] = opcode & 0x00FF;

	printf("%04x %02x %02x ", pc, codes[0], codes[1]);

	switch (firstnib)
	{
	case 0x00:
	{
		uint8_t secondnib = code[1];
		switch (secondnib)
		{
		case 0xE0:
		{
			printf("%-10s", "CLS");
			break;
		}
		case 0xEE:
		{
			printf("%-10s", "RET");
			break;
		}
		}
		break;
	}
	case 0x01:
	{
		// code is 1NNN, where NNN is the address I need.
		// split it up into two seperate parts, shift second part into first part
		// 0x11: firstnib
		// 0x23: secondnib
		// adress is 0x123

		//uint8_t first = code[0] & 0x0f;
		//uint8_t second = code[1];
		//uint16_t location = (first << 8) + second;

		uint16_t location = ((code[0] & 0x0f) << 8) + code[1];
		printf("%-10s #$%03x", "JP", location);
		break;
	}
	case 0x02:
	{
		uint16_t addr = ((code[0] & 0x0f) << 8) + code[1];
		printf("%-10s #$%03x", "CALL", addr);
		break;
	}
	case 0x03:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01X,#$%02x", "SE", reg, code[1]);
		break;

	}
	case 0x04:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01X,#$%02x", "SNE", reg, code[1]);
		break;

	}
	case 0x05:
	{
		uint8_t regx = code[0] & 0x0f;
		uint8_t regy = code[1] & 0x0f;
		printf("%-10s V%01X,V%01X", "SNE", regx, regy);
		break;
	}
	case 0x06:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01X,#$%02x", "MVI", reg, code[1]);
		break;
	}
	case 0x07:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01X,#$%02x", "ADD", reg, code[1]);
		break;
	}
	case 0x08:
	{
		uint8_t secondnib = code[1] & 0x0f;
		switch (secondnib)
		{
		case 0x00:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "LD", regx, regy);
			break;
		}
		case 0x01:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "OR", regx, regy);
			break;
		}
		case 0x02:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "AND", regx, regy);
			break;
		}
		case 0x03:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "XOR", regx, regy);
			break;
		}
		case 0x04:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "ADD", regx, regy);
			break;
		}
		case 0x05:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "SUB", regx, regy);
			break;
		}
		case 0x06:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "SHR", regx, regy);
			break;
		}
		case 0x07:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "SUBN", regx, regy);
			break;
		}
		case 0x0E:
		{
			uint8_t regx = code[0] & 0x0f;
			uint8_t regy = (code[1] & 0xf0) >> 4;
			printf("%-10s V%01X,V%01X", "SHL", regx, regy);
			break;
		}

		default:
		{
			printf("not yet enabled");
		}
		}
		break;

		// not fininished!!!
	}
	case 0x09:
	{
		uint8_t regx = code[0] & 0x0f;
		uint8_t regy = (code[1] & 0xf0) >> 4;
		printf("%-10s V%01X,V%01X", "SNE", regx, regy);
		break;
	}
	case 0x0A:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01X,#$%02x", "MVI", reg, code[1]);
		break;
	}
	case 0x0B:
	{
		uint16_t location = ((code[0] & 0x0f) << 8) + code[1];
		printf("%-10s #$%03x", "JP", location);
		break;
	}
	case 0x0C:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01X,#$%02x", "RND", reg, code[1]);
		break;

	}
	case 0x0D:
	{
		uint8_t regx = code[0] & 0x0f;
		uint8_t regy = (code[1] & 0xf0) >> 4;
		uint8_t nibble = code[1] & 0x0f;
		printf("%-10s V%01X,V%01X,#$%01X", "DRW", regx, regy, nibble);
		break;
	}
	case 0x0E:
	{
		uint8_t secondnib = code[1];
		switch (secondnib)
		{
		case 0x9E:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "SKP", reg);
			break;
		}
		case 0xA1:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "SKNP", reg);
			break;
		}
		}
		break;
	}
	case 0x0F:
	{
		uint8_t secondnib = code[1];
		switch (secondnib)
		{
		case 0x07:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X,#DT", "LD Vx DT", reg);
			break;
		}
		case 0x0A:
		{
			uint8_t key = code[0] & 0x0f;
			printf("%-10s V%01X,#K", "LD Vx K", key);
			break;
		}
		case 0x15:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "LD DT Vx", reg);
			break;
		}
		case 0x18:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "LD ST Vx", reg);
			break;
		}
		case 0x1E:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "ADD F Vx", reg);
			break;
		}
		case 0x29:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "LD F Vx", reg);
			break;
		}
		case 0x33:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "LD B Vx", reg);
			break;
		}
		case 0x55:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "LD [I] Vx", reg);
			break;
		}
		case 0x65:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01X", "LD Vx [I]", reg);
			break;
		}
		default:
		{
			printf("not yet enabled");
		}
		}
	}
	break;
	}
}

