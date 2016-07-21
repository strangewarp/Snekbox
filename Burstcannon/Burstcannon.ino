
/*

		Burstcannon is a MIDI burst-generator for the "Snekbox" hardware.
		THIS CODE IS UNDER DEVELOPMENT AND DOESN'T DO ANYTHING!
		Copyright (C) 2016-onward, Christian D. Madsen (sevenplagues@gmail.com).

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License along
		with this program; if not, write to the Free Software Foundation, Inc.,
		51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
		
*/

// Digital-pin keypad library
#include <Key.h>
#include <Keypad.h>

// MAX7219/MAX7221 LED-matrix library
#include <LedControl.h>

// EEPROM data-storage library
#include <EEPROM.h>

// Initialize the object that controls the MAX7221's LED-grid
LedControl lc = LedControl(2, 3, 4, 1);

// Initialize the object that controls the Keypad buttons.
const byte ROWS PROGMEM = 5;
const byte COLS PROGMEM = 9;
char KEYS[ROWS][COLS] = {
	{'0', '1', '2', '3', '4', '5', '6', '7'},
	{'8', '9', ':', ';', '<', '=', '>', '?'},
	{'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G'},
	{'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O'}
};
byte rowpins[ROWS] = {5, 6, 7, 8};
byte colpins[COLS] = {9, 10, 14, 15, 16, 17, 18, 19};
Keypad kpd(makeKeymap(KEYS), rowpins, colpins, ROWS, COLS);

// Versioning vars
const char FIRMWAREVERSION[17] PROGMEM = "BS_BURSTCN_00003"; // Program name, plus EEPROM variant number, to be matched with the EEPROM contents

// LED GUI vars
byte ROWUPDATE = 255; // Track which rows of LEDs to update on a given iteration of the main loop. 1 = row 0; 2 = row 1; 4 = row 2; ... 128 = row 7
boolean BLINKING = false; // Track whether the rows that light up during a burst are lit

// General GUI vars
boolean OCTAVE = false; // Toggle whether an octave-up is applied to burst-notes
boolean INVERT = false; // Toggle whether burst-note pitches are inverted within a given octave
boolean MONOMODE = true; // Toggle whether outgoing bursts will be monophonic or polyphonic
boolean HUMANIZE = false; // Toggle whether velocity will be humanized or not
word DINTERVAL = 0; // Stores flags for density-interval intersection points that correspond to the right 4x4 button section

// Burst vars
byte CHAN = 0; // Channel on which burst-notes are sent
byte PITCH = 40; // Base-pitch for burst-notes
byte VELOCITY = 127; // Base burst-note velocity

// Sequencing vars
boolean PLAYING = false; // Controls whether the MIDI bursts must adhere to external MIDI CLOCK signals
byte CURTICK = 0; // Current global tick
byte CONTINUETICK = 0; // Stores the previous MIDI CLOCK-controlled CURTICK value, for correct response to MIDI CONTINUE commands
unsigned long ABSOLUTETIME = 0; // Current absolute time, in microseconds; will wrap around after reaching the maximum unsigned-long value
word ELAPSEDTIME = 0; // Currently elapsed time since the last time-check
const word BURSTDELAY PROGMEM = 20000; // Delay between burst-notes when no MIDI CLOCK is being received

// MIDI-CATCH vars
boolean SYSIGNORE = false; // Flags whether an incoming SYSEX command is currently being ignored

// MIDI-OUT SUSTAIN vars
// Format: SUSTAIN = {channel, pitch, remaining duration in ticks}
// Note: "255" in byte 0 means "empty"
byte SUSTAIN[4] = {255, 0, 0};


