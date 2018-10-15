


const char PREFS_FILENAME[] PROGMEM = {80, 46, 68, 65, 84, 0}; // Filename of the preferences-file, in PROGMEM to save RAM

// Store pointers to RECORD-MODE functions in PROGMEM
const CmdFunc COMMANDS[] PROGMEM = {
	genericCmd,       //  0: Unused duplicate pointer to genericCmd
	armCmd,           //  1: TOGGLE RECORDNOTES
	chanCmd,          //  2: CHANNEL
	clearCmd,         //  3: CLEAR NOTES
	clockCmd,         //  4: CLOCK-MASTER
	copyCmd,          //  5: COPY
	durationCmd,      //  6: DURATION
	genericCmd,       //  7: All possible note-entry commands
	gridConfigCmd,    //  8: GRID-CONFIG
	humanizeCmd,      //  9: HUMANIZE
	octaveCmd,        // 10: OCTAVE
	pasteCmd,         // 11: PASTE
	posCmd,           // 12: SHIFT CURRENT POSITION
	qrstCmd,          // 13: QUANTIZE-RESET
	quantizeCmd,      // 14: QUANTIZE
	repeatCmd,        // 15: TOGGLE REPEAT
	rSweepCmd,        // 16: REPEAT-SWEEP
	sizeCmd,          // 17: SEQ-SIZE
	swAmtCmd,         // 18: SWING AMOUNT
	swGranCmd,        // 19: SWING GRANULARITY
	switchCmd,        // 20: SWITCH RECORDING-SEQUENCE
	tempoCmd,         // 21: BPM
	trackCmd,         // 22: TRACK
	upperBitsCmd,     // 23: UPPER CHAN BITS
	veloCmd           // 24: VELOCITY
};

// Matches control-row binary button-values to the decimal values that stand for their corresponding functions in COMMANDS
// Note: These binary values are flipped versions of what is displayed in button-key.txt
const byte KEYTAB[] PROGMEM = {
	7,  //  0, 000000:  7, genericCmd (REGULAR NOTE)
	15, //  1, 000001: 15, repeatCmd
	22, //  2, 000010: 22, trackCmd
	12, //  3, 000011: 12, posCmd
	24, //  4, 000100: 24, veloCmd
	21, //  5, 000101: 21, tempoCmd
	16, //  6, 000110: 16, rSweepCmd
	8,  //  7, 000111:  8, gridConfigCmd
	10, //  8, 001000: 10, octaveCmd
	4,  //  9, 001001:  4, clockCmd
	11, // 10, 001010: 11, pasteCmd
	0,  // 11, 001011:  0, ignore
	9,  // 12, 001100:  9, humanizeCmd
	0,  // 13, 001101:  0, ignore
	0,  // 14, 001110:  0, ignore
	3,  // 15, 001111:  3, clearCmd
	14, // 16, 010000: 14, quantizeCmd
	19, // 17, 010001: 19, swGranCmd
	13, // 18, 010010: 13, qrstCmd
	0,  // 19, 010011:  0, ignore
	5,  // 20, 010100:  5, copyCmd
	0,  // 21, 010101:  0, ignore
	0,  // 22, 010110:  0, ignore
	0,  // 23, 010111:  0, ignore
	6,  // 24, 011000:  6, durationCmd
	0,  // 25, 011001:  0, ignore
	0,  // 26, 011010:  0, ignore
	0,  // 27, 011011:  0, ignore
	0,  // 28, 011100:  0, ignore
	0,  // 29, 011101:  0, ignore
	0,  // 30, 011110:  0, ignore
	0,  // 31, 011111:  0, ignore
	1,  // 32, 100000:  1, armCmd
	20, // 33, 100001: 20, switchCmd
	18, // 34, 100010: 18, swAmtCmd
	0,  // 35, 100011:  0, ignore
	23, // 36, 100100: 23, upperBitsCmd
	0,  // 37, 100101:  0, ignore
	0,  // 38, 100110:  0, ignore
	0,  // 39, 100111:  0, ignore
	2,  // 40, 101000:  2, chanCmd
	0,  // 41, 101001:  0, ignore
	0,  // 42, 101010:  0, ignore
	0,  // 43, 101011:  0, ignore
	0,  // 44, 101100:  0, ignore
	0,  // 45, 101101:  0, ignore
	0,  // 46, 101110:  0, ignore
	0,  // 47, 101111:  0, ignore
	17, // 48, 110000: 17, sizeCmd
	0,  // 49, 110001:  0, ignore
	0,  // 50, 110010:  0, ignore
	0,  // 51, 110011:  0, ignore
	0,  // 52, 110100:  0, ignore
	0,  // 53, 110101:  0, ignore
	0,  // 54, 110110:  0, ignore
	0,  // 55, 110111:  0, ignore
	0,  // 56, 111000:  0, ignore
	0,  // 57, 111001:  0, ignore
	0,  // 58, 111010:  0, ignore
	0,  // 59, 111011:  0, ignore
	0,  // 60, 111100:  0, ignore (ERASE-WHILE-HELD is handled by other routines)
	0,  // 61, 111101:  0, ignore
	0,  // 62, 111110:  0, ignore
	0,  // 63, 111111:  0, ignore
};

