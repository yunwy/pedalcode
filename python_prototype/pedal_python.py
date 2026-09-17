import sounddevice as sd
import numpy as np


SAMPLE_RATE = 48000
BUFFER_SIZE = 64


###### Effector pedals ######

class OVERDRIVE:
    def __init__(self, gain):
        self.gain = gain

    def __call__(self, signal):
        return np.tanh(self.gain*signal)


class DISTORTION:
    def __init__(self, gain, limit):
        self.gain = gain
        self.limit = limit

    def __call__(self, signal):
        return np.clip(self.gain*signal, -self.limit, self.limit)


class BITCRUSHER:
    def __init__(self, bits, downsample):
        self.bits = bits
        self.step = 2**(1 - self.bits)
        self.downsample = downsample

    def __call__(self, signal):
        # Crushing
        crushed = np.round(signal/self.step)*self.step

        # Downsmapling
        indices = np.arange(len(crushed))
        aligned = indices - (indices%self.downsample)
        crushed = crushed[aligned]

        return crushed

#############################


###### Pedal board ######

class BOARD_SEQ:
    # Sequential pedal board
    def __init__(self, *seq):
        self.seq = list(seq)

    def __call__(self, signal):
        for pedal in self.seq:
            signal = pedal(signal)

        return signal

#########################

overdrive = OVERDRIVE(3)
distortion = DISTORTION(30, 0.3)
bitcrusher = BITCRUSHER(4, 6)

board1 = BOARD_SEQ(bitcrusher)


def callback(input, output, frames, time, status):
    #if status:
    #    print(status)

    output[:] = board1(input)


with sd.Stream(device=(2,4), samplerate=SAMPLE_RATE, blocksize=BUFFER_SIZE, dtype='float32', channels=2, callback=callback):
    print("pedal on")
    sd.sleep(30*1000)