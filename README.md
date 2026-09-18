# PSY - ZUMBO

Windows 10/11 x64 VST3 synthesizer foundation for Dark Forest Psytrance.

## Για αρχάριο χρήστη

Δεν χρειάζεται να εγκαταστήσεις Visual Studio, CMake ή compiler στον δικό σου υπολογιστή για να κάνεις το build.

Το GitHub Actions κάνει το compilation σε Windows runner.

### 1. Δημιούργησε repository

Φτιάξε ένα νέο GitHub repository με όνομα:

`PSY-ZUMBO`

Ανέβασε όλα τα αρχεία αυτού του project.

### 2. Κάνε build

Στο GitHub:

`Actions` → `Build PSY - ZUMBO (Windows VST3)` → `Run workflow`

Μετά περίμενε να ολοκληρωθεί το build.

### 3. Κατέβασε το plugin

Άνοιξε το ολοκληρωμένο workflow run.

Στο κάτω μέρος θα βρεις:

`Artifacts`

Κατέβασε:

`PSY-ZUMBO-Windows-x64`

### 4. Εγκατάσταση

Αποσυμπίεσε το ZIP.

Αντέγραψε το:

`PSY - ZUMBO.vst3`

στο:

`C:\Program Files\Common Files\VST3\`

### 5. FL Studio

Άνοιξε το Plugin Manager και κάνε rescan των plugins.

### 6. Ableton Live

Άνοιξε:

Preferences → Plug-ins

και κάνε rescan αν χρειάζεται.

## Τι περιλαμβάνει αυτή η πρώτη build

- 4-oscillator synthesis core
- wavetable-style morphing
- granular texture engine
- filter
- amp envelope
- filter envelope foundation
- LFO
- MIDI note input
- parameter automation/state saving
- psychedelic UI
- low-CPU architecture foundation
- GitHub automated Windows x64 VST3 build

## Επόμενες εκδόσεις

Η αρχιτεκτονική προορίζεται να επεκταθεί με:

- πραγματικό sample loading
- WAV/AIFF granular sampling
- SFZ multisample engine
- spectral/additive engine
- 4 LFOs
- πλήρες modulation matrix
- effects rack
- factory preset browser
- CPU quality modes
- 8 oscillator expansion