// Update every ledControl LED-row that's flagged for an update
void updateLEDs() {
	for (byte i = 0; i < 8; i++) { // For every LED row...
		byte bval = 1 << i; // Get the row's corresponding bitwise value
		if ((ROWUPDATE & bval) > 0) { // If the row is flagged for a GUI update...
			if (i == 0) { // If this is the first row...
				lc.setRow(0, i, (CHAN << 4) | (1 << (3 - byte(floor(CURTICK / 24))))); // Display the MIDI CHANNEL value, and the position in the current beat
			} else if (i == 1) { // If this is the second row...
				lc.setRow(0, i, (PITCH + ((OCTAVE && ((PITCH + 12) <= 127)) ? 12 : 0)) | (MONOMODE ? 128 : 0)); // Display the pitch-value, offset by the octave-value if applicable, plus the monophonic-mode toggle in the leftmost LED
			} else if (i == 2) { // If this is the third row...
				lc.setRow(0, i, VELOCITY | (HUMANIZE ? 128 : 0)); // Display the velocity-value, plus the velocity-humanize toggle in the leftmost LED
			} else { // Else, if this is a blink-display row...
        byte offset = (round(ABSOLUTETIME / 30000) + random(0, (round((ABSOLUTETIME % 100000) / 23000) + 1)) + i) % 8; // Get a semirandom offset that mostly reflects the current time
        byte dispval = (DINTERVAL & 255) | (DINTERVAL >> 8); // Get a display-value that shows a combination of the density-interval groups
        byte composite = (dispval << offset) | (dispval >> (8 - offset)); // Get a composite of two offsets of the display-value, to fill the entire row
				lc.setRow(0, i, BLINKING ? composite : 0); // If a blink is active, display the cool pseudorandom value generated by the previous several lines
			}
			ROWUPDATE ^= bval; // Unset this row's ROWUPDATE flag
		}
	}
}

// Parse any incoming keystrokes in the Keypad grid
void parseKeystrokes() {
	if (kpd.getKeys()) { // If any keys are pressed...
		for (byte i = 0; i < 10; i++) { // For every keypress slot...
			if (kpd.key[i].stateChanged) { // If the key's state has just changed...
				byte keynum = byte(kpd.key[i].kchar) - 48; // Convert the key's unique ASCII character into a number that reflects its position
				byte kcol = keynum & 7; // Get the key's column
				byte krow = keynum >> 3; // Get the key's row
				if (kpd.key[i].kstate == PRESSED) { // If the button is pressed...
					assignCommandAction(kcol, krow); // Interpret the keystroke
				} else if (kpd.key[i].kstate == RELEASED) { // Else, if the button is released...
					unassignCommandAction(kcol, krow); // Interpret the key-release
				}
			}
		}
	}
}

// Interpret an incoming keystroke, using a given button's row and column
void assignCommandAction(byte col, byte row) {
	if (col <= 3) { // If this keystroke was in the left half of the button-grid...
		if (row == 0) { // If this keystroke was on the first row...
			CHAN ^= 1 << (3 - col); // Toggle the bit in the channel-value that corresponds to the column's bitwise value
			EEPROM.write(16, CHAN); // Write the new channel value to EEPROM memory, for storage across shutdowns
		} else if (row == 3) { // Else, if this keystroke was on the fourth row...
      VELOCITY &= 120; // Remove all sub-8 bits from the velocity value
			VELOCITY ^= 8 << (3 - col); // Toggle the bit in the velocity-value that corresponds to the column's bitwise value, left-shifted by 3 to (64-32-16-8)
      VELOCITY |= (VELOCITY == 120) ? 7 : ((VELOCITY == 0) ? 1 : 0); // Set the velocity to 127 or 1 if it is filled or emptied
      EEPROM.write(18, VELOCITY); // Write the new velocity value to EEPROM memory, for storage across shutdowns
		} else { // Else, if this keystroke was in one of the middle rows...
			if ((row == 1) && (col == 0)) { // If this was the first button of the second row...
        int compare; // This will hold the interval plus the current PITCH value
        do { // Get an acceptable pitch-shift value...
          compare = PITCH + (random(2, 6) * (INVERT ? -1 : 1)); // Add a likely-consonant offset interval to an int-converted duplicate of the PITCH value
        } while ((compare > 127) || (compare < 0)); // ...Keep getting random values until one doesn't wrap around
        PITCH = compare; // Put the acceptable interval-plus-pitch value back into the global PITCH variable
			} else { // Else, if this was any other button...
				PITCH ^= max(1, (2 - row) << 4) << (3 - col); // Modify the PITCH value by a binary value corresponding to a button in the middle two rows
			}
      EEPROM.write(17, PITCH); // Write the new pitch value to EEPROM memory, for storage across shutdowns
		}
		ROWUPDATE |= 1 << (row - (row >> 1)); // Flag the button-row's corresponding LED-row for a GUI update
	} else { // Else, if the keystroke was in the right half of the button-grid...
    if (row == 3) { // If this keystroke was in the bottom row...
      if (col == 4) { // If this keystroke was on the octave-up key...
        OCTAVE = true; // Set the octave-toggle to true (will be unset on key-up)
        ROWUPDATE |= B00000010; // Flag the LED pitch-row for a GUI update
      } else if (col == 5) { // Else, if this keystroke was on the invert-interval key...
        INVERT = true; // Set the invert-interval-toggle to true (will be unset on key-up)
        ROWUPDATE |= B00000010; // Flag the LED pitch-row for a GUI update
      } else if (col == 6) { // Else, if this keystroke was on the monophonic/polyphonic mode toggle key...
        MONOMODE = !MONOMODE; // Toggle or untoggle monophonic-mode
        EEPROM.write(19, MONOMODE ? 1 : 0); // Write the new monophonic-mode value to EEPROM memory, for storage across shutdowns
        ROWUPDATE |= B00000010; // Flag the LED pitch-row for a GUI update
      } else { // Else, if this keystroke was on the humanize-toggle key...
        HUMANIZE = !HUMANIZE; // Toggle or untoggle velocity-humanize mode
        EEPROM.write(20, HUMANIZE ? 1 : 0); // Write the new humanize-toggle value to EEPROM memory, for storage across shutdowns
        ROWUPDATE |= B00000100; // Flag the LED velocity-row for a GUI update
      }
    } else { // Else, if this keystroke was in the top three rows...
      DINTERVAL |= (1 << ((2 * (2 - row)) | (col & 1))) << ((col & 2) << 2); // Flag the button's corresponding density-interval coordinates
      ROWUPDATE |= B11111000; // Flag the burst-activity LED-rows for a GUI update
    }
	}
  randomSeed(CHAN + PITCH + VELOCITY); // Set a random-seed based on current internal values
}

