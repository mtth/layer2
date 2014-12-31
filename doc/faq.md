# FAQ


#### I'm connected to Wi-Fi but seeing Ethernet frames, how is this possible?

Your network card driver is probably reconstructing "cooked" Ethernet frames
from the underlying frames. The upside is that this is done in a more efficient
way than if you had to do it yourself (and transparently deals with decrypting
/ encrypting the data). However if your goal is to analyze the underlying
frames, it's hiding that information. Activating monitor mode should allow you
to see the Wi-Fi headers.

Note that this typically also prevents promiscuous mode from working.


#### I'm seeing a lot of invalid frames, why is this?

If the invalid frames are destined for your machine and encrypted, the network
card driver is probably decrypting the contents of the frame but not updating
the checksum. This can happen even in monitor mode.

There are several ways to check if this is the case. The obvious one is to look
through the frames' contents and find unencrypted content (typically HTTP
headers are easy to spot). Another would be to use another device to capture
the traffic and look at the difference between frames.

To fix this, you'll need to disassociate from your network. On OS X, this can
be done with (the path might be slightly different depending on the version of
your OS):

```bash
$ /System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -z
```

You will also probably want to check that your card is still on the correct
channel after disassociating (using the `-c` flag of the `airport` command).
Otherwise your captures will look entirely different.
