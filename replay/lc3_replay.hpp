#ifndef LC3_REPLAY_HPP
#define LC3_REPLAY_HPP

#include <lc3_all.hpp>
#include <string>

void lc3_setup_replay(lc3_state& state, const std::string& filename, const std::string& replay_string, std::stringstream& newinput);

#endif
