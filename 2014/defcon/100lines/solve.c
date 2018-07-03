#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
uint32_t randpad_len = 0x100;
uint8_t randpad[256] = {0xFC,0x8A,0x45,0x51,0x67,0x8C,0xA9,0xC0,0xB0,0xFD,0xF7,0x6F,0xB8,0x50,0xF1,0x2F,0x7A,0x62,0x66,0xE3,0xD3,0xC3,0x6E,0xBE,0x37,0x39,0x33,0x68,0x3B,0xC6,0x76,0x1E,0xAE,0xAA,0x83,0xED,0x57,0x1A,0xF1,0x29,0xE6,0xC1,0xB9,0x9E,0xDD,0xA2,0x86,0x2C,0x1A,0xDC,0x49,0x9D,0x82,0x01,0xD5,0x3A,0xB5,0xD3,0x33,0x12,0x1C,0xCE,0x94,0x2B,0xC3,0xB0,0x6C,0xBC,0x46,0x73,0x39,0x5E,0x7B,0xC7,0xB4,0x9E,0x56,0xF0,0xAD,0x72,0x5E,0x83,0xC7,0x05,0xC5,0xE9,0x2E,0x85,0x88,0x79,0x94,0xF7,0xE7,0xAC,0x34,0xFE,0x5C,0xCE,0x2E,0x13,0xF1,0xCC,0x8E,0xEA,0x60,0x83,0xBE,0xDC,0x4A,0xBB,0xE8,0xDF,0x65,0x20,0xEF,0x44,0xAD,0xFA,0xD6,0x12,0x83,0xD5,0xDC,0x94,0xAD,0x1F,0xE1,0x5F,0xE8,0xFA,0x7E,0x3F,0xDA,0x61,0xE3,0xDF,0xAB,0x5B,0x4F,0x2A,0x6C,0x24,0x82,0xAD,0x17,0x89,0xBA,0x29,0xB9,0x46,0x34,0x74,0x64,0xF7,0x45,0x22,0x8D,0xAF,0x33,0xD6,0x52,0xB5,0xDE,0x10,0xE4,0x53,0x5D,0x96,0xB7,0xE2,0x2E,0xCB,0xB1,0x75,0xBC,0x74,0x5A,0x21,0x29,0x8C,0x57,0xB3,0x16,0x5E,0xC7,0xC8,0xC2,0x26,0x35,0x48,0x2D,0x3C,0x60,0x7B,0x5D,0xDD,0xA8,0x29,0x61,0x19,0xD0,0xEF,0xEE,0x6D,0x04,0xDD,0x20,0x51,0x95,0x1D,0x01,0xE1,0xDA,0xDA,0xB4,0xA5,0x46,0xD9,0xCB,0xAF,0x56,0xB5,0x20,0x05,0xD0,0x6B,0xD2,0x22,0x21,0x2F,0x2D,0xD3,0x73,0x97,0x56,0x89,0xAE,0xAC,0x02,0xB6,0x35,0xD2,0x14,0x87,0xC6,0x49,0xDF,0x0E,0x17,0x85,0x64,0xE5,0xAF,0x6E,0x93,0x61};

uint64_t calc(uint32_t state, uint8_t* pad, uint64_t row, uint64_t col)
{
//      printf("[*] calc : %x %llx %llx\n", state, row, col);
        uint8_t esi = pad[(row >> 3) + col] << (row & 7);
        uint8_t edx = pad[(row >> 3) + col + 1] >> (8 - (row & 7));
        uint8_t tmp = esi | edx;
        uint8_t shift = (((0 - col) << 3) + 0x18);
        state |= (tmp << shift);
//      printf("%x %x %x %x %x\n", esi, edx, tmp, shift, state);
//      getchar();
        return state;

}
void loop(uint64_t num, uint8_t* pad, uint8_t* table)
{
        uint64_t end = num - 0x20;
        uint64_t i, k;
        uint64_t j, j2, j3;
        uint32_t state, state2;
        for (i = 0; i < end; i++)
        {
                state = 0;
                for (j = 0; j <= 3;  j++)
                {
                        state = calc(state, pad, i, j);
                }
                //printf("[*] state : %x\n", state);
                //getchar();

                for (k = 0; k < end; k++)
                {
                        state2 = 0;
                        for (j2 = 0; j2 <= 3; j2++)
                        {
                                state2 = calc(state2, pad, k, j2);
                        }
                        //printf("[*] state2 : %x\n", state2);
                        state2 ^= state;

                        for (j3 = 0; j3 <= 3; j3++)
                        {
                                uint32_t index = (end * i + k) * 4 + j3;
                                uint8_t val = state2 >> (24 - j3 * 8);
                                table[(end * i + k) * 4 + j3] = state2 >> (24 - j3 * 8);
                                //printf("table[%x] : %x\n", index, val);
                        }
                }
        }
}