// Interpret a key-release according to whatever command-mode is active
void unassignCommandAction(byte col, byte row) {
	if (col >= 4) { // If this key-release was in the right half of the button-grid...
    if (row == 3) { // If this key-release was in the bottom row...
      if (col == 4) { // If this was in the fourth-from-left column...
        OCTAVE = false; // Unset the octave-toggle
        ROWUPDATE |= B00000010; // Flag the LED pitch-row for a GUI update
      } else if (col == 5) { // Else, if this was in the fifth-from-left column...
        INVERT = false; // Unset the invert-interval toggle
        ROWUPDATE |= B00000010; // Flag the LED pitch-row for a GUI update
      }
    } else { // Else, if this key-release was in any of the three upper rows...
      DINTERVAL ^= (1 << ((2 * (2 - row)) | (col & 1))) << ((col & 2) << 2); // Flag the button's corresponding density-interval coordinates
      ROWUPDATE |= B11111000; // Flag the burst-activity LED-rows for a GUI update
    }
	}
}

// Play a note based on currently-active burst commands, if they correspond to the current global MIDI CLOCK tick
void playBurst() {

  byte interv[9]; // Will hold all active intervals that are being applied to this tick
  byte dur; // Will hold the duration value for the outgoing burst-note(s)
  byte pressed = 0; // Number of interval-types, represented by buttons, that are currently being held down

  for (byte i = 0; i < 8; i++) { // For every interval entry in the DENSITYINTERVAL array...
    byte ibin = 1 << i; // Get the bitwise position of this entry
    boolean pr6 = (DINTERVAL & ibin) > 0; // Check whether a 1/6-density button is pressed for this interval
    boolean pr3 = ((DINTERVAL >> 8) & ibin) > 0; // Check whether a 1/3-density button is pressed for this interval
    byte pr1 = pr6 && pr3; // Check whether both buttons are pressed for this interval, in which case it applies to 1/1 ticks
    if ( // If...
      (pr3 && ((CURTICK % 3) == 0)) // This interval's 1/3-density key is pressed, and the global tick matches it...
      || (pr6 && ((CURTICK % 6) == 0)) // Or this interval's 1/6-density key is pressed, and the global tick matches it...
      || pr1 // Or both the 1/6-density and 1/3-density keys are pressed for this interval...
    ) { // Then...
      interv[pressed] = i; // Remember that the current interval should be applied to this tick
      byte newdur = pr1 ? 1 : (pr6 ? 6 : 3); // Get the duration for this interval's keypress
      dur = (dur == 0) ? newdur : min(dur, newdur); // Accumulate the smallest active duration from all current keypresses
      pressed++; // Increase the "pressed" value to track the number of interval-types that are being applied to this tick
    }
  }

  if (pressed == 0) { // If no bursts are active...
    BLINKING = false; // Unset the flag that controls whether to illuminate burst-display LEDs
    return; // Exit the function
  } else { // Else, if one or more bursts are active...
    BLINKING = true; // Set the flag that controls whether to illuminate burst-display LEDs
  }

  byte addoctave = OCTAVE ? 12 : 0; // Get a value to add to any outgoing pitch, based on whether an octave-value is active

  if (MONOMODE) { // If we're in monophonic-output mode...
    byte outpitch = PITCH + addoctave; // Get a pitch-value to modify, optionally raised by an octave if OCTAVE is active
    byte outvelo = min(127, max(1, VELOCITY - (HUMANIZE ? random(0, (VELOCITY >> 2) + 1) : 0))); // Get a velocity value, with a humanized value if HUMANIZE is enabled, bounded to the MIDI velocity limits
    for (byte i = 0; i < pressed; i++) { // For every currently-pressed interval type...
      outpitch += interv[i] * (INVERT ? -1 : 1); // Apply the interval to the outgoing pitch, with an inverted pitch-direction applied if INVERT is active
    }
    while (((outpitch > 127) && (outpitch <= 191)) || (outpitch > 192)) { // While the pitch is outside of its correct MIDI-pitch-value boundaries...
      outpitch += 12 * min(1, max(-1, 127 - outpitch)); // Reel the pitch back in toward the 0-127 range, in increments of 12 (as in: an octave)
    }
    sendNoteOn(CHAN, outpitch, outvelo, 1); // Send a note on the currently-active MIDI channel, with the outgoing pitch
  } else { // Else, if we're in polyphonic-output mode...
    for (byte i = 0; i < pressed; i++) { // For every interval-type that has been pressed...
      byte outpitch = min(127, max(0, PITCH + interv[i] + addoctave)); // Create an output-pitch based on all applicable pitch-offset values, bounded to the minimum and maximum MIDI pitch values
      byte outvelo = min(127, max(1, VELOCITY - (HUMANIZE ? random(0, (VELOCITY >> 2) + 1) : 0))); // Get a velocity value, with a humanized value if HUMANIZE is enabled, bounded to the MIDI velocity limits
      sendNoteOn(CHAN, outpitch, outvelo, 1); // Send a note on the currently-active MIDI channel, with the outgoing pitch
    }
  }

  ROWUPDATE |= B11111000; // Flag the burst-activity LED-rows for a GUI update

}

