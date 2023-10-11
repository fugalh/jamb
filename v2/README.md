# Aeolus Stop Jamb v2
I want to repurpose a Launchpad Mini Mk2 as a dedicated jamb. And I want to
make the integration with Aeolus tighter.

I believe it's straightforward to add an OSC interface to Aeolus itself, and
have a separate program that is dedicated to talking to the Launchpad over
MIDI and sending OSC messages to Aeolus, and also receiving OSC messages so
that it is never out of sync even if stops are changed through another
mechanism. The Control OSC integration (or any other app) would also be able
to talk directly to Aeolus. Hopefully Fons won't mind merging some OSC code.

Aeolus uses a message passing mechanism for all model and interface updates,
so the changes should be minimally disruptive.

The OSC interface will listen for OSC messages on the configured port and respond:
- register(host, port) send interface updates here
- deregister(host, port) stop sending interface udpates here
- handle control messages: forward them to the model, and the model responds by sending interface update messages, which we forward to registered observers

Aeolus maps "interface group" as an additional layer of indirection around "division" and "keyboard" for what reason I'm not sure but that's how it indexes things.

Control Events (To Aeolus)
```
/aeolus/control/knob/push ,ii (group, knob)
/aeolus/control/knob/pull ,ii (group, knob)
/aeolus/control/preset/store ,ii (bank, preset)
/aeolus/control/preset/recall ,ii (bank, preset)
/aeolus/control/all_notes_off
/aeolus/control/general_cancel
/aeolus/control/cancel ,i (group)
/aeolus/control/save
```

View Events (To Aeolus)
```
# Aeolus will send description messages (see below)
# after successful registration
/aeolus/view/register ,si (host,port)
/aeolus/view/deregister ,si (host,port)
# Request that Aeolus send the description messages again
/aeolus/view/describe
/aeolus/view/ping
```

View Events (From Aeolus)
```
/aeolus/view/knob/push ,ii (group, knob)
# Sent for each knob as appropriate when described and on updates
/aeolus/view/knob/pull ,ii (group, knob)
# Sent for each knob when described
# With "type" in {"stop","coupler","tremulant"}
/aeolus/view/knob/describe ,iiss (group, knob, type, name)
/aeolus/view/pong

## The Launchpad UI
Use the "functional UI" approach a la Elm and [Raj](https://jew.ski/raj/).

```
dispatch(message)
  (state, effect) := update(state, message)
  effect(dispatch)
  view(state, dispatch)
```

`state` (model) is the state of the Launchpad, (a subset of) the `effect`s send
control events to Aeolus over OSC, and `view` updates the Launchpad.

Incoming OSC `/aeolus/view` messages are `dispatch`ed, as are Launchpad button
press/release events.

I briefly debated the Raj approach (`effect` is a function) and the approach
described in [Couriol] where a list of commands is returned. The latter seemed
more testable, but less convenient. But then I realized we can still have a
command dispatch (or list of commands) and `effect` just closes over calling
it

    auto effect = [aeolusLink]{ aeolusLink->dispatch(Command{...}); }

And the link object can be substituted with a test fake during tests.
