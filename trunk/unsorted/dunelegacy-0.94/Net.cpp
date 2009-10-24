#include "sand.h"
//#include "Items.h"

void netSendToClients(char msg[MAX_MSGLENGTH])
{
	bool ok;
	int i;

	for(i = 1; i < MAX_PLAYERS; i++)
	if (sockets[i] != NULL)
	{
		ok = (SDLNet_TCP_Send(sockets[i], msg, MAX_MSGLENGTH) == MAX_MSGLENGTH);
		if (!ok)
		{
			sprintf(msg, "%s has left the game\n", settings.playerName[i]);
			showMessage(msg);
			settings.playerName[i][0] = '\0';
			SDLNet_TCP_DelSocket(socketSet, sockets[i]);
			SDLNet_TCP_Close(sockets[i]);
			sockets[i] = NULL;
	//		playersJoined--;
		}
	}
}

void netSendToServer(char msg[MAX_MSGLENGTH])
{
	bool ok;

	ok = (SDLNet_TCP_Send(server, msg, MAX_MSGLENGTH) == MAX_MSGLENGTH);
	if (!ok)
	{
		showMessage("Disconnected from server.\n");
		SDLNet_TCP_DelSocket(socketSet, server);
		SDLNet_TCP_Close(server);
		server = NULL;
	}
}

void netClientMessage(int playerID, char* message)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_MESSAGE;
	msg[1] = playerID;
	strncpy(&msg[2], message, MAX_MSGLENGTH);

	netSendToServer(msg);
}

void netServerMessage(int playerID, char* message)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_MESSAGE;
	msg[1] = playerID;
	strncpy(&msg[2], message, MAX_MSGLENGTH);

	netSendToClients(msg);
	showMessage(&msg[2]);
}

void netClientObjectMessage(int objectID, int destObjectID, int lx, int ly, int dx, int dy, bool forced)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_OBJECTCOMMAND;
	sprintf(&msg[1], "%d,%d,%d,%d,%d,%d,%d", objectID, destObjectID, lx, ly, dx, dy, forced ? 1 : 0);

	netSendToServer(msg);
}

void netServerObjectMessage(int objectID, int destObjectID, int lx, int ly, int dx, int dy, bool forced)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_OBJECTCOMMAND;
	sprintf(&msg[1], "%d,%d,%d,%d,%d,%d,%d", objectID, destObjectID, lx, ly, dx, dy, forced ? 1 : 0);

	netSendToClients(msg);
}

void netServerAttackMessage(int objectID, int destObjectID, int lx, int ly, int dx, int dy)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_ATTACK;
	sprintf(&msg[1], "%d,%d,%d,%d,%d,%d", objectID, destObjectID, lx, ly, dx, dy);

	netSendToClients(msg);
}

void netServerConsistancyMessage(int objectID, COORDTYPE* location, int angle, int turretAngle)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_CONSISTANCY;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", objectID, location->x, location->y, angle, turretAngle);

	netSendToClients(msg);
}

void netClientDestroyObject(int objectID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_OBJECTDESTROY;
	sprintf(&msg[1], "%d,%d,%d", objectID, lx, ly);

	netSendToServer(msg);
}

void netServerDestroyObject(int objectID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_OBJECTDESTROY;
	sprintf(&msg[1], "%d,%d,%d", objectID, lx, ly);

	netSendToClients(msg);
}

void netClientBuildMessage(int objectID, COORDTYPE* location, bool multipleMode, int num)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_BUILDCOMMAND;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", objectID, location->x, location->y, (int)multipleMode, num);

	netSendToServer(msg);
}

void netServerBuildMessage(int objectID, COORDTYPE* location, bool multipleMode, int num)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_BUILDCOMMAND;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", objectID, location->x, location->y, (int)multipleMode, num);

	netSendToClients(msg);
}

void netClientCancelMessage(int objectID, COORDTYPE* location, bool multipleMode, int num)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_CANCELCOMMAND;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", objectID, location->x, location->y, (int)multipleMode, num);

	netSendToServer(msg);
}

void netServerCancelMessage(int objectID, COORDTYPE* location, bool multipleMode, int num)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_CANCELCOMMAND;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", objectID, location->x, location->y, (int)multipleMode, num);

	netSendToClients(msg);
}

