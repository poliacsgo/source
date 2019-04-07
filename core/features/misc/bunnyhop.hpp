#include "../../../dependencies/common_includes.hpp"
class bhop : public singleton<bhop> {
public:
	bool key_state(unsigned int key);
	void bunnyhop(c_usercmd* user_cmd);
	void autostrafe(c_usercmd* user_cmd);
private:
	bool keys[256];
	bool old_keys[256];
};