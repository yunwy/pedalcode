import sounddevice as sd
from pedalboard import Pedalboard, Distortion, Reverb, Delay, Phaser, Chorus, Compressor, NoiseGate


SAMPLE_RATE = 48000
BUFFER_SIZE = 64
CHANNELS = 1



board1 = Pedalboard([
Compressor(
    threshold_db=-20,
    ratio=4,
    attack_ms=10,
    release_ms=100
)

])


def callback(input, output, frames, time, status):
    #if status:
    #    print(status)

    output[:] = board1(input, SAMPLE_RATE)


with sd.Stream(device=(2,4), samplerate=SAMPLE_RATE, blocksize=BUFFER_SIZE, dtype='float32', channels=2, callback=callback):
    print("pedal on")
    sd.sleep(30*1000)