uint8_t getByteWithoutTable(uint64_t index, uint64_t table_size, uint8_t* pad)
{
        uint64_t end = table_size - 0x20;
        uint64_t i = index / (end * 4);
        index = index % (end * 4);
        uint64_t k = index / 4;
        uint64_t j3 = index % 4;

        uint32_t state, state2;
        uint64_t j, j2;
        state = 0;
        for (j = 0; j <= 3; j++)
        {
                state = calc(state, pad, i, j);
        }

        state2 = 0;
        for (j2 = 0; j2 <= 3; j2++)
        {
                state2 = calc(state2, pad, k, j2);
        }
        state2 ^= state;

        index = (end * i + k) * 4 + j3;
        uint8_t val = state2 >> (24 - j3 * 8);
//      printf("[*] val : %x\n", val);
        return val;
}

int8_t getByte(uint64_t seed, uint64_t table_size, uint8_t* table)
{
        uint64_t some_size = ((table_size << 2) - 0x80) * (table_size - 0x20);
        printf("[*] some_size : %llx\n", some_size);
        uint8_t* ptr = malloc(some_size);
        loop(table_size, table, ptr);
        uint8_t ret = ptr[seed];
        free(ptr);
        return ret;
}

uint8_t calculate(uint8_t ret)
{
        uint8_t eax = (((ret * 3) << 5) + ret) >> 8;
        uint8_t calc = (ret - (((eax + ((ret - eax) >> 1)) >> 6) * 0x5d)) + 0x20;
        return calc;
}

int main()
{
        uint32_t calc_num = (randpad_len + 0x1ffffffc) << 3;
        uint64_t calc_num2 = ((calc_num * calc_num) << 2) - 0x20;
        uint64_t calc_num3 = (calc_num * calc_num) << 2;
        uint8_t* table = malloc(calc_num3);
        uint8_t* buf = malloc(0x26);
        int i;

        uint64_t calc_num4 = (calc_num2 * calc_num2) << 2;
        loop(calc_num, randpad, table);
uint64_t otp[38]={0x000189de37a70569,0x00019df1ea725270,0x00027af59720c5ed,0x00038103633209f2,0x00031234b21a33b2,0x000089ace015dc06,0x0000143a4eb25848,0x000311273b23716c,0x0000aeb665039a7a,0x00034a9157061ea2,0x0002f848ce102b99,0x0003b59f75fe50fc,0x00007118afc6f1b9,0x00017e948c6a3795,0x00005a49bbd783a1,0x000123d8400c7830,0x0002f54a3a2e133f,0x00033275dc37f1e7,0x0001223302c752bd,0x0002a1724611ae75,0x00021e44402b1bb7,0x0003459c72d9a74d,0x0000174afd90f1e1,0x00037d05d916966b,0x0001791cc0a42c5b,0x0000b46b7ac69c65,0x0000eb2c2b2beec2,0x00036ebb00da092a,0x00013bfbb52527e6,0x0002292f29918254,0x0002fa4b3fddcff0,0x0002aee22f17336e,0x00009b31491d7309,0x0002f494a84f857e,0x00015df5e9b451e1,0x0002827388e25dc3,0x00018728ceff666f,0x00020a282b2ac0fb};
        for (i = 0; i <= 7; i++)
        {
                uint8_t ret = getByteWithoutTable(otp[i], calc_num3, table);
                printf("%c", calculate(ret));
        }
        printf("\n");
        char cipher_text[] = {0x76,0x5c,0x21,0xeb,0x1e,0x7f,0x49,0xa9,0x6d,0xba,0xba,0x49,0xf8,0x46,0x76,0xa8,0x92,0x55,0x1e,0x16,0xba,0x71,0x9c,0x4d,0x38,0xc9,0x92,0xf7,0xfe,0xce,0x6c,0xb9,0x24,0xe0,0x14,0xf7,0x12,0x49};

        for (i = 0; i < 38; i++)
        {
                uint8_t ret = getByteWithoutTable(otp[i], calc_num3, table);
                printf("%c", ret ^ cipher_text[i]);
        }
        printf("\n");
}