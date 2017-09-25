
// Display the startup-animation
void startupAnimation() {
	for (byte i = 0; i < 5; i++) {
		for (byte row = 0; row < 8; row++) {
			lc.setRow(0, row, (pgm_read_dword_near(LOGO + row) >> ((i == 4) ? 16 : (8 * (3 - i))) & 255));
		}
		delay(150);
		lc.clearDisplay(0);
		delay(35);
	}
	for (byte row = 0; row < 8; row++) {
		lc.setRow(0, row, 255);
	}
	delay(210);
	lc.clearDisplay(0);
	delay(40);
}

// Get the LED-values for a given GUI row within the lower 6 rows (SCATTER/SEQ rows)
byte getRowVals(byte r) {
	byte ib = (PAGE * 24) + (r << 2); // Get the row's contents' base array positions
	byte ib2 = ib + 1;
	byte ib3 = ib2 + 1;
	byte ib4 = ib3 + 1;
	return ((SCATTER[ib] & 7) ? 128 : 0) // Return all the row's SCATTER and PLAYING info as a row's worth of bits
		| ((SCATTER[ib2] & 7) ? 64 : 0) // ^
		| ((SCATTER[ib3] & 7) ? 32 : 0) // ^
		| ((SCATTER[ib4] & 7) ? 16 : 0) // ^
		| ((STATS[ib] & 128) >> 4) // ^
		| ((STATS[ib2] & 128) >> 5) // ^
		| ((STATS[ib3] & 128) >> 6) // ^
		| ((STATS[ib4] & 128) >> 7); // ^
}

