

// Number of bytes in a savefile
const unsigned long FILE_BYTES PROGMEM = 7077961;

// Buffer of empty bytes, for writing into a newly-emptied tick en masse
const byte EMPTY_TICK[17] PROGMEM = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

// Partial-screen letter M, for the CLOCK MASTER/FOLLOW screen
const byte TEXT_M[6] PROGMEM = {
	B10000001,
	B11000011,
	B10100101,
	B10011001,
	B10000001,
	B10000001
};

// Partial-screen letter F, for the CLOCK MASTER/FOLLOW screen
const byte TEXT_F[6] PROGMEM = {
	B01111110,
	B01000000,
	B01111100,
	B01000000,
	B01000000,
	B01000000
};

// Partial-screen letter S, displayed during data-save functions
const byte TEXT_S[6] PROGMEM = {
	B00111110;
	B01000000;
	B00111100;
	B00000010;
	B00000010;
	B01111100;
};

// Partial-screen letter L, displayed during data-load functions
const byte TEXT_L[6] PROGMEM = {
	B01000000;
	B01000000;
	B01000000;
	B01000000;
	B01000000;
	B01111110;
};


// Full-screen picture of a half-note
const byte PICTURE_NOTE[9] PROGMEM = {
	B00010000,
	B00010000,
	B00010000,
	B00010000,
	B00010000,
	B01110000,
	B01010000,
	B01110000
};

// Partial-screen picture of a plus-sign, for notes with intervals that are neither plain nor dotted
const byte PICTURE_NOTEPLUS[3] PROGMEM = {
	B00000010,
	B00000111,
	B00000010
};


// Partial-screen number 0
const byte NUM_0[7] PROGMEM = {
	B00001110,
	B00010001,
	B00010001,
	B00010001,
	B00010001,
	B00001110,
};

// Partial-screen number 1
const byte NUM_1[7] PROGMEM = {
	B00000100,
	B00001100,
	B00000100,
	B00000100,
	B00000100,
	B00000100,
};

// Partial-screen number 2
const byte NUM_2[7] PROGMEM = {
	B00001110,
	B00010001,
	B00000010,
	B00000100,
	B00001000,
	B00011111,
};

// Partial-screen number 3
const byte NUM_3[7] PROGMEM = {
	B00011111,
	B00000001,
	B00001110,
	B00000001,
	B00010001,
	B00001110,
};

// Partial-screen number 4
const byte NUM_4[7] PROGMEM = {
	B00010001,
	B00010001,
	B00011111,
	B00000001,
	B00000001,
	B00000001,
};

// Partial-screen number 5
const byte NUM_5[7] PROGMEM = {
	B00011111,
	B00010000,
	B00011110,
	B00000001,
	B00010001,
	B00001110,
};

// Partial-screen number 6
const byte NUM_6[7] PROGMEM = {
	B00001110,
	B00010001,
	B00010000,
	B00011110,
	B00010001,
	B00001110,
};

// Partial-screen number 7
const byte NUM_7[7] PROGMEM = {
	B00011111,
	B00000001,
	B00000010,
	B00000100,
	B00001000,
	B00010000,
};

// Partial-screen number 8
const byte NUM_8[7] PROGMEM = {
	B00001110,
	B00010001,
	B00001110,
	B00010001,
	B00010001,
	B00001110,
};

// Partial-screen number 9
const byte NUM_9[7] PROGMEM = {
	B00001110,
	B00010001,
	B00001111,
	B00000001,
	B00010001,
	B00001110,
};
