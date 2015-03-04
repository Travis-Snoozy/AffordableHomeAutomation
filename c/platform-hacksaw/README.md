#Platform-HACKSAW
This is a plugin (platform) for Handaxe, and is responsible for creating and managing virtual devices. Hacksaw virtual devices are presently controlled via an _entirely not-secure_ network connection that the Hacksaw plugin spins up once its host calls the platform's "start" function.

#Design discussion
As previously mentioned, this protocol is _entirely not-secure_ at the moment. Research needs to be done into the most appropriate means to secure the transport. While HTTPS is an obvious first choice (given the ubiquity of its use, especially in mobile and other "appliance"-style consumer devices), the push nature of the server's data makes this an awkward proposition, requiring that the client either consistently poll, or be capable of querying the server and patiently waiting for a response for an extended period of time. The latter proposition also requires that the client be able to establish a second connection to the server in the case where the client needs to push information up to the server, while the first connection remains waiting for spontaneous/unsolicited data from the server. HTTP/2 (over TLS) may be viable technically, but its newness (and the corresponding lack of client support) means that this will probably not be a first choice for implementation. This leaves the first priority being a simple socket implementation over TLS -- either using client certs to authenticate endpoints (less-likely), or a traditional password or challenge/response authentication sequence (more likely).

Presuming that the authentication and transport-level security are taken care of, that leaves the matters of authorization, parse-safety, and hard ordering. Authorization will only receive a brief mention here, in that it may be worthwhile to have a mechanism for allowing authenticated clients to have less than full access to the system -- e.g., global read-only access, or even a device ID based whitelist of read/write access. Parse-safety, likewise, is mentioned in passing here because it must be attended to carefully to ensure that the system internals can't get into a bad state because of tainted input (intentional or otherwise).

The last bit, hard ordering, has to do with ensuring that multiple clients interacting with the system do so in a consistent and predictable fashion, even in the case where two clients attempt to update the same part of the system simultaneously. Hard ordering is not presently implemented, but the sequence number in the protocol is there to assist with such implementation when it eventually occurs. The algorithm under consideration is as follows:

- Clients will be notified of the current sequence number (post-authentication).
- Requests from the client will always contain the next valid sequence number.
    - If the sequence number is too big (client is trying to cheat), the client will be disconnected.
- Responses from the server will always contain a single digit at the start that indicates if the response is caused by the current client (0), or caused by a different client (1).
- Following the local/remote flag, will be the sequence number associated with the response.
- If the client receives a response for a sequence number it sent in a request, and that response was not caused by the current client, the client may need to retry its previous request (if applicable).
- Clients will recalculate the next valid sequence number when they receive updates from the server (regardless if they have recently made a request or not).

Example:

Let's say that there are two devices connected to Hacksaw. Hacksaw is set up with a single light-switch. Both devices' users simultaneously flip the switch. The first device's request is processed first, causing an update to be propagated to both itself (with a 0 in the "remote" field) and the other device (with a 1 in the "remote" field). The second device, upon receiving the update, would change the status of its local switch representation to let the user know the switch was "on." In this case, it would *not* resubmit the request, as the light is already in the state that the user requested.

Almost identical to the scenario above, if the users were instead changing a dial or slider, the device *may* either choose to resubmit the request (overwriting the first user's input), *or* give some sort of UI indication to the user (e.g., a ghosted hand over the control) and *not* resubmit the request (so that the user knows why the control was not updated as they expected). Regardless, if both users happened to change the sliders/dials to the same value simultaneously, the second client need not resubmit anything.

The above may require some changes to the existing protocol; notably the addition of the "remote" field to responses, and (probably) the removal of a sequence number from the status and bye requests (though not the responses). Alternatively, after authentication, the client may simply receive the current status from the server (along with the current sequence number), removing the need for that command altogether.

#Existing protocol
- Unencrypted _entirely not-secure_ TCP stream on port 55455
- No authentication (_entirely not-secure_)
- Global authorization (_entirely not-secure_)
- Commands issued by the client always start with a sequence number (unsigned int), and are one of the following
    - status
        - Responds with the device ID and value of all current virtual devices (in "new" format), terminating the list with the status keyword.
    - bye
        - Shuts down the connection to the Hacksaw server
    - new,[device type],[initial value]
        - Creates a new device of [device type], with initial value set to [initial value].
    - delete,[device id]
        - Deletes the device with the given [device id].
    - update,[device id],[new value]
        - Changes the value of [device id] to be [new value].
- Device types are
    - switch; valid values 0-1
- The server will send unsolicited messages to the client in the following cases
    - A device is created ([sequence #],new,[device id],[device type],[new value])
    - A device is deleted ([sequence #],delete,[device id])
    - A device is updated ([sequence #],update,[device id],[device type],[new value])