// Update the GUI based on update-flags that have been set by the current tick's events
void updateGUI() {

	// Note: The top two LED-rows are the same in both RECORD and PLAY modes

	byte ctrl = BUTTONS & B00111111; // Get the control-row buttons' activity

	if (TO_UPDATE & 1) { // If the first row is flagged for a GUI update...
		if (RECORDMODE && ctrl) { // If this is RECORD-MODE, and any ctrl-buttons are held...
			if (ctrl == B00100000) { // If TOGGLE NOTE-RECORDING is pressed...
				lc.setRow(0, 0, RECORDNOTES * 255); // Light up row if notes are being recorded
			} else if (ctrl == B00010000) { // Else, if BASENOTE is pressed...
				lc.setRow(0, 0, BASENOTE); // Display BASENOTE value
			} else if (ctrl == B00001000) { // Else, if OCTAVE is pressed...
				lc.setRow(0, 0, OCTAVE); // Display OCTAVE value
			} else if (ctrl == B00000100) { // Else, if DURATION is pressed...
				lc.setRow(0, 0, DURATION); // Display DURATION value
			} else if (ctrl == B00101000) { // Else, if CLOCK-MASTER is pressed...
				lc.setRow(0, 0, CLOCKMASTER * 255); // Light up row if CLOCKMASTER mode is active
			} else if (ctrl == B00100100) { // Else, if CHAN-LISTEN is pressed...
				lc.setRow(0, 0, LISTEN); // Display the CHAN-LISTEN value
			} else if (ctrl == B00100010) { // Else, if CONTROL-CHANGE is pressed...
				lc.setRow(0, 0, (CHAN >> 4) * 255); // Light up row if the CONTROL-CHANGE flag is active
			} else if (ctrl == B00011000) { // Else, if VELO is pressed...
				lc.setRow(0, 0, VELO); // Display VELO value
			} else if (ctrl == B00010100) { // Else, if HUMANIZE is pressed...
				lc.setRow(0, 0, HUMANIZE); // Display HUMANIZE value
			} else if (ctrl == B00001100) { // Else, if QUANTIZE is pressed...
				lc.setRow(0, 0, QUANTIZE); // Display QUANTIZE value
			} else if (ctrl == B00010010) { // Else, if CHAN is pressed...
				lc.setRow(0, 0, CHAN); // Display CHAN value
			} else if (ctrl == B00001010) { // Else, if SEQ-SIZE is pressed...
				lc.setRow(0, 0, STATS[RECORDSEQ] & 127); // Display current record-seq's SIZE value
			} else if (ctrl == B00000110) { // Else, if BPM is pressed...
				lc.setRow(0, 0, BPM); // Display BPM value
			} else if (ctrl == B00110011) { // Else, if GLOBAL PLAY/STOP is pressed...
				lc.setRow(0, 0, PLAYING * 255); // Light up row if the PLAYING flag is active
			} else { // Else...
				// B00000010: If SWITCH RECORDING-SEQUENCE is held...
				// B00110000: Or LOAD is held...
				// Or some other unassigned button-combination is held...
				lc.setRow(0, 0, 0); // Clear this row
			}
		} else { // Else, if this isn't RECORD-MODE, OR no ctrl-buttons are held...
			lc.setRow(0, 0, 128 >> ((CUR16 >> 4) - 1)); // Display the global-tick position
		}
	}

	if (TO_UPDATE & 2) { // If the second row is flagged for a GUI update...
		// Display the current number of sustains (0-8), leaving a bar of LEDs lit for values >1
		lc.setRow(0, 1, ~(255 >> SUST_COUNT));
	}

	if (TO_UPDATE & 252) { // If any of the bottom 6 rows are flagged for a GUI update...

		if (RECORDMODE) { // If RECORD MODE is active...

			if (TO_UPDATE & 4) { // If the third row is flagged for an update...
				// If a CTRL-button is held, clear the row; else get this row's SEQ and SCATTER values
				lc.setRow(0, 2, ctrl ? 0 : getRowVals(0));
			}

			for (byte i = 0; i < 5; i++) { // For each of the bottom 5 GUI rows...

				// If the row is not flagged for an update, continue to the next row
				if ((~TO_UPDATE) & (8 << i)) { continue; }

				// Holds the LED-row's contents, which will be assembled based on which commands are held
				byte row = 0;

				if (!ctrl) { // If no command-buttons are held...
					row = getRowVals(i + 1); // Get the row's standard SEQ and SCATTER values
				} else if (ctrl == B00100000) { // If TOGGLE NOTE-RECORDING is held...
					// Grab a section of the RECORDING-glyph for display, or its ARMED variant, if notes are being recorded
					row = pgm_read_byte_near((RECORDNOTES ? GLYPH_RECORDING_ARMED : GLYPH_RECORDING) + i);
				} else if (ctrl == B00010000) { // If BASE-NOTE command is held...
					row = pgm_read_byte_near(GLYPH_BASENOTE + i); // Grab a section of the BASENOTE glyph for display
				} else if (ctrl == B00001000) { // If OCTAVE command is held...
					row = pgm_read_byte_near(GLYPH_OCTAVE + i); // Grab a section of the OCTAVE glyph for display
				} else if (ctrl == B00000100) { // If DURATION command is held...
					row = pgm_read_byte_near(GLYPH_DURATION + i); // Grab a section of the DURATION glyph for display
				} else if (ctrl == B00000010) { // If SWITCH-SEQ command is held...
					row = pgm_read_byte_near(GLYPH_RSWITCH + i); // Grab a section of the SWITCH-SEQ-glyph for display
				} else if (ctrl == B00000001) { // If ERASE WHILE HELD command is held...
					row = pgm_read_byte_near(GLYPH_ERASE + i); // Grab a section of the ERASE-glyph for display
				} else if (ctrl == B00110000) { // If LOAD command is held...
					row = pgm_read_byte_near(GLYPH_LOAD + i); // Grab a section of the LOAD-glyph for display
				} else if (ctrl == B00101000) { // If CLOCK-MASTER command is held...
					row = pgm_read_byte_near(GLYPH_CLOCKMASTER + i); // Grab a section of the CLOCK-MASTER glyph for display
				} else if (ctrl == B00100100) { // If CHAN-LISTEN command is held...
					row = pgm_read_byte_near(GLYPH_LISTEN + i); // Grab a section of the CHAN-LISTEN glyph for display
				} else if (ctrl == B00100010) { // If CONTROL-CHANGE command is held...
					row = pgm_read_byte_near(GLYPH_CONTROLCHANGE + i); // Grab a section of the CONTROL-CHANGE glyph for display
				} else if (ctrl == B00011000) { // If VELO command is held...
					row = pgm_read_byte_near(GLYPH_VELO + i); // Grab a section of the VELO glyph for display
				} else if (ctrl == B00010100) { // If HUMANIZE command is held...
					row = pgm_read_byte_near(GLYPH_HUMANIZE + i); // Grab a section of the HUMANIZE glyph for display
				} else if (ctrl == B00010010) { // If CHAN command is held...
					row = pgm_read_byte_near(GLYPH_CHAN + i); // Grab a section of the CHAN glyph for display
				} else if (ctrl == B00001100) { // If QUANTIZE command is held...
					row = pgm_read_byte_near(GLYPH_QUANTIZE + i); // Grab a section of the QUANTIZE glyph for display
				} else if (ctrl == B00001010) { // If SEQ-SIZE command is held...
					row = pgm_read_byte_near(GLYPH_SIZE + i); // Grab a section of the SEQ-SIZE glyph for display
				} else if (ctrl == B00000110) { // If BPM command is held...
					row = pgm_read_byte_near(GLYPH_BPM + i); // Grab a section of the BPM glyph for display
				} else if (ctrl == B00110011) { // If GLOBAL PLAY/STOP is held...
					// Grab a section of the PLAYING-glyph for display, or its ARMED version, if sequences are playing
					row = pgm_read_byte_near((PLAYING ? GLYPH_PLAYSTOP_ARMED : GLYPH_PLAYSTOP) + i);
				} else if (ctrl == B00111111) { // Else, if RECORD-MODE TOGGLE is held...
					row = pgm_read_byte_near(GLYPH_RECORDING + i); // Grab a section of the RECORDING glyph for display
				}
				// If any ctrl-buttons are held in an unknown configuration,
				// then no action is taken, leaving "row" set to 0 (all row-LEDs off).

				lc.setRow(0, i + 3, row); // Set the LED-row based on the current display-row

			}

		} else { // Else, if PLAYING MODE is actve...
			for (byte i = 2; i < 8; i++) { // For each of the bottom 6 GUI rows...
				if (!(TO_UPDATE & (1 << i))) { continue; } // If the row is not flagged for an update, continue to the next row
				lc.setRow(0, i, getRowVals(i - 2)); // Display the row of SCATTER and PLAYING info
			}
		}

	}

	TO_UPDATE = 0; // Unset the GUI-row-update flags

}