// Increment the timer that's used for blinking LEDs
void incrementTimer() {
  unsigned long micr = micros(); // Get the current microsecond-timer value
  if (micr < ABSOLUTETIME) { // If the micros-value has wrapped around its finite counting-space to be less than last loop's absolute-time position...
    ELAPSEDTIME = (4294967295 - ABSOLUTETIME) + micr; // Put the wrapped-around microseconds into the elapsed-time value
  } else if (micr > ABSOLUTETIME) { // Else, if the micros-value is greater than last loop's absolute-time position...
    ELAPSEDTIME += micr - ABSOLUTETIME; // Add the difference between the current time and the previous time to the elapsed-time value
  }
  ABSOLUTETIME = micr; // Set the absolute-time to the current time-value
  if (ELAPSEDTIME >= BURSTDELAY) { // If the elapsed-time has reached the burst-delay length...
    ELAPSEDTIME -= BURSTDELAY; // Subtract the burst-delay length from the elapsed-time variable
    if (!PLAYING) { // If burst timing is running off this internal timer, not an external MIDI CLOCK...
      CURTICK = (CURTICK + 1) % 96; // Increment the global tick var, bounded to the number of ticks in a beat
      ROWUPDATE |= B00000001; // Flag the chan-and-tick LED-row for updating
      playBurst(); // Play whatever burst-notes are currently active
    }
  }
}

// Increment the internal sustain-value
void incrementSustain() {
  if (SUSTAIN[0] == 255) { return; } // If the sustain is empty, do nothing
  if (SUSTAIN[2] == 0) { // If the sustain's duration is over...
    sendNoteOff(SUSTAIN[0], SUSTAIN[1]); // Call the note-off function, which will also clear the current sustain's contents
  } else { // Else, if the sustain's duration is still ongoing...
    SUSTAIN[2]--; // Decrease the sustain's remaining duration by 1 tick
  }
}

