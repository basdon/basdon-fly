-- kneeboard hide/show is added at bit 4, so it comes right after hide/show gps setting
UPDATE usr SET prefs = ((prefs & 0xF8) << 1) | 8 | (prefs & 0x7);
