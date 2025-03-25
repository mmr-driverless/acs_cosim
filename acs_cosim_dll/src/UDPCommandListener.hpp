#ifndef UDP_COMMAD_LISTENER_HPP
#define UDP_COMMAD_LISTENER_HPP

#include "Sim.hpp"

struct UDPCommandListener {
	char undefined[88];
	Sim* sim;
	unsigned long sok;
};

#endif // UDP_COMMAD_LISTENER_HPP