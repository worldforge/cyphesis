# Media sync

__Status__: not implemented


We need a good mechanism for syncing media between the server and client. The project already has the WFUT library, bit that's not really sufficient for all of our needs. These requirements need to be
met!

* Allow media to be reused between different servers. It's expected that there will be a lot of shared media as well as specific server local variants. Our system need to allow for sharing where
  possible.
* Allow for easy signalling from the server when media had been changed.
* Allow clients to quickly see if they are missing media.
* 