// Add a sustain-note to the internal sustain-tracking system
void addSustain(byte chan, byte pitch, byte dur) {
  if (SUSTAIN[0] <= 15) { // If the sustain-system is already tracking a note...
    sendNoteOff(SUSTAIN[0], SUSTAIN[1]); // Call the note-off function, which will also clear the current sustain's contents
  }
  SUSTAIN[0] = chan; // Put the given note-data into the sustain-note array
  SUSTAIN[1] = pitch;
  SUSTAIN[2] = dur;
}

// Remove the current sustain value, if it corresponds to the given channel and pitch values
void removeSustain(byte chan, byte pitch) {
  if (SUSTAIN[0] == 255) { return; } // If the sustain is empty, do nothing
  if ((SUSTAIN[0] == chan) && (SUSTAIN[1] == pitch)) { // If the currently-active sustain matches the given note values...
    SUSTAIN[0] = 255; // Empty out the sustain-note array
    SUSTAIN[1] = 0;
    SUSTAIN[2] = 0;
  }
}

// Send a note-off for, and then clear, whatever note is currently being sustained
void clearSustain() {
  if (SUSTAIN[0] <= 15) { // If the sustain-array is tracking a sustain...
    sendNoteOff(SUSTAIN[1], SUSTAIN[2]); // Send a note-off for that sustain, removing its sustain-entry in the process
  }
}

// Play a given MIDI NOTE-ON, start tracking its sustain status, and send its data to the MIDI-OUT TX line
void sendNoteOn(byte chan, byte pitch, byte velo, byte dur) {
  addSustain(chan, pitch, dur); // Add a sustain for the note
  sendMidi(144 + chan, pitch, velo); // Send the NOTE-ON command onward to the TX MIDI-OUT line
}

// Play a given NOTE-OFF, remove its corresponding sustain-command (if applicable), and send its data to the MIDI-OUT TX line
void sendNoteOff(byte chan, byte pitch) {
  removeSustain(chan, pitch); // Remove the note's corresponding sustain, if one exists
  sendMidi(128 + chan, pitch, 127); // Send the NOTE-OFF command onward to the TX MIDI-OUT line
}

// Send a 3-byte MIDI command on the TX MIDI-OUT line
void sendMidi(byte b1, byte b2, byte b3) {
  Serial.write(b1); // Send the command-byte
  Serial.write(b2); // Send the pitch-byte / second data-byte
  Serial.write(b3); // Send the velocity-byte / third data-byte
}

// Setup function: called exactly once on launch, after the initial global variable declarations but before the main loop.
void setup() {

	// Check EEPROM for a firmware match, and either overwrite it or set local variables to stored variables
	boolean match = true; // Set the match-flag to true by default
	for (int i = 15; i >= 0; i--) { // For every byte of the program's name in EEPROM...
		char nchar = char(EEPROM.read(i)); // Get that byte, translated into a char
		if (nchar != FIRMWAREVERSION[i]) { // If the char from EEPROM doesn't match the corresponding char in the current firmware-name...
			for (byte j = 0; j < 16; j++) { // For the 16-char-wide program-name space in the EEPROM...
				EEPROM.write(j, FIRMWAREVERSION[j]); // Fill it with the program's name
			}
			EEPROM.write(16, 0); // Write default MIDI-channel value
      EEPROM.write(17, 40); // Write default MIDI-note value
      EEPROM.write(18, 127); // Write default MIDI-velocity value
      EEPROM.write(19, 1); // Write default monophonic-mode value
      EEPROM.write(20, 0); // Write default humanize-toggle value
			match = false; // Since the EEPROM and the current firmware-name didn't match, set the match-flag to false
			break; // As a match has been disproven, there is no need to check any further, so break from the outer for loop
		}
	}
	if (match) { // If the program-name in EEPROM was found to match the current firmware-name...
		CHAN = EEPROM.read(16); // Set MIDI-CHANNEL to its EEPROM-stored value
    PITCH = EEPROM.read(17); // Set MIDI PITCH to its EEPROM-stored value
    VELOCITY = EEPROM.read(18); // Set MIDI VELOCITY to its EEPROM-stored value
    MONOMODE = EEPROM.read(19) == 1; // Set monophonic-mode to the boolean conversion of its EEPROM-stored value
    HUMANIZE = EEPROM.read(20) == 1; // Set the humanize-toggle to the boolean conversion of its EEPROM-stored value
	}

	// Tune the Keypad settings
	kpd.setDebounceTime(1); // Set an extremely minimal keypad-debounce time, since we're using clicky buttons
	kpd.setHoldTime(50); // Set no gap between a keypress' PRESS and HOLD states

	// Initialize the ledControl system
	lc.shutdown(0, false); // Turn on the ledControl object, using an inverse shutdown command (weird!)
	lc.setIntensity(0, 15); // Set the ledControl brightness to maximum intensity

	// Start sending/receiving serial data
	Serial.begin(31250); // Run the sketch at a bitrate of 31250, which is what MIDI-over-TX/RX requires
	
}

