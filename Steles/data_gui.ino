

// Holds which bits in the button-var correspond to which raw notes, for any given grid-configuration.
// (Within a given block in this array: first value = top left; each rightward row of note-values here represents a physical downward column of buttons)
const byte GRIDS[] PROGMEM = {

	0,  1,  2,  3,  4,  5,  // Config 0: Guitar-like, with horizontally adjacent fourths
	5,  6,  7,  8,  9,  10,
	10, 11, 12, 13, 14, 15,
	15, 16, 17, 18, 19, 20,

	5,  4,  3,  2,  1,  0,  // Config 1: Reverse-guitar-like, with horizontally adjacent fourths
	10, 9,  8,  7,  6,  5,
	15, 14, 13, 12, 11, 10,
	20, 19, 18, 17, 16, 15,

	0,  1,  2,  3,  4,  5,  // Config 2: Guitar-like, with horizontally adjacent tritones
	6,  7,  8,  9,  10, 11,
	12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23,

	5,  4,  3,  2,  1,  0,  // Config 3: Reverse-guitar-like, with horizontally adjacent tritones
	11, 10, 9,  8,  7,  6,
	17, 16, 15, 14, 13, 12,
	23, 22, 21, 20, 19, 18,

	20, 16, 12, 8,  4,  0,  // Config 4: Ascending grid, bottom-left to top-right
	21, 17, 13, 9,  5,  1,
	22, 18, 14, 10, 6,  2,
	23, 19, 15, 11, 7,  3,

	0,  4,  8,  12, 16, 20, // Config 5: Descending grid, top-left to bottom-right
	1,  5,  9,  13, 17, 21,
	2,  6,  10, 14, 18, 22,
	3,  7,  11, 15, 19, 23,

};


// Long glyph: logo to display on startup
const byte LOGO[] PROGMEM = {
	B01111110, B01111110, B01000110, B01111110,
	B01111110, B01111110, B01100110, B01111110,
	B00011000, B00011000, B01110110, B01100000,
	B00011000, B00011000, B01111110, B01111100,
	B00011000, B00011000, B01101110, B01100000,
	B00011000, B01111110, B01100110, B01111110,
	B00011000, B01111110, B01100010, B01111110
};

// Standalone glyph: ERASING NOTES
const byte GLYPH_ERASE[] PROGMEM = {
	B00000000,
	B11101010,
	B10001010,
	B11101010,
	B10000000,
	B11101010
};

// Standalone glyph: RESET TIMING
const byte GLYPH_RESET_TIMING[] PROGMEM = {
	B00000000,
	B11100111,
	B10010010,
	B11100010,
	B10010010,
	B10010010,
};

// Standalone glyph: SHIFT GLOBAL CUE
const byte GLYPH_SHIFT[] PROGMEM = {
	B00000000,
	B11101110,
	B10001000,
	B11101000,
	B00101000,
	B11101110,
};

