-- PREF_CONSTANT_WORK is now PREF_SHOW_GPS, which should be enabled by default
UPDATE usr SET prefs=prefs|4
