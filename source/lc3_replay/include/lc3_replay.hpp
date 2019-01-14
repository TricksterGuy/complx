#ifndef LC3_REPLAY_HPP
#define LC3_REPLAY_HPP

#include <lc3.hpp>

#include <sstream>
#include <string>

void lc3_setup_replay(lc3_state& state, const std::string& filename, const std::string& replay_string, std::stringstream& newinput);
std::string lc3_describe_replay(const std::string& replay_string);

#endif