// Lookup-table for all base BPM values, in microseconds-per-tick
const float MCS_TABLE[] PROGMEM = {
	19531.25,  18939.394, 18382.353, 17857.143, // 32 33 34 35
	17361.111, 16891.892, 16447.368, 16025.641, // 36 37 38 39
	15625.0,   15243.902, 14880.952, 14534.884, // 40 41 42 43
	14204.545, 13888.889, 13586.957, 13297.872, // 44 45 46 47
	13020.833, 12755.102, 12500.0,   12254.902, // 48 49 50 51
	12019.231, 11792.453, 11574.074, 11363.636, // 52 53 54 55
	11160.714, 10964.912, 10775.862, 10593.22,  // 56 57 58 59
	10416.667, 10245.902, 10080.645, 9920.635,  // 60 61 62 63
	9765.625,  9615.385,  9469.697,  9328.358,  // 64 65 66 67
	9191.176,  9057.971,  8928.571,  8802.817,  // 68 69 70 71
	8680.556,  8561.644,  8445.946,  8333.333,  // 72 73 74 75
	8223.684,  8116.883,  8012.821,  7911.392,  // 76 77 78 79
	7812.5,    7716.049,  7621.951,  7530.12,   // 80 81 82 83
	7440.476,  7352.941,  7267.442,  7183.908,  // 84 85 86 87
	7102.273,  7022.472,  6944.444,  6868.132,  // 88 89 90 91
	6793.478,  6720.43,   6648.936,  6578.947,  // 92 93 94 95
	6510.417,  6443.299,  6377.551,  6313.131,  // 96 97 98 99
	6250.0,    6188.119,  6127.451,  6067.961,  // 100 101 102 103
	6009.615,  5952.381,  5896.226,  5841.121,  // 104 105 106 107
	5787.037,  5733.945,  5681.818,  5630.631,  // 108 109 110 111
	5580.357,  5530.973,  5482.456,  5434.783,  // 112 113 114 115
	5387.931,  5341.88,   5296.61,   5252.101,  // 116 117 118 119
	5208.333,  5165.289,  5122.951,  5081.301,  // 120 121 122 123
	5040.323,  5000.0,    4960.317,  4921.26,   // 124 125 126 127
	4882.813,  4844.961,  4807.692,  4770.992,  // 128 129 130 131
	4734.848,  4699.248,  4664.179,  4629.63,   // 132 133 134 135
	4595.588,  4562.044,  4528.986,  4496.403,  // 136 137 138 139
	4464.286,  4432.624,  4401.408,  4370.629,  // 140 141 142 143
	4340.278,  4310.345,  4280.822,  4251.701,  // 144 145 146 147
	4222.973,  4194.631,  4166.667,  4139.073,  // 148 149 150 151
	4111.842,  4084.967,  4058.442,  4032.258,  // 152 153 154 155
	4006.41,   3980.892,  3955.696,  3930.818,  // 156 157 158 159
	3906.25,   3881.988,  3858.025,  3834.356,  // 160 161 162 163
	3810.976,  3787.879,  3765.06,   3742.515,  // 164 165 166 167
	3720.238,  3698.225,  3676.471,  3654.971,  // 168 169 170 171
	3633.721,  3612.717,  3591.954,  3571.429,  // 172 173 174 175
	3551.136,  3531.073,  3511.236,  3491.62,   // 176 177 178 179
	3472.222,  3453.039,  3434.066,  3415.301,  // 180 181 182 183
	3396.739,  3378.378,  3360.215,  3342.246,  // 184 185 186 187
	3324.468,  3306.878,  3289.474,  3272.251,  // 188 189 190 191
	3255.208,  3238.342,  3221.649,  3205.128,  // 192 193 194 195
	3188.776,  3172.589,  3156.566,  3140.704,  // 196 197 198 199
	3125.0                                      // 200
};

// Lookup-table for all SWING multiplication values
const float SWING_TABLE[] PROGMEM = {
	0.0,   0.003, 0.006, 0.01,  // 0 1 2 3
	0.014, 0.018, 0.023, 0.028, // 4 5 6 7
	0.034, 0.04,  0.046, 0.053, // 8 9 10 11
	0.061, 0.068, 0.076, 0.085, // 12 13 14 15
	0.094, 0.103, 0.113, 0.123, // 16 17 18 19
	0.133, 0.144, 0.156, 0.168, // 20 21 22 23
	0.18,  0.192, 0.205, 0.219, // 24 25 26 27
	0.232, 0.247, 0.261, 0.276, // 28 29 30 31
	0.292, 0.307, 0.324, 0.34,  // 32 33 34 35
	0.357, 0.375, 0.393, 0.411, // 36 37 38 39
	0.43,  0.449, 0.468, 0.488, // 40 41 42 43
	0.508, 0.529, 0.55,  0.572, // 44 45 46 47
	0.594, 0.616, 0.639, 0.662, // 48 49 50 51
	0.686, 0.71,  0.734, 0.759, // 52 53 54 55
	0.784, 0.809, 0.835, 0.862, // 56 57 58 59
	0.889, 0.916, 0.944, 0.972, // 60 61 62 63
	1.0                         // 64
};
