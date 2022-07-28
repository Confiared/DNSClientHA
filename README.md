== Target ==
The target of this code is be integrated into compatible code.
It's **high availability DNS client**:
  * When DNS error, if have in cache a entry then return the last valid entry
  * Use mutiple dns servers, try resolv 2 time by server
    * If a server failed then be the last server try to not impact resolution time when a dns server failed for newer query
  * Support IPv4 and IPv6 dns server, useful when only IPv4 or IPv6 failing.
  * Keep 100000 dns entry in memory. **Only support AAAA query** (mandatory for Confiared's CDN).

The general target is try return most of the time a valid entry.