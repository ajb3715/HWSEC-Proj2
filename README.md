This encrytption engine uses a barebones AES implementation. There are various improvements for performance such as a SBox Look-up table and 2 multiplication tables for the MixColumns step of AES. 
The main security enchancement is the use of masking in the encryption. A mask is generated at the beginning of encryption by generating a random number based off the system time. 
This mask is then added to the plaintext to make the first state array of the encryption. This is then used throughout the encryption. 
At the end of the encryption, the mask matrix is then added to the state array matrix to get the actually ciphertext. This ciphertext is then sent via UART to the python notebook. 

Folder path for the C files: 
/hardware/victims/firmware/simpleserial-base