// Loop function: called continuously, starting immediately after the only call to the setup() function.
void loop() {

	parseKeystrokes(); // Check for any incoming keystrokes

  incrementTimer(); // Increment the internal timer, and play burst-notes if external MIDI CLOCK signals aren't being sent

	// Parse all incoming MIDI bytes
	while (Serial.available() > 0) { // While new MIDI bytes are available to read from the MIDI-IN port...

		byte b = Serial.read(); // Get the frontmost incoming byte

		if (SYSIGNORE) { // If we are currently ignoring a SYSEX command...
			Serial.write(b); // Send the current byte to MIDI-OUT immediately, without processing it
			if (b == 247) { // If this was an END SYSEX byte...
				SYSIGNORE = false; // Stop ignoring incoming bytes
			}
		} else { // Else, if we aren't currently ignoring a SYSEX command...
			if (b == 252) { // If this is a STOP command...
				Serial.write(b); // Send it onward to MIDI-OUT
				PLAYING = false; // Set the PLAYING var to false, to stop acting on any MIDI CLOCKs that might still be received
        BLINKING = false; // Unset the flag that controls whether to illuminate burst-display LEDs
        CONTINUETICK = CURTICK; // Set the continue-tick to the current tick-value, as internal routines will continue changing CURTICK while incoming MIDI CLOCK is unavailable
				clearSustain(); // Halt the currently-sustained MIDI NOTE
        ABSOLUTETIME = micros(); // Update the current absolute-time value, for MIDI CLOCK-less timing measurements
			} else if (b == 251) { // If this is a CONTINUE command...
				Serial.write(b); // Send it onward to MIDI-OUT
				PLAYING = true; // Set the PLAYING var to true, to begin acting on any incoming MIDI CLOCK commands
        CURTICK = CONTINUETICK; // Set the current tick to the stored continue-tick value
			} else if (b == 250) { // If this is a START command...
				Serial.write(b); // Send it onward to MIDI-OUT
				PLAYING = true; // Set the PLAYING var to true, to begin acting on any incoming MIDI CLOCK commands
        CURTICK = 95; // Set the current tick to just before the start-point
        ROWUPDATE |= B00000001; // Flag the chan-and-tick LED-row for updating
			} else if (b == 248) { // If this is a TIMING CLOCK command...
				Serial.write(b); // Send it onward to MIDI-OUT
        CURTICK = (CURTICK + 1) % 96; // Increment the global tick-counter, bounded by the number of ticks in a beat
        ROWUPDATE |= B00000001; // Flag the chan-and-tick LED-row for updating
        incrementSustain(); // Increment the currently-sustained note
        playBurst(); // Play notes based on whatever burst-keys are being held
			} else if (b == 247) { // If this is an END SYSEX MESSAGE command...
				// If you're dealing with an END-SYSEX command while SYSIGNORE is inactive,
				// then that implies you've received either an incomplete or corrupt command,
				// and so nothing should be done here aside from sending it onward.
				Serial.write(b);
			} else if ( // If the byte represents a...
				(b == 244) // MISC SYSEX command,
				|| (b == 240) // or a START SYSEX MESSAGE command,
			) { // then...
				Serial.write(b); // Send it onward to MIDI-OUT
				SYSIGNORE = true; // Begin ignoring an incoming arbitrarily-sized SYSEX message
      } else { // Else, if this is any other byte...
        Serial.write(b); // Send it onward to MIDI-OUT
      }
		}

	}

	updateLEDs(); // Update whatever rows of LEDs have been flagged for GUI-updates

}
