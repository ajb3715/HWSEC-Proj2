/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal.h"
#include <stdint.h>
#include "simpleserial.h"
#include "AES_128.h"
#include <time.h>

//AES current state matrix
unsigned char StateArray [4][4];
//Expanded Key matrix
unsigned char ExpandedKey[11][4][4];
//Masked state array matrix
unsigned char ST_Matrix [4][4];
//Mask Matrix
unsigned char Mask_Matrix[4][4];
//State array
unsigned char S_Matrix[4][4];
//Flag for if the key was sent to the board
unsigned char key_entered = 0;

unsigned char Key[4][4]= {  {0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00} };

unsigned char PlainText[4][4]  = {  {0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00} };

void AESRound(unsigned int StateArray[4],unsigned int Key [4] );

void encrypt();
uint8_t get_key(uint8_t* k, uint8_t len)
{
	//Set the internal key
	memcpy(Key, k, 16);
	return 0x00;
}

void encrypt(){
	//generate 16 random values for the mask
	srand(time(NULL));
	for (int i = 0; i < 4; i ++){
		for(int j = 0; j<4; j++){
			Mask_Matrix[i][j] = rand() % 256;
		}
	}
	//Mask the plaintext 
	AddMatrices(Mask_Matrix, PlainText, ST_Matrix);

	ExpandKey(Key, ExpandedKey);
	//Set the state array
	memcpy(StateArray, PlainText, 4 * 4 * sizeof(unsigned char));

	//Add the round key 
	AddRoundKey(ExpandedKey[0], StateArray);

	AddRoundKey(ExpandedKey[0], ST_Matrix);
	int i;
	for(i=1; i<=10; i++){

		//Unmask input
		AddMatrices(ST_Matrix, Mask_Matrix, S_Matrix);
		//Apply Sbox
		SubBytes(S_Matrix);
		SubBytes(StateArray);
		//Remask Output
		AddMatrices(S_Matrix, Mask_Matrix, ST_Matrix);
		//ShiftRows 
		ShiftRows(StateArray);
		ShiftRows(ST_Matrix);
		ShiftRows(Mask_Matrix);

		if(i!=10){
			//Mix Columns 9 times 
			MixColumns(StateArray);
			MixColumns(ST_Matrix);
			MixColumns(Mask_Matrix);
		}
		//Add the round key
		AddRoundKey(ExpandedKey[i], StateArray);
		AddRoundKey(ExpandedKey[i], ST_Matrix);
	}
	//Do unmasking stuff
	AddMatrices(ST_Matrix, Mask_Matrix, StateArray);
}	

uint8_t get_pt(uint8_t* pt, uint8_t len)
{
	/**********************************
	* Start user-specific code here. */
	

	if ( key_entered == 1 ){

	memcpy(PlainText, pt, 16);

	trigger_high();

	encrypt();

	trigger_low();	

	pt = (uint8_t*)StateArray;

	

	simpleserial_put('r', 16, pt);

	return 0x00;

	}
	else{
		memcpy(Key, pt, 16);
		key_entered = 1;

		trigger_low();

		simpleserial_put('r', 16, pt);
		return 0x00;
	}

}

uint8_t reset(uint8_t* x, uint8_t len)
{
	// Reset key here if needed
	return 0x00;
}

#if SS_VER == SS_VER_2_1
uint8_t aes(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
{
    uint8_t req_len = 0;
    uint8_t err = 0;

    if (scmd & 0x02) {
        req_len += 16;
        if (req_len > len) {
            return SS_ERR_LEN;
        }
        err = get_key(buf + req_len - 16, 16);
        if (err)
            return err;
    }
    if (scmd & 0x01) {
        req_len += 16;
        if (req_len > len) {
            return SS_ERR_LEN;
        }
        err = get_pt(buf + req_len - 16, 16);
        if (err)
            return err;
    }

    if (len != req_len) {
        return SS_ERR_LEN;
    }

    return 0x00;

}
#endif

int main(void)
{
    platform_init();
	init_uart();
	trigger_setup();

	simpleserial_init();
#if SS_VER != SS_VER_2_1
	simpleserial_addcmd('p', 16, get_pt);
	simpleserial_addcmd('k', 16, get_key);
	simpleserial_addcmd('x', 0, reset);
#else
    simpleserial_addcmd(0x01, 16, aes);
#endif
	while(1)
		simpleserial_get();
}

uint32_t Cat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3){
	uint32_t word = 0;
	word += b0;
	word = (word << 8);
	word += b1;
	word = (word << 8);
	word += b2;
	word = (word << 8);
	word += b3;
	return word;
}
