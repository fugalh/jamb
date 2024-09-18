## Raspberry Pi Installation
    apt-get install tup libasound-dev libfmt-dev

[Install gtest](https://github.com/google/googletest/releases) e.g.
    apt-get install cmake
    cmake .
    make
    sudo make install

Build and install
    ./build.sh
    install linux/jamb /usr/local/bin

## Raspberry Pi Development
    apt-get install tup libasound-dev libfmt-dev
    
[Install gtest](https://github.com/google/googletest/releases) e.g.
    apt-get install cmake
    cmake .
    make
    sudo make install

then `./test.sh` or `tup`

## Mac setup (for core development)
[Install buck2](https://buck2.build/docs/about/getting_started/#installing-buck2)
e.g. download the binary then
    unzstd buck2-*
    mv buck2-* /usr/local/bin/buck2

[Install gtest](https://github.com/google/googletest/releases) e.g.
    cmake .
    make
    sudo make install

Run tests
    ./test.sh

## Plan
I've had a few false starts on this but I have a week left in recharge (2024)
and I think I can make something that works. I'm less ambitious than previous
versions of myself, what I hope to accomplish this week is simply:
- general cancel
- midi panic
- recall Aeolus presets

Later I'll move toward something more sophisticated, where I assume full
control of the state and presets and can light up buttons accordingly.

In spite of the modest goals, I am still being a bit pedagogic. I'm playing
with approval testing to test the core in the absence of Alsa MIDI (on my
Mac). I'll have a testing midi transport that just logs the midi messages, and
implement that interface using the alsa sequencer API without any extra
testing of that other than "it works".

I'm also playing with the Mikado Method for making forward progress, and doing
it TDD style.
