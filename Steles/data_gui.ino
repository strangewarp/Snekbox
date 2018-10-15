

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

	// Glyph: CLOCK-MASTER
	B00000000,
	B11101000,
	B10101000,
	B10001010,
	B10101100,
	B11101010,

	// Glyph: COPY
	B00000000,
	B11100000,
	B10101110,
	B10001010,
	B10101010,
	B11101110,

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

	// Glyph: PASTE
	B00000000,
	B11101110,
	B10100010,
	B11101110,
	B10001010,
	B10001110,

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