// Multiglyph array: holds all straightforward command-glyphs
const byte GLYPHS[] PROGMEM = {

	// Glyph: blank
	0, 0, 0, 0, 0, 0,

	// Glyph: ARM RECORDNOTES
	B00000000,
	B11100000,
	B10010000,
	B11100000,
	B10010000,
	B10010000,

	// Glyph: ARPEGGIATOR MODE
	B00000000,
	B01001011,
	B01001011,
	B01011010,
	B11011010,
	B11000010,

	// Glyph: ARPEGGIATOR REFRESH
	B00000000,
	B01001110,
	B10101001,
	B11101110,
	B10101001,
	B10101001,

	// Glyph: CHANNEL
	B00000000,
	B11101000,
	B10101000,
	B10001100,
	B10101010,
	B11101010,

	// Glyph: CLEAR NOTES
	B00000000,
	B10001001,
	B01010001,
	B00100001,
	B01010011,
	B10001011,

	// Glyph: DURATION
	B00000000,
	B11000000,
	B10100000,
	B10101110,
	B10101000,
	B11001000,

	// Glyph: GENERIC (should not be used)
	B00000000,
	B00111100,
	B00111100,
	B00111100,
	B00111100,
	B00000000,

	// Glyph: GRID-CONFIG
	B00000000,
	B11110111,
	B10000100,
	B10110100,
	B10010100,
	B11110111,

	// Glyph: HUMANIZE
	B00000000,
	B10100000,
	B10100000,
	B11101010,
	B10101010,
	B10101110,

	// Glyph: OCTAVE
	B00000000,
	B11100000,
	B10100000,
	B10101110,
	B10101000,
	B11101110,

	// Glyph: SHIFT RECORDING POSITION
	B00000000,
	B00100100,
	B01100110,
	B11100111,
	B01100110,
	B00100100,

	// Glyph: QUANTIZE-RESET
	B00000000,
	B11100111,
	B10100101,
	B10100110,
	B10100101,
	B11110101,

	// Glyph: QUANTIZE
	B00000000,
	B11100000,
	B10100000,
	B10100101,
	B10100101,
	B11110111,

	// Glyph: REPEAT
	B00000000,
	B00101110,
	B00101001,
	B00101110,
	B01101001,
	B01101001,

	// Glyph: REPEAT-SWEEP
	B00000000,
	B11100111,
	B10010100,
	B11100111,
	B10010001,
	B10010111,

	// Glyph: SEQ-SIZE
	B00000000,
	B11101110,
	B10000010,
	B01000100,
	B00101000,
	B11101110,

	// Glyph: SWING AMOUNT
	B00000000,
	B11100110,
	B10001001,
	B01001111,
	B00101001,
	B11101001,

	// Glyph: SWING GRANULARITY
	B00000000,
	B11101111,
	B10001000,
	B01001011,
	B00101001,
	B11101111,

	// Glyph: SWITCH RECORDING-SEQUENCE
	B00000000,
	B11101110,
	B10101000,
	B11000100,
	B10100010,
	B10101110,

	// Glyph: TEMPO
	B00000000,
	B01010001,
	B01011011,
	B01010101,
	B11010001,
	B11010001,

	// Glyph: TRACK
	B00000000,
	B11100000,
	B01000000,
	B01001110,
	B01001000,
	B01001000,

	// Glyph: UPPER CHAN BITS
	B00000000,
	B10101110,
	B10101010,
	B10101000,
	B10101010,
	B11101110,

	// Glyph: VELO
	B00000000,
	B10010000,
	B10010000,
	B01010000,
	B00110000,
	B00010000

};

// Number multiglyph: holds all numbers (0-9)
const byte NUMBER_GLYPHS[] PROGMEM = {
	B11100000, // 0
	B10100000,
	B10100000,
	B10100000,
	B10100000,
	B11100000,

	B10000000, // 1
	B10000000,
	B10000000,
	B10000000,
	B10000000,
	B10000000,

	B11100000, // 2
	B10100000,
	B00100000,
	B01000000,
	B10000000,
	B11100000,

	B11100000, // 3
	B00100000,
	B11100000,
	B00100000,
	B00100000,
	B11100000,

	B10100000, // 4
	B10100000,
	B11100000,
	B00100000,
	B00100000,
	B00100000,

	B11100000, // 5
	B10000000,
	B11100000,
	B00100000,
	B10100000,
	B11100000,

	B11100000, // 6
	B10000000,
	B11100000,
	B10100000,
	B10100000,
	B11100000,

	B11100000, // 7
	B00100000,
	B01000000,
	B01000000,
	B10000000,
	B10000000,

	B11100000, // 8
	B10100000,
	B11100000,
	B10100000,
	B10100000,
	B11100000,

	B11100000, // 9
	B10100000,
	B11100000,
	B00100000,
	B10100000,
	B11100000

};
