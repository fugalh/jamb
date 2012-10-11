The stop jamb is the panel that holds stop knobs.

Hooking up MIDI keyboards to
[Aeolus](http://www.muse-sequencer.org/index.php/Aeolus) is straightforward:
connect them to your PC over a joystick port or with a USB MIDI adapter. But
changing stops at the organ console is a more difficult proposition. This suite
of tools will enable your Android tablet/iPad to become a stop jamb for
controlling Aeolus.

- Put Aeolus.js on your webserver (or use http://hans.fugal.net/jamb/Aeolus.js)
- Start aeolus (perhaps with `-t` in a screen session for a headless setup)
- Start `aeolus-osc.py` (perhaps providing non-default options)
- Install [Control](http://charlie-roberts.com/Control/) on your tablet 
- Add an Interface to Control using the Aeolus.js URL
- Set the Control OSC destination to your server running Aeolus
  (default port is 8080)
- Don't forget to connect your MIDI keyboard to Aeolus
- Pull stops and perform!

`aeolus-osc.py` requires [pyOSC](https://trac.v2.nl/wiki/pyOSC) and
[alsaseq](http://pp.com.mx/python/alsaseq/project.html)

## Hints
- Start Aeolus and get all the audio and MIDI settings just as you like them
  and then press Save. Thereafter you can run aeolus headless using `-t` and a
  screen session, and never have to quit.
- Don't forget realtime priority and memory locking capabilities.
- Control has a screen autolock off setting, but it doesn't work in Android.
  There are apps that will keep your display from going to sleep while certain
  applications are running, e.g. [Screen On](http://bit.ly/SS548q)
- It works on a phone too in a pinch, but the labels are all screwed up because
  the buttons are too small for the font.
- Run `aeolus-osc.py` with `-v` to see debug information.
- `aeolus-osc.py` will connect to the client named `aeolus`, and if it loses a
  connection it will try again to find `aeolus` for every event. So you don't
  need to restart `aeolus-osc.py` if you restart Aeolus.
- Presets are stored client-side, so they do not correspond to Aeolus' presets.
- Edit your `~/.aeolusrc` file to match your preferences, but leave off `-t` and
  give it only when you start Aeolus headless.
- `jamb.screen` is a screen session file for starting up aeolus, connecting it
  to MIDI (change `connect.sh` as needed), and starting `aeolus-osc.py`. e.g.
  `screen -S aeolus -c jamb.screen`
- The makefile will build the three standard Aeolus instruments. You probably
  won't need to do this, just use the ones in the distrubtion, but if you want
  to hack `stops2control.py`...
