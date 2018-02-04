
// Get the SEQUENCE-ACTIVITY LED-values for a given GUI row within the lower 6 rows
byte getRowSeqVals(byte r) {
	byte ib = r << 2; // Get the row's left-side's global-array position
	return (STATS[ib] & 128) // Return the row's PLAYING info, from both pages, as a row's worth of bits
		| ((STATS[ib + 1] & 128) >> 1)
		| ((STATS[ib + 2] & 128) >> 2)
		| ((STATS[ib + 3] & 128) >> 3)
		| ((STATS[ib + 24] & 128) >> 4)
		| ((STATS[ib + 25] & 128) >> 5)
		| ((STATS[ib + 26] & 128) >> 6)
		| ((STATS[ib + 27] & 128) >> 7);
}

// Get the SCATTER-ACTIVITY LED-values for a given GUI row within the lower 6 rows
byte getRowScatterVals(byte r) {
	byte ib = r << 2; // Get the row's left-side's global-array position
	return ((!!(SCATTER[ib] & 15)) << 7) // Return the row's SCATTER info, from both pages, as a row's worth of bits
		| ((!!(SCATTER[ib + 1] & 15)) << 6)
		| ((!!(SCATTER[ib + 2] & 15)) << 5)
		| ((!!(SCATTER[ib + 3] & 15)) << 4)
		| ((!!(SCATTER[ib + 24] & 15)) << 3)
		| ((!!(SCATTER[ib + 25] & 15)) << 2)
		| ((!!(SCATTER[ib + 26] & 15)) << 1)
		| (!!(SCATTER[ib + 27] & 15));
}

// Display the global beat and quarter-note in the topmost row of LEDs
void displayGlobalBeat() {
	byte beat = CUR16 >> 4; // Get the current global beat
	byte quart = (CUR16 >> 2) & 3; // Get the current global quarter-note
	// Display the global beat in the 4 leftmost LEDs,
	// and the global quarter-note in the 4 rightmost LEDs,
	// with an extra disambiguation marker on beat 4
	lc.setRow(
		0,
		0,
		(beat >= 4) ?
			((~(255 >> (beat - 3))) | ((beat == 4) << 4) | ((15 << (3 - quart)) & 15))
			: ((128 >> beat) | (8 >> quart))
	);
}

// Update the first row of LEDs
void updateFirstRow(byte ctrl) {
	if (RECORDMODE) { // If this is RECORD-MODE...
		if (ctrl == B00100000) { // If ARM RECORDING is pressed...
			lc.setRow(0, 0, RECORDSEQ); // Display currently-recording sequence (in binary)
		} else if (ctrl == B00010000) { // If BASENOTE is pressed...
			lc.setRow(0, 0, BASENOTE); // Display BASENOTE value
		} else if (ctrl == B00001000) { // Else, if OCTAVE is pressed...
			lc.setRow(0, 0, OCTAVE); // Display OCTAVE value
		} else if (ctrl == B00000100) { // Else, if DURATION is pressed...
			lc.setRow(0, 0, DURATION); // Display DURATION value
		} else if (ctrl == B00000010) { // Else, if CHAN is pressed...
			lc.setRow(0, 0, CHAN); // Display CHAN value
		} else if (ctrl == B00101000) { // Else, if CLOCK-MASTER is pressed...
			lc.setRow(0, 0, CLOCKMASTER * 255); // Light up row if CLOCKMASTER mode is active
		} else if (ctrl == B00100100) { // Else, if QUANTIZE is pressed...
			lc.setRow(0, 0, QUANTIZE); // Display QUANTIZE value
		} else if (ctrl == B00100001) { // Else, if CONTROL-CHANGE is pressed...
			lc.setRow(0, 0, (CHAN >> 4) * 255); // Light up row if the CONTROL-CHANGE flag is active
		} else if (ctrl == B00011000) { // Else, if VELO is pressed...
			lc.setRow(0, 0, VELO); // Display VELO value
		} else if (ctrl == B00010100) { // Else, if HUMANIZE is pressed...
			lc.setRow(0, 0, HUMANIZE); // Display HUMANIZE value
		} else if (ctrl == B00010010) { // Else, if SEQ-SIZE is pressed...
			lc.setRow(0, 0, STATS[RECORDSEQ] & 127); // Display current record-seq's SIZE value
		} else if (ctrl == B00010001) { // Else, if BPM is pressed...
			lc.setRow(0, 0, BPM); // Display BPM value
		} else if (ctrl == B00110000) { // Else, if SWITCH RECORDING-SEQUENCE is held...
			lc.setRow(0, 0, RECORDSEQ); // Show the currently-active seq
		} else { // Else...
			// If ARM-RECORDING or INTERVAL or ERASE WHILE HELD is held...
			// Or some other unassigned button-combination is held...
			// Or no control-buttons are held...
			lc.setRow(0, 0, byte(POS[RECORDSEQ] >> 4)); // Display the RECORDSEQ's realtime beat-position, in binary
		}
	} else { // Else, if this isn't RECORD-MODE...
		displayGlobalBeat(); // Display the global-beat row
	}
}