void netServerBuildComplete(int objectID, COORDTYPE* location)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_BUILDCOMPLETE;
	sprintf(&msg[1], "%d,%d,%d", objectID, location->x, location->y);

	netSendToClients(msg);
}

void netClientCreateObject(int player, int itemID, int lx, int ly, int angle)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_CREATEOBJECT;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", player, itemID, lx, ly, angle);

	netSendToServer(msg);
}

void netServerCreateObject(int player, int itemID, int lx, int ly, int angle)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_CREATEOBJECT;
	sprintf(&msg[1], "%d,%d,%d,%d,%d", player, itemID, lx, ly, angle);

	netSendToClients(msg);
}

void netClientPlaceStructure(int player, int builderID, int x, int y, int itemID, int lx, int ly, int angle)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_PLACESTRUCTURE;
	sprintf(&msg[1], "%d,%d,%d,%d,%d,%d,%d,%d", player, builderID, x, y, itemID, lx, ly, angle);

	netSendToServer(msg);
}

void netServerPlaceStructure(int player, int builderID, int x, int y, int itemID, int lx, int ly, int angle)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_PLACESTRUCTURE;
	sprintf(&msg[1], "%d,%d,%d,%d,%d,%d,%d,%d", player, builderID, x, y, itemID, lx, ly, angle);

	netSendToClients(msg);
}

void netClientPlaceOrder(int player, int itemID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_STARPORTORDER;
	sprintf(&msg[1], "%d,%d,%d,%d", player, itemID, lx, ly);

	netSendToServer(msg);
}

void netServerPlaceOrder(int player, int itemID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_STARPORTORDER;
	sprintf(&msg[1], "%d,%d,%d,%d", player, itemID, lx, ly);

	netSendToClients(msg);
}

void netClientCancelOrder(int player, int itemID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_STARPORTCANCEL;
	sprintf(&msg[1], "%d,%d,%d,%d", player, itemID, lx, ly);

	netSendToServer(msg);
}

void netServerCancelOrder(int player, int itemID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_STARPORTCANCEL;
	sprintf(&msg[1], "%d,%d,%d,%d", player, itemID, lx, ly);

	netSendToClients(msg);
}

void netClientPalaceSpecial(int player, int itemID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_PALACESPECIAL;
	sprintf(&msg[1], "%d,%d,%d,%d", player, itemID, lx, ly);

	netSendToServer(msg);
}

void netServerPalaceSpecial(int player, int itemID, int lx, int ly, int dx, int dy)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_PALACESPECIAL;
	sprintf(&msg[1], "%d,%d,%d,%d,%d,%d", player, itemID, lx, ly, dx, dy);

	netSendToClients(msg);
}

void netServerPalaceEnable(int player, int itemID, int lx, int ly)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_PALACEENABLE;
	sprintf(&msg[1], "%d,%d,%d,%d", player, itemID, lx, ly);

	netSendToClients(msg);
}

//units special commands

void netClientSetAttackMode(int itemID, int lx, int ly, ATTACKTYPE attackMode)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_SETATTACKMODE;
	sprintf(&msg[1], "%d,%d,%d,%d", itemID, lx, ly, attackMode);

	netSendToServer(msg);
}

void netServerSetAttackMode(int itemID, int lx, int ly, ATTACKTYPE attackMode)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_SETATTACKMODE;
	sprintf(&msg[1], "%d,%d,%d,%d", itemID, lx, ly, attackMode);

	netSendToClients(msg);
}

void netClientDevastate(int itemID, int dx, int dy)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_DEVASTATE;
	sprintf(&msg[1], "%d,%d,%d", itemID, dx, dy);

	netSendToServer(msg);
}

void netServerDevastate(int itemID, int dx, int dy)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_DEVASTATE;
	sprintf(&msg[1], "%d,%d,%d", itemID, dx, dy);

	netSendToClients(msg);
}

void netServerDeviate(int itemID, int dx, int dy, int player)
{
	char	msg[MAX_MSGLENGTH];

	msg[0] = MSG_DEVIATE;
	sprintf(&msg[1], "%d,%d,%d,%d", itemID, dx, dy, player);

	netSendToClients(msg);
}

