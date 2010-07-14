extern TCPsocket sockets[MAX_PLAYERS];
extern TCPsocket server;
extern SDLNet_SocketSet socketSet;

void netClientMessage(int playerID, char* message);
void netServerMessage(int playerID, char* message);

void netClientObjectMessage(int objectID, int destObjectID, int lx, int ly, int dx, int dy, bool forced);
void netServerObjectMessage(int objectID, int destObjectID, int lx, int ly, int dx, int dy, bool forced);

inline void netClientObjectMessage(int objectID, int destObjectID, COORDTYPE *location, COORDTYPE *destination, bool forced) { netClientObjectMessage(objectID, destObjectID, location->x, location->y, destination->x, destination->y, forced); }
inline void netServerObjectMessage(int objectID, int destObjectID, COORDTYPE *location, COORDTYPE *destination, bool forced) { netServerObjectMessage(objectID, destObjectID, location->x, location->y, destination->x, destination->y, forced); }

void netServerAttackMessage(int objectID, int destObjectID, int lx, int ly, int dx, int dy);

void netServerConsistancyMessage(int objectID, COORDTYPE* location, int angle, int turretAngle);

void netClientDestroyObject(int objectID, int lx, int ly);
void netServerDestroyObject(int objectID, int lx, int ly);

void netClientBuildMessage(int objectID, COORDTYPE* location, bool multipleMode, int num);
void netServerBuildMessage(int objectID, COORDTYPE* location, bool multipleMode, int num);

void netClientCancelMessage(int objectID, COORDTYPE* location, bool multipleMode, int num);
void netServerCancelMessage(int objectID, COORDTYPE* location, bool multipleMode, int num);

void netServerBuildComplete(int objectID, COORDTYPE* location);

void netClientCreateObject(int player, int itemID, int lx, int ly, int angle);
void netServerCreateObject(int player, int itemID, int lx, int ly, int angle);

void netClientPlaceStructure(int player, int builderID, int x, int y, int itemID, int lx, int ly, int angle);
void netServerPlaceStructure(int player, int builderID, int x, int y, int itemID, int lx, int ly, int angle);

//starport commands
void netClientPlaceOrder(int player, int itemID, int lx, int ly);
void netServerPlaceOrder(int player, int itemID, int lx, int ly);

void netClientCancelOrder(int player, int itemID, int lx, int ly);
void netServerCancelOrder(int player, int itemID, int lx, int ly);

//palace commands
void netClientPalaceSpecial(int player, int itemID, int lx, int ly);
void netServerPalaceSpecial(int player, int itemID, int lx, int ly, int dx, int dy);

void netServerPalaceEnable(int player, int itemID, int lx, int ly);

//special unit commands
void netClientSetAttackMode(int itemID, int lx, int ly, ATTACKTYPE attackMode);
void netServerSetAttackMode(int itemID, int lx, int ly, ATTACKTYPE attackMode);

void netClientDevastate(int itemID, int dx, int dy);
void netServerDevastate(int itemID, int dx, int dy);

void netServerDeviate(int itemID, int dx, int dy, int player);