// Update the bottom LED-rows for LOAD-MODE
void updateLoadBottomRows(byte ctrl) {
	for (byte i = 0; i < 6; i++) { // For each of the bottom 6 GUI rows...
		if (!(TO_UPDATE & (4 << i))) { continue; } // If the row is not flagged for an update, continue to the next row
		lc.setRow( // Set the LED-row based on the current display-row:
			0, // LED-grid 0 (only LED-grid used)...
			i, // LED-row corresponding to the current row...
			pgm_read_byte_near( // Get LED data from PROGMEM:
				MULTIGLYPH_NUMBERS // Get a number-glyph,
				+ (6 * ctrlToButtonIndex(ctrl)) // based on the highest pressed control-button,
				+ i // whose contents correspond to the current row
			)
		);
	}
}

// Update the bottom LED-rows for RECORD-MODE
void updateRecBottomRows(byte ctrl) {

	for (byte i = 0; i < 6; i++) { // For each of the bottom 6 GUI rows...

		if (!(TO_UPDATE & (4 << i))) { continue; } // If the row is not flagged for an update, continue to the next row

		byte row = 0; // Holds the LED-row's contents, which will be assembled based on which commands are held

		if (!ctrl) { // If no command-buttons are held...
			row = getRowSeqVals(i); // Get the row's standard SEQ values
		} else if (ctrl == B00100000) { // If ARM-RECORDING is held...
			row = RECORDNOTES ? 255 : pgm_read_byte_near(GLYPH_RECORD + i); // Display either the RECORDING-glyph or a solid block
		} else if (ctrl == B00010000) { // If BASENOTE command is held...
			row = pgm_read_byte_near(GLYPH_BASENOTE + i); // Grab a section of the BASENOTE glyph for display
		} else if (ctrl == B00001000) { // If OCTAVE command is held...
			row = pgm_read_byte_near(GLYPH_OCTAVE + i); // Grab a section of the OCTAVE glyph for display
		} else if (ctrl == B00000100) { // If DURATION command is held...
			row = pgm_read_byte_near(GLYPH_DURATION + i); // Grab a section of the DURATION glyph for display
		} else if (ctrl == B00000010) { // If CHAN command is held...
			row = pgm_read_byte_near(GLYPH_CHAN + i); // Grab a section of the CHAN glyph for display
		} else if (ctrl == B00000001) { // If REPEAT is held...
			// Grab a section of the REPEAT glyphs for display
			row = pgm_read_byte_near((REPEAT ? GLYPH_REPEAT_ARMED : GLYPH_REPEAT) + i);
		} else if (ctrl == B00000001) { // If INTERVAL-UP is held...
			row = pgm_read_byte_near(GLYPH_UP + i); // Grab a section of the UP-glyph for display
		} else if (ctrl == B00110000) { // If SWITCH-SEQ command is held...
			row = pgm_read_byte_near(GLYPH_RSWITCH + i); // Grab a section of the SWITCH-SEQ-glyph for display
		} else if (ctrl == B00101000) { // If CLOCK-MASTER command is held...
			row = pgm_read_byte_near(GLYPH_CLOCKMASTER + i); // Grab a section of the CLOCK-MASTER glyph for display
		} else if (ctrl == B00100100) { // If QUANTIZE command is held...
			row = pgm_read_byte_near(GLYPH_QUANTIZE + i); // Grab a section of the QUANTIZE glyph for display
		} else if (ctrl == B00100001) { // If CONTROL-CHANGE command is held...
			row = pgm_read_byte_near(GLYPH_CONTROLCHANGE + i); // Grab a section of the CONTROL-CHANGE glyph for display
		} else if (ctrl == B00011000) { // If VELO command is held...
			row = pgm_read_byte_near(GLYPH_VELO + i); // Grab a section of the VELO glyph for display
		} else if (ctrl == B00010100) { // If HUMANIZE command is held...
			row = pgm_read_byte_near(GLYPH_HUMANIZE + i); // Grab a section of the HUMANIZE glyph for display
		} else if (ctrl == B00010010) { // If SEQ-SIZE command is held...
			row = pgm_read_byte_near(GLYPH_SIZE + i); // Grab a section of the SEQ-SIZE glyph for display
		} else if (ctrl == B00010001) { // If BPM command is held...
			row = pgm_read_byte_near(GLYPH_BPM + i); // Grab a section of the BPM glyph for display
		} else if (ctrl == B00001111) { // If ERASE WHILE HELD command is held...
			row = pgm_read_byte_near(GLYPH_ERASE + i); // Grab a section of the ERASE-glyph for display
		} else if (ctrl == B00000111) { // If INTERVAL-DOWN-RANDOM is held...
			// Grab a composite of the DOWN-glyph and RANDOM-glyph for display
			row = pgm_read_byte_near(GLYPH_DOWN + i) | pgm_read_byte_near(GLYPH_RANDOM + i);
		} else if (ctrl == B00000101) { // If INTERVAL-UP-RANDOM is held...
			// Grab a composite of the UP-glyph and RANDOM-glyph for display
			row = pgm_read_byte_near(GLYPH_UP + i) | pgm_read_byte_near(GLYPH_RANDOM + i);
		} else if (ctrl == B00000011) { // If INTERVAL-DOWN is held...
			row = pgm_read_byte_near(GLYPH_DOWN + i); // Grab a section of the DOWN-glyph for display
		}
		// If any ctrl-buttons are held in an unknown configuration,
		// then no action is taken, leaving "row" set to 0 (all row-LEDs off).

		lc.setRow(0, i + 2, row); // Set the LED-row based on the current display-row

	}

}

// Update the bottom LED-rows for PLAY-MODE
void updatePlayBottomRows(byte ctrl) {
	byte heldsc = (ctrl & B00000011) == B00000011; // Make sure this is, indeed, a command with SCATTER shape
	for (byte i = 2; i < 8; i++) { // For each of the bottom 6 GUI rows...
		if (TO_UPDATE & (1 << i)) { // If the row is flagged for an update...
			// If a SCATTER-related command is held, display a row of SCATTER info; else display a row of SEQ info
			lc.setRow(0, i, heldsc ? getRowScatterVals(i - 2) : getRowSeqVals(i - 2));
		}
	}
}

// Update the 6 bottom rows of LEDs
void updateBottomRows(byte ctrl) {
	if (LOADMODE) { // If LOAD MODE is active...
		updateLoadBottomRows(ctrl); // Update the bottom LED-rows for LOAD-MODE
	} else if (RECORDMODE) { // If RECORD MODE is active...
		updateRecBottomRows(ctrl); // Update the bottom-rows for RECORD-MODE
	} else { // Else, if PLAYING MODE is actve...
		updatePlayBottomRows(ctrl); // Update the bottom LED-rows for PLAYING-MODE
	}
}

// Update the GUI based on update-flags that have been set by the current tick's events
void updateGUI() {

	byte ctrl = BUTTONS & B00111111; // Get the control-row buttons' activity

	if (TO_UPDATE & 1) { // If the first row is flagged for a GUI update...
		updateFirstRow(ctrl); // Update the first row of LEDs
	}

	if (TO_UPDATE & 2) { // If the second row is flagged for a GUI update...
		lc.setRow(0, 1, (256 >> SUST_COUNT) % 256); // Display the current number of sustains (0-8)
	}

	if (TO_UPDATE & 252) { // If any of the bottom 6 rows are flagged for a GUI update...
		updateBottomRows(ctrl); // Update the 6 bottom rows of LEDs
	}

	TO_UPDATE = 0; // Unset the GUI-row-update flags

}
