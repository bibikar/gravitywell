# Get lookup table
import math

samples = 32;
bitness = 6;
resolution = pow(2,bitness) - 1;

print("uint8_t sine_lookup["+str(samples)+"] = {",end='');

for x in range(0,samples):
    print(str(int(resolution/2+resolution/2*math.sin(math.pi*2*x/samples))),end='');
    if (x == samples - 1):
        print("};");
    else:
        print(",",end='');
        
# Get lookup table for note -> period

semitone_factor = math.pow(2,1/12);

notes_total = 196;
tuning_note = 69;
tuning_freq = 440;
tick_time = 0.0000000125; # 12.5 ns

print("uint32_t note_lookup["+str(notes_total)+"] = {",end='');
for x in range(0, notes_total):
    frequency = tuning_freq * math.pow(semitone_factor, x - tuning_note);
    interrupt_frequency = frequency*samples;
    interrupt_time = 1/interrupt_frequency;
    ticks = interrupt_time / tick_time;
    print(str(int(ticks)),end='');
    if (x == notes_total - 1):
        print("};");
    else:
        print(",",end='');

    